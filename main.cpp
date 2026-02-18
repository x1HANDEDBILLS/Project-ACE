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

#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QThread>
#include <QSurfaceFormat>
#include <QDebug>

// 1. Thread-safe Shutdown Flag
std::atomic<bool> keep_running(true);

// Clean Exit Handler
void signalHandler(int signum) {
    keep_running = false;
}

void apply_process_wide_realtime_settings() {
    std::cout << "[ACE] Initializing Process-Wide Real-Time Environment..." << std::endl;

    if (mlockall(MCL_CURRENT | MCL_FUTURE) == -1) {
        perror("[!] Warning: mlockall failed. Run with sudo");
    }

    // Note: Priority and affinity will be set per thread
}

void set_thread_affinity_and_priority(pthread_t thread, int core, int priority, int policy = SCHED_FIFO) {
    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);
    CPU_SET(core, &cpuset);
    if (pthread_setaffinity_np(thread, sizeof(cpu_set_t), &cpuset) != 0) {
        perror("[!] Warning: Could not set thread affinity");
    }

    struct sched_param param;
    param.sched_priority = priority;
    if (pthread_setschedparam(thread, policy, &param) == -1) {
        perror("[!] Warning: Could not set thread scheduler");
    }
}

class LogicThread : public QThread {
public:
    LogicThread() {}

    ~LogicThread() {}

protected:
    void run() override {
        // Set affinity to core 3 and high priority
        set_thread_affinity_and_priority(pthread_self(), 3, 80, SCHED_FIFO);

        // Prefault stack for this thread
        const size_t STACK_SIZE = 4 * 1024 * 1024;
        unsigned char dummy[STACK_SIZE];
        std::memset(dummy, 0, STACK_SIZE);
        (void)dummy[0];

        Logic engine;
        if (engine.initialize()) {
            std::cout << "[ACE] Engine Active (Core 3 | 1000Hz Target)" << std::endl;
            while (keep_running && !isInterruptionRequested()) {
                engine.run_iteration();
            }
        }
        std::cout << "\n[ACE] Shutting down" << std::endl;
    }
};

int main(int argc, char* argv[]) {
    std::ios_base::sync_with_stdio(false);

    struct sigaction sa;
    sa.sa_handler = signalHandler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGINT, &sa, NULL);
    sigaction(SIGTERM, &sa, NULL);

    apply_process_wide_realtime_settings();

    // Set main thread (GUI) affinity to core 2 and normal priority
    set_thread_affinity_and_priority(pthread_self(), 2, 0, SCHED_OTHER);
    setpriority(PRIO_PROCESS, 0, 0);  // Normal nice value

    // Set up surface format for vsync (typically 60Hz)
    QSurfaceFormat format;
    format.setSwapInterval(1);  // Enable vsync for 60Hz refresh
    QSurfaceFormat::setDefaultFormat(format);

    QGuiApplication app(argc, argv);

    // Register Logic class using the CMake module name (AceModule)
    qmlRegisterType<Logic>("AceModule", 1, 0, "Logic");

    QQmlApplicationEngine engine;
    const QUrl url(QStringLiteral("qrc:/main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
                         if (!obj && url == objUrl) {
                             qDebug() << "Failed to load QML object from" << url;
                             QCoreApplication::exit(-1);
                         }
                     }, Qt::QueuedConnection);
    engine.load(url);

    if (engine.rootObjects().isEmpty()) {
        qDebug() << "No root objects loaded from QML";
        return -1;
    }

    LogicThread thread;
    thread.start();
    thread.setPriority(QThread::TimeCriticalPriority);

    int ret = app.exec();

    keep_running = false;
    thread.requestInterruption();
    thread.wait();

    return ret;
}