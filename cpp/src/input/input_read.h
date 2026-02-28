#pragma once
#include <SDL3/SDL.h>
#include <string>
#include <cstdint>
#include <cmath> // Added for M_PI support if needed elsewhere

// Reserved memory capacity
constexpr int MAX_AXES = 50;     
constexpr int MAX_BUTTONS = 100; 

struct RawPortData {
    int16_t axes[MAX_AXES] = {0};
    bool buttons[MAX_BUTTONS] = {false};
    float motion[6] = {0.0f};       // 0-2: Gyro (deg/s), 3-5: Accel (G)
    uint64_t timestamp = 0;
};

struct DeviceState {
    bool connected = false;
    std::string name = "Unknown";
    std::string path = "";          
    std::string guid = "";

    int16_t axes[MAX_AXES] = {0};
    bool buttons[MAX_BUTTONS] = {false};
    
    // Standardized Units for RC Control
    float accel[3] = {0.0f}; // Units: G (1.0 = Earth Gravity)
    float gyro[3] = {0.0f};  // Units: Degrees per Second (DPS)

    RawPortData raw;
};

class InputReader {
public:
    InputReader(SDL_JoystickID id);
    ~InputReader();
    void update();

    // Getters for the Socket/Manager
    const DeviceState& getState() const { return state; }
    SDL_JoystickID getInstanceId() const { return instance_id; }
    int getActiveAxes() const { return active_axes; }
    int getActiveButtons() const { return active_buttons; }

private:
    SDL_Gamepad* gamepad = nullptr;
    SDL_JoystickID instance_id;
    DeviceState state;
    int active_axes = 0;    // Hardware count clamped to MAX_AXES
    int active_buttons = 0; // Hardware count clamped to MAX_BUTTONS
};