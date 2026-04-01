// Location: src/main.cpp
#include "common_constants.h"
#include "input/input_manage.h"
#include "logic/logic.h"
#include "profiles/profile_manager.h"
#include "protocols/nomad_daemon_link.h" // New: Hardware Link
#include "theme/theme.h"
#include "windows/mainwindow.h"
#include <QApplication>
#include <QSocketNotifier>
#include <atomic>
#include <iostream>
#include <pthread.h>
#include <sched.h>
#include <signal.h>
#include <sys/mman.h>
#include <sys/socket.h>
#include <time.h>
#include <unistd.h>
std::atomic<bool> keep_running(true);
int sig_pipe[2];
void signalHandler(int signum) {
    char a = 1;
    if (write(sig_pipe[1], &a, 1) == -1) {}
}
void *logic_thread_func(void *arg) {
    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);
    CPU_SET(ISOLATED_CORE, &cpuset);
    pthread_setaffinity_np(pthread_self(), sizeof(cpu_set_t), &cpuset);
    struct sched_param param;
    param.sched_priority = RT_PRIORITY;
    pthread_setschedparam(pthread_self(), SCHED_FIFO, &param);
    // FIX: Access the Singleton directly instead of passing a pointer
    auto &engine = Logic::instance();
    engine.initialize();
    struct timespec deadline;
    clock_gettime(CLOCK_MONOTONIC, &deadline);
    long interval_ns = 1000000000 / LOGIC_HZ;
    while (keep_running) {
        InputManager::instance().update();
        engine.run_iteration();
        deadline.tv_nsec += interval_ns;
        if (deadline.tv_nsec >= 1000000000) {
            deadline.tv_sec++;
            deadline.tv_nsec -= 1000000000;
        }
        clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &deadline, NULL);
    }
    return nullptr;
}
int main(int argc, char *argv[]) {
    if (socketpair(AF_UNIX, SOCK_STREAM, 0, sig_pipe)) return 1;
    signal(SIGINT, signalHandler);
    signal(SIGTERM, signalHandler);
    mlockall(MCL_CURRENT | MCL_FUTURE);
    QApplication app(argc, argv);
    ThemeManager::instance();
    InputManager::instance().initialize();
    // PERSISTENCE FIX: Load before UI
    ProfileManager::instance().boot_load();
    // NEW: Start the integrated Hardware Driver (Nomad Daemon)
    NomadHardware::start();
    QSocketNotifier sn(sig_pipe[0], QSocketNotifier::Read);
    QObject::connect(&sn, &QSocketNotifier::activated, [&]() {
        sn.setEnabled(false);
        char tmp;
        read(sig_pipe[0], &tmp, 1);
        keep_running = false;
        app.quit();
    });
    // FIX: Removed "Logic engine;" and passing nullptr as arg
    pthread_t logic_thread;
    pthread_create(&logic_thread, nullptr, logic_thread_func, nullptr);
    MainWindow w;
    w.show();
    int result = app.exec();
    // Cleanup
    keep_running = false;
    NomadHardware::stop(); // Stop hardware thread
    pthread_join(logic_thread, nullptr);
    munlockall();
    close(sig_pipe[0]);
    close(sig_pipe[1]);
    return result;
}
