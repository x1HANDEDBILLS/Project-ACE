#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#include <fcntl.h>
#include <pthread.h>
#include <sched.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <unistd.h>
#include <cstdint>
#include <vector>
#include <thread>
#include <array>
#include "Core/app_state.h"
#include "protocols/nomad_daemon_link.h"

std::atomic<bool> NomadHardware::running{false};
std::array<std::atomic<int>, 16> NomadHardware::shared_channels;

struct termios2 {
    uint32_t c_iflag, c_oflag, c_cflag, c_lflag;
    uint8_t c_line, c_cc[19];
    uint32_t c_ispeed, c_ospeed;
};

uint8_t internal_crsf_crc(const uint8_t* d, size_t len) {
    uint8_t crc = 0;
    for (size_t i = 0; i < len; ++i) {
        crc ^= d[i];
        for (int j = 0; j < 8; ++j) crc = (crc & 0x80) ? ((crc << 1) ^ 0xD5) : (crc << 1);
    }
    return crc;
}

void internal_pack_channels(const std::array<int, 16>& ch, uint8_t* p) {
    uint32_t val = 0;
    int bits = 0, pos = 0;
    for (int c : ch) {
        val |= (static_cast<uint32_t>(c) << bits);
        bits += 11;
        while (bits >= 8) {
            p[pos++] = val & 0xFF;
            val >>= 8;
            bits -= 8;
        }
    }
}

void NomadHardware::start() {
    if (running.load()) return;
    running.store(true);
    std::thread worker(run_loop);
    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);
    CPU_SET(3, &cpuset);
    pthread_setaffinity_np(worker.native_handle(), sizeof(cpu_set_t), &cpuset);
    worker.detach();
}

void NomadHardware::stop() { running.store(false); }

void NomadHardware::run_loop() {
    int ser = open("/dev/ttyAMA5", O_RDWR | O_NOCTTY | O_NONBLOCK);
    if (ser != -1) {
        struct termios2 opt;
        ioctl(ser, 0x802C542A, &opt);
        opt.c_cflag &= ~0010017;
        opt.c_cflag |= 0010000 | CLOCAL | CREAD | CS8;
        opt.c_ispeed = opt.c_ospeed = 420000;
        opt.c_iflag = opt.c_oflag = opt.c_lflag = 0;
        ioctl(ser, 0x402C542B, &opt);
    }

    std::vector<uint8_t> t_buf;
    auto& state = AppState::instance();

    while (running.load()) {
        // --- TX ---
        std::array<int, 16> local_ch;
        for (int i = 0; i < 16; i++)
            local_ch[i] = shared_channels[i].load(std::memory_order_relaxed);

        uint8_t pkt[26] = {0xC8, 24, 0x16};
        internal_pack_channels(local_ch, pkt + 3);
        pkt[25] = internal_crsf_crc(pkt + 2, 23);
        
        if (ser != -1) {
            write(ser, pkt, 26);
            // Wait just long enough for the 26 bytes to clear at 420k baud (~620us)
            // then flush the echo so we don't parse our own data.
            usleep(700); 
            tcflush(ser, TCIFLUSH);
        }

        // --- RX ---
        uint8_t rb[512];
        while (true) {
            ssize_t b = read(ser, rb, sizeof(rb));
            if (b <= 0) break;
            t_buf.insert(t_buf.end(), rb, rb + b);
        }

        // --- PARSER ---
        size_t head = 0;
        while (head + 2 < t_buf.size()) {
            // Check for valid CRSF Sync headers
            if (t_buf[head] == 0xEE || t_buf[head] == 0xEA || t_buf[head] == 0xC8) {
                uint8_t len = t_buf[head + 1];
                if (len < 2 || head + len + 2 > t_buf.size()) break;

                if (t_buf[head + len + 1] == internal_crsf_crc(&t_buf[head + 2], len - 1)) {
                    uint8_t type = t_buf[head + 2], *d = &t_buf[head + 3];
                    if (type == 0x14) { // Link Statistics
                        state.tele.rssi = -1 * (int8_t)d[0];
                        state.tele.rssi2 = -1 * (int8_t)d[1];
                        state.tele.lq = d[2];
                        state.tele.snr = (int8_t)d[3];
                        state.tele.ant = d[4];
                        state.tele.mode = d[5];
                        state.tele.pwr = d[8];
                    } else if (type == 0x08) { // Battery
                        state.tele.bat = ((d[0] << 8) | d[1]) / 10.0f;
                    } else if (type == 0x02) { // GPS
                        state.tele.lat = (int32_t)((d[0] << 24) | (d[1] << 16) | (d[2] << 8) | d[3]) / 1e7;
                        state.tele.lon = (int32_t)((d[4] << 24) | (d[5] << 16) | (d[6] << 8) | d[7]) / 1e7;
                        state.tele.alt = (int16_t)((d[12] << 8) | d[13]) - 1000.0f;
                        state.tele.sats = d[14];
                    }
                    head += len + 2;
                    continue;
                }
            }
            head++;
        }
        if (head > 0) t_buf.erase(t_buf.begin(), t_buf.begin() + head);
        if (t_buf.size() > 2048) t_buf.clear();

        // Total loop timing including the usleep(700) above
        usleep(300); 
    }
    if (ser != -1) close(ser);
}
