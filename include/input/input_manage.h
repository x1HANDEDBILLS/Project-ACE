#ifndef INPUT_MANAGE_H
#define INPUT_MANAGE_H

#include <SDL3/SDL.h>

#include <memory>
#include <vector>

#include "input_read.h"

class InputManager
{
   public:
    static InputManager& instance();
    bool initialize();
    void update();
    void setVehicleType(int type);
    int getVehicleType() const
    {
        return current_vehicle_type;
    }
    DeviceState getSlotState(int index) const;
    ~InputManager();

   private:
    InputManager();
    void scoutDevices();

    // Renamed from 'slots' to 'm_slots' to avoid Qt keyword conflict
    std::unique_ptr<InputReader> m_slots[4];
    int current_vehicle_type;
};
#endif
