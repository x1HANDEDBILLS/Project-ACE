// Location: include/vehicles/plane/input_tune/input_tune_plane.h
#ifndef INPUT_TUNE_PLANE_H
#define INPUT_TUNE_PLANE_H
class InputTunePlane {
public:
    InputTunePlane(); // Explicit declaration fixed the "implicitly-declared" error
    void process_all_slots();
};
#endif
