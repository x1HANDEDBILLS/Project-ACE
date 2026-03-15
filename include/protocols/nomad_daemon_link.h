#pragma once
#include <array>
#include <atomic>
#include <thread>

class NomadHardware
{
   public:
    static void start();
    static void stop();
    // Logic thread writes here directly
    static std::array<std::atomic<int>, 16> shared_channels;

   private:
    static void run_loop();
    static std::atomic<bool> running;
};
