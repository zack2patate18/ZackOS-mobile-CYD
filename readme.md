# ZackOS Mobile CYD Version

A near-real phone interface for Cyd, with multiple apps and a custom app store (maybe).

## Description

CydPhone aims to simulate a real phone experience on Cyd.
It supports:

- Making and receiving calls (as close as possible to real calls).

- Multiple apps with interactive UI.

- A custom app store to download apps from a server to the SD card.

Goal: Create a fully functional mini-phone prototype for Cyd.

## Features

- Phone-like interface.

- Realistic call system.

- Multiple built-in apps.

- Custom app store to download apps to the SD card.

- Touchscreen support and intuitive navigation.

## Installation

Clone the repo:

```bash
git clone https://github.com/userpetitpain/ZackOS-mobile-CYD.git
cd ZackOS-mobile-CYD
```

Install esptool.py
```bash
pip install esptool
```

Upload

```bash
esptool.py --chip esp32 --port /dev/ttyUSB0 --baud 460800 write_flash -z 0x1000 bin/firmware.bin
```

## License

This project is released under The Unlicense – do whatever you want with it.
For more info, see [https://unlicense.org/](https://unlicense.org/)