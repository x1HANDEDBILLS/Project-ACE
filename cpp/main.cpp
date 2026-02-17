// =====================================================
// FILE: /home/user/RC-Ground-Control/cpp/main.cpp
// =====================================================
#include "input_selection.h"
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <cstring>
#include <iostream>
#include <sched.h>
#include <signal.h>
#include <time.h>
#include <dirent.h>
#include <cstdint>

SharedTouchState* shared_gui_ptr = nullptr;
InputManager* manager_ptr = nullptr;

const char* SHM_NAME = "/gui_input_stream";

void cleanup(int sig) {
    std::cout << "\n--- RC GOVERNOR: SHUTTING DOWN ---\n";
    if (shared_gui_ptr) {
        munmap(shared_gui_ptr, sizeof(SharedTouchState));
        shm_unlink(SHM_NAME);
    }
    if (manager_ptr) delete manager_ptr;
    exit(0);
}

int main() {
    signal(SIGINT,  cleanup);
    signal(SIGTERM, cleanup);

    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);
    CPU_SET(3, &cpuset);
    sched_setaffinity(0, sizeof(cpu_set_t), &cpuset);

    struct sched_param param{};
    param.sched_priority = 80;
    sched_setscheduler(0, SCHED_FIFO, &param);

    mlockall(MCL_CURRENT | MCL_FUTURE);

    // SHARED MEMORY — WORLD READABLE
    int shm_fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
    ftruncate(shm_fd, sizeof(SharedTouchState));
    fchmod(shm_fd, 0666);                    // ← allows Python to read it
    shared_gui_ptr = (SharedTouchState*)mmap(nullptr, sizeof(SharedTouchState),
                                             PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    close(shm_fd);

    InputManager manager;
    manager_ptr = &manager;

    std::cout << "\n=== REGISTERING DEVICES ===\n";
    manager.add_device("/dev/input/event1", DeviceRole::SCREEN_TOUCH);   // ← ONLY BTT-HDMI7
    std::cout << "============================\n\n";

    std::cout << "--- RC GOVERNOR: ONLY BTT SCREEN ACTIVE ---\n\n";

    struct timespec next_time;
    clock_gettime(CLOCK_MONOTONIC, &next_time);
    uint32_t loop_counter = 0;

    while (true) {
        next_time.tv_nsec += 1'000'000;
        if (next_time.tv_nsec >= 1'000'000'000) {
            next_time.tv_sec++;
            next_time.tv_nsec -= 1'000'000'000;
        }

        manager.poll_all();

        SharedTouchState gui_data = manager.get_gui_state();
        if (gui_data.updated) {
            memcpy(shared_gui_ptr, &gui_data, sizeof(SharedTouchState));
        }

        if (++loop_counter % 1000 == 0) {
            RCState rc = manager.get_rc_state();
            std::cout << "RC → Throttle: " << rc.throttle
                      << "   Steering: " << rc.steering
                      << "   Aux: " << rc.auxiliary << "\r" << std::flush;
        }

        clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &next_time, nullptr);
    }

    return 0;
}