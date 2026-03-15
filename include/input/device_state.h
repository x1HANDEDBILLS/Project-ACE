#ifndef DEVICE_STATE_H
#define DEVICE_STATE_H

#include <SDL3/SDL.h>

#include <cstdint>
#include <string>

struct DeviceState
{
    bool connected = false;
    std::string name = "DISCONNECTED";
    std::string path = "";
    std::string guid = "";

    int16_t axes[16] = {0};
    bool buttons[32] = {false};

    float gyro[3] = {0.0f};
    float accel[3] = {0.0f};

    struct RawData
    {
        int16_t axes[SDL_GAMEPAD_AXIS_COUNT] = {0};
        bool buttons[SDL_GAMEPAD_BUTTON_COUNT] = {false};
        float motion[6] = {0.0f};
        uint64_t timestamp = 0;
    } raw;
};
#endif
