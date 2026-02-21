#include "logic2.h"
#include <iostream>
#include <cstdio>

InputDashboard::InputDashboard() {}
InputDashboard::~InputDashboard() {}

void InputDashboard::refresh(const InputManager& input) {
    // Move cursor to 0,0 and clear screen properly to prevent "scrolling" spam
    std::printf("\033[H\033[2J"); 
    std::printf("==========================================\n");
    std::printf("        ACE ENGINE: FULL SENSOR MERGE     \n");
    std::printf("==========================================\n");

    for (size_t i = 0; i < input.getDeviceCount(); ++i) {
        DeviceState state = input.getDeviceState(i);
        if (state.connected) {
            std::printf("CONTROLLER [%zu]: %s\n", i, state.name.c_str());
            std::printf("  STICKS: LX:%-6d LY:%-6d RX:%-6d RY:%-6d\n", 
                        state.axes[0], state.axes[1], state.axes[2], state.axes[3]);
            
            // This is the data you wanted merged back in
            std::printf("  GYRO:   P:%-8.4f R:%-8.4f Y:%-8.4f\n", 
                        state.gyro[0], state.gyro[1], state.gyro[2]);
            
            std::printf("  ACCEL:  X:%-8.4f Y:%-8.4f Z:%-8.4f\n", 
                        state.accel[0], state.accel[1], state.accel[2]);

            std::printf("  BTNS:   A:%d B:%d X:%d Y:%d\n", 
                        state.buttons[0], state.buttons[1], state.buttons[2], state.buttons[3]);
            std::printf("------------------------------------------\n");
        }
    }

    if (input.getDeviceCount() == 0) {
        std::printf("\n      [!] WAITING FOR DEVICE...\n\n");
    }
    std::fflush(stdout);
}