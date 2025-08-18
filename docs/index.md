---
hide:
  - navigation
  - toc
---

# MoonLight

<div class="moonlight-hero">
  <div class="hero-title">🌙 MoonLight</div>
  <div class="hero-subtitle">
    Professional ESP32 LED control platform for stunning 2D & 3D light effects
  </div>
  <div class="hero-description">
    Drive up to 12,288 LEDs at 120 FPS with web-based control. Built for creators, makers, and lighting professionals.
  </div>

  <div class="moonlight-buttons">
    <a href="gettingstarted/overview/" class="moonlight-btn">🚀 Get Started</a>
    <a href="https://github.com/MoonModules/MoonLight" class="moonlight-btn moonlight-btn-secondary" >⭐ GitHub</a>
    <a href="https://discord.gg/TC8NSUSCdV" class="moonlight-btn moonlight-btn-secondary">💬 Discord</a>
  </div>
</div>

<div class="moonlight-stats">
  <div class="moonlight-stat">
    <span class="moonlight-stat-number">12,288</span>
    <div class="moonlight-stat-label">LEDs at 120 FPS</div>
  </div>
  <div class="moonlight-stat">
    <span class="moonlight-stat-number">65,000</span>
    <div class="moonlight-stat-label">Max LEDs Supported</div>
  </div>
  <div class="moonlight-stat">
    <span class="moonlight-stat-number">16</span>
    <div class="moonlight-stat-label">Parallel LED Strips</div>
  </div>
  <div class="moonlight-stat">
    <span class="moonlight-stat-number">120</span>
    <div class="moonlight-stat-label">Virtual Strips</div>
  </div>
  <div class="moonlight-stat">
    <span class="moonlight-stat-number">3D</span>
    <div class="moonlight-stat-label">Effects Engine</div>
  </div>
</div>

<div class="features-section">
  <div class="section-header">
    <div class="section-title">Powerful Features</div>
    <div class="section-subtitle">Everything you need for professional LED control and IoT development</div>
  </div>

  <div class="features-grid">
    <div class="feature-card">
      <div class="feature-icon">🚀</div>
      <h3 class="feature-title">High Performance</h3>
      <p class="feature-description">Drive up to 12,288 LEDs at 120 FPS, or scale to 65K+ LEDs at lower framerates. Optimized for ESP32 hardware with parallel processing capabilities.</p>
    </div>
    
    <div class="feature-card">
      <div class="feature-icon">🌐</div>
      <h3 class="feature-title">Web Interface</h3>
      <p class="feature-description">Intuitive browser-based control panel built with SvelteKit. Control your lights from any device on your network - no apps required.</p>
    </div>
    
    <div class="feature-card">
      <div class="feature-icon">🎨</div>
      <h3 class="feature-title">3D Effects</h3>
      <p class="feature-description">Advanced node-based system supporting 2D and 3D effects with virtual and physical layers. Create stunning visual experiences with professional-grade tools.</p>
    </div>
    
    <div class="feature-card">
      <div class="feature-icon">🔧</div>
      <h3 class="feature-title">IoT Platform</h3>
      <p class="feature-description">Built on MoonBase - a complete IoT framework. Create custom applications from simple smart plugs to complex greenhouse management systems.</p>
    </div>
    
    <div class="feature-card">
      <div class="feature-icon">🎭</div>
      <h3 class="feature-title">DMX Support</h3>
      <p class="feature-description">Control DMX lights via Art-Net protocol. Mix LED strips, panels, and professional lighting equipment seamlessly.</p>
    </div>
    
    <div class="feature-card">
      <div class="feature-icon">🔄</div>
      <h3 class="feature-title">WLED Compatible</h3>
      <p class="feature-description">Audio sync and compatibility with WLED ecosystem. Easy migration path for existing WLED users with enhanced capabilities.</p>
    </div>
  </div>
</div>

