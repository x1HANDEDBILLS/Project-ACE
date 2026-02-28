#include "input_tune.h"
#include "input_manage.h"
#include <cmath>
#include <algorithm>

/**
 * The Brain of the Pipeline: Shuffles, Flips, and Curves the data.
 */
void InputTune::applyTuning(DeviceState& state, const ControlProfile& profile) {
    
    // Temporary buffers for sticks
    int16_t processedAxes[MAX_AXES] = {0};
    bool processedButtons[MAX_BUTTONS] = {false};

    // 1. TUNE & MAP STICK AXES
    for (int i = 0; i < MAX_AXES; i++) {
        int targetIndex = profile.axisMap[i]; 
        
        int16_t tunedValue = processAxis(
            state.raw.axes[i], 
            profile.tuning[i], 
            profile.inverted[i]
        );

        if (targetIndex >= 0 && targetIndex < MAX_AXES) {
            processedAxes[targetIndex] = tunedValue;
        }
    }

    // 2. TUNE MOTION AXES (Gyro/Tilt)
    // We treat state.gyro[0,1,2] as 16-bit inputs.
    // Note: Since we scaled these to 90 degrees = 32767 in InputManager,
    // processAxis will now treat 90 degrees as "Full Stick Throw".
    for (int i = 0; i < 3; i++) {
        state.gyro[i] = processAxis(
            state.gyro[i],           // Source: Already scaled 16-bit value
            profile.motionTuning[i], // Using the new motion tuning array
            profile.motionInverted[i]
        );
    }

    // 3. MAP BUTTONS
    for (int i = 0; i < MAX_BUTTONS; i++) {
        int targetBtn = profile.buttonMap[i];
        if (targetBtn >= 0 && targetBtn < MAX_BUTTONS) {
            processedButtons[targetBtn] = state.raw.buttons[i];
        }
    }

    // 4. COMMIT TO THE STATE
    for (int i = 0; i < MAX_AXES; i++) {
        state.axes[i] = processedAxes[i];
    }
    for (int i = 0; i < MAX_BUTTONS; i++) {
        state.buttons[i] = processedButtons[i];
    }
}

/**
 * Deadzone Handler updated to support both Sticks and Motion
 */
void InputTune::setDeadzone(ControlProfile& profile, int axis_id, int inner, int outer) {
    // If axis_id is within stick range
    if (axis_id >= 0 && axis_id < MAX_AXES) {
        profile.tuning[axis_id].deadzone = static_cast<int16_t>(inner * 327.67f);
        profile.tuning[axis_id].outer_deadzone = static_cast<int16_t>(outer * 327.67f);
        
        if (profile.tuning[axis_id].outer_deadzone <= profile.tuning[axis_id].deadzone) {
            profile.tuning[axis_id].outer_deadzone = 32767;
        }
    }
    // If axis_id refers to Motion (you may need to define IDs for motion, e.g., 10, 11, 12)
    // For now, this logic covers the primary sticks.
}

/**
 * Core Math Engine: Signal Conditioning
 */
int16_t InputTune::processAxis(int16_t raw, const AxisTuning& tune, bool inverted) {
    int32_t absVal = std::abs(static_cast<int32_t>(raw));
    
    // 1. INNER DEADZONE (Vital for Gyro to stop "sensor crawl" or hand shake)
    if (absVal <= tune.deadzone) return 0;

    // 2. OUTER DEADZONE (Saturation / Capping)
    int32_t outer = (tune.outer_deadzone > tune.deadzone) ? tune.outer_deadzone : 32767;
    if (absVal >= outer) {
        int16_t maxVal = static_cast<int16_t>(32767.0f * tune.sensitivity);
        int16_t output = (raw < 0) ? -maxVal : maxVal;
        return inverted ? -output : output;
    }

    // 3. NORMALIZATION
    float normalized = static_cast<float>(absVal - tune.deadzone) / static_cast<float>(outer - tune.deadzone);
    
    // 4. RESPONSE CURVE (Expo)
    // Use expo > 1.0 for "soft center" feel on the plane
    float result = std::pow(normalized, tune.expo);
    
    // 5. SENSITIVITY
    result *= tune.sensitivity;
    
    // 6. FINAL CLAMP & SIGN RESTORE
    result = std::clamp(result, 0.0f, 1.0f);
    int16_t finalVal = static_cast<int16_t>(result * 32767.0f);
    int16_t finalOutput = (raw < 0) ? -finalVal : finalVal;

    return inverted ? -finalOutput : finalOutput;
}