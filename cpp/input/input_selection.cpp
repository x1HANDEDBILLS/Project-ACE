// =====================================================
// FILE: /home/user/RC-Ground-Control/cpp/input/input_selection.cpp
// =====================================================
#include "input_selection.h"
#include <fcntl.h>
#include <unistd.h>
#include <iostream>
#include <sys/ioctl.h>
#include <linux/input.h>

InputManager::InputManager() {
    gui_state = {0, 0, false, false};
    // Initialize RC state to neutral/zero
    rc_state = {0, 0, 0};
}

InputManager::~InputManager() {
    for (auto& dev : devices) {
        if (dev.fd >= 0) close(dev.fd);
    }
}

bool InputManager::add_device(const std::string& path, DeviceRole role) {
    int fd = open(path.c_str(), O_RDONLY | O_NONBLOCK);
    if (fd < 0) {
        std::cerr << "CRITICAL: Failed to open " << path << std::endl;
        return false;
    }

    char dev_name[256] = {0};
    ioctl(fd, EVIOCGNAME(sizeof(dev_name) - 1), dev_name);
    std::cout << "Added device: " << path << " (" << dev_name << ") → " << role_name(role) << std::endl;

    devices.push_back({fd, path, role});
    return true;
}

void InputManager::poll_all() {
    struct input_event ev;
    for (auto& dev : devices) {
        while (read(dev.fd, &ev, sizeof(ev)) == sizeof(ev)) {
            process_event(ev, dev.role);
        }
    }
}

// --- THIS WAS THE MISSING FUNCTION CAUSING THE LINKER ERROR ---
RCState InputManager::get_rc_state() const {
    return rc_state;
}

SharedTouchState InputManager::get_gui_state() {
    return gui_state; 
}

void InputManager::process_event(const struct input_event& ev, DeviceRole role) {
    if (role == DeviceRole::SCREEN_TOUCH) {
        if (ev.type == EV_ABS) {
            if (ev.code == ABS_X || ev.code == ABS_MT_POSITION_X) {
                gui_state.x = ev.value;
            } 
            else if (ev.code == ABS_Y || ev.code == ABS_MT_POSITION_Y) {
                gui_state.y = ev.value;
            }
        } 
        else if (ev.type == EV_KEY && ev.code == BTN_TOUCH) {
            gui_state.is_down = (ev.value > 0);
        }
        
        if (ev.type == EV_SYN) {
            gui_state.updated = true;
        }
    } 
    else {
        // Handle RC Hardware (Throttle/Steering/Aux)
        if (ev.type == EV_ABS) {
            switch (role) {
                case DeviceRole::RC_THROTTLE:  rc_state.throttle = ev.value; break;
                case DeviceRole::RC_STEERING:  rc_state.steering = ev.value; break;
                case DeviceRole::RC_AUXILIARY: rc_state.auxiliary = ev.value; break;
                default: break;
            }
        }
    }
}

std::string InputManager::role_name(DeviceRole r) {
    switch (r) {
        case DeviceRole::SCREEN_TOUCH: return "SCREEN_TOUCH";
        case DeviceRole::RC_THROTTLE:  return "RC_THROTTLE";
        case DeviceRole::RC_STEERING:  return "RC_STEERING";
        case DeviceRole::RC_AUXILIARY: return "RC_AUXILIARY";
        default: return "UNKNOWN";
    }
}