# PROJECT ACE: Adaptive Control Engine

ACE is a high-performance ground control station and flight logic engine designed for the Raspberry Pi 4. It features a custom **420,000 baud** CRSF link, real-time telemetry parsing (NOMAD), and a high-frequency (1kHz) stick input loop.

## Key Features
* **NOMAD Telemetry:** Real-time parsing of CRSF frames (Link Stats, Battery, GPS).
* **High-Speed Link:** 420k Baud synchronization optimized for ELRS/Crossfire.
* **CPU Pinning:** Dedicated core isolation for hardware threads to ensure 0ms jitter.
* **Adaptive Theme Engine:** Dynamic UI scaling and color profiles (Cyan, Red, Green, Amber).
* **Split-Mixer Logic:** Advanced control surface mixing for complex airframes.

## Hardware Requirements
* **Host:** Raspberry Pi 4 / Compute Module 4
* **UART:** ttyAMA5 (GPIO 12/13)
* **Protocol:** CRSF (ExpressLRS / Crossfire)

## Build Instructions
```bash
mkdir build && cd build
cmake ..
make -j4
export DISPLAY=:0
export QT_QPA_PLATFORM=xcb
sudo -E /home/user/ACE/build/ACE

```
```
