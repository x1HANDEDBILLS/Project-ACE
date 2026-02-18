#ifndef LOGIC_H
#define LOGIC_H

#include "../input/input_manage.h"
#include <cstdint>

class Logic {
public:
    Logic();
    ~Logic();

    bool initialize();
    void run_iteration();

private:
    InputManager inputManager;   // now supports up to 3 controllers combined
    uint64_t frame_count;
};

#endif
