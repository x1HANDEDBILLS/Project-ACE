#pragma once
#include "input_read.h"
#include "input_tune.h"
#include <map>
#include <memory>
#include <string>
#include <vector>
#include <cstdint>

struct ControlProfile {
    std::string profileName = "Default";
    
    int axisMap[MAX_AXES];
    int buttonMap[MAX_BUTTONS];
    int motionMap[6];   
    
    bool inverted[MAX_AXES] = {false};
    bool motionInverted[6] = {false};

    AxisTuning tuning[MAX_AXES]; 
    AxisTuning motionTuning[6]; // <--- ADD THIS: Tuning specifically for Gyro/Accel
    
    ControlProfile() {
        for (int i = 0; i < MAX_AXES; i++) {
            axisMap[i] = i;
            tuning[i] = {1500, 32767, 1.0f, 1.0f}; 
            inverted[i] = false;
        }
        for (int i = 0; i < MAX_BUTTONS; i++) {
            buttonMap[i] = i;
        }
        for (int i = 0; i < 6; i++) {
            motionMap[i] = i;
            motionInverted[i] = false;
            // Initialize motion tuning with a small inner deadzone (e.g. 500) 
            // to stop hand-shake jitter from moving the RC surfaces.
            motionTuning[i] = {500, 32767, 1.0f, 1.0f}; 
        }
    }
};

class InputManager {
public:
    bool initialize();
    void update();
    void scoutDevices();
    
    // Command & Control
    void setDeadzone(int slot, int axis_id, int inner, int outer);
    void handleCommand(const std::string& cmd);
    void loadProfile(int slot, const std::string& profileName);
    void assignProfileToSlot(int slot, const ControlProfile& profile);
    
    // IMU Zeroing/Calibration
    void resetOrientation(int slotID);
    
    // Data Output for Socket/Logic
    DeviceState getSlotState(int slot) const;

private:
    int getSlotFromPath(const std::string& path);

    // Physical device readers keyed by Slot ID (1-4)
    std::map<int, std::unique_ptr<InputReader>> slots;
    
    // Profiles for each port (1-4).
    ControlProfile activeProfiles[5]; 

    // --- IMU / Motion Processing State ---
    // Tracked per slot (1-4)
    float current_pitch[5];
    float current_roll[5];
    float pitch_offset[5];
    float roll_offset[5];
    uint64_t last_timestamp[5];

    // The math engine (Handles Deadzones, Curves, Expo)
    std::unique_ptr<InputTune> tuningEngine;
};