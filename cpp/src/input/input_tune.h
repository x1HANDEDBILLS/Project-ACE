#pragma once
#include "input_read.h"
#include <cstdint>

struct AxisTuning {
    int16_t deadzone;       // Inner deadzone (e.g., 1500)
    int16_t outer_deadzone; // Outer limit (e.g., 32000)
    float expo;             // 1.0 = linear, higher = softer center
    float sensitivity;      // Multiplier (Rate)
};

// Forward declaration of ControlProfile if not included
struct ControlProfile; 

class InputTune {
public:
    // This is the core math function we need to write next
    void applyTuning(DeviceState& state, const ControlProfile& profile);
    
    // The new command handler we discussed
    void setDeadzone(ControlProfile& profile, int axis_id, int inner, int outer);

private:
    int16_t processAxis(int16_t raw_val, const AxisTuning& cfg, bool inverted);
};