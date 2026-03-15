#include "vehicles/plane/rc_map_plane.h"

#include <algorithm>

#include "Core/app_state.h"

static int16_t get_tuned_val(int16_t raw, bool is_center, bool is_reverse)
{
    int32_t val = raw;
    if (is_center) val += 32768;
    if (is_reverse) val *= -1;
    return static_cast<int16_t>(std::max(-32768, std::min(32767, (int)val)));
}

void RCMapPlane::process()
{
    auto& state = AppState::instance();

    // First, fill channels with standard mapping
    for (int ch = 0; ch < 16; ch++) {
        int source_id = state.channel_map[ch];
        if (source_id >= 0 && source_id < 200) {
            state.channels[ch] =
                get_tuned_val(state.tuned_slots[source_id], false, state.inverted[ch]);
        }
    }

    // Second, override specific channels with the 6 Split Mixers
    for (int i = 0; i < 6; i++) {
        int target = state.split_mixers[i].target_ch;
        if (target >= 0 && target < 16) {
            int32_t pos =
                get_tuned_val(state.tuned_slots[state.split_mixers[i].pos_id],
                              state.split_mixers[i].pos_center, state.split_mixers[i].pos_reverse);
            int32_t neg =
                get_tuned_val(state.tuned_slots[state.split_mixers[i].neg_id],
                              state.split_mixers[i].neg_center, state.split_mixers[i].neg_reverse);

            // Mirror/Cancel Logic
            int32_t combined = pos + neg;
            state.channels[target] =
                static_cast<int16_t>(std::max(-32768, std::min(32767, (int)combined)));
        }
    }
}
