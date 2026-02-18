#include "input_manage.h"
#include <cstring>

InputManager::InputManager() {
    std::memset(&combined, 0, sizeof(RawData));
}

InputManager::~InputManager() {
    // InputReader destructors handle SDL cleanup
}

bool InputManager::initialize() {
    numConnected = 0;
    for (int i = 0; i < 3; i++) {
        if (readers[i].initialize()) {
            numConnected++;
            // All console messages removed
        }
    }
    // No printf here anymore
    return numConnected > 0;
}

void InputManager::update() {
    std::memset(&combined, 0, sizeof(RawData));

    for (int i = 0; i < 3; i++) {
        RawData r = readers[i].fetch_new_data();

        // Combine axes (add + clamp)
        for (int a = 0; a < 16; a++) {
            combined.axes[a] += r.axes[a];
            if (combined.axes[a] > 32767) combined.axes[a] = 32767;
            if (combined.axes[a] < -32768) combined.axes[a] = -32768;
        }

        // Combine buttons (OR = any controller pressing it counts)
        for (int b = 0; b < 32; b++) {
            if (r.buttons[b]) combined.buttons[b] = true;
        }

        // Gyro + touch from first controller only
        if (i == 0) {
            std::memcpy(combined.gyro, r.gyro, sizeof(float)*3);
            combined.touch[0] = r.touch[0];
        }
    }

    combined.connected = (numConnected > 0);
}

RawData InputManager::getCombined() const {
    return combined;
}

RawData InputManager::getRaw(int index) {
    if (index < 0 || index > 2) {
        RawData empty;
        std::memset(&empty, 0, sizeof(RawData));
        return empty;
    }
    return readers[index].fetch_new_data();  // live raw data
}

int InputManager::getNumConnected() const {
    return numConnected;
}