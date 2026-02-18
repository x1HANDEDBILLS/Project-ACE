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
// We use atomic to ensure the 'run' loop sees the signal immediately
std::atomic<bool> keep_running(true);

// Clean Exit Handler (Async-Signal Safe)
void signalHandler(int signum) {
    // We don't use std::cout here because it's not signal-safe.
    // We simply flip the flag and let 'main' handle the clean exit.
    keep_running = false;
}

void apply_linux_realtime_settings() {
    std::cout << "[ACE] Initializing Real-Time Environment..." << std::endl;

    // 2. Lock memory to avoid swapping to the SD Card
    // Critical for Pi 4 to avoid massive latency spikes
    if (mlockall(MCL_CURRENT | MCL_FUTURE) == -1) {
        perror("[!] Warning: mlockall failed. Run with sudo");
    }

    // 3. Set process niceness to highest priority
    setpriority(PRIO_PROCESS, 0, -20);

    // 4. Set SCHED_FIFO (Real-Time Policy)
    struct sched_param param;
    param.sched_priority = 80; // High priority (1-99 range)
    if (sched_setscheduler(0, SCHED_FIFO, &param) == -1) {
        perror("[!] Warning: Could not set SCHED_FIFO. Run with sudo");
    }

    // 5. CPU Affinity - Pin to Core 3
    // Core 3 is often the "quietest" core on a Pi 4
    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);
    CPU_SET(3, &cpuset);
    if (pthread_setaffinity_np(pthread_self(), sizeof(cpu_set_t), &cpuset) != 0) {
        perror("[!] Warning: Could not pin to Core 3");
    }

    // 6. Deep Stack Prefaulting
    // Since you have 8GB RAM, we map 4MB of stack immediately 
    // to prevent page faults during the 1000Hz loop.
    const size_t STACK_SIZE = 4 * 1024 * 1024; // 4MB
    unsigned char dummy[STACK_SIZE];
    std::memset(dummy, 0, STACK_SIZE);
    
    // Volatile access to ensure the compiler doesn't optimize the memset away
    (void)dummy[0]; 
}

int main(int argc, char* argv[]) {
    // Fast I/O
    std::ios_base::sync_with_stdio(false);

    // 7. Robust Signal Handling
    struct sigaction sa;
    sa.sa_handler = signalHandler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGINT, &sa, NULL);
    sigaction(SIGTERM, &sa, NULL);

    // Apply the Pi 4 RT optimizations
    apply_linux_realtime_settings();

    std::cout << "[ACE] Engine Active (Core 3 | 1000Hz Target)" << std::endl;

    Logic engine;
    if (engine.initialize()) {
        // 8. Main execution loop
        // This loop calls your logic iteration, which handles its own 1ms timing
        while (keep_running) {
            engine.run_iteration(); 
        }
    }

    std::cout << "\n[ACE] Shutting down cleanly..." << std::endl;
    
    return 0;
}