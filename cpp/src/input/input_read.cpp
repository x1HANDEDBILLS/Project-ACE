#include "input_read.h"
#include <iostream>
#include <unistd.h>
#include <limits.h>
#include <libgen.h>
#include <string>
#include <stdlib.h> // For realpath

InputReader::InputReader(SDL_JoystickID id) : instance_id(id) {
    gamepad = SDL_OpenGamepad(id);
    if (gamepad) {
        const char* gName = SDL_GetGamepadName(gamepad);
        state.name = gName ? gName : "Controller";

        // 1. Get the GUID for unique hardware identification
        SDL_Joystick* joy = SDL_GetGamepadJoystick(gamepad);
        if (joy) {
            SDL_GUID sdl_guid = SDL_GetJoystickGUID(joy);
            char guid_buffer[33];
            SDL_GUIDToString(sdl_guid, guid_buffer, sizeof(guid_buffer));
            state.guid = guid_buffer;
        }

        // 2. Resolve PHYSICAL USB Port (The "Slot Path")
        // This allows us to map Slot 1 to the physical top-left port on a hub.
        const char* devPath = SDL_GetGamepadPath(gamepad); 
        if (devPath && std::string(devPath).find("hidraw") != std::string::npos) {
            char realPath[PATH_MAX];
            std::string pathBuffer = devPath;
            size_t lastSlash = pathBuffer.find_last_of('/');
            std::string bName = (lastSlash != std::string::npos) ? pathBuffer.substr(lastSlash + 1) : pathBuffer;
            std::string classPath = "/sys/class/hidraw/" + bName;
            
            if (realpath(classPath.c_str(), realPath)) {
                std::string fullPath = realPath;
                size_t usbPos = fullPath.find("usb");
                if (usbPos != std::string::npos) {
                    size_t startSearch = fullPath.find('/', usbPos);
                    size_t endPos = fullPath.find(':', startSearch);
                    if (startSearch != std::string::npos && endPos != std::string::npos) {
                        state.path = fullPath.substr(startSearch + 1, endPos - startSearch - 1);
                    } else {
                        state.path = bName; 
                    }
                } else {
                    state.path = bName; 
                }
            }
        } else {
            state.path = devPath ? devPath : "Unknown";
        }

        // 3. Enable High-Speed IMU Sensors
        // Critical for DualShock/DualSense/Switch Motion
        SDL_SetGamepadSensorEnabled(gamepad, SDL_SENSOR_ACCEL, true);
        SDL_SetGamepadSensorEnabled(gamepad, SDL_SENSOR_GYRO, true);
        
        state.connected = true;
        std::cout << "[ACE] Hardware Found: " << state.name << " | Path: " << state.path << std::endl;
    } else {
        state.connected = false;
    }
}

InputReader::~InputReader() {
    if (gamepad) {
        SDL_CloseGamepad(gamepad);
    }
}

void InputReader::update() {
    if (!gamepad || !state.connected) return;

    // 4. Scrape Raw Data (16-bit Native)
    // SDL_GetGamepadAxis returns Sint16 (-32768 to 32767)
    for (int i = 0; i < 12; i++) {
        state.raw.axes[i] = SDL_GetGamepadAxis(gamepad, (SDL_GamepadAxis)i);
    }
    
    // Digital Buttons (bool state)
    for (int i = 0; i < 32; i++) {
        state.raw.buttons[i] = SDL_GetGamepadButton(gamepad, (SDL_GamepadButton)i);
    }

    // 5. Scrape IMU (Motion)
    // Gyro: Rad/s (indices 0, 1, 2)
    SDL_GetGamepadSensorData(gamepad, SDL_SENSOR_GYRO, &state.raw.motion[0], 3);
    // Accel: m/s^2 (indices 3, 4, 5)
    SDL_GetGamepadSensorData(gamepad, SDL_SENSOR_ACCEL, &state.raw.motion[3], 3);

    // 6. Mirror to Active State
    // These remain "Raw" until InputManager applies alignment/normalization math
    for (int i = 0; i < 12; i++) state.axes[i] = state.raw.axes[i];
    for (int i = 0; i < 32; i++) state.buttons[i] = state.raw.buttons[i];

    // Mirror Gyro (0-2)
    state.gyro[0] = state.raw.motion[0];
    state.gyro[1] = state.raw.motion[1];
    state.gyro[2] = state.raw.motion[2];

    // Mirror Accel (3-5)
    state.accel[0] = state.raw.motion[3];
    state.accel[1] = state.raw.motion[4];
    state.accel[2] = state.raw.motion[5];

    state.raw.timestamp = SDL_GetTicksNS();
}