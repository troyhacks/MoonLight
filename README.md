# 🌙 MoonLight

<div align="center">
  <img width="350" src="https://github.com/user-attachments/assets/278ed02e-7f7a-497c-80ee-089486ddf379" alt="MoonLight Logo" />
  
  **The open-source lighting platform that scales from art installations to professional stages**
  
  Drive lots of lights or thousands of LEDs with stunning effects running on ESP32 microcontrollers. For creators who demand professional results without enterprise costs.

  [![GitHub Stars](https://img.shields.io/github/stars/MoonModules/MoonLight?style=social)](https://github.com/MoonModules/MoonLight)
  [![Discord](https://img.shields.io/discord/YOUR_DISCORD_ID?logo=discord&label=Discord)](https://discord.gg/TC8NSUSCdV)
  [![License: GPL v3](https://img.shields.io/badge/License-GPLv3-blue.svg)](https://www.gnu.org/licenses/gpl-3.0)
  
  [📚 Documentation](https://moonmodules.org/MoonLight/) • [💬 Discord Community](https://discord.gg/TC8NSUSCdV) • [🗨️ Reddit](https://reddit.com/r/moonmodules)
</div>

---

## ✨ Key Features

| Feature | Capability |
|---------|-----------|
| **LED Performance** | 12,288 LEDs @ 120 FPS with 48 parallel outputs |
| **Max Scale** | Up to 128K LEDs with 120 parallel outputs |
| **Effects Engine** | Advanced 3D layered effects system |
| **Protocols** | DMX, Art-Net, WiFi integration |
| **Platform** | ESP32 + Svelte 5 web interface |
| **License** | GPL v3 - Free for commercial use |

### 🎯 Why Choose MoonLight?

**MoonLight bridges the gap between hobbyist lighting projects and professional lighting systems.** Get enterprise-grade performance and flexibility without the enterprise price tag.

- ⚡ **High Performance** - 12K+ LEDs at 120 FPS with FastLED + I2S drivers
- 🎨 **3D Effects Engine** - Stunning visuals for 1D strips, 2D panels, 3D cubes, and custom layouts
- 🏗️ **Flexible Setups** - From simple strips to complex installations and DMX fixtures
- 🌐 **Modern IoT** - ESP32-powered with responsive Svelte 5 interface
- 🎭 **Professional Integration** - DMX/Art-Net support for stage lighting
- 💝 **Open Source** - GPL v3 licensed, budget-friendly, community-driven

---

## 🚀 Quick Start

### 1️⃣ Flash & Go
Use our [web installer](https://moonmodules.org/MoonLight/gettingstarted/installation/) to flash MoonLight directly to your ESP32. No complex setup needed.

### 2️⃣ Connect & Configure
Built-in access point makes WiFi setup effortless. Configure through the intuitive web interface.

### 3️⃣ Create & Enjoy
Start creating stunning effects immediately on both mobile and desktop.

**[📖 Full Installation Guide](https://moonmodules.org/MoonLight/gettingstarted/overview/)**

---

## 🛠️ Built With Modern Technologies

<table>
<tr>
<td width="50%">

**Firmware**
- 🔧 ESP-IDF 5 - Modern ESP32 framework
- 💡 FastLED 3.10 - Industry-standard LED library
- 📡 PsychicHTTP 1.21 - High-performance web server
- 📊 ArduinoJson 7 - Advanced JSON processing

</td>
<td width="50%">

**Interface**
- ⚡ Svelte 5 - Lightning-fast reactive UI
- 🎨 DaisyUI 5 - Modern component library
- 🎯 Tailwind 4 - Utility-first CSS
- 📱 Mobile & Desktop responsive

</td>
</tr>
</table>

---

## 🎯 Perfect For

- 🎨 **Artists & Creators** - Bring your vision to life without breaking the bank
- 🏛️ **Small Venues** - Professional lighting for theaters, events, and installations
- 🔧 **Makers & Hobbyists** - Advanced features with user-friendly interfaces
- 💼 **Professionals** - Integrate into existing lighting systems with standard protocols
- 🏫 **Educational** - Open source platform perfect for learning and teaching

---

## 📊 Technical Specifications

### LED Control
- **Performance**: 12,288 LEDs @ 120 FPS (recommended max)
- **Maximum**: 128K LEDs with 120 parallel outputs
- **Outputs**: 16-48 parallel LED strips (recommended)
- **Drivers**: FastLED + I2S for high-speed parallel processing

### Effects & Layouts
- **Dimensions**: 1D strips, 2D panels, 3D cubes and custom layouts
- **Effects**: Layered effects system with modifiers
- **Fixtures**: DMX lights (PAR lights, Light Bars, Moving Heads)

### Connectivity
- **Protocols**: DMX, Art-Net, WiFi, Ethernet
- **Platform**: ESP32 with modern web interface
- **Integration**: Professional lighting system compatible

---

## 📈 Release Roadmap

### Current Status
**Version 0.5.9.3** - Developer preview available now

### Version 0.6.0 - October 2025
**The user-friendly baseline release**

✨ **Coming Features:**
- Plug & Play Experience
- Preset Controller for quick effect management
- Enhanced Web Installer with one-click flashing
- Large Display Drivers for massive installations
- WLED Audio Sync for music synchronization
- Refined Interface with polished UX

**From 0.6.0 Forward:** Community-driven development focused on ease of use, more effects, and expanded hardware support.

---

## 🏗️ Architecture & Flexibility

MoonLight is built on **MoonBase**, our complete IoT framework:

- 🎮 **For LED Enthusiasts** - Complete lighting solution out of the box
- 🔧 **For Developers** - Fork MoonBase to create custom IoT applications
- 🏭 **For Integrators** - Embed into larger systems using standard protocols
- 🔌 **Modular Design** - Add or remove features as needed
- 🔩 **GPIO Access** - Full ESP32 hardware interface

---

## 🤝 Community & Support

### Get Help & Share Your Creations

- 💬 **[Discord Community](https://discord.gg/TC8NSUSCdV)** - Real-time support & project sharing
- 🗨️ **[Reddit](https://reddit.com/r/moonmodules)** - Discussion and showcase
- 📋 **[GitHub Issues](https://github.com/MoonModules/MoonLight/issues)** - Bug reports and feature requests
- 📚 **[Documentation](https://moonmodules.org/MoonLight/)** - Complete technical guides

### Contributing

We welcome contributions! Whether it's:
- 🐛 Bug fixes and improvements
- ✨ New effects and features
- 📝 Documentation updates
- 🎨 UI/UX enhancements
- 🔧 Hardware support

Check our [contributing guidelines](CONTRIBUTING.md) to get started.

---

## ❤️ Support the Project

MoonLight is free and open source. Help us continue building amazing tools:

- ⭐ **Star this repo** - Show your support
- 💰 **[GitHub Sponsors](https://github.com/sponsors/ewowi)** - Recurring support
- 💵 **[PayPal](https://www.paypal.com/donate?business=moonmodules@icloud.com)** - One-time donation

---

## 📦 Installation

### Prerequisites
- ESP32 development board
- LED strips (WS2812B, SK6812, APA102, etc.)
- USB cable for flashing

### Quick Install

**Option 1: Web Installer (Recommended)**
1. Visit our [web installer](https://moonmodules.org/MoonLight/gettingstarted/installation/)
2. Connect your ESP32 via USB
3. Click "Install" and follow the prompts

**Option 2: Manual Flashing**
1. Download the [latest release](https://github.com/MoonModules/MoonLight/releases)
2. Flash using esptool.py or ESP Flash Tool
3. Configure via the web interface

**[📖 Detailed Installation Guide](https://moonmodules.org/MoonLight/gettingstarted/overview/)**

---

## 📄 License & Credits

**License:** GPL-v3 - Free for personal and commercial use

### Built on Amazing Open Source Projects

- [ESP32-sveltekit](https://github.com/theelims/ESP32-sveltekit) - Foundation framework
- [PsychicHttp](https://github.com/hoeken/PsychicHttp) - High-performance web server
- [FastLED](https://github.com/FastLED/FastLED) - Industry-standard LED library
- [ArduinoJson](https://github.com/bblanchon/ArduinoJson) - Advanced JSON processing
- [I2SClocklessLedDriver](https://github.com/hpwit/I2SClocklessLedDriver) - Parallel LED control
- [I2SClocklessVirtualLedDriver](https://github.com/hpwit/I2SClocklessVirtualLedDriver) - Virtual driver
- [ESPLiveScript](https://github.com/hpwit/ESPLiveScript) - Scripts
- [WLED-sync](https://github.com/netmindz/WLED-sync) - Audio synchronization

---

## 🌙 About MoonModules

MoonLight is a [MoonModules.org](https://moonmodules.org) project - Created by the lighting enthusiasts behind WLED-MM.

**Our Mission:** Make professional-grade LED control accessible to everyone, from hobbyists to professionals.

---

## ⚠️ Disclaimer

Using this software is at your own risk. While we strive for quality, this software is not bug-free. Contributors to this repository are not liable for any issues, including but not limited to spontaneous combustion of LED strips, hardware damage, or the inevitable heat death of the universe. 🔥🕺🌌

---

<div align="center">

**Ready to light up your world?** 🌙✨

[Get Started](https://moonmodules.org/MoonLight/gettingstarted/overview/) • [Download Latest](https://github.com/MoonModules/MoonLight/releases) • [Join Discord](https://discord.gg/TC8NSUSCdV)

Made with ❤️ by the MoonModules community

</div>
