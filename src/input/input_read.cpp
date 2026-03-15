#include "input/input_read.h"

#include <SDL3/SDL.h>

#include <algorithm>
#include <cstring>

InputReader::InputReader(SDL_JoystickID id) : instance_id(id)
{
    gamepad = SDL_OpenGamepad(id);
    if (gamepad) {
        state.name = SDL_GetGamepadName(gamepad);
        SDL_SetGamepadSensorEnabled(gamepad, SDL_SENSOR_ACCEL, true);
        SDL_SetGamepadSensorEnabled(gamepad, SDL_SENSOR_GYRO, true);
        state.connected = true;
    }
}

void InputReader::update()
{
    if (!gamepad) return;

    // 16-bit Axis Pull
    for (int i = 0; i < 16; i++) state.axes[i] = SDL_GetGamepadAxis(gamepad, (SDL_GamepadAxis)i);

    std::memset(state.buttons, 0, sizeof(state.buttons));
    for (int i = 0; i < 32; i++)
        state.buttons[i] = SDL_GetGamepadButton(gamepad, (SDL_GamepadButton)i);

    float raw_gyro[3] = {0}, raw_accel[3] = {0};
    SDL_GetGamepadSensorData(gamepad, SDL_SENSOR_GYRO, raw_gyro, 3);
    SDL_GetGamepadSensorData(gamepad, SDL_SENSOR_ACCEL, raw_accel, 3);

    // LEAKY INTEGRATOR: The natural "Passive Return"
    // 0.9995f means it takes roughly 4-5 seconds of being still to hit center.
    // This is invisible during active flight/driving.
    float decay = 0.9995f;
    accum_roll = (accum_roll * decay) + (raw_gyro[0] * 2500.0f);
    accum_pitch = (accum_pitch * decay) + (raw_gyro[1] * 2500.0f);

    state.gyro[0] = std::clamp(accum_roll, -32768.0f, 32767.0f);
    state.gyro[1] = std::clamp(accum_pitch, -32768.0f, 32767.0f);
    state.gyro[2] = raw_gyro[2] * 16384.0f;  // Raw normalized Yaw

    for (int i = 0; i < 3; i++) state.accel[i] = raw_accel[i] * 16384.0f;
}

InputReader::~InputReader()
{
    if (gamepad) SDL_CloseGamepad(gamepad);
}
