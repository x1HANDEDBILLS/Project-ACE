#pragma once
#include "input_read.h"
#include <map>
#include <memory>
#include <string>
#include <vector>

/**
 * ControlProfile: The "Translation Map"
 * This struct defines how the raw USB data from a physical port is 
 * transformed into the virtual state used by the Logic engine.
 */
struct ControlProfile {
    std::string profileName = "Default";
    
    // Mapping arrays: VirtualIndex = RawData[Map[VirtualIndex]]
    // These indices point to where in the raw data we should pull from
    int axisMap[12];
    int buttonMap[32];
    int motionMap[6];   // 0-2: Accel (X,Y,Z), 3-5: Gyro (P,R,Y)
    
    // Inversion toggles
    bool inverted[12] = {false};
    bool motionInverted[6] = {false};
    
    ControlProfile() {
        // Initialize with default 1:1 mapping
        for (int i = 0; i < 12; i++) axisMap[i] = i;
        for (int i = 0; i < 32; i++) buttonMap[i] = i;
        for (int i = 0; i < 6; i++)  motionMap[i] = i;
    }
};

class InputManager {
public:
    /**
     * initialize: Starts SDL subsystems and runs the first hardware scout.
     */
    bool initialize();

    /**
     * update: The heartbeat of the remapper. 
     * 1. Polls SDL events (Hot-plugging).
     * 2. Scrapes raw data from all active readers.
     * 3. Applies active profiles and normalization to the raw data.
     */
    void update();

    /**
     * scoutDevices: Scans all connected gamepads and assigns them to 
     * slots based on their physical USB topology.
     */
    void scoutDevices();
    
    /**
     * handleCommand: Parses strings from the Python IPC (e.g., "SET_SLOT|1|racing.json")
     */
    void handleCommand(const std::string& cmd);

    /**
     * loadProfile: Opens a JSON file from the /profiles directory and 
     * updates the profile for a specific slot.
     */
    void loadProfile(int slot, const std::string& profileName);
    
    /**
     * assignProfileToSlot: Direct manual assignment of a profile struct.
     */
    void assignProfileToSlot(int slot, const ControlProfile& profile);
    
    /**
     * getSlotState: Returns the final, remapped and normalized state for a specific slot.
     */
    DeviceState getSlotState(int slot) const;

private:
    /**
     * getSlotFromPath: Translates a system device path (USB topology) 
     * into a slot ID (1-4).
     */
    int getSlotFromPath(const std::string& path);

    // Map of active InputReaders, keyed by Slot ID (1-4)
    std::map<int, std::unique_ptr<InputReader>> slots;

    // Devices connected but not mapped to a primary slot
    std::vector<std::unique_ptr<InputReader>> unmapped_devices;
    
    // The active remapping rules for each of the 4 ports (Slot 1-4)
    // index 0 is ignored to keep the 1-4 numbering intuitive
    ControlProfile activeProfiles[5]; 
};