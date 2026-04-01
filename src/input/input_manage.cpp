// Location: src/input/input_manage.cpp
#include "Core/app_state.h"
#include "input/input_manage.h"
#include "logic/logic.h"
#include <SDL3/SDL.h>
#include <cstring>
InputManager &InputManager::instance() {
    static InputManager inst;
    return inst;
}
InputManager::InputManager() {
    for (int i = 0; i < 4; i++) m_slots[i] = nullptr;
}
InputManager::~InputManager() { SDL_Quit(); }
bool InputManager::initialize() {
    if (!SDL_Init(SDL_INIT_GAMEPAD | SDL_INIT_SENSOR | SDL_INIT_JOYSTICK)) return false;
    scoutDevices();
    Logic::instance().initialize(); // Handshake: Unlock the logic engine
    return true;
}
void InputManager::scoutDevices() {
    int count = 0;
    SDL_JoystickID *joysticks = SDL_GetJoysticks(&count);
    for (int i = 0; i < count && i < 4; i++) {
        if (!m_slots[i]) m_slots[i] = std::make_unique<InputReader>(joysticks[i]);
    }
    SDL_free(joysticks);
}
void InputManager::pollHotplugs() {}
void InputManager::update() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_EVENT_JOYSTICK_ADDED || event.type == SDL_EVENT_JOYSTICK_REMOVED) scoutDevices();
    }
    SDL_UpdateSensors();
    auto &app = AppState::instance();
    for (int i = 0; i < 4; i++) {
        if (m_slots[i]) {
            m_slots[i]->update();
            DeviceState ds = m_slots[i]->getState();
            int offset = i * 50;
            for (int j = 0; j < 16; j++) app.raw_slots[offset + j] = ds.axes[j];
            for (int j = 0; j < 32; j++) app.raw_slots[offset + 16 + j] = ds.buttons[j] ? 32767 : 0;
            app.raw_slots[offset + 48] = (int16_t)ds.gyro[0];
            app.raw_slots[offset + 49] = (int16_t)ds.gyro[1];
        }
    }
    // This is the heartbeat of your tree
    Logic::instance().run_iteration();
}
DeviceState InputManager::getSlotState(int index) const {
    return (index >= 0 && index < 4 && m_slots[index]) ? m_slots[index]->getState() : DeviceState();
}
void InputManager::setVehicleType(int type) {}
