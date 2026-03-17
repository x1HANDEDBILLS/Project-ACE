// Location: include/input/input_tune.h
#ifndef INPUT_TUNE_H
#define INPUT_TUNE_H
#include "common_constants.h"
#include <cstdint>
struct DeviceState; // Forward declaration
class InputTune {
public:
    virtual ~InputTune() = default;
    virtual void applyTuning(DeviceState &state, const ControlProfile &profile);

protected:
    int16_t processAxis(int16_t raw, const AxisTuning &t, bool inv);
};
class InputTunePlane : public InputTune {
public:
    void applyTuning(DeviceState &state, const ControlProfile &profile) override;
};
class InputTuneCar : public InputTune {
public:
    void applyTuning(DeviceState &state, const ControlProfile &profile) override;
};
class InputTuneDrone : public InputTune {
public:
    void applyTuning(DeviceState &state, const ControlProfile &profile) override;
};
class InputTuneHeli : public InputTune {
public:
    void applyTuning(DeviceState &state, const ControlProfile &profile) override;
};
#endif
