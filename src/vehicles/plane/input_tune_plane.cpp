// Location: src/vehicles/plane/input_tune_plane.cpp
#include "Core/app_state.h"
#include "input/input_manage.h"
#include "input_tune_plane.h"
#include <algorithm>
#include <cmath>
InputTunePlane::InputTunePlane() {}
int16_t InputTunePlane::apply_deadzone_smooth(int16_t raw, float dz_percent) {
    int32_t threshold = static_cast<int32_t>(32768.0f * (dz_percent / 100.0f));
    int32_t abs_raw = std::abs(raw);
    if (abs_raw <= threshold) return 0;
    float norm = (abs_raw - threshold) / (32768.0f - (float)threshold);
    return (raw > 0) ? static_cast<int16_t>(norm * 32767.0f) : static_cast<int16_t>(-norm * 32767.0f);
}
int16_t InputTunePlane::apply_expo_raw(int16_t raw, float expo_percent) {
    double x = raw / 32768.0;
    double e = expo_percent / 100.0;
    double curve = e * (x * x * x) + (1.0 - e) * x;
    return static_cast<int16_t>(curve * 32767.0);
}
void InputTunePlane::process_all_slots() {
    auto &state = AppState::instance();
    auto &im = InputManager::instance();
    for (int slot = 0; slot < 4; slot++) {
        DeviceState d = im.getSlotState(slot);
        int offset = slot * 50;
        if (d.connected) {
            process_to_buffer(slot, d);
        } else {
            for (int i = 0; i < 50; i++) state.tuned_slots[offset + i] = 0;
        }
    }
}
void InputTunePlane::process_to_buffer(int slot, const DeviceState &raw) {
    auto &state = AppState::instance();
    int offset = slot * 50;
    for (int i = 0; i < 8; i++) {
        int16_t val = raw.axes[i];
        val = apply_deadzone_smooth(val, state.deadzones[i]);
        if (state.expo_enabled[i]) val = apply_expo_raw(val, state.current_expo);
        state.tuned_slots[offset + i] = val;
    }
    for (int i = 0; i < 32; i++) state.tuned_slots[offset + 8 + i] = static_cast<int16_t>(raw.buttons[i]);
    state.tuned_slots[offset + 40] = static_cast<int16_t>(raw.gyro[0]);
    state.tuned_slots[offset + 41] = static_cast<int16_t>(raw.gyro[1]);
    state.tuned_slots[offset + 42] = static_cast<int16_t>(raw.gyro[2]);
    for (int i = 0; i < 3; i++) state.tuned_slots[offset + 43 + i] = static_cast<int16_t>(raw.accel[i]);
    for (int i = 0; i < 3; i++) state.tuned_slots[offset + 46 + i] = static_cast<int16_t>(raw.gyro[i]);
}
