// Location: include/input/input_read.h
#ifndef INPUT_READ_H
#define INPUT_READ_H
#include "input/device_state.h"
#include <SDL3/SDL.h>
class InputReader {
public:
    InputReader(SDL_JoystickID id);
    ~InputReader();
    void update();
    DeviceState getState() const { return state; }
    // FIX 1: Added public getter so the manager can track disconnects
    SDL_JoystickID getInstanceID() const { return instance_id; }

private:
    SDL_Gamepad *gamepad = nullptr;
    SDL_JoystickID instance_id;
    DeviceState state;
    // Sensor Fusion & Adaptive Smoothing State
    float filtered_accel[3] = {0.0f, 0.0f, 0.0f};
    float accum_roll = 0.0f;
    float accum_pitch = 0.0f;
    float smooth_out_roll = 0.0f;
    float smooth_out_pitch = 0.0f;
};
#endif
