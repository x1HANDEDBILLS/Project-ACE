#include "input_read.h"
#include <iostream>

InputReader::InputReader(SDL_JoystickID id) : instance_id(id) {
    gamepad = SDL_OpenGamepad(id);
    if (gamepad) {
        // 1. Get the Name
        const char* gName = SDL_GetGamepadName(gamepad);
        state.name = gName ? gName : "Controller";

        // 2. Get the GUID (SDL3 Way)
        // First get the Joystick handle from the Gamepad
        SDL_Joystick* joy = SDL_GetGamepadJoystick(gamepad);
        if (joy) {
            SDL_GUID sdl_guid = SDL_GetJoystickGUID(joy);
            char guid_buffer[33];
            SDL_GUIDToString(sdl_guid, guid_buffer, sizeof(guid_buffer));
            state.guid = guid_buffer;
        }

        // 3. Enable Sensors
        SDL_SetGamepadSensorEnabled(gamepad, SDL_SENSOR_ACCEL, true);
        SDL_SetGamepadSensorEnabled(gamepad, SDL_SENSOR_GYRO, true);
        
        state.connected = true;
        std::cout << "[DEBUG] Controller Connected: " << state.name << std::endl;
    } else {
        state.connected = false;
    }
}

InputReader::~InputReader() {
    if (gamepad) SDL_CloseGamepad(gamepad);
}

void InputReader::update() {
    if (!gamepad) return;

    for (int i = 0; i < 6; i++) {
        state.axes[i] = SDL_GetGamepadAxis(gamepad, (SDL_GamepadAxis)i);
    }

    for (int i = 0; i < 15; i++) {
        state.buttons[i] = SDL_GetGamepadButton(gamepad, (SDL_GamepadButton)i);
    }

    SDL_GetGamepadSensorData(gamepad, SDL_SENSOR_ACCEL, state.accel, 3);
    SDL_GetGamepadSensorData(gamepad, SDL_SENSOR_GYRO, state.gyro, 3);
}