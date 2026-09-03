# Workspace State: K3N Armoni Composer

Last Updated: **September 2026**

---

## 📌 Project Vision & Manifesto
- High-performance standalone C++17 / JUCE digital audio workstation and live loop station.
- **Mission & Frugal Innovation**: Democratizing music production through hardware upcycling (turning €2-€5 discarded USB computer keyboards into an expressive multi-instrument workstation via Win32 Raw Input hardware disambiguation).
- **Interactive Simulation**: `preview/index.html` featuring full multi-layer overdub loop station, per-layer modulable FX chains (Delay, Reverb, Low-Pass filter), 30+ synthesized instruments, session save/load (`.armoni`), and live WebM audio export.
- **License**: GNU Affero General Public License v3.0 (AGPL-3.0) with attribution & commercial licensing requirements.

---

## 🏗️ Architecture & Component Status

1. **`Source/`**:
   - `RawInputHandler.h/.cpp`: Win32 Raw Input `WM_INPUT` hardware disambiguation up to 16 keyboards.
   - `DeviceManager.h/.cpp`: Per-hardware role assignment and `%APPDATA%` JSON persistence.
   - `MappingEngine.h/.cpp`: Per-key sound mapping and presets.
   - `AudioEngine.h/.cpp`: 32-voice polyphonic sampler, lock-free voice stealing, procedural starter kit, click buffer generator.
   - `MetronomeClock.h/.cpp`: Sample-accurate BPM clock (40-300 BPM), time signatures, lock-free atomic phase sync.
   - `BpmQuantizer.h/.cpp`: Snap-to-grid input quantizer (1/4 to 1/32 triplets), humanize factor, auto bar-snap.
   - `MainComponent.h/.cpp`: JUCE UI router & AudioDeviceManager setup.
   - `MappingEditorComponent.h/.cpp` & `PerformanceViewComponent.h/.cpp`: GUI views.
2. **`CMakeLists.txt`**: JUCE 7/8 build setup including `MetronomeClock` and `BpmQuantizer` targets.
3. **`Tests/`**: Automated Unit Test Suite (`Tests/TestRunner.cpp`) and Docker verification suite (`Dockerfile` / `DOCKER_BUILD_AND_TEST.bat`).
4. **`preview/`**: Standalone browser workstation simulator (`preview/index.html`), `AVVIA_SIMULATORE.bat`, and `BUILD_AND_RUN.bat`.
5. **Documentation**:
   - `README.md`, `ARCHITECTURE.md`, `ROADMAP.md`, `AUDIT.md`, `CONTRIBUTING.md`, `TESTING.md`, `LICENSE` (AGPL-3.0), `.github/workflows/build.yml`.
