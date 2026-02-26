#include "input_manage.h"
#include <SDL3/SDL.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <nlohmann/json.hpp>
#include <cmath>

using json = nlohmann::json;

bool InputManager::initialize() {
    // Initialize SDL3 Gamepad, Sensor, and Joystick subsystems
    if (!SDL_Init(SDL_INIT_GAMEPAD | SDL_INIT_SENSOR | SDL_INIT_JOYSTICK)) {
        return false;
    }
    scoutDevices();
    return true;
}

void InputManager::handleCommand(const std::string& cmd) {
    // Protocol: "SET_SLOT|Slot#|ProfileName.json"
    std::stringstream ss(cmd);
    std::string action, slotStr, profileName;

    if (std::getline(ss, action, '|') && action == "SET_SLOT") {
        if (std::getline(ss, slotStr, '|') && std::getline(ss, profileName, '|')) {
            try {
                int slot = std::stoi(slotStr);
                loadProfile(slot, profileName);
            } catch (...) {
                std::cerr << "[ACE] Command Error: Invalid Slot/Format" << std::endl;
            }
        }
    }
}

void InputManager::loadProfile(int slot, const std::string& profileName) {
    if (slot < 1 || slot > 4) return;

    std::string path = "profiles/" + profileName;
    std::ifstream file(path);

    if (!file.is_open()) {
        std::cerr << "[ACE] Profile Not Found: " << path << std::endl;
        return;
    }

    try {
        json j;
        file >> j;

        ControlProfile& p = activeProfiles[slot];
        p.profileName = profileName;

        for (int i = 0; i < 12; i++) {
            p.axisMap[i] = j["axis_map"][i];
            p.inverted[i] = j["inversions"][i];
        }

        for (int i = 0; i < 32; i++) {
            p.buttonMap[i] = j["button_map"][i];
        }

        for (int i = 0; i < 6; i++) {
            p.motionMap[i] = j["motion_map"][i];
            p.motionInverted[i] = j["motion_inversions"][i];
        }

        std::cout << "[ACE] Slot " << slot << " configured with: " << profileName << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "[ACE] JSON Error in " << profileName << ": " << e.what() << std::endl;
    }
}

void InputManager::scoutDevices() {
    int count;
    SDL_JoystickID* joysticks = SDL_GetGamepads(&count);
    if (joysticks) {
        for (int i = 0; i < count; i++) {
            auto reader = std::make_unique<InputReader>(joysticks[i]);
            int slot = getSlotFromPath(reader->getState().path);
            
            if (slot != -1) {
                slots[slot] = std::move(reader);
            } else {
                unmapped_devices.push_back(std::move(reader));
            }
        }
        SDL_free(joysticks);
    }
}

int InputManager::getSlotFromPath(const std::string& path) {
    if (path.empty()) return 0;

    size_t lastDot = path.find_last_of('.');
    if (lastDot != std::string::npos && lastDot + 1 < path.length()) {
        try {
            std::string portStr = path.substr(lastDot + 1);
            int port = std::stoi(portStr);
            if (port >= 1 && port <= 4) return port;
        } catch (...) {}
    }

    if (path.find("1-1") != std::string::npos) return 1;
    if (path.find("1-2") != std::string::npos) return 2;
    return 0; 
}

void InputManager::update() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_EVENT_GAMEPAD_ADDED) {
            auto reader = std::make_unique<InputReader>(event.gdevice.which);
            int slot = getSlotFromPath(reader->getState().path);
            if (slot != -1) {
                slots[slot] = std::move(reader);
            } else {
                unmapped_devices.push_back(std::move(reader));
            }
        } else if (event.type == SDL_EVENT_GAMEPAD_REMOVED) {
            for (auto it = slots.begin(); it != slots.end(); ++it) {
                if (it->second->getInstanceId() == event.gdevice.which) {
                    slots.erase(it);
                    break;
                }
            }
        }
    }

    for (auto& [slotID, device] : slots) {
        device->update(); 
        
        DeviceState& state = const_cast<DeviceState&>(device->getState());
        ControlProfile& p = activeProfiles[slotID];

        // 1. AXIS ALIGNMENT (16-BIT)
        for (int i = 0; i < 12; i++) {
            int16_t rawVal = state.raw.axes[p.axisMap[i]];
            if (p.inverted[i]) {
                state.axes[i] = (rawVal == -32768) ? 32767 : -rawVal;
            } else {
                state.axes[i] = rawVal;
            }
        }

        // 2. BUTTON PASS-THROUGH
        for (int i = 0; i < 32; i++) {
            state.buttons[i] = state.raw.buttons[p.buttonMap[i]];
        }

        // 3. ACCEL SCALING (9.81 m/s^2 -> 32765.0f)
        for (int i = 0; i < 3; i++) {
            float rawF = state.raw.motion[p.motionMap[i]]; 
            float val = p.motionInverted[i] ? -rawF : rawF;
            state.accel[i] = val * 3340.0f; 
        }

        // 4. GYRO SCALING (Rad/s -> High Res Float)
        for (int i = 0; i < 3; i++) {
            float rawF = state.raw.motion[p.motionMap[i + 3]]; 
            float val = p.motionInverted[i + 3] ? -rawF : rawF;
            state.gyro[i] = val * 10000.0f;
        }
    }
}

DeviceState InputManager::getSlotState(int slot) const {
    auto it = slots.find(slot);
    if (it != slots.end()) return it->second->getState();
    return DeviceState();
}