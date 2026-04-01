// Location: include/logic/logic.h
#ifndef LOGIC_H
#define LOGIC_H
#include <atomic>
class Logic {
public:
    static Logic &instance();
    void initialize();
    void run_iteration();

private:
    Logic() = default;
    static std::atomic<bool> backend_unlocked;
};
#endif
