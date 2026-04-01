// Location: src/vehicles/plane/map/rc_map_plane.cpp
#include "Core/app_state.h"
#include "vehicles/plane/map/rc_map_plane.h"
#include <algorithm>
static int16_t get_processed_val(int16_t raw, bool is_center, bool is_reverse) {
    int32_t val = raw;
    // Unipolar/Center Logic: Scale [-32768, 32767] to [0, 32767]
    // This allows a stick to act as a 0-100% slider for things like flaps
    if (is_center) { val = (val + 32768) / 2; }
    if (is_reverse) val *= -1;
    return static_cast<int16_t>(std::clamp<int32_t>(val, -32768, 32767));
}
void RCMapPlane::process() {
    auto &state = AppState::instance();
    // STAGE 1: Standard 1-to-1 Mapping
    for (int ch = 0; ch < 16; ch++) {
        int source_id = state.channel_map[ch];
        if (source_id >= 0 && source_id < 200 && source_id != 22) {
            // Apply standard inversion from Page 3
            state.mapped_channels[ch] = get_processed_val(state.tuned_slots[source_id], false, state.inverted[ch]);
        } else {
            state.mapped_channels[ch] = 0;
        }
    }
    // STAGE 2: Split Mixer Overrides (Page 2 Logic)
    for (int i = 0; i < 6; i++) {
        auto &m = state.split_mixers[i];
        int target = m.target_ch;
        // Only process if this mixer slot has a valid target channel
        if (target >= 0 && target < 16) {
            int32_t mixed_total = 0;
            // Add Negative Source contribution
            if (m.neg_id >= 0 && m.neg_id < 200 && m.neg_id != 22) {
                mixed_total += get_processed_val(state.tuned_slots[m.neg_id], m.neg_center, m.neg_reverse);
            }
            // Add Positive Source contribution
            if (m.pos_id >= 0 && m.pos_id < 200 && m.pos_id != 22) {
                mixed_total += get_processed_val(state.tuned_slots[m.pos_id], m.pos_center, m.pos_reverse);
            }
            // OVERRIDE the standard mapping with the combined mix
            state.mapped_channels[target] = static_cast<int16_t>(std::clamp<int32_t>(mixed_total, -32768, 32767));
        }
    }
}
