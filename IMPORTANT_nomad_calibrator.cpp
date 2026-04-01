#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#include <algorithm>
#include <array>
#include <atomic>
#include <cstdint>
#include <fcntl.h>
#include <iostream>
#include <sys/ioctl.h>
#include <termios.h>
#include <thread>
#include <unistd.h>
#include <chrono>

struct termios2 {
    uint32_t c_iflag, c_oflag, c_cflag, c_lflag;
    uint8_t c_line, c_cc[19];
    uint32_t c_ispeed, c_ospeed;
};

std::atomic<int> loop_val{0}; 
std::atomic<bool> running{true};

// CRSF Standard: 172 (988us) to 1811 (2012us). Center is 992 (1500us).
int to_crsf(int v) {
    return std::clamp(v, 172, 1811);
}

uint8_t crc8(const uint8_t *d, size_t len) {
    uint8_t crc = 0;
    for (size_t i = 0; i < len; ++i) {
        crc ^= d[i];
        for (int j = 0; j < 8; j++) crc = (crc & 0x80) ? ((crc << 1) ^ 0xD5) : (crc << 1);
    }
    return crc;
}

void pack_11bit_crsf(const std::array<int, 16> &ch, uint8_t *p) {
    uint32_t val = 0; int bits = 0, pos = 0;
    for (int c : ch) {
        val |= (static_cast<uint32_t>(c & 0x07FF) << bits);
        bits += 11;
        while (bits >= 8) { p[pos++] = val & 0xFF; val >>= 8; bits -= 8; }
    }
}

void tx_loop_1khz(int fd) {
    std::array<int, 16> tx_channels;
    while (running) {
        int target = loop_val.load();
        for (int i=0; i<16; i++) tx_channels[i] = target;
        
        uint8_t crsf_packet[26] = {0xC8, 24, 0x16};
        pack_11bit_crsf(tx_channels, crsf_packet + 3);
        crsf_packet[25] = crc8(crsf_packet + 2, 23);
        
        write(fd, crsf_packet, 26);
        std::this_thread::sleep_for(std::chrono::microseconds(1000)); // Precise 1kHz
    }
}

int main() {
    // Open UART with low-latency settings
    int fd = open("/dev/ttyAMA5", O_RDWR | O_NOCTTY | O_NONBLOCK);
    if (fd == -1) {
        perror("FATAL: UART Open Failed");
        return 1;
    }

    struct termios2 opt;
    ioctl(fd, 0x802C542A, &opt);
    opt.c_cflag &= ~0010017; opt.c_cflag |= 0010000 | CLOCAL | CREAD | CS8;
    opt.c_ispeed = opt.c_ospeed = 420000;
    opt.c_iflag = opt.c_oflag = opt.c_lflag = 0;
    ioctl(fd, 0x402C542B, &opt);

    struct termios oldt, newt;
    tcgetattr(0, &oldt); newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(0, TCSANOW, &newt);

    loop_val = 172; // Start at safe Zero
    std::thread transmitter_thread(tx_loop_1khz, fd);

    std::cout << "\n[!] NOMAD ESC CALIBRATOR - USE WITH PROPS REMOVED\n";
    std::cout << "--------------------------------------------------\n";
    std::cout << "[m] MAX (1811) | [z] ZERO (172) | [c] CENTER (992)\n";
    std::cout << "[UP/DOWN] Fine Tune +/- 10      | [q] QUIT\n";

    char key;
    while (running && read(0, &key, 1)) {
        if (key == 'q') break;
        if (key == 'm') loop_val = 1811;
        if (key == 'z') loop_val = 172;
        if (key == 'c') loop_val = 992;
        if (key == '\033') { 
            read(0, &key, 1); read(0, &key, 1);
            if (key == 'A') loop_val += 10; 
            if (key == 'B') loop_val -= 10;
        }
        loop_val = std::clamp((int)loop_val, 172, 1811);
        std::cout << "\rCRSF Output: " << loop_val << " units    " << std::flush;
    }

    loop_val = 172;
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    running = false;
    transmitter_thread.join();
    tcsetattr(0, TCSANOW, &oldt);
    close(fd);
    return 0;
}
