#include "logic.h"
#include "input_read.h" // For MAX_AXES, MAX_BUTTONS
#include <iostream>
#include <sstream>
#include <iomanip>
#include <cstdio> 

Logic::Logic() : frame_count(0) {}
Logic::~Logic() {}

bool Logic::initialize() {
    // Initialize InputManager (handles the 4-port hardware hub and IMU fusion)
    if (!input.initialize()) {
        std::cerr << "[LOGIC] Failed to initialize InputManager." << std::endl;
        return false;
    }
    std::cout << "[LOGIC] Systems Initialized. Ready for RC Input." << std::endl;
    return true;
}

/**
 * Acts as a High-Level Dispatcher.
 * It receives strings from the Network/Socket and routes them to the correct subsystem.
 */
void Logic::handle_command(const std::string& cmd) {
    if (cmd.empty()) return;

    // Log incoming commands for debugging the GUI link
    std::cout << "[LOGIC] Command: " << cmd << std::endl;

    // Direct Pass-through:
    // InputManager now handles SET_SLOT, SET_DZ, and CALIBRATE internally.
    // This keeps Logic clean and focused on data flow.
    input.handleCommand(cmd);
}

void Logic::run_iteration() {
    frame_count++;

    // 1. Scrape USB data from all controllers
    // 2. Run IMU Complementary Filters for Gyro/Accel fusion
    // 3. Apply User Tuning (Remapping, Expo, Deadzones)
    input.update(); 
}

/**
 * Builds the JSON Telemetry packet for the Python GUI.
 * This sends a snapshot of all 4 slots, showing both RAW and TUNED data.
 */
std::string Logic::get_telemetry_json(uint64_t late_count) {
    std::stringstream ss;
    
    // Fixed precision is vital for the Python parser to stay stable
    ss << std::fixed << std::setprecision(2);

    // 1. GLOBAL HEADER
    ss << "{\"hb\":" << frame_count 
       << ",\"late\":" << late_count;

    // 2. THE 4-PORT DATA HUB
    ss << ",\"slots\":{"; 
    for (int i = 1; i <= 4; i++) {
        DeviceState state = input.getSlotState(i);
        
        ss << "\"" << i << "\":{";
        ss << "\"conn\":" << (state.connected ? "true" : "false");
        
        if (state.connected) {
            ss << ",\"name\":\"" << state.name << "\"";
            
            // RAW HARDWARE DATA 
            // Useful for the GUI to show "Physical" stick position
            ss << ",\"raw\":[";
            for(int a = 0; a < 12; a++) {
                ss << (int)state.raw.axes[a] << (a < 11 ? "," : "");
            }
            ss << "]";

            // TUNED/DEADZONED DATA
            // This is the "Black Box" output ready for the RC transmitter
            ss << ",\"tuned\":[";
            for(int a = 0; a < 12; a++) {
                ss << (int)state.axes[a] << (a < 11 ? "," : "");
            }
            ss << "]";
            
            // BUTTON DATA (Mapped and Filtered)
            ss << ",\"btns\":[";
            for(int b = 0; b < 32; b++) {
                ss << (state.buttons[b] ? "1" : "0") << (b < 31 ? "," : "");
            }
            ss << "]";

            // MOTION DATA (Fused Angles)
            // gyro[0] = Pitch Angle, gyro[1] = Yaw Rate, gyro[2] = Roll Angle
            ss << ",\"accel\":[" << state.accel[0] << "," << state.accel[1] << "," << state.accel[2] << "]";
            ss << ",\"gyro\":[" << state.gyro[0] << "," << state.gyro[1] << "," << state.gyro[2] << "]";
        }
        
        ss << "}";
        if (i < 4) ss << ","; 
    }

    ss << "}}";
    return ss.str();
}