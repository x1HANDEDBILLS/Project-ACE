#include "input_manage.h"
#include <SDL3/SDL.h>
#include <algorithm>
#include <iostream>

bool InputManager::initialize() {
    if (!SDL_Init(SDL_INIT_GAMEPAD | SDL_INIT_SENSOR | SDL_INIT_JOYSTICK)) return false;
    scoutDevices();
    return true;
}

void InputManager::scoutDevices() {
    int count;
    SDL_JoystickID* joysticks = SDL_GetGamepads(&count);
    if (joysticks) {
        for (int i = 0; i < count; i++) {
            devices.push_back(std::make_unique<InputReader>(joysticks[i]));
        }
        SDL_free(joysticks);
    }
}

void InputManager::update() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_EVENT_GAMEPAD_ADDED) {
            // Check if we already have this physical device registered to avoid duplicates
            bool exists = false;
            for(auto& d : devices) {
                if(d->getInstanceId() == event.gdevice.which) { exists = true; break; }
            }
            if(!exists) devices.push_back(std::make_unique<InputReader>(event.gdevice.which));
            
        } else if (event.type == SDL_EVENT_GAMEPAD_REMOVED) {
            devices.erase(
                std::remove_if(devices.begin(), devices.end(),
                    [&](const std::unique_ptr<InputReader>& d) {
                        return d->getInstanceId() == event.gdevice.which;
                    }),
                devices.end());
        }
    }
    for (auto& device : devices) device->update();
}