#include "input/input_manage.h"

#include <SDL3/SDL.h>

#include <algorithm>
#include <iostream>
#include <vector>

InputManager& InputManager::instance()
{
    static InputManager inst;
    return inst;
}

InputManager::InputManager() : current_vehicle_type(-1)
{
    for (int i = 0; i < 4; i++) m_slots[i] = nullptr;
}

InputManager::~InputManager()
{
    SDL_Quit();
}

bool InputManager::initialize()
{
    if (!SDL_Init(SDL_INIT_GAMEPAD | SDL_INIT_SENSOR | SDL_INIT_JOYSTICK)) return false;
    scoutDevices();
    return true;
}

void InputManager::scoutDevices()
{
    int count;
    SDL_JoystickID* joysticks = SDL_GetJoysticks(&count);
    std::vector<std::unique_ptr<InputReader>> discovered;

    if (joysticks) {
        for (int i = 0; i < count; i++) {
            discovered.push_back(std::make_unique<InputReader>(joysticks[i]));
        }
        SDL_free(joysticks);
    }

    for (int i = 0; i < 4; i++) m_slots[i] = nullptr;

    std::sort(discovered.begin(), discovered.end(),
              [](const auto& a, const auto& b) { return a->getState().path < b->getState().path; });

    for (size_t i = 0; i < discovered.size() && i < 4; i++) {
        m_slots[i] = std::move(discovered[i]);
    }
}

void InputManager::update()
{
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_EVENT_GAMEPAD_ADDED || event.type == SDL_EVENT_GAMEPAD_REMOVED) {
            scoutDevices();
        }
    }
    SDL_UpdateSensors();
    for (int i = 0; i < 4; i++) {
        if (m_slots[i]) m_slots[i]->update();
    }
}

DeviceState InputManager::getSlotState(int index) const
{
    if (index >= 0 && index < 4 && m_slots[index]) {
        return m_slots[index]->getState();
    }
    return DeviceState();
}

void InputManager::setVehicleType(int type)
{
    current_vehicle_type = type;
}
