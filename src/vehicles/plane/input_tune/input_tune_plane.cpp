#include "Core/app_state.h"
#include "vehicles/plane/input_tune/input_tune_plane.h"
#include <algorithm>
#include <cmath>

InputTunePlane::InputTunePlane() {}

void InputTunePlane::process_all_slots() {
    auto &state = AppState::instance();

    for (int i = 0; i < 200; i++) {
        double raw_val = (double)state.raw_slots[i];
        double final_val = 0.0;
        
        // 1. Calculate the Effective Deadzone (Whichever is higher)
        double dz_limit = std::max((double)state.deadzones[i], (double)state.axial_deadzones[i]);
        double threshold = (dz_limit / 100.0) * 32767.0;

        // 2. Linear Remapping (The Fix for the "Jolt")
        // If the stick is past the threshold, we subtract the threshold from the 
        // input and then scale the REMAINING throw back to full range.
        if (std::abs(raw_val) > threshold) {
            double sign = (raw_val > 0) ? 1.0 : -1.0;
            double remaining_throw = 32767.0 - threshold;
            
            // This math ensures the signal starts at 0.0001 right after the deadzone
            final_val = sign * ((std::abs(raw_val) - threshold) / remaining_throw) * 32767.0;
        } else {
            final_val = 0.0;
        }

        // 3. Apply Per-Slot Expo on the smoothed signal
        double expo_val = (double)state.expo_values[i];
        if (std::abs(expo_val) > 0.1 && std::abs(final_val) > 0.1) {
            double x = final_val / 32767.0;
            double k = expo_val / 100.0;
            // Cubic Expo: k*x^3 + (1-k)*x
            final_val = (k * (x * x * x) + (1.0 - k) * x) * 32767.0;
        }

        state.tuned_slots[i] = (int16_t)std::clamp(final_val, -32768.0, 32767.0);
    }
}
