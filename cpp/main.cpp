#include <iostream>
#include <cstring>
#include <sched.h>
#include <sys/mman.h>
#include <sys/resource.h>
#include <pthread.h>
#include <signal.h>
#include <atomic>
#include <unistd.h>
#include <chrono> 
#include <thread> 
#include <sys/stat.h> // Required for chmod

#include "src/logic/logic.h"
#include "src/shared/socket.h" 

// 1. Thread-safe Shutdown Flag
std::atomic<bool> keep_running(true);

// Clean Exit Handler
void signalHandler(int signum) {
    keep_running = false;
}

// Memory locking (Prevents the OS from moving your code to slow disk swap)
void apply_process_wide_realtime_settings() {
    std::cout << "[ACE] Initializing Process-Wide Real-Time Environment..." << std::endl;
    if (mlockall(MCL_CURRENT | MCL_FUTURE) == -1) {
        perror("[!] Warning: mlockall failed. Run with sudo for best performance");
    }
}

// Core 3 Pinning & SCHED_FIFO Priority for low-latency jitter reduction
void set_thread_affinity_and_priority(int core, int priority, int policy = SCHED_FIFO) {
    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);
    CPU_SET(core, &cpuset);
    if (sched_setaffinity(0, sizeof(cpu_set_t), &cpuset) != 0) {
        perror("[!] Warning: Could not set thread affinity");
    }

    struct sched_param param;
    param.sched_priority = priority;
    if (sched_setscheduler(0, policy, &param) == -1) {
        perror("[!] Warning: Could not set thread scheduler. Check permissions");
    }
}

int main(int argc, char* argv[]) {
    // Fast I/O
    std::ios_base::sync_with_stdio(false);

    // Setup Signal Handling for Graceful Shutdown
    struct sigaction sa;
    sa.sa_handler = signalHandler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGINT, &sa, NULL);
    sigaction(SIGTERM, &sa, NULL);

    // Real-Time System Setup
    apply_process_wide_realtime_settings();
    set_thread_affinity_and_priority(3, 80, SCHED_FIFO);

    // Stack Prefaulting (4MB) to prevent page faults during execution
    const size_t STACK_SIZE = 4 * 1024 * 1024;
    unsigned char dummy[STACK_SIZE];
    std::memset(dummy, 0, STACK_SIZE);
    (void)dummy[0];

    // Initialize Core Components
    Logic engine;
    SocketServer ipc;

    if (!ipc.init()) {
        std::cerr << "[!] IPC Error: Socket failed to initialize" << std::endl;
        return -1;
    }

    // Secure the socket so only the current user can access it
    chmod("/tmp/xace.sock", 0600);

    if (!engine.initialize()) {
        std::cerr << "[!] Logic Error: Engine failed to initialize" << std::endl;
        return -1;
    }

    // Performance Tracking Counters
    uint64_t overruns = 0;
    uint64_t total_ticks = 0;

    std::cout << "[ACE] Engine Active (Core 3 | 2000Hz Brain Mode)" << std::endl;

    // --- 2000Hz TIMING SETUP (500us interval) ---
    using namespace std::chrono;
    auto next_tick = steady_clock::now();
    const microseconds interval(500); 

    // THE MAIN LOOP
    while (keep_running) {
        // Move target goalpost exactly 500 microseconds for the next cycle
        next_tick += interval;
        total_ticks++;

        // A. Poll Commands (Non-blocking)
        // This is where we catch "SET_SLOT|1|custom_profile.json" from Python
        std::string cmd = ipc.poll_commands();
        if (!cmd.empty()) {
            // Dispatch the command to the logic engine to reload profiles
            engine.handle_command(cmd);
        }

        // B. Run Logic Iteration
        // This updates the sticks, buttons, and sensors using active profiles
        engine.run_iteration();

        // C. Send Telemetry
        // Packages the remapped data as JSON and pushes it to the Python side
        ipc.send_telemetry(engine.get_telemetry_json(overruns));

        // D. PRECISION SLEEP SYNC with Overrun Detection
        auto now = steady_clock::now();
        if (now > next_tick) {
            // Deadline missed (logic took longer than 500us)
            overruns++;
            
            // Re-sync next_tick to 'now' to prevent the system from trying to 
            // "catch up" and spamming the Python GUI with rapid updates.
            next_tick = now + interval;
        } else {
            // We have time left; sleep until the exact deadline.
            std::this_thread::sleep_until(next_tick);
        }
    }

    // Graceful Cleanup when SIGINT/SIGTERM is received
    std::cout << "\n[ACE] Final Stats | Ticks: " << total_ticks << " | Overruns: " << overruns << std::endl;
    std::cout << "[ACE] Shutting down gracefully..." << std::endl;
    
    ipc.cleanup(); // Unlinks the socket file and closes connections
    
    return 0;
}