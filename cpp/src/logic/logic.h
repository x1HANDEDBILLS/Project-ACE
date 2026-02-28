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

    /**
     * Prepares the InputManager and the SDL subsystems.
     */
    bool initialize();
    
    /**
     * The main execution step. 
     * In your 2000Hz loop, this scrapes hardware and applies all deadzone/expo tuning.
     */
    void run_iteration();
    
    /**
     * Command Bridge: Translates socket strings (like "SET_DZ|...") 
     * into actual hardware profile changes.
     */
    void handle_command(const std::string& cmd);

    /**
     * IPC Telemetry: Builds the JSON string to send to the Python GUI.
     * Uses the fixed 12-axis / 32-button floor for GUI stability.
     */
    std::string get_telemetry_json(uint64_t late_count);

    /**
     * Accessor for the input engine if other C++ components need direct state access.
     */
    InputManager& getInputManager() { return input; }

private:
    InputManager input;
    uint64_t frame_count; // Tracks total iterations (heartbeat)
};

#endif