<div class="performance-section">
  <div class="performance-title">🚀 Performance Metrics</div>
  <p style="font-size: 1.1rem; opacity: 0.8; position: relative; z-index: 1;">Industry-leading specifications for demanding applications</p>
  
  <div class="performance-grid">
    <div class="performance-metric">
      <span class="performance-number">120</span>
      <div class="performance-label">Frames Per Second</div>
    </div>
    <div class="performance-metric">
      <span class="performance-number">12K</span>
      <div class="performance-label">LEDs (High FPS)</div>
    </div>
    <div class="performance-metric">
      <span class="performance-number">65K</span>
      <div class="performance-label">Max LEDs</div>
    </div>
    <div class="performance-metric">
      <span class="performance-number">16</span>
      <div class="performance-label">Parallel Strips</div>
    </div>
  </div>
</div>

---

## 🌟 What is MoonLight?

MoonLight is software which runs on [ESP32 microcontrollers](https://www.espressif.com/en/products/socs/esp32) providing a web-browser interface to control lights in particular and any IOT service in general. ESP32 devices can be connected to your local network via WiFi or Ethernet, MoonLight can be controlled via a web browser selecting the IP-address or network name of the device.

## 🚀 High Performance LED Control

- **12,288 LEDs at 120 FPS** - Maximum performance for demanding applications
- **65,000+ LEDs** - Scale to massive installations at lower framerates  
- **16 Parallel Strips** - Drive multiple LED strips simultaneously
- **120 Virtual Strips** - Advanced mapping and control capabilities

## 🌐 Web-Based Interface

MoonLight can be installed on ESP32 by means of a web installer or if you are a developer, via Visual Studio Code with the PlatformIO plugin installed.

- Intuitive browser-based control panel built with **SvelteKit**
- Control from any device on your network
- No apps required - works in any modern browser
- Real-time updates and responsive design

## 🎨 Advanced Effects Engine

- **2D & 3D Effects** - Professional-grade visual effects
- **Node-based System** - Flexible effect creation and modification  
- **Virtual & Physical Layers** - Advanced mapping capabilities
- **WLED Compatibility** - Audio sync and ecosystem integration

## 🔧 Complete IoT Platform

MoonLight's core functionality is called **MoonBase**, Lighting specific functionality is called **MoonLight**. MoonLight is a standalone product for end users controlling lights but can also be used to create custom IoT services:

- Create GitHub forks for custom applications
- Build anything from smart plugs to greenhouse management
- Utilize ESP32 GPIO ports and network protocols
- Extensible architecture for developers

### 🏗️ Architecture

MoonLight's core functionality is called **MoonBase**, Lighting specific functionality is called **MoonLight**. MoonLight is a standalone product for end users controlling lights but can also be used to create custom IoT services:

- **Fork and customize** - Create GitHub forks for custom applications
- **Extensible platform** - Build anything from smart plugs to greenhouse management
- **Hardware interfaces** - Utilize ESP32 GPIO ports and network protocols
- **Modular design** - Add or remove features as needed

### 🎯 Installation Options

<div class="moonlight-buttons">
  <a href="gettingstarted/installation/" class="moonlight-btn">📱 Web Installer</a>
  <a href="gettingstarted/hardware/" class="moonlight-btn moonlight-btn-secondary">🔧 Hardware Guide</a>
</div>

**Easy Installation Methods:**
- **Web Installer** - Flash your ESP32 device directly from your browser
- **Developer Setup** - Visual Studio Code with PlatformIO plugin for customization

---

## 📊 Technical Specifications

| Feature | Specification | Details |
|---------|---------------|---------|
| **Max LEDs (High FPS)** | 12,288 @ 120 FPS | Optimal for real-time applications |
| **Max LEDs (Standard)** | 65,000+ @ lower FPS | Scale to massive installations |
| **Parallel Strips** | Up to 16 strips | Simultaneous control |
| **Virtual Strips** | Up to 120 strips | Advanced mapping capabilities |
| **Protocols** | WiFi, Ethernet, Art-Net, DMX | Complete connectivity |
| **Hardware** | ESP32 microcontrollers | Industry-standard platform |
| **Interface** | SvelteKit web application | Modern, responsive UI |
| **Effects Engine** | 2D & 3D support | Professional-grade visuals |

---

## 🚀 Quick Start Guide

<div class="features-grid" style="margin-top: 2rem;">
  <div class="feature-card scale-in">
    <div class="feature-icon">1️⃣</div>
    <h3 class="feature-title">Install</h3>
    <p class="feature-description">Use our web installer to flash MoonLight directly to your ESP32 device. No complex setup required.</p>
    <div style="margin-top: 1rem;">
      <a href="gettingstarted/installation/" class="moonlight-btn" style="font-size: 0.9rem; padding: 0.8rem 1.5rem;">Start Installation</a>
    </div>
  </div>
  
  <div class="feature-card scale-in">
    <div class="feature-icon">2️⃣</div>
    <h3 class="feature-title">Connect</h3>
    <p class="feature-description">Configure your WiFi or Ethernet connection. MoonLight creates its own access point for easy initial setup.</p>
    <div style="margin-top: 1rem;">
      <a href="wifi/sta/" class="moonlight-btn" style="font-size: 0.9rem; padding: 0.8rem 1.5rem;">WiFi Setup</a>
    </div>
  </div>
  
  <div class="feature-card scale-in">
    <div class="feature-icon">3️⃣</div>
    <h3 class="feature-title">Control</h3>
    <p class="feature-description">Open your browser to the device's IP address and start creating stunning LED effects immediately.</p>
    <div style="margin-top: 1rem;">
      <a href="moonlight/overview/" class="moonlight-btn" style="font-size: 0.9rem; padding: 0.8rem 1.5rem;">Learn Effects</a>
    </div>
  </div>
</div>

---

## 🌟 Key Features & Capabilities

### 🎮 MoonLight Features  
- **Multi-light Control** - LED strips, panels, and DMX equipment
- **Art-Net Protocol** - Professional lighting network support
- **Virtual & Physical Layers** - Advanced effect mapping
- **Node-based System** - Flexible effect creation and modification
- **Real-time Performance** - Live monitoring and adjustment

### ⚙️ MoonBase Functionality
- **Device Management** - Monitor and control multiple devices
- **WiFi Management** - Easy network configuration  
- **Firmware Updates** - Over-the-air update system
- **System Status** - Real-time device monitoring
- **File Manager** - Direct device file access

## 🚀 Quick Start

1. **Install via Web Installer** - Flash your ESP32 device easily
2. **Connect to Network** - Configure WiFi or Ethernet  
3. **Access Interface** - Open browser to device IP address
4. **Start Creating** - Design and deploy your LED effects

---

<div class="cta-section">
  <div class="cta-title">Ready to Get Started?</div>
  <div class="cta-subtitle">
    Join thousands of creators using MoonLight for their LED projects
  </div>
  
  <div class="moonlight-buttons">
    <a href="gettingstarted/overview/" class="moonlight-btn">🚀 Get Started Now</a>
    <a href="https://github.com/MoonModules/MoonLight/releases" class="moonlight-btn moonlight-btn-secondary">📦 Download Latest</a>
  </div>
</div>

---

## 📈 Project Status & Roadmap

!!! info "Current Release Status"
    **Version 0.5.8** - Developer release available now
    
    **Version 0.6.0** - End-user release expected **September 2025**
    
    ✨ **Coming in 0.6.0:**
    - Presets controller pad
    - User-friendly installer  
    - Physical and virtual driver for large displays
    - WLED audio sync integration
    - Enhanced web interface

**Project Links:**
- 📋 [Kanban Board](https://github.com/users/MoonModules/projects/2/) - Track development progress
- 🔄 [Evolution Story](https://moonmodules.org/Star-Mod-Base-Light-Moon-Svelte-Live) - From StarMod to MoonLight
- 📚 [Complete Documentation](https://moonmodules.org/MoonLight/) - Full technical documentation

## 📈 Project Status

- **Latest Release**: [v0.5.8](https://github.com/MoonModules/MoonLight/releases) (Developer Preview)
- **Progress Tracking**: [Kanban Board](https://github.com/users/MoonModules/projects/2/)
- **Documentation**: [Complete Docs](https://moonmodules.org/MoonLight/)
- **Evolution**: [From StarMod to MoonLight](https://moonmodules.org/Star-Mod-Base-Light-Moon-Svelte-Live)

---

## 🤝 Community & Support

<div class="features-grid" style="margin-top: 2rem;">
  <div class="feature-card slide-in-left">
    <div class="feature-icon">💬</div>
    <h3 class="feature-title">Discord Community</h3>
    <p class="feature-description">Join our active Discord server for real-time help, project sharing, and community discussions.</p>
    <div style="margin-top: 1rem;">
      <a href="https://discord.gg/TC8NSUSCdV" class="moonlight-btn" style="font-size: 0.9rem; padding: 0.8rem 1.5rem;">Join Discord</a>
    </div>
  </div>
  
  <div class="feature-card slide-in-right">
    <div class="feature-icon">🗨️</div>
    <h3 class="feature-title">Reddit Community</h3>
    <p class="feature-description">Connect with fellow makers and share your LED projects on our dedicated subreddit.</p>
    <div style="margin-top: 1rem;">
      <a href="https://reddit.com/r/moonmodules" class="moonlight-btn" style="font-size: 0.9rem; padding: 0.8rem 1.5rem;">Visit Reddit</a>
    </div>
  </div>
</div>

**MoonLight is a [MoonModules.org](https://moonmodules.org) project** created by lighting enthusiasts who also developed WLED MM and contribute to the broader WLED ecosystem.

---

- **GitHub**: [Star the Repository](https://github.com/MoonModules/MoonLight) ⭐
- **Discord**: [Join MoonLight Community](https://discord.gg/TC8NSUSCdV) 💬  
- **Reddit**: [r/moonmodules](https://reddit.com/r/moonmodules) 🗨️
- **Discussions**: [GitHub Discussions](https://github.com/MoonModules/MoonLight/discussions)

## 👨‍💻 For Developers

**We're looking for contributors!** Help us build the future of LED control:

**Current Opportunities:**
- 🎨 **New layouts, effects, and modifiers** - Expand the creative possibilities
- 🔄 **CI/CD automations** - Improve development workflow  
- 🎹 **MIDI controller integration** - Professional control interfaces
- 🌐 **Web installer improvements** - Simplify user onboarding
- 🎯 **UI/UX enhancements** - Make the interface even better
- 📖 **Documentation and tutorials** - Help users succeed

**Get Involved:**
- 💬 [Developer Discord Channel](https://discord.com/channels/700041398778331156/1203994211301728296)
- 🗨️ [Reddit Developer Discussions](https://reddit.com/r/moonmodules)
- 📋 [GitHub Issues](https://github.com/MoonModules/MoonLight/issues)
- 🔄 [Pull Requests Welcome](https://github.com/MoonModules/MoonLight/pulls)

---

## ❤️ Support the Project

**Love MoonLight?** Help us continue development:

<div class="moonlight-buttons">
  <a href="https://github.com/MoonModules/MoonLight" class="moonlight-btn">⭐ Star on GitHub</a>
  <a href="https://github.com/sponsors/ewowi" class="moonlight-btn moonlight-btn-secondary">💰 GitHub Sponsors</a>
  <a href="https://www.paypal.com/donate?business=moonmodules@icloud.com" class="moonlight-btn moonlight-btn-secondary">💵 PayPal</a>
</div>

---

## 📄 License & Acknowledgments

**License**: GPL-v3

**Built with amazing open source projects:**
- [ESP32-sveltekit](https://github.com/theelims/ESP32-sveltekit) - Base framework and inspiration
- [PsychicHttp](https://github.com/hoeken/PsychicHttp) - High-performance web server
- [FastLED](https://github.com/FastLED/FastLED) - Robust LED driver library
- [ArduinoJson](https://github.com/bblanchon/ArduinoJson) - JSON processing
- [I2SClocklessLedDriver](https://github.com/hpwit/I2SClocklessLedDriver) - Parallel LED control
- [I2SClocklessVirtualLedDriver](https://github.com/hpwit/I2SClocklessVirtualLedDriver): Virtual Driver
- [ESPLiveScript](https://github.com/hpwit/ESPLiveScript) - Scripts
- [WLED-sync](https://github.com/netmindz/WLED-sync): Audio

---

**⚠️ Disclaimer**: Using this software is at your own responsibility. While we strive for reliability, contributors are not liable for any issues including but not limited to spontaneous combustion of LED strips, houses, or the inevitable heat death of the universe! 🔥🌌

*Ready to light up your world? [Get started now!](gettingstarted/overview/)* 🌙✨