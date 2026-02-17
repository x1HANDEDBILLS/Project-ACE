// =====================================================
// FILE: /home/user/RC-Ground-Control/cpp/input/input_selection.h
// =====================================================
#ifndef INPUT_SELECTION_H
#define INPUT_SELECTION_H

#include <string>
#include <vector>
#include <linux/input.h>

enum class DeviceRole {
    SCREEN_TOUCH,
    RC_THROTTLE,
    RC_STEERING,
    RC_AUXILIARY
};

struct DeviceSlot {
    int fd;
    std::string path;
    DeviceRole role;
};

struct SharedTouchState {
    int x = 0;
    int y = 0;
    bool is_down = false;
    bool updated = false;
};

struct RCState {
    int throttle   = 1500;
    int steering   = 1500;
    int auxiliary  = 1500;
};

class InputManager {
public:
    InputManager();
    ~InputManager();

    bool add_device(const std::string& path, DeviceRole role);
    void poll_all();

    SharedTouchState get_gui_state();
    RCState get_rc_state() const;

private:
    std::vector<DeviceSlot> devices;
    SharedTouchState gui_state;
    RCState rc_state;

    void process_event(const struct input_event& ev, DeviceRole role);
    static std::string role_name(DeviceRole r);
};

#endif