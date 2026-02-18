#ifndef INPUT_READ_H
#define INPUT_READ_H

#include <libudev.h>
#include <libevdev/libevdev.h>
#include <string>
#include <vector>
#include <map>
#include <cstdint>

struct RawData {
    uint64_t timestamp;
    int32_t values[128]; 
    uint32_t device_mask;
};

class InputReader {
public:
    InputReader();
    ~InputReader();
    bool initialize();
    RawData fetch_new_data();
    
    // ADD THIS LINE TO FIX THE COMPILER ERROR
    size_t get_open_fd_count() const { return open_fds.size(); }

private:
    struct udev* udev_ctx;
    struct udev_monitor* monitor;
    int monitor_fd;
    int epoll_fd;

    std::map<int, struct libevdev*> dev_states;
    std::vector<int> open_fds;

    void cleanup();
    void update_device_list();
};

#endif