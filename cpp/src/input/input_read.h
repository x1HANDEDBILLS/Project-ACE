#pragma once
#include <string>
#include <cstdint>
#include <SDL3/SDL.h>

struct DeviceState {
    std::string name = "Unknown";
    std::string guid = "0000000000000000"; 
    int16_t axes[6] = {0};
    bool buttons[15] = {false};
    float gyro[3] = {0.0f, 0.0f, 0.0f};
    float accel[3] = {0.0f, 0.0f, 0.0f};
    bool connected = false;
};

class InputReader {
public:
    InputReader(SDL_JoystickID id);
    ~InputReader();
    void update();
    const DeviceState& getState() const { return state; }
    SDL_JoystickID getInstanceId() const { return instance_id; }

private:
    SDL_Gamepad* gamepad;
    SDL_JoystickID instance_id;
    DeviceState state;
};