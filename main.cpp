#include <iostream>
#include <cstring>
#include <sched.h>
#include <sys/mman.h>
#include <sys/resource.h>
#include <pthread.h>
#include <signal.h>
#include <atomic>
#include <unistd.h>
#include "src/logic/logic.h"


// 1. Thread-safe Shutdown Flag
std::atomic<bool> keep_running(true);

// Clean Exit Handler
void signalHandler(int signum) {
    keep_running = false;
}

void apply_linux_realtime_settings() {
    std::cout << "[ACE] Initializing Real-Time Environment..." << std::endl;

    if (mlockall(MCL_CURRENT | MCL_FUTURE) == -1) {
        perror("[!] Warning: mlockall failed. Run with sudo");
    }

    setpriority(PRIO_PROCESS, 0, -20);

    struct sched_param param;
    param.sched_priority = 80;
    if (sched_setscheduler(0, SCHED_FIFO, &param) == -1) {
        perror("[!] Warning: Could not set SCHED_FIFO. Run with sudo");
    }

    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);
    CPU_SET(3, &cpuset);
    if (pthread_setaffinity_np(pthread_self(), sizeof(cpu_set_t), &cpuset) != 0) {
        perror("[!] Warning: Could not pin to Core 3");
    }

    const size_t STACK_SIZE = 4 * 1024 * 1024;
    unsigned char dummy[STACK_SIZE];
    std::memset(dummy, 0, STACK_SIZE);
    (void)dummy[0]; 
}

int main(int argc, char* argv[]) {
    std::ios_base::sync_with_stdio(false);

    struct sigaction sa;
    sa.sa_handler = signalHandler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGINT, &sa, NULL);
    sigaction(SIGTERM, &sa, NULL);

    apply_linux_realtime_settings();

    std::cout << "[ACE] Engine Active (Core 3 | 1000Hz Target)" << std::endl;

    Logic engine;
    if (engine.initialize()) {
        while (keep_running) {
            engine.run_iteration(); 
        }
    }

    std::cout << "\n[ACE] Shutting down" << std::endl;
    
    return 0;
}
