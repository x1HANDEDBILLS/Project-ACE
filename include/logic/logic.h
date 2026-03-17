// Location: include/logic/logic.h
#ifndef LOGIC_H
#define LOGIC_H
#include "vehicles/plane/input_tune_plane.h"
#include <atomic>
#include <cstdint>
class Logic {
public:
    void initialize();
    void run_iteration();
    static void unlock_backend() { backend_unlocked.store(true); }

private:
    InputTunePlane plane_processor;
    static std::atomic<bool> backend_unlocked;
};
#endif
