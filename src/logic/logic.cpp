#include "src/logic/logic.h"
#include <iostream>
#include <time.h>
#include <cstdio>

Logic::Logic() : frame_count(0) {
    // Clear screen and hide cursor
    printf("\033[2J\033[H\033[?25l"); 
}

Logic::~Logic() {
    printf("\033[?25h\n");
}

bool Logic::initialize() {
    return reader.initialize();
}

void Logic::run_iteration() {
    struct timespec next_beat;
    static struct timespec start_time = {0, 0};
    if (start_time.tv_sec == 0) clock_gettime(CLOCK_MONOTONIC, &start_time);

    RawData raw = reader.fetch_new_data();

    // 30Hz Display update is enough for human eyes, while engine stays 1000Hz
    if (frame_count % 33 == 0) {
        printf("\033[H"); // Move cursor to top-left
        printf("====================================================\n");
        printf("   xACE ALL-INPUT TESTER (Static Persistence)       \n");
        printf("====================================================\n");
        printf(" NODES: %-2zu | MASK: %-4u | FRAME: %-8lu\n", 
                reader.get_open_fd_count(), raw.device_mask, frame_count);
        printf("----------------------------------------------------\n");

        printf(" [ANALOG SLOTS]\n");
        int count = 0;
        for (int i = 0; i < 64; i++) {
            // Only show slots that have actually been touched
            if (raw.values[i] != 0) {
                printf(" %2d: %-8d ", i, raw.values[i]);
                if (++count % 4 == 0) printf("\n");
            }
        }
        // Clean up the line if we didn't end on a newline
        if (count % 4 != 0) printf("\n");

        printf("\n [BUTTONS ACTIVE]\n ");
        bool any_btn = false;
        for (int i = 64; i < 128; i++) {
            if (raw.values[i] > 0) {
                printf("[%d] ", i);
                any_btn = true;
            }
        }
        if(!any_btn) printf("None                ");
        
        printf("\n\033[K"); // Clear to end of line
        printf("====================================================\n");
        fflush(stdout);
    }

    frame_count++;
    next_beat = start_time;
    next_beat.tv_nsec += frame_count * 1000000;
    while (next_beat.tv_nsec >= 1000000000) {
        next_beat.tv_nsec -= 1000000000;
        next_beat.tv_sec++;
    }
    clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &next_beat, NULL);
}