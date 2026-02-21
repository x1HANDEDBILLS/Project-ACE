#pragma once
#include "input_read.h"
#include <vector>
#include <memory>

class InputManager {
public:
    bool initialize();
    void update();
    void scoutDevices(); // <--- Add this line
    
    size_t getDeviceCount() const { return devices.size(); }
    DeviceState getDeviceState(size_t index) const {
        if (index < devices.size()) return devices[index]->getState();
        return DeviceState();
    }

private:
    std::vector<std::unique_ptr<InputReader>> devices;
};