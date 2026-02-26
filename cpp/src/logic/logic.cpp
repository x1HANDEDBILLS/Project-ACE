#include "logic.h"
#include <iostream>
#include <sstream>
#include <iomanip>

Logic::Logic() : frame_count(0) {}
Logic::~Logic() {}

bool Logic::initialize() {
    // Initialize InputManager (which handles our 4-port hub mapping)
    if (!input.initialize()) return false;
    return true;
}

// NEW: This resolves the compiler error in main.cpp
void Logic::handle_command(const std::string& cmd) {
    // Passes the socket command (e.g., "SET_SLOT|1|racing.json") to the remapper
    input.handleCommand(cmd);
}

void Logic::run_iteration() {
    frame_count++;
    // Clear the SDL event queue, scrape USB data, and apply profile remapping
    input.update(); 
}

std::string Logic::get_telemetry_json(uint64_t late_count) {
    std::stringstream ss;
    
    // Set fixed precision for float sensors to keep JSON small but accurate
    ss << std::fixed << std::setprecision(4);

    // Global telemetry header
    ss << "{\"hb\":" << frame_count 
       << ",\"late\":" << late_count 
       << ",\"slots\":{"; 

    // Explicitly loop 1-4 to match your physical 4-port hub
    for (int i = 1; i <= 4; i++) {
        DeviceState state = input.getSlotState(i);
        
        ss << "\"" << i << "\":{";
        ss << "\"conn\":" << (state.connected ? "true" : "false");
        
        if (state.connected) {
            ss << ",\"name\":\"" << state.name << "\""
               << ",\"axes\":[";
            
            // Output all 6 standard virtual axes
            for(int a = 0; a < 6; a++) {
                ss << (int)state.axes[a] << (a < 5 ? "," : "");
            }
            
            ss << "],\"btns\":[";
            // Output standard button set
            for(int b = 0; b < 15; b++) {
                ss << (state.buttons[b] ? "1" : "0") << (b < 14 ? "," : "");
            }
            ss << "]";

            // NEW: Rigging for Motion Telemetry (Accel and Gyro)
            ss << ",\"accel\":[" << state.accel[0] << "," << state.accel[1] << "," << state.accel[2] << "]";
            ss << ",\"gyro\":[" << state.gyro[0] << "," << state.gyro[1] << "," << state.gyro[2] << "]";
        }
        
        ss << "}";
        if (i < 4) ss << ","; // Comma between slot objects
    }

    ss << "}}";
    return ss.str();
}