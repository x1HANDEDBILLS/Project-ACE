#include "src/input/input_read.h"
#include <libevdev/libevdev.h>
#include <iostream>
#include <fcntl.h>
#include <unistd.h>
#include <sys/epoll.h>
#include <sys/ioctl.h>
#include <cstring>

static RawData global_state = {0};

InputReader::InputReader() : udev_ctx(nullptr), monitor(nullptr), monitor_fd(-1), epoll_fd(-1) {}
InputReader::~InputReader() { cleanup(); }

void InputReader::cleanup() {
    for (auto const& [fd, evdev] : dev_states) {
        ioctl(fd, EVIOCGRAB, 0);
        libevdev_free(evdev);
        close(fd);
    }
    dev_states.clear();
    open_fds.clear();
    if (epoll_fd >= 0) close(epoll_fd);
}

bool InputReader::initialize() {
    if (!udev_ctx) udev_ctx = udev_new();
    if (!monitor) {
        monitor = udev_monitor_new_from_netlink(udev_ctx, "udev");
        udev_monitor_filter_add_match_subsystem_devtype(monitor, "input", NULL);
        udev_monitor_enable_receiving(monitor);
        monitor_fd = udev_monitor_get_fd(monitor);
    }

    cleanup();
    epoll_fd = epoll_create1(0);

    struct udev_enumerate* enumerate = udev_enumerate_new(udev_ctx);
    udev_enumerate_add_match_subsystem(enumerate, "input");
    udev_enumerate_scan_devices(enumerate);

    struct udev_list_entry *devices, *entry;
    devices = udev_enumerate_get_list_entry(enumerate);

    udev_list_entry_foreach(entry, devices) {
        struct udev_device* dev = udev_device_new_from_syspath(udev_ctx, udev_list_entry_get_name(entry));
        const char* devnode = udev_device_get_devnode(dev);
        
        if (devnode && strstr(devnode, "event")) {
            int fd = open(devnode, O_RDONLY | O_NONBLOCK);
            if (fd >= 0) {
                struct libevdev *evdev = nullptr;
                if (libevdev_new_from_fd(fd, &evdev) >= 0) {
                    if (libevdev_has_event_type(evdev, EV_ABS) || libevdev_has_event_type(evdev, EV_KEY)) {
                        ioctl(fd, EVIOCGRAB, 1);
                        dev_states[fd] = evdev;
                        open_fds.push_back(fd);

                        struct epoll_event ev_ep;
                        ev_ep.events = EPOLLIN;
                        ev_ep.data.fd = fd;
                        epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd, &ev_ep);
                    } else { libevdev_free(evdev); close(fd); }
                } else { close(fd); }
            }
        }
        udev_device_unref(dev);
    }
    udev_enumerate_unref(enumerate);
    return true;
}

RawData InputReader::fetch_new_data() {
    update_device_list();
    struct epoll_event events[16];
    // Polling with 0 timeout for non-blocking real-time speed
    int n = epoll_wait(epoll_fd, events, 16, 0);

    for (int i = 0; i < n; i++) {
        int fd = events[i].data.fd;
        struct libevdev *evdev = dev_states[fd];
        struct input_event ev;
        
        // DRAIN LOOP: This is the secret to "Smooth" inputs.
        // We keep reading until the buffer is totally empty.
        while (libevdev_next_event(evdev, LIBEVDEV_READ_FLAG_NORMAL, &ev) == LIBEVDEV_READ_STATUS_SUCCESS) {
            if (ev.type == EV_ABS && ev.code < 64) {
                global_state.values[ev.code] = ev.value;
                global_state.device_mask = 1;
            } else if (ev.type == EV_KEY) {
                // Universal Button Mapping
                int btn_idx = 64 + (ev.code - 0x100);
                if (btn_idx >= 64 && btn_idx < 128) {
                    global_state.values[btn_idx] = ev.value;
                }
            }
        }
    }
    return global_state;
}

void InputReader::update_device_list() {
    if (!monitor) return;
    struct udev_device* dev = udev_monitor_receive_device(monitor);
    if (dev) {
        initialize();
        udev_device_unref(dev);
    }
}