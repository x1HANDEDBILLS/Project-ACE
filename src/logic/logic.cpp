// Location: src/logic/logic.cpp
#include "Core/app_state.h"
#include "input_tune_plane.h"
#include "logic/logic.h"
#include "rc_map_plane.h"
#include "rc_trim_plane.h"
std::atomic<bool> Logic::backend_unlocked{false};
// Unified instances for the pipeline
static InputTunePlane plane_tuner;
static RCMapPlane plane_mapper;
static RCTrimPlane plane_trimmer;
Logic &Logic::instance() {
    static Logic inst;
    return inst;
}
void Logic::initialize() { backend_unlocked.store(true); }
void Logic::run_iteration() {
    if (!backend_unlocked.load()) return;
    // Stage 1: Condition Raw slots into Tuned slots (200 slots)
    plane_tuner.process_all_slots();
    // Stage 2: Map Tuned slots to the 16 Output Channels
    plane_mapper.process();
    // Stage 3: Apply Trims and Inversions
    plane_trimmer.process();
}
