#include "logic/logic.h"

#include <algorithm>

#include "Core/app_state.h"
#include "input/input_manage.h"
#include "vehicles/plane/input_tune_plane.h"

// Link to our new CRSF Bridge
extern void transmit_to_daemon();

std::atomic<bool> Logic::backend_unlocked{false};
static InputTunePlane plane_processor;

void Logic::initialize()
{
    backend_unlocked.store(true);
}

int32_t process_mixer_side(int id, int16_t raw_val, bool center, bool reverse)
{
    if (id < 0 || id >= 200 || id == 22) return 0;
    int32_t val = raw_val;
    int local_id = id % 50;
    bool is_button = (local_id >= 8 && local_id <= 39);

    if (is_button) {
        if (center)
            val = (raw_val == 0) ? 0 : 32767;
        else
            val = (raw_val == 0) ? -32767 : 32767;
    } else {
        if (center)
            val = (static_cast<int32_t>(raw_val) * 2) - 32767;
        else
            val = raw_val;
    }
    if (reverse) val = -val;
    return val;
}

void Logic::run_iteration()
{
    if (!backend_unlocked.load()) return;
    auto& state = AppState::instance();
    auto& im = InputManager::instance();

    // 1. Fill Input Slots
    for (int slot = 0; slot < 4; slot++) {
        DeviceState d = im.getSlotState(slot);
        if (d.connected)
            plane_processor.process_to_buffer(slot, d);
        else {
            int offset = slot * 50;
            for (int i = 0; i < 50; i++) state.tuned_slots[offset + i] = 0;
        }
    }

    // 2. Compute Raw Channel Data (Mixing Stage)
    for (int ch = 0; ch < 16; ch++) {
        int32_t raw_combined = 0;
        int mixerIdx = -1;

        for (int m = 0; m < 6; m++) {
            if (state.split_mixers[m].target_ch == ch) {
                mixerIdx = m;
                break;
            }
        }

        if (mixerIdx != -1) {
            auto& mix = state.split_mixers[mixerIdx];
            int32_t p = process_mixer_side(mix.pos_id, state.tuned_slots[mix.pos_id],
                                           mix.pos_center, mix.pos_reverse);
            int32_t n = process_mixer_side(mix.neg_id, state.tuned_slots[mix.neg_id],
                                           mix.neg_center, mix.neg_reverse);
            raw_combined = p + n;
        } else {
            int sid = state.channel_map[ch];
            if (sid >= 0 && sid < 200 && sid != 22) {
                raw_combined = state.tuned_slots[sid];
            }
        }

        // Store intermediate mixed value
        state.channels[ch] = (int16_t)std::clamp(raw_combined, (int32_t)-32768, (int32_t)32767);

        // 3. FINAL STAGE: Apply Page 3 Inversion to EVERYTHING
        int32_t final_output = state.channels[ch];
        if (state.inverted[ch]) {
            final_output = -final_output;
        }

        // Save to mapped_channels (This is what the Blackbox & Diagnostics see)
        state.mapped_channels[ch] =
            (int16_t)std::clamp(final_output, (int32_t)-32768, (int32_t)32767);
    }

    // 4. Send to CRSF Transmitter
    // This is the bridge call that pushes data to the nomad_daemon at 1000Hz
    transmit_to_daemon();
}