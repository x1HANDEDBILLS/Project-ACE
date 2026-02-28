#include "input_read.h"
#include <SDL3/SDL.h>
#include <iostream>
#include <unistd.h>
#include <limits.h>
#include <libgen.h>
#include <string>
#include <stdlib.h> 
#include <algorithm> 
#include <cmath>

InputReader::InputReader(SDL_JoystickID id) : instance_id(id) {
    gamepad = SDL_OpenGamepad(id);
    
    if (gamepad) {
        const char* gName = SDL_GetGamepadName(gamepad);
        state.name = gName ? gName : "Controller";

        SDL_Joystick* joy = SDL_GetGamepadJoystick(gamepad);
        if (joy) {
            SDL_GUID sdl_guid = SDL_GetJoystickGUID(joy);
            char guid_buffer[33];
            SDL_GUIDToString(sdl_guid, guid_buffer, sizeof(guid_buffer));
            state.guid = guid_buffer;
        }

        // --- USB Path Resolution ---
        const char* devPath = SDL_GetGamepadPath(gamepad); 
        if (devPath) {
            std::string pathStr = devPath;
            size_t hidrawPos = pathStr.find("hidraw");
            
            if (hidrawPos != std::string::npos) {
                std::string bName = pathStr.substr(hidrawPos); // Defined here
                std::string classPath = "/sys/class/hidraw/" + bName;
                char realPath[PATH_MAX];
                
                if (realpath(classPath.c_str(), realPath)) {
                    std::string fullPath = realPath;
                    size_t usbPos = fullPath.find("/usb");
                    if (usbPos != std::string::npos) {
                        size_t colonPos = fullPath.find(':', usbPos + 4);
                        if (colonPos != std::string::npos) {
                            state.path = fullPath.substr(usbPos + 5, colonPos - usbPos - 5);
                        } else { state.path = bName; }
                    } else { state.path = bName; }
                } else { state.path = bName; }
            } else { state.path = pathStr; }
        } else { state.path = "Unknown"; }

        SDL_SetGamepadSensorEnabled(gamepad, SDL_SENSOR_ACCEL, true);
        SDL_SetGamepadSensorEnabled(gamepad, SDL_SENSOR_GYRO, true);
        
        state.connected = true;
        std::cout << "[INPUT] Linked: " << state.name << " | Path: " << state.path << std::endl;
    } else {
        state.connected = false;
    }
}

InputReader::~InputReader() {
    if (gamepad) SDL_CloseGamepad(gamepad);
}

void InputReader::update() {
    if (!gamepad || !state.connected) return;

    // 1. Scrape Axes (Standardized Mapping)
    // SDL_GAMEPAD_AXIS_COUNT ensures we get sticks and triggers
    for (int i = 0; i < SDL_GAMEPAD_AXIS_COUNT; i++) {
        state.raw.axes[i] = SDL_GetGamepadAxis(gamepad, (SDL_GamepadAxis)i);
        state.axes[i] = state.raw.axes[i];
    }

    // 2. Scrape Buttons (Includes D-Pad & Touchpad)
    // D-Pad will be 11, 12, 13, 14. Touchpad will be 17.
    for (int i = 0; i < SDL_GAMEPAD_BUTTON_COUNT; i++) {
        state.raw.buttons[i] = SDL_GetGamepadButton(gamepad, (SDL_GamepadButton)i);
        state.buttons[i] = state.raw.buttons[i];
    }

    // 3. Motion Data (Gyro/Accel)
    float gyro_data[3] = {0}, accel_data[3] = {0};
    
    if (SDL_GetGamepadSensorData(gamepad, SDL_SENSOR_GYRO, gyro_data, 3)) {
        const float radToDeg = 180.0f / (float)M_PI;
        state.raw.motion[0] = gyro_data[0] * radToDeg; 
        state.raw.motion[1] = gyro_data[1] * radToDeg; 
        state.raw.motion[2] = gyro_data[2] * radToDeg; 
    }

    if (SDL_GetGamepadSensorData(gamepad, SDL_SENSOR_ACCEL, accel_data, 3)) {
        const float invG = 1.0f / 9.80665f;
        state.raw.motion[3] = accel_data[0] * invG;
        state.raw.motion[4] = accel_data[1] * invG;
        state.raw.motion[5] = accel_data[2] * invG;
    }

    for (int i = 0; i < 3; i++) {
        state.gyro[i] = state.raw.motion[i];
        state.accel[i] = state.raw.motion[i + 3];
    }

    state.raw.timestamp = SDL_GetTicksNS();
}