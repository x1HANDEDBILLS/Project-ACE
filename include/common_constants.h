// LOCATION: /home/user/ACE/include/common_constants.h
#ifndef COMMON_CONSTANTS_H
#define COMMON_CONSTANTS_H

#include <cstdint>

#define MAX_AXES 8
#define MAX_BUTTONS 32
#define LOGIC_HZ 1000
#define UI_REFRESH_MS 16
#define ISOLATED_CORE 3
#define RT_PRIORITY 80
#define AXIS_FULL_SCALE 32767
#define DEFAULT_DEADZONE 500

struct AxisTuning
{
    int deadzone = DEFAULT_DEADZONE;
    float expo = 1.0f;
    float sensitivity = 1.0f;
};

struct ControlProfile
{
    AxisTuning tuning[MAX_AXES];
    int axisMap[MAX_AXES] = {0, 1, 2, 3, 4, 5, 6, 7};
    bool inverted[MAX_AXES] = {false};
};
#endif
