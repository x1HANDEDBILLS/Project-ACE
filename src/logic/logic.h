#ifndef LOGIC_H
#define LOGIC_H

#include "src/input/input_read.h"
#include <cstdint>

class Logic {
public:
    Logic();
    ~Logic();
    bool initialize();
    void run_iteration();

private:
    InputReader reader;
    uint64_t frame_count;
};

#endif