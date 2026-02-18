#include "input_read.h"
#include <cstring>
#include <cstdio>

InputReader::InputReader() {
    std::memset(&state, 0, sizeof(RawData));
    initialize();
}

InputReader::~InputReader() {
    if (gamepad) SDL_CloseGamepad(gamepad);
    SDL_QuitSubSystem(SDL_INIT_GAMEPAD | SDL_INIT_SENSOR | SDL_INIT_JOYSTICK);
}

void InputReader::rescanForGamepad() {
    if (gamepad) return;

    int count = 0;
    SDL_JoystickID* ids = SDL_GetGamepads(&count);
    if (count > 0 && ids != nullptr) {
        gamepad = SDL_OpenGamepad(ids[0]);
        if (gamepad) {
            state.connected = true;
            SDL_SetGamepadSensorEnabled(gamepad, SDL_SENSOR_GYRO, true);
            SDL_SetGamepadSensorEnabled(gamepad, SDL_SENSOR_ACCEL, true);
            printf("\n[xACE] Gamepad Auto-Connected (%.0f ms): %s\n", (double)SDL_GetTicks() - initTime, SDL_GetGamepadName(gamepad));
        }
    }
    if (ids) SDL_free(ids);
}

bool InputReader::initialize() {
    if (!SDL_InitSubSystem(SDL_INIT_GAMEPAD | SDL_INIT_SENSOR | SDL_INIT_JOYSTICK)) {
        printf("[xACE] SDL_Init failed: %s\n", SDL_GetError());
        return false;
    }

    SDL_SetHint(SDL_HINT_JOYSTICK_HIDAPI, "1");
    SDL_SetHint(SDL_HINT_JOYSTICK_HIDAPI_PS4, "1");
    SDL_SetHint(SDL_HINT_JOYSTICK_ALLOW_BACKGROUND_EVENTS, "1");

    initTime = SDL_GetTicks();

    rescanForGamepad();

    if (gamepad) {
        printf("\n[xACE] *** CONTROLLER READY AT STARTUP! ***\n");
        SDL_RumbleGamepad(gamepad, 0xFF, 0xFF, 1500);   // strong 1.5s rumble to fully wake PS4
        SDL_Delay(200);
        printf("[xACE] *** STRONG RUMBLE SENT (feel vibration now?) ***\n");
        printf("[xACE] Press the big PS button 2-3 times now!\n");
    }

    printf("[xACE] InputReader initialized\n\n");
    return true;
}

RawData InputReader::fetch_new_data() {
    callCount++;

    SDL_PumpEvents();
    SDL_UpdateGamepads();   // THIS FORCES SDL TO READ FRESH DATA FROM THE KERNEL EVERY FRAME

    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_EVENT_GAMEPAD_AXIS_MOTION:
                if (event.gaxis.axis < 16) state.axes[event.gaxis.axis] = event.gaxis.value;
                break;
            case SDL_EVENT_GAMEPAD_BUTTON_DOWN:
            case SDL_EVENT_GAMEPAD_BUTTON_UP:
                if (event.gbutton.button < 32) state.buttons[event.gbutton.button] = (event.type == SDL_EVENT_GAMEPAD_BUTTON_DOWN);
                break;
            case SDL_EVENT_GAMEPAD_SENSOR_UPDATE:
                if (event.gsensor.sensor == SDL_SENSOR_GYRO) std::memcpy(state.gyro, event.gsensor.data, sizeof(float)*3);
                break;
            case SDL_EVENT_GAMEPAD_TOUCHPAD_MOTION:
            case SDL_EVENT_GAMEPAD_TOUCHPAD_DOWN:
            case SDL_EVENT_GAMEPAD_TOUCHPAD_UP:
                state.touch[0].x = event.gtouchpad.x;
                state.touch[0].y = event.gtouchpad.y;
                state.touch[0].down = (event.type != SDL_EVENT_GAMEPAD_TOUCHPAD_UP);
                break;
        }
    }

    if (gamepad) {
        for (int i = 0; i < 16; i++) {
            state.axes[i] = SDL_GetGamepadAxis(gamepad, static_cast<SDL_GamepadAxis>(i));
        }
        for (int i = 0; i < 32; i++) {
            state.buttons[i] = SDL_GetGamepadButton(gamepad, static_cast<SDL_GamepadButton>(i));
        }

        static Uint32 lastDebug = 0;
        Uint32 now = SDL_GetTicks();
        if (now - lastDebug > 800) {   // every 0.8 seconds
            lastDebug = now;
            printf("[SDL LIVE] LX=%d LY=%d RX=%d RY=%d | A=%d B=%d X=%d Y=%d | PS=%d Touch=%d\n",
                   state.axes[SDL_GAMEPAD_AXIS_LEFTX],
                   state.axes[SDL_GAMEPAD_AXIS_LEFTY],
                   state.axes[SDL_GAMEPAD_AXIS_RIGHTX],
                   state.axes[SDL_GAMEPAD_AXIS_RIGHTY],
                   state.buttons[SDL_GAMEPAD_BUTTON_SOUTH],
                   state.buttons[SDL_GAMEPAD_BUTTON_EAST],
                   state.buttons[SDL_GAMEPAD_BUTTON_WEST],
                   state.buttons[SDL_GAMEPAD_BUTTON_NORTH],
                   state.buttons[SDL_GAMEPAD_BUTTON_START],
                   state.buttons[SDL_GAMEPAD_BUTTON_TOUCHPAD]);
        }
    }

    return state;
}
