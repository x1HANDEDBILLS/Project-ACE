#pragma once
#include "input_read.h"

class InputManager {
public:
    InputManager();
    ~InputManager();

    bool initialize();           // opens up to 3 controllers
    void update();               // call every frame
    RawData getCombined() const; // merged input from all controllers
    RawData getRaw(int index); // raw data from controller 0, 1 or 2
    int getNumConnected() const;

private:
    InputReader readers[3];
    RawData combined;
    int numConnected = 0;
};
