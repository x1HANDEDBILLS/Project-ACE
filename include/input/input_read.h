#ifndef INPUT_READ_H
#define INPUT_READ_H

#include <SDL3/SDL.h>

#include "input/device_state.h"

class InputReader
{
   public:
    InputReader(SDL_JoystickID id);
    ~InputReader();
    void update();
    DeviceState getState() const
    {
        return state;
    }

   private:
    SDL_Gamepad* gamepad = nullptr;
    SDL_JoystickID instance_id;
    DeviceState state;

    // Persistent Tracking State for Leaky Integration
    float accum_roll = 0.0f;
    float accum_pitch = 0.0f;
};
#endif
