#include "logic.h"
#include <cstdio>

Logic::Logic() : frame_count(0) {
    printf("\033[2J\033[H\033[?25l");
}

Logic::~Logic() { 
    printf("\033[?25h\n"); 
}

bool Logic::initialize() {
    return inputManager.initialize();
}

void Logic::run_iteration() {
    inputManager.update();
    RawData data = inputManager.getCombined();

    if (frame_count % 50 == 0) { 
        printf("\033[H"); 
        printf("╔═══════════════ xACE ABSOLUTE DYNAMIC ENGINE ══════════════╗\n");
        if (data.connected) {
            printf("║ STICKS │ LX:%6d LY:%6d │ RX:%6d RY:%6d ║\n", 
                   data.axes[0], data.axes[1], data.axes[2], data.axes[3]);
            
            printf("╠══════════════════════ BUTTON MAPPING ═════════════════════╣\n");
            printf("║ FACE:  A:%s B:%s X:%s Y:%s │ DPAD: %s %s %s %s ║\n",
                data.buttons[0]?"█":"·", data.buttons[1]?"█":"·", data.buttons[2]?"█":"·", data.buttons[3]?"█":"·",
                data.buttons[11]?"↑":"·", data.buttons[12]?"↓":"·", data.buttons[13]?"←":"·", data.buttons[14]?"→":"·");
            
            printf("║ SYSTEM: PS:%s  TPAD-CLK:%s  SHARE:%s  OPT:%s         ║\n",
                data.buttons[5]?"█":"·", data.buttons[15]?"█":"·", data.buttons[4]?"█":"·", data.buttons[6]?"█":"·");

            printf("╠═══════════════════════ SENSOR DATA ═══════════════════════╣\n");
            printf("║ GYRO   │ P:%7.2f  Y:%7.2f  R:%7.2f         ║\n", 
                   data.gyro[0], data.gyro[1], data.gyro[2]);
            printf("║ TOUCH  │ X:%7.2f  Y:%7.2f  DOWN:%-3s            ║\n", 
                   data.touch[0].x, data.touch[0].y, data.touch[0].down ? "YES" : "NO ");

            printf("╠══════════════════════ RAW SCANNER ════════════════════════╣\n");
            printf("║ BTNS 0-7  : ");
            for(int i=0; i<8; i++) printf("%d", data.buttons[i]);
            printf("  BTNS 8-15 : ");
            for(int i=8; i<16; i++) printf("%d", data.buttons[i]);
            printf(" ║\n");
        } else {
            printf("║          LISTENING FOR ANY USB/BT DEVICE...               ║\n");
            for(int i=0; i<8; i++) printf("║                                                           ║\n");
        }
        printf("╚═══════════════════════════════════════════════════════════╝\n");
    }
    frame_count++;
}
