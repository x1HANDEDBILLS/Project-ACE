#ifndef LOGIC_H
#define LOGIC_H

#include <vector>
#include <memory>
#include <string>
#include <cstdint>
#include "../input/input_manage.h"

class Logic {
public:
    Logic();
    ~Logic();

    bool initialize();
    
    // The main 2000Hz execution step
    void run_iteration();
    
    // RIGGING: This allows main.cpp to pass Python socket commands down to InputManager
    void handle_command(const std::string& cmd);

    // Generates the JSON for the 2000Hz IPC pipe
    // Takes the 'overruns' count from main.cpp to include in telemetry
    std::string get_telemetry_json(uint64_t late_count);

    // Helper to allow main.cpp or other components to access input states if needed
    InputManager& getInputManager() { return input; }

private:
    InputManager input;
    uint64_t frame_count;
};

#endif