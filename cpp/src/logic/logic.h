#ifndef LOGIC_H
#define LOGIC_H

#include <vector>
#include <memory>
#include <string>
#include "../input/input_manage.h"

class Logic {
public:
    Logic();
    ~Logic();

    bool initialize();
    void run_iteration();
    
    // Helper to build the JSON string for the SocketServer in main.cpp
    std::string get_telemetry_json(uint64_t late_count);

private:
    InputManager input;
    uint64_t frame_count;
};

#endif