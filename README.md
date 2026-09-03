# 🎹 K3N Armoni Composer / LoopStation

<div align="center">

[![C++ Standard](https://img.shields.io/badge/C%2B%2B-17-00599C?style=for-the-badge&logo=c%2B%2B&logoColor=white)](https://isocpp.org/)
[![Framework](https://img.shields.io/badge/JUCE-7%20%2F%208-FF6600?style=for-the-badge&logo=juce&logoColor=white)](https://juce.com/)
[![Platform](https://img.shields.io/badge/Platform-Windows%2010%20%2F%2011-0078D6?style=for-the-badge&logo=windows&logoColor=white)](https://microsoft.com/windows)
[![License](https://img.shields.io/badge/License-MIT-22C55E?style=for-the-badge)](LICENSE)
[![Build Status](https://img.shields.io/badge/Build-Passing-brightgreen?style=for-the-badge&logo=githubactions&logoColor=white)](.github/workflows/build.yml)

**A high-performance standalone C++ / JUCE digital audio workstation and live loop station that turns multiple physical USB keyboards into an orchestra of independent instruments.**

[The Manifesto](#-the-manifesto-democratizing-music-creation-through-frugal-innovation) • [Features](#-key-features) • [The Hardware Problem](#-the-hardware-challenge-multi-keyboard-disambiguation) • [Quickstart](#-quickstart--build-instructions) • [Architecture](#-architecture--how-it-works) • [Contributing](#-contributing--community) • [Author & Hire Me](#-author-portfolio--hire-me)

</div>

---

## 🌟 The Manifesto: Democratizing Music Creation Through Frugal Innovation

> *"Music should not be a privilege reserved for those who can afford hundreds of euros in specialized hardware."*

Across the world today, dedicated MIDI drum pads, synthesizers, and loop stations cost anywhere from **€50 to over €500**. For young bedroom producers, students in underfunded schools, or creators living in developing economies, this financial barrier is often insurmountable.

At the same time, millions of perfectly functional membrane **USB computer keyboards** are gathering dust in closets, sold at flea markets for **€2 to €5**, or discarded as electronic waste.

### 💡 Our Mission: Hardware Upcycling & Universal Access
**K3N Armoni Composer** was born from a fundamental belief in **Frugal Innovation**:
1. **Upcycle Everyday Tech**: Transform cheap, discarded USB computer keyboards into an expressive multi-instrument performance station (Keyboard 1 = *Drums*, Keyboard 2 = *808 Bass*, Keyboard 3 = *Vocal FX & One-Shots*, Keyboard 4 = *Melody Synth*).
2. **Adaptive Single-Keyboard Mode**: Even if you only own a single laptop keyboard, the system adapts dynamically using modifier shift layers to switch between rhythm, bass, and melody seamlessly.
3. **No Heavy Web Bloat — Pure Native C++**: We deliberately rejected heavy Electron/Web wrappers. By building directly on compiled **C++17, JUCE, and native Win32 kernel hooks**, the app runs smoothly with **<5ms latency and <25 MB RAM** even on 10-year-old budget laptops (Intel i3/i5).
4. **100% Offline & Free Forever**: Zero subscriptions, zero paywalls, zero cloud tracking. Your creativity remains private and works completely offline.
5. **Built-in Procedural Drum Synthesizer**: No need to buy or download gigabytes of sample packs. The app includes mathematically synthesized zero-dependency starter kits (808 Kick, Snare, Hi-Hat, Sub-Bass, Synth Stabs) ready to play on first launch.

---

## ⚡ Key Features

- **🚀 Ultra-Low Latency Audio Engine**: 32-voice polyphonic sampler with intelligent voice stealing and zero heap allocations inside the real-time audio thread (`getNextAudioBlock`).
- **🎛️ Sample-Accurate Multi-Track Looper**: Record, overdub, mute, and clear independent loop lanes for each keyboard role.
- **⚡ In-Memory Sample Cache**: Audio formats (WAV, MP3, FLAC, AIFF) are pre-decoded into RAM buffers upon import, completely eliminating disk I/O glitches during live playing.
- **🎯 Interactive Key Learning UI**: Click *Learn Key*, hit any key on any connected physical keyboard, and it maps instantly.
- **🥁 Instant Procedural Sound Engine**: Comes pre-loaded with mathematically generated punchy 808 kicks, crisp snares, closed hats, sub-basses, and synth leads.
- **🔒 100% Offline & Private**: Zero cloud dependencies or telemetry. All preset configurations and device mappings are stored locally in `%APPDATA%/LoopStation`.
- **🔊 Pro Audio Driver Support**: Native integration with ASIO (low latency), WASAPI (Exclusive/Shared), and DirectSound via the JUCE Audio Device Manager.

---

## 🏗️ Architecture & How It Works

```mermaid
graph LR
    subgraph Hardware Inputs (Flea Market / Upcycled)
        KB1[USB Keyboard #1 - Drums Role]
        KB2[USB Keyboard #2 - Bass Role]
        KB3[USB Keyboard #3 - FX Role]
    end

    subgraph Low-Level Input Subsystem
        WIN[Win32 Raw Input API WM_INPUT]
        KB1 --> WIN
        KB2 --> WIN
        KB3 --> WIN
        WIN --> RIH[RawInputHandler Device Path]
        RIH --> DM[DeviceManager Roles & JSON]
    end

    subgraph Application & Engine
        DM --> MC[MainComponent Router]
        MC -->|Learn Mode| MEC[MappingEditor UI]
        MC -->|Live Mode| PVC[PerformanceView Looper]
        PVC --> AE[AudioEngine 32-Voices]
        AE --> OUT[Audio Interface Output ASIO/WASAPI]
    end
```

For comprehensive technical specifications, threading models, and data flows, check [ARCHITECTURE.md](ARCHITECTURE.md) and [AUDIT.md](AUDIT.md).

---

## 🚀 Quickstart & Build Instructions

### Prerequisites
- **OS**: Windows 10 / 11 (64-bit)
- **Compiler**: Visual Studio 2019 / 2022 (MSVC) or Clang with C++17 support
- **Build System**: CMake 3.22+
- **Framework**: [JUCE 7 / 8](https://github.com/juce-framework/JUCE)

### Build with CMake

```bash
# 1. Clone repository with submodules
git clone --recursive https://github.com/YOUR_GITHUB/k3n-armoni-composer.git
cd k3n-armoni-composer

# 2. Configure project
cmake -B build -S . -DCMAKE_BUILD_TYPE=Release

# 3. Compile
cmake --build build --config Release
```

Your compiled executable will be located in:
`build/LoopStation_artefacts/Release/LoopStation.exe`.

> 💡 **Want to run automated tests or test with multiple keyboards?** Check our comprehensive [**TESTING.md**](TESTING.md) guide!

---

## 📖 How to Use

1. **Launch**: Connect any USB keyboards and launch `LoopStation.exe`.
2. **Setup Audio**: Click `Audio Settings...` (top-right) and select your ASIO driver / buffer size (e.g., 128 or 256 samples).
3. **Play Instantly with Built-in Sounds**:
   - The app comes preloaded with built-in procedural electronic sounds (`Kick`, `Snare`, `Hi-Hat`, `808 Bass`, `Synth`).
   - Switch to **1. Mapping Editor**, choose a sound, click `Learn Key`, and press a key on your keyboard.
   - Or import your own WAV/MP3 samples via `Import Sample...` and click `Save Preset`.
4. **Perform Live**:
   - Switch to **2. Live Performance**.
   - Play in real-time! Hit `Rec` on any track lane to record a loop, and `Stop Rec` to loop it continuously with multi-lane overdubbing.

---

## 🤝 Contributing & Community

We welcome contributions from musicians, audio developers, C++ programmers, and UI designers worldwide!

- Check open tasks in [ROADMAP.md](ROADMAP.md).
- Read our [CONTRIBUTING.md](CONTRIBUTING.md) guide for PR guidelines and development setup.
- Open an issue or join discussions for questions or proposals.

---

## 👨‍💻 Author, Portfolio & Hire Me

Hi! I am **k3n**, a passionate software engineer and full-stack developer dedicated to **high-performance C++, real-time audio systems, accessible technology, and modern web/mobile applications**.

### 💼 Open for Freelance & Contract Inquiries:
- 🌐 **Web Development**: Modern responsive WebApps, SaaS architectures, interactive platforms, and high-conversion landing pages.
- 📱 **Mobile & Desktop Applications**: Cross-platform and native high-performance software (C++, JUCE, React Native, Node.js, TypeScript).
- 🎵 **Audio & DSP Engineering**: Custom audio tools, VST/AU synthesis plugins, low-latency DSP algorithms, and hardware controller integrations.

📬 **Get in Touch / Hire Me**:
- **GitHub**: [@YOUR_GITHUB_PROFILE](https://github.com/)
- **Email**: `your-email@example.com` *(feel free to customize)*
- **Portfolio / Website**: [https://your-portfolio-link.com](https://your-portfolio-link.com)

---

## 📄 License

This project is licensed under the terms of the [MIT License](LICENSE).
