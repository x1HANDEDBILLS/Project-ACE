#pragma once
#include "../input/input_manage.h"

class InputDashboard {
public:
    InputDashboard();
    ~InputDashboard();

    // Added 'const' here to match the .cpp and the stock main.cpp
    void refresh(const InputManager& manager);

private:
    void clear_line();
};