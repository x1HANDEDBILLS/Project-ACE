#include "logic.h"
#include <iostream>
#include <sstream>

Logic::Logic() : frame_count(0) {}
Logic::~Logic() {}

bool Logic::initialize() {
    // We only need to init the input manager now
    if (!input.initialize()) return false;
    return true;
}

void Logic::run_iteration() {
    frame_count++;
    input.update();
    // Logic handles internal processing here. 
    // Data is sent via the socket in main.cpp.
}

std::string Logic::get_telemetry_json(uint64_t late_count) {
    std::stringstream ss;
    
    // --- Added late_count to the top level of the JSON ---
    ss << "{\"hb\":" << frame_count 
       << ",\"late\":" << late_count 
       << ",\"slots\":[";

    for (int i = 0; i < 4; i++) {
        if (i < (int)input.getDeviceCount()) {
            DeviceState state = input.getDeviceState(i);
            ss << "{\"id\":" << i 
               << ",\"name\":\"" << state.name << "\""
               << ",\"conn\":true"
               << ",\"axes\":[";
            
            // Add all 6 axes
            for(int a=0; a<6; a++) {
                ss << (int)state.axes[a] << (a < 5 ? "," : "");
            }
            
            ss << "],\"btns\":[";
            
            // Add all 15 buttons as 0 or 1
            for(int b=0; b<15; b++) {
                ss << (state.buttons[b] ? "1" : "0") << (b < 14 ? "," : "");
            }
            ss << "]}";
        } else {
            // Placeholder for disconnected slots to keep JSON structure consistent
            ss << "{\"id\":" << i << ",\"conn\":false}";
        }
        if (i < 3) ss << ",";
    }

    ss << "]}";
    return ss.str();
}