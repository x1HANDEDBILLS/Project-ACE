#pragma once
#include <SDL3/SDL.h>
#include <string>
#include <vector>
#include <cstdint>

/**
 * 1. The "Raw Scrape" structure
 * This holds the direct output from SDL before any normalization.
 */
struct RawPortData {
    // UPDATED: Changed from 'int' to 'int16_t' to match SDL3 Sint16 native output
    int16_t axes[12] = {0};         
    bool buttons[32] = {false};     
    
    // 0-2: Gyro (P,R,Y), 3-5: Accel (X,Y,Z) 
    // We keep this order consistent for the InputManager math.
    float motion[6] = {0.0f};       
    uint64_t timestamp = 0;
};

/**
 * 2. The Final State used by Logic/Dashboard
 * This is the structure your Python code and Dashboard will actually read.
 */
struct DeviceState {
    bool connected = false;
    std::string name = "Unknown";
    std::string path = "";          // Resolved USB Path (e.g., "1-1.2")
    std::string guid = "";

    // Processed values locked to 16-bit for the Python UI Bars
    int16_t axes[12] = {0};
    bool buttons[32] = {false};
    
    // Scaled values (High-Fidelity PlayStation-style math)
    float accel[3] = {0.0f};
    float gyro[3] = {0.0f};

    // The raw data container (The "Original" data is kept here)
    RawPortData raw;
};

/**
 * InputReader Class
 * Handles the low-level SDL3 hardware interface.
 */
class InputReader {
public:
    InputReader(SDL_JoystickID id);
    ~InputReader();

    // The heart of the hardware scrape
    void update();
    
    const DeviceState& getState() const { return state; }
    SDL_JoystickID getInstanceId() const { return instance_id; }

private:
    SDL_Gamepad* gamepad = nullptr;
    SDL_JoystickID instance_id;
    DeviceState state;
};