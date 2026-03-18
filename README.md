# Project ACE — Adaptive Control Engine

**ACE** is a powerful, ultra-responsive ground control system built specifically for the Raspberry Pi 4.

It delivers extremely fast and precise control for RC planes, drones, helicopters, and cars using ExpressLRS or Crossfire radios.

### What Makes ACE Special

- **Lightning-Fast Response**  
  Runs at 1000 times per second (1000 Hz) with almost zero delay — giving you the smoothest and most precise stick feel possible.

- **Super Fast Radio Link**  
  Uses a custom high-speed 420,000 baud connection to your ExpressLRS or Crossfire module for the lowest possible latency.

- **Smart Mixing Engine**  
  Advanced mixing system that handles even complex aircraft and control surfaces with ease.

- **Beautiful & Easy-to-Use Interface**  
  Clean, modern touchscreen-friendly display with multiple color themes (Cyan, Red, Green, Amber) that you can change on the fly.

- **Full Support for Multiple Vehicles**  
  Dedicated screens and tuning pages for:
  - Airplanes
  - (soon)Drones / Multirotors
  - (soon)Helicopters
  - (soon)Cars

  Each vehicle gets its own dashboard, easy 5-page tuning wizards, and custom RC mapping.

- **Highly Tunable Controls**  
  Full adjustment for deadzone, expo, rates, endpoints, and more — all with real-time feedback.

- **Rock-Solid Performance**  
  Designed from the ground up to run smoothly with minimal jitter, even under heavy load.

### Hardware You Need

- Raspberry Pi 4 or Compute Module 4
- ExpressLRS or Crossfire radio module connected to GPIO 12 & 13
- Touchscreen or HDMI monitor

### How to Install & Run

1. Open a terminal on your Raspberry Pi
2. Run these commands:

```bash
mkdir build && cd build
cmake ..
make -j4

export DISPLAY=:0
export QT_QPA_PLATFORM=xcb
sudo -E ./ACE
