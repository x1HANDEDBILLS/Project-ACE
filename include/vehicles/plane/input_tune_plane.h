#ifndef INPUT_TUNE_PLANE_H
#define INPUT_TUNE_PLANE_H

#include <cstdint>

#include "input/device_state.h"

class InputTunePlane
{
   public:
    InputTunePlane();
    void update();
    // Added declaration to match the .cpp usage
    void process_to_buffer(int slot, const DeviceState& raw);

    int16_t apply_deadzone_smooth(int16_t raw, float dz_percent);
    int16_t apply_expo_raw(int16_t raw, float expo_percent);
};
#endif
