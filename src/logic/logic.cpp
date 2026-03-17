// Location: src/logic/logic.cpp
#include "Core/app_state.h"
#include "logic/logic.h"
#include "vehicles/plane/input_tune_plane.h"
#include "vehicles/plane/rc_map_plane.h"
#include "vehicles/plane/rc_trim_plane.h"
std::atomic<bool> Logic::backend_unlocked{false};
static InputTunePlane plane_tuner;
static RCMapPlane plane_mapper;
static RCTrimPlane plane_trimmer;
void Logic::initialize() { backend_unlocked.store(true); }
void Logic::run_iteration() {
    if (!backend_unlocked.load()) return;
    plane_tuner.process_all_slots();
    plane_mapper.process();
    plane_trimmer.process();
}
