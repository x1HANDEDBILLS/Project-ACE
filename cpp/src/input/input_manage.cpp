#include "input_manage.h"
#include <SDL3/SDL.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <nlohmann/json.hpp>
#include <cmath>
#include <algorithm>

using json = nlohmann::json;

// Per-slot smoothing and state (for up to 4 controllers)
static float current_pitch[5] = {0.0f};
static float current_roll[5]  = {0.0f};
static float pitch_offset[5]  = {0.0f};
static float roll_offset[5]   = {0.0f};
static uint64_t last_timestamp[5] = {0};

static float stg1_p[5] = {0.0f}, stg1_r[5] = {0.0f};
static float stg2_p[5] = {0.0f}, stg2_r[5] = {0.0f};

bool InputManager::initialize() {
    if (!SDL_Init(SDL_INIT_GAMEPAD | SDL_INIT_SENSOR | SDL_INIT_JOYSTICK)) {
        return false;
    }

    for (int i = 0; i < 5; i++) {
        current_pitch[i] = current_roll[i] = 0.0f;
        pitch_offset[i] = roll_offset[i] = 0.0f;
        last_timestamp[i] = 0;
        stg1_p[i] = stg1_r[i] = stg2_p[i] = stg2_r[i] = 0.0f;
    }

    tuningEngine = std::make_unique<InputTune>();
    scoutDevices();
    return true;
}

void InputManager::handleCommand(const std::string& cmd) {
    std::stringstream ss(cmd);
    std::string action;

    if (std::getline(ss, action, '|')) {
        if (action == "SET_SLOT") {
            std::string slotStr, profileName;
            if (std::getline(ss, slotStr, '|') && std::getline(ss, profileName, '|')) {
                try {
                    int slot = std::stoi(slotStr);
                    loadProfile(slot, profileName);
                } catch (...) {}
            }
        }
        else if (action == "CALIBRATE") {
            int slot;
            if (sscanf(cmd.c_str(), "CALIBRATE|%d", &slot) == 1) {
                resetOrientation(slot);
            }
        }
    }
}

void InputManager::loadProfile(int slot, const std::string& profileName) {
    if (slot < 1 || slot > 4) return;
    std::string path = "profiles/" + profileName;
    std::ifstream file(path);
    if (!file.is_open()) return;

    try {
        json j;
        file >> j;
        ControlProfile& p = activeProfiles[slot];

        if (j.contains("axis_map")) {
            auto amap = j["axis_map"].get<std::vector<int>>();
            for (int i = 0; i < MAX_AXES; i++) {
                p.axisMap[i] = (i < (int)amap.size()) ? amap[i] : i;
            }
        }
        std::cout << "[MANAGER] Loaded Profile: " << profileName << std::endl;
    } catch (...) {}
}

void InputManager::scoutDevices() {
    int count;
    SDL_JoystickID* joysticks = SDL_GetJoysticks(&count);
    if (joysticks) {
        for (int i = 0; i < count; i++) {
            bool alreadyMapped = false;
            for(auto const& [sID, dev] : slots) {
                if(dev->getInstanceId() == joysticks[i]) { alreadyMapped = true; break; }
            }
            if (alreadyMapped) continue;

            auto reader = std::make_unique<InputReader>(joysticks[i]);
            int targetSlot = getSlotFromPath(reader->getState().path);
            
            if (targetSlot == 0) {
                for (int s = 1; s <= 4; s++) {
                    if (slots.find(s) == slots.end()) { targetSlot = s; break; }
                }
            }

            if (targetSlot >= 1 && targetSlot <= 4) {
                slots[targetSlot] = std::move(reader);
            }
        }
        SDL_free(joysticks);
    }
}

int InputManager::getSlotFromPath(const std::string& path) {
    if (path.empty()) return 0;
    if (path.find(".1.1") != std::string::npos) return 1;
    return 0; 
}

void InputManager::resetOrientation(int slotID) {
    if (slotID < 1 || slotID > 4) return;
    pitch_offset[slotID] = current_pitch[slotID];
    roll_offset[slotID] = current_roll[slotID];
    std::cout << "[MANAGER] Calibrated Slot " << slotID << std::endl;
}

void InputManager::update() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_EVENT_GAMEPAD_ADDED) scoutDevices(); 
    }

    for (auto& [slotID, device] : slots) {
        if (!device) continue;
        
        device->update(); 
        DeviceState& state = const_cast<DeviceState&>(device->getState());
        
        uint64_t now = state.raw.timestamp;
        float dt = (last_timestamp[slotID] == 0) ? 0.004f : (float)(now - last_timestamp[slotID]) / 1e9f;
        last_timestamp[slotID] = now;

        // PS4 Gyro/Accel Mapping (SDL3 standard)
        float gx = state.raw.motion[0]; // Pitch rate
        float gy = state.raw.motion[1]; // Yaw rate
        float gz = state.raw.motion[2]; // Roll rate

        float ax = state.raw.motion[3]; // X (Roll axis)
        float ay = state.raw.motion[4]; // Y (Pitch axis)
        float az = state.raw.motion[5]; // Z (Vertical/Gravity)

        // Gravity-referenced angles (flat = 0, up positive, down negative)
        float raw_acc_p = std::atan2(-ay, std::sqrt(ax*ax + az*az)) * (180.0f / M_PI);
        float raw_acc_r = std::atan2(ax, az) * (180.0f / M_PI);

        // Double exponential smoothing
        stg1_p[slotID] = (raw_acc_p * 0.10f) + (stg1_p[slotID] * 0.90f);
        stg1_r[slotID] = (raw_acc_r * 0.10f) + (stg1_r[slotID] * 0.90f);
        stg2_p[slotID] = (stg1_p[slotID] * 0.10f) + (stg2_p[slotID] * 0.90f);
        stg2_r[slotID] = (stg1_r[slotID] * 0.10f) + (stg2_r[slotID] * 0.90f);

        // Magnitude gate
        float mag = std::sqrt(ax*ax + ay*ay + az*az);
        float trust = (std::abs(mag - 1.0f) < 0.15f) ? 0.04f : 0.0f;

        // Complementary filter
        current_pitch[slotID] = 0.96f * (current_pitch[slotID] + gx * dt) + (trust * stg2_p[slotID]);
        current_roll[slotID]  = 0.96f * (current_roll[slotID]  + gz * dt) + (trust * stg2_r[slotID]);

        // Calibrated output (subtract flat offset)
        float cp = current_pitch[slotID] - pitch_offset[slotID];
        float cr = current_roll[slotID]  - roll_offset[slotID];

        cp = std::clamp(cp, -90.0f, 90.0f);
        cr = std::clamp(cr, -90.0f, 90.0f);

        // Scale to full 16-bit range
        state.gyro[0] = static_cast<int16_t>((cp / 90.0f) * 32767.0f);
        state.gyro[1] = static_cast<int16_t>(gy);  // Raw yaw rate (unchanged)
        state.gyro[2] = static_cast<int16_t>((cr / 90.0f) * 32767.0f);

        for (int i = 0; i < 3; i++) state.accel[i] = state.raw.motion[i + 3]; 

        // Apply axis tuning (deadzone, sens, expo) - unchanged
        tuningEngine->applyTuning(state, activeProfiles[slotID]);
    }
}

DeviceState InputManager::getSlotState(int slot) const {
    auto it = slots.find(slot);
    return (it != slots.end() && it->second) ? it->second->getState() : DeviceState();
}