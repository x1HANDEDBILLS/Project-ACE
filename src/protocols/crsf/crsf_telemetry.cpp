// Location: src/protocols/crsf/crsf_telemetry.cpp
#include "Core/app_state.h"
#include <sstream>
#include <string>
#include <vector>
void process_telemetry_feedback(char *buf) {
    if (!buf) return;
    auto &state = AppState::instance();
    try {
        // Skipping the CRSF header offset
        std::string d(buf + 5);
        if (d.empty()) return;
        std::stringstream ss(d);
        std::string seg;
        std::vector<std::string> t;
        while (std::getline(ss, seg, ',')) { t.push_back(seg); }
        if (t.size() >= 10) {
            state.tele.rssi = std::stoi(t[0]);
            state.tele.lq = std::stoi(t[1]);
            state.tele.snr = std::stoi(t[2]);
            state.tele.mode = std::stoi(t[3]);
            state.tele.pwr = std::stoi(t[4]);
            state.tele.bat = std::stof(t[5]);
            state.tele.lat = std::stod(t[6]);
            state.tele.lon = std::stod(t[7]);
            state.tele.alt = std::stof(t[8]);
            state.tele.sats = std::stoi(t[9]);
        }
    } catch (...) {
        // Silently catch malformed strings to prevent crashes
    }
}
