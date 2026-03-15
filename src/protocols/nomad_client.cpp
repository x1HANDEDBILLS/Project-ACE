#include <algorithm>

#include "Core/app_state.h"
#include "protocols/nomad_daemon_link.h"

void transmit_to_daemon()
{
    auto& state = AppState::instance();

    for (int i = 0; i < 16; i++) {
        float norm = (state.mapped_channels[i] + 32768.0f) / 65535.0f;
        int cv = static_cast<int>(172 + (norm * 1639));

        // Direct atomic write to the hardware buffer
        NomadHardware::shared_channels[i].store(std::clamp(cv, 172, 1811),
                                                std::memory_order_relaxed);
    }
}
