# Contributing to K3N Armoni Composer / LoopStation

First of all, thank you for your interest in contributing to **K3N Armoni Composer**! 🎉 We welcome contributions from developers, audio engineers, sound designers, and musicians of all skill levels.

---

## 🛠️ How Can You Contribute?

Here are some great ways to get involved:
1. **Audio DSP & Engine Enhancements**: Help us implement BPM quantization, metronome sync, or lock-free command queues (see [ROADMAP.md](ROADMAP.md)).
2. **Cross-Platform Support**: Implement low-level input backends for macOS (*IOKit HID Manager*) or Linux (*libevdev*).
3. **UI / UX & Visuals**: Add animated loop playheads, sleek waveform renderers, or responsive UI enhancements.
4. **Bug Reports & Feedback**: Test the app with different USB hubs, keyboards, and ASIO audio interfaces and submit issue reports.

---

## 🚀 Development Workflow

1. **Fork the Repository**: Click the **Fork** button on GitHub.
2. **Clone your fork locally**:
   ```bash
   git clone --recursive https://github.com/YOUR_USERNAME/k3n-armoni-composer.git
   cd k3n-armoni-composer
   ```
   *(If you already cloned without `--recursive`, run `git submodule update --init --recursive`)*

3. **Create a Feature Branch**:
   ```bash
   git checkout -b feature/amazing-new-feature
   ```

4. **Build and Test**:
   ```bash
   cmake -B build -S . -DCMAKE_BUILD_TYPE=Release
   cmake --build build --config Release
   ```

5. **Commit your changes**:
   Write clear, concise commit messages following standard conventions:
   ```bash
   git commit -m "feat(audio): add metronome beat clock generator"
   ```

6. **Push and Open a Pull Request**:
   Push to your fork and submit a PR to the `main` branch with a clear description of your changes.

---

## 📜 Coding Guidelines

- **C++ Standard**: C++17.
- **Real-Time Safety**: Never allocate heap memory (`new`/`malloc`/`std::vector::resize`) or perform file I/O within the real-time audio callback (`getNextAudioBlock()`).
- **Naming Conventions**: Follow JUCE PascalCase for classes (`AudioEngine`) and camelCase for methods/variables (`processAudioBlock`).

---

## 💬 Community & Questions

Feel free to open a [GitHub Discussion](https://github.com/YOUR_GITHUB/k3n-armoni-composer/discussions) or join our community threads for architectural questions, feature proposals, or pairing sessions.
