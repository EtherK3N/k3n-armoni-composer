# Workspace State: K3N Armoni Composer

Last Updated: **September 2026**

---

## 🎵 Project Vision & Manifesto
- High-performance standalone C++17 / JUCE digital audio workstation and live loop station.
- **Mission & Frugal Innovation**: Democratizing music production through hardware upcycling (turning discarded €2-€5 USB computer keyboards into an expressive multi-instrument workstation via Win32 Raw Input hardware disambiguation).
- **Interactive Simulation**: preview/index.html featuring full multi-layer overdub loop station, per-layer modulable FX chains (Delay, Reverb, Low-Pass filter), 30+ synthesized instruments, session save/load (.armoni), and live WebM audio export.
- **License**: GNU Affero General Public License v3.0 (AGPL-3.0) with attribution & commercial licensing requirements.

---

## 🏛 Architecture & Component Status

1. **Source/**:
   - RawInputHandler.h/.cpp: Win32 Raw Input WM_INPUT hardware disambiguation up to 16 keyboards.
   - DeviceManager.h/.cpp: Per-hardware role assignment and %APPDATA% JSON persistence.
   - MappingEngine.h/.cpp: Per-key sound mapping and presets.
   - AudioEngine.h/.cpp: 32-voice polyphonic sampler, lock-free voice stealing, procedural starter kit, click buffer generator.
   - MetronomeClock.h/.cpp: Sample-accurate BPM clock (40-300 BPM), time signatures, lock-free atomic phase sync.
   - BpmQuantizer.h/.cpp: Snap-to-grid input quantizer (1/4 to 1/32 triplets), humanize factor, auto bar-snap.
   - MainComponent.h/.cpp: JUCE UI router & AudioDeviceManager setup.
   - MappingEditorComponent.h/.cpp & PerformanceViewComponent.h/.cpp: GUI views.
2. **CMakeLists.txt**: JUCE 7/8 build setup including MetronomeClock and BpmQuantizer targets.
3. **Tests/**: Automated Unit Test Suite (Tests/TestRunner.cpp) and Docker verification suite (Dockerfile / DOCKER_BUILD_AND_TEST.bat).
4. **preview/**: Standalone browser workstation simulator (preview/index.html), LAUNCH_SIMULATOR.bat (and legacy AVVIA_SIMULATORE.bat), and BUILD_AND_RUN.bat.
5. **CI/CD**: .github/workflows/build.yml with native MSVC runner, parallel build, always-on  uild.log artifact capture, and executable release upload.
6. **Documentation**:
   - README.md, ARCHITECTURE.md, ROADMAP.md, AUDIT.md, CONTRIBUTING.md, TESTING.md, SECURITY.md (SBOM & CRA compliance), LICENSE (AGPL-3.0), RELEASE_NOTES_v0.1.0.md.

---

## 🎯 Current Milestone: Phase 2 (Musical Performance & Hardware Feel)

- [x] **Metronome & Clock Engine**: Sample-accurate MetronomeClock (40-300 BPM, odd meters, click buffer).
- [x] **Input Quantization**: BpmQuantizer snap-to-grid (1/4 to 1/32 triplets, humanize factor, bar snapping).
- [ ] **Single-Keyboard Shift Layer System**:
  - Shift held: switch active key bank from Drums to Bass.
  - Caps Lock toggle: switch to Synth/FX bank.
  - Tab + number keys: octave transpose per bank.
  - Numpad layout mode: optimized 3x4 grid for separate USB numeric keypads.

---

## 📋 Completed Remediation Tasks (Technical Audit & Hygiene)
Detailed post-mortem and audit documented in `TECHNICAL_AUDIT_ACTION_PLAN.md`:

1. [x] **Fixed MSVC CI Failure (`MainComponent.cpp`)**:
   - Resolved `error C3861: 'getApplicationVersion': identifier not found` at line 119 using `juce::JUCEApplication::getInstance()->getApplicationVersion()`.
   - Updated release URL placeholder (`YOUR_GITHUB` -> `EtherK3N/k3n-armoni-composer`).
2. [x] **LoopTrack Event Scheduling Optimization**:
   - Replaced $O(N)$ linear loop `for (const auto& ev : recordedEvents)` per audio block with a sorted timeline cursor ($O(1)$ block check) and modulo wrap-around handling.
3. [x] **De-buzzwording & Realism in Documentation**:
   - Stripped AI-generated marketing hyperboles from `README.md` and `ARCHITECTURE.md`.
   - Documented real concurrency status (audioLock ScopedLock vs future lock-free SPSC FIFO queue).
   - Framed truthfully as an open-source v0.1.0 prototype / hardware-upcycling project.
4. [x] **Git Cleanup & History Hygiene**:
   - Consolidated trial-and-error commits into clean Conventional Commits.
5. [x] **Modern Modular JUCE Refactoring (Zero JuceHeader.h)**:
   - Removed monolithic auto-generated `JuceHeader.h` from all sources and tests.
   - Replaced with fine-grained direct module includes (`<juce_core/juce_core.h>`, `<juce_gui_basics/juce_gui_basics.h>`, `<juce_audio_basics/juce_audio_basics.h>`, etc.).
   - Removed deprecated `NEEDS_JUCE_HEADER` and explicitly linked required JUCE modules in `CMakeLists.txt`.
   - Translated `DOCKER_BUILD_AND_TEST.bat` to English and purged legacy `AVVIA_SIMULATORE.bat`.


