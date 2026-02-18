#!/bin/bash
echo "========================================"
echo "   xACE Game Controller Setup (FINAL)"
echo "========================================"

cd /tmp
wget -q https://codeberg.org/fabiscafe/game-devices-udev/archive/main.zip -O main.zip
unzip -q main.zip

sudo cp game-devices-udev/*.rules /etc/udev/rules.d/
echo 'uinput' | sudo tee /etc/modules-load.d/uinput.conf > /dev/null

# === NEW: passwordless udevadm trigger for your app ===
echo "user ALL=(ALL) NOPASSWD: /usr/bin/udevadm" | sudo tee /etc/sudoers.d/99-udev-no-pass > /dev/null
sudo chmod 440 /etc/sudoers.d/99-udev-no-pass

sudo udevadm control --reload-rules
sudo udevadm trigger

rm -rf game-devices-udev main.zip

echo "✅ SUCCESS! All controller support + passwordless USB wake installed."
echo "You only need to run this script once per Pi."
echo "Now reboot with: sudo reboot"
