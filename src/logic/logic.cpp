#include "logic.h"
#include <iostream>
#include <unistd.h> // <--- ADD THIS for usleep()

bool Logic::initialize() {
    std::cout << "[ACE] Logic System Initializing..." << std::endl;
    return true; 
}

void Logic::run() {
    // This is your 1000Hz loop
    while(true) {
        // High speed magic goes here
        usleep(1000); 
    }
}