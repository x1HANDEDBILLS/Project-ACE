#include "logic2.h"
#include <iostream>
#include <cstdio>

InputDashboard::InputDashboard() {}
InputDashboard::~InputDashboard() {}

void InputDashboard::refresh(const InputManager& input) {
    // Clear screen and home cursor
    std::printf("\033[H\033[2J"); 
    std::printf("====================================================\n");
    std::printf("      ACE ENGINE | 2000Hz REAL-TIME DASHBOARD       \n");
    std::printf("====================================================\n");

    bool any_connected = false;

    for (int i = 1; i <= 4; ++i) {
        DeviceState state = input.getSlotState(i);
        
        if (state.connected) {
            any_connected = true;
            std::printf("SLOT [%d] | %s\n", i, state.name.c_str());
            
            // 1. Show Primary (Remapped) Sticks
            std::printf("  STICKS (V): LX:%-6d LY:%-6d RX:%-6d RY:%-6d\n", 
                        state.axes[0], state.axes[1], state.axes[2], state.axes[3]);
            
            // 2. Show Gyro & Accel Readings
            std::printf("  GYRO  (R):  P:%-8.2f R:%-8.2f Y:%-8.2f\n", 
                        state.gyro[0], state.gyro[1], state.gyro[2]);
            std::printf("  ACCEL (R):  X:%-8.2f Y:%-8.2f Z:%-8.2f\n", 
                        state.accel[0], state.accel[1], state.accel[2]);

            // 3. Show Button Matrix (First 16 buttons)
            std::printf("  BTNS: ");
            for(int b=0; b<16; b++) {
                std::printf("%d ", state.buttons[b] ? 1 : 0);
            }
            std::printf("\n----------------------------------------------------\n");
        } else {
            std::printf("SLOT [%d]: -- EMPTY --\n", i);
            std::printf("----------------------------------------------------\n");
        }
    }

    if (!any_connected) {
        std::printf("\n      [!] WAITING FOR USB HID DEVICES...\n\n");
    }
    std::fflush(stdout);
}