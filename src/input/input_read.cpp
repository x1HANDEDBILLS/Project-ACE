// Location: src/input/input_read.cpp
#include "input/input_read.h"
#include <SDL3/SDL.h>
#include <algorithm>
#include <cmath>
#include <cstring>
InputReader::InputReader(SDL_JoystickID id) : instance_id(id) {
    gamepad = SDL_OpenGamepad(id);
    if (gamepad) {
        state.name = SDL_GetGamepadName(gamepad);
        SDL_SetGamepadSensorEnabled(gamepad, SDL_SENSOR_ACCEL, true);
        SDL_SetGamepadSensorEnabled(gamepad, SDL_SENSOR_GYRO, true);
        state.connected = true;
    }
}
void InputReader::update() {
    if (!gamepad) return;
    // 1. Standard 16-bit Axis & Button Pull
    for (int i = 0; i < 16; i++) { state.axes[i] = SDL_GetGamepadAxis(gamepad, (SDL_GamepadAxis)i); }
    std::memset(state.buttons, 0, sizeof(state.buttons));
    for (int i = 0; i < 32; i++) { state.buttons[i] = SDL_GetGamepadButton(gamepad, (SDL_GamepadButton)i); }
    // 2. Sensor Data Pull (Radians per Second & m/s^2)
    float raw_gyro[3] = {0}, raw_accel[3] = {0};
    SDL_GetGamepadSensorData(gamepad, SDL_SENSOR_GYRO, raw_gyro, 3);
    SDL_GetGamepadSensorData(gamepad, SDL_SENSOR_ACCEL, raw_accel, 3);
    // 3. ADAPTIVE SENSOR FUSION MATH
    const float GYRO_NOISE_FLOOR = 0.015f;
    const float RAD_TO_DEG = 57.2958f;
    const float DT = 0.001f;       // 1000Hz Loop Time
    const float MAX_ANGLE = 50.0f; // +/- 50 Degrees Physical Throw
    // A. Gyro Noise Gate
    float clean_gyro_roll = (std::abs(raw_gyro[0]) > GYRO_NOISE_FLOOR) ? raw_gyro[0] : 0.0f;
    float clean_gyro_pitch = (std::abs(raw_gyro[1]) > GYRO_NOISE_FLOOR) ? raw_gyro[1] : 0.0f;
    // B. Accel Pre-Filter (Smooths out micro-vibrations)
    for (int i = 0; i < 3; i++) { filtered_accel[i] = (filtered_accel[i] * 0.95f) + (raw_accel[i] * 0.05f); }
    // C. The Magnitude Gate (Calculates Error from 1G)
    float accel_mag =
        std::sqrt(raw_accel[0] * raw_accel[0] + raw_accel[1] * raw_accel[1] + raw_accel[2] * raw_accel[2]);
    float g_force = accel_mag / 9.81f;
    float g_error = std::abs(g_force - 1.0f);
    // Calculate angles based on Gravity (Assuming Y is Gravity)
    float acc_roll = std::atan2(filtered_accel[0], filtered_accel[1]) * RAD_TO_DEG;
    float acc_pitch = std::atan2(filtered_accel[2], filtered_accel[1]) * RAD_TO_DEG;
    float gyro_rate_roll = clean_gyro_roll * RAD_TO_DEG;
    float gyro_rate_pitch = clean_gyro_pitch * RAD_TO_DEG;
    // D. Dynamic Complementary Filter
    float dynamic_alpha = 0.98f + std::clamp(g_error * 0.05f, 0.0f, 0.019f);
    accum_roll = dynamic_alpha * (accum_roll + gyro_rate_roll * DT) + (1.0f - dynamic_alpha) * acc_roll;
    accum_pitch = dynamic_alpha * (accum_pitch + gyro_rate_pitch * DT) + (1.0f - dynamic_alpha) * acc_pitch;
    // Scale directly to 16-bit (-32767 to 32767)
    float target_roll = (accum_roll / MAX_ANGLE) * 32767.0f;
    float target_pitch = (accum_pitch / MAX_ANGLE) * 32767.0f;
    // E. Adaptive Output Smoothing
    float rotation_speed_roll = std::abs(raw_gyro[0]);
    float rotation_speed_pitch = std::abs(raw_gyro[1]);
    float base_smoothing_roll = 0.85f * (1.0f - std::clamp(rotation_speed_roll / 2.0f, 0.0f, 1.0f));
    float base_smoothing_pitch = 0.85f * (1.0f - std::clamp(rotation_speed_pitch / 2.0f, 0.0f, 1.0f));
    // Shake Override: Force heavy smoothing to absorb shocks if G-force spikes
    float final_smoothing_roll = (g_error > 0.3f) ? 0.95f : base_smoothing_roll;
    float final_smoothing_pitch = (g_error > 0.3f) ? 0.95f : base_smoothing_pitch;
    smooth_out_roll = (smooth_out_roll * final_smoothing_roll) + (target_roll * (1.0f - final_smoothing_roll));
    smooth_out_pitch = (smooth_out_pitch * final_smoothing_pitch) + (target_pitch * (1.0f - final_smoothing_pitch));
    // 4. Map the final values back into your ACE hardware state
    state.gyro[0] = (int16_t)std::clamp(smooth_out_roll, -32767.0f, 32767.0f);
    state.gyro[1] = (int16_t)std::clamp(smooth_out_pitch, -32767.0f, 32767.0f);
    // Yaw and Accel stay raw-scaled for now
    state.gyro[2] = (int16_t)std::clamp(raw_gyro[2] * 16384.0f, -32767.0f, 32767.0f);
    for (int i = 0; i < 3; i++) { state.accel[i] = (int16_t)std::clamp(raw_accel[i] * 16384.0f, -32767.0f, 32767.0f); }
}
InputReader::~InputReader() {
    if (gamepad) SDL_CloseGamepad(gamepad);
}
