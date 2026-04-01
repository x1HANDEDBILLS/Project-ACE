// Location: include/input/input_manage.h
#ifndef INPUT_MANAGE_H
#define INPUT_MANAGE_H
#include "input/device_state.h"
#include "input/input_read.h"
#include <memory>
class InputManager {
public:
    static InputManager &instance();
    bool initialize();
    void update();
    // FIX 2: Declared the hotplug function so it can be linked
    void pollHotplugs();
    void scoutDevices();
    DeviceState getSlotState(int index) const;
    void setVehicleType(int type);

private:
    InputManager();
    ~InputManager();
    std::unique_ptr<InputReader> m_slots[4];
    int current_vehicle_type;
};
#endif
