#pragma once
#include <SDL3/SDL.h>

struct TouchData {
    float x = 0.0f;
    float y = 0.0f;
    bool down = false;
};

struct RawData {
    bool connected = false;
    int axes[16] = {0};
    bool buttons[32] = {false};
    float gyro[3] = {0.0f};
    TouchData touch[1];
    int battery_percent = 0;
};

class InputReader {
public:
    InputReader();
    ~InputReader();

    bool initialize();
    RawData fetch_new_data();

private:
    SDL_Gamepad* gamepad = nullptr;
    RawData state;
    Uint32 initTime = 0;
    int callCount = 0;          // ← Fixed: was missing

    void rescanForGamepad();
};
