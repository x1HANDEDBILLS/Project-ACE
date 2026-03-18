// Location: src/vehicles/plane/rc_trim_plane.cpp
#include "Core/app_state.h"
#include "rc_trim_plane.h"
#include <algorithm>
void RCTrimPlane::process() {
    auto &state = AppState::instance();
    for (int ch = 0; ch < 16; ch++) {
        int32_t raw_input = state.mapped_channels[ch];
        // Throttle bypass for safety
        if (state.throttle_mode[ch]) {
            state.trim_channels[ch] = static_cast<int16_t>(raw_input);
            continue;
        }
        // Flight Trim: 310 units (100 clicks = 31,000 / ~95% throw)
        // Alignment: 80 units (High precision / ~2 CRSF steps)
        int32_t offset = (state.physical_trims[ch] * 310) + (state.sub_trims[ch] * 80);
        // Safety clamp to prevent signal wrap-around
        offset = std::clamp(offset, -31000, 31000);
        int32_t final_val;
        // Proportional Scaling: Preserves full stick authority regardless of trim
        if (raw_input >= 0) {
            int32_t room_up = 32767 - offset;
            final_val = offset + (raw_input * room_up / 32767);
        } else {
            int32_t room_down = offset - (-32768);
            final_val = offset + (raw_input * room_down / 32768);
        }
        state.trim_channels[ch] = static_cast<int16_t>(std::clamp(final_val, -32768, 32767));
    }
}
