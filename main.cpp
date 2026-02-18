#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <iostream>
#include <thread>
#include <sched.h>
#include <sys/mman.h>
#include <signal.h>
#include <unistd.h>
#include <pthread.h>

#include "src/logic/logic.h"

// Clean Exit Handler
void signalHandler(int signum) {
    std::cout << "\n[ACE] Shutting down cleanly..." << std::endl;
    QGuiApplication::exit(signum);
}

/**
 * CORE OPTIMIZER
 * Prevents SD card swapping and locks memory for real-time stability.
 */
void apply_global_optimizations() {
    if (mlockall(MCL_CURRENT | MCL_FUTURE) == -1) {
        std::cerr << "[!] Warning: mlockall failed. Run with sudo!" << std::endl;
    }
}

int main(int argc, char *argv[]) {
    // Register Ctrl+C
    signal(SIGINT, signalHandler);

    // Fast I/O
    std::ios_base::sync_with_stdio(false);

    // Initialize Qt App
    QGuiApplication app(argc, argv);

    // Apply Real-Time Memory Optimizations
    apply_global_optimizations();

    // --- GUI THREAD PINNING (CORE 2) ---
    cpu_set_t gui_cpuset;
    CPU_ZERO(&gui_cpuset);
    CPU_SET(2, &gui_cpuset);
    pthread_setaffinity_np(pthread_self(), sizeof(cpu_set_t), &gui_cpuset);

    // --- HEAP ALLOCATION (SEGFAULT FIX) ---
    // We use a pointer so the object never dies while threads are accessing it.
    Logic* engine = new Logic();

    // --- LOGIC THREAD PINNING (CORE 3) ---
    std::thread logicThread([engine]() {
        // Pin this worker thread to isolated Core 3
        cpu_set_t logic_cpuset;
        CPU_ZERO(&logic_cpuset);
        CPU_SET(3, &logic_cpuset);
        pthread_setaffinity_np(pthread_self(), sizeof(cpu_set_t), &logic_cpuset);

        // Set Real-Time Priority (SCHED_FIFO 99)
        struct sched_param param;
        param.sched_priority = 99;
        if (sched_setscheduler(0, SCHED_FIFO, &param) == -1) {
            std::cerr << "[!] Warning: SCHED_FIFO failed. Run with sudo!" << std::endl;
        }

        // Enter the high-precision loop
        if (engine->initialize()) {
            engine->run(); 
        }
    });
    logicThread.detach(); 

    // --- START QT QUICK GUI ---
    QQmlApplicationEngine qmlEngine;

    // THE BRIDGE: Expose the C++ pointer to QML
    qmlEngine.rootContext()->setContextProperty("aceEngine", engine);

    // Load GUI from Resource
    const QUrl url(QStringLiteral("qrc:/main.qml"));

    // Error checking for QML loading
    QObject::connect(&qmlEngine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl) {
            std::cerr << "[!] Critical: QML Object could not be created!" << std::endl;
            QCoreApplication::exit(-1);
        }
    }, Qt::QueuedConnection);

    qmlEngine.load(url);

    std::cout << "[ACE] GUI on Core 2 | Logic on Core 3 | System Ready" << std::endl;

    return app.exec();
}