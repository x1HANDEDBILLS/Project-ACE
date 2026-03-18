// Location: include/vehicles/plane/input_tune_plane.h
#ifndef INPUT_TUNE_PLANE_H
#define INPUT_TUNE_PLANE_H
#include "input/device_state.h"
#include <cstdint>
class InputTunePlane {
public:
    InputTunePlane();
    void process_all_slots(); // The declaration the compiler was missing
    void process_to_buffer(int slot, const DeviceState &raw);
    int16_t apply_deadzone_smooth(int16_t raw, float dz_percent);
    int16_t apply_expo_raw(int16_t raw, float expo_percent);
};
#endif
