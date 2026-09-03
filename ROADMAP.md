# 🗺️ Master Technical Roadmap: K3N Armoni Composer

This roadmap represents the multi-phase engineering and product blueprint for **K3N Armoni Composer**. It is designed to scale the system from a rock-solid low-latency looper into an expansive, cross-platform accessible workstation.

---

## 🧭 Overview of Development Phases

```
┌─────────────────────────────────────────────────────────────────────────┐
│ Phase 1: Core Engine & Hardware Disambiguation (v0.1.0) [COMPLETED]    │
│  └─ Win32 Raw Input, 32-Voice Sampler, Procedural Kit, Loop Engine     │
├─────────────────────────────────────────────────────────────────────────┤
│ Phase 2: Rhythmic Precision, BPM Quantization & Adaptive UX (v0.2.0)    │
│  └─ Metronome Engine, Swing/Snap Grid, Single-Keyboard Shift Layers     │
├─────────────────────────────────────────────────────────────────────────┤
│ Phase 3: DSP Sound Sculpting, Pitch Shifting & FX Rack (v0.3.0)        │
│  └─ Per-Track Filters, Ping-Pong Delay, Reverb, Granular Pitch/Time     │
├─────────────────────────────────────────────────────────────────────────┤
│ Phase 4: Studio Integration, Stem Export & Ableton Link (v0.4.0)       │
│  └─ Multitrack Audio Bounce, MIDI Sequence Export, Ableton Link Sync   │
├─────────────────────────────────────────────────────────────────────────┤
│ Phase 5: Cross-Platform Portability & Plugin Ecosystem (v0.5.0+)       │
│  └─ macOS IOKit, Linux libevdev, VST3/CLAP Plugin Edition, Web Preset Hub│
└─────────────────────────────────────────────────────────────────────────┘
```

---

## 🚀 Phase 1: Core Foundation & Frugal Innovation (v0.1.0) — `[COMPLETED]`
- [x] **Win32 Raw Input Hardware Disambiguation**: Intercept `WM_INPUT` via subclassed `WndProc` to distinguish concurrent USB keyboards.
- [x] **Physical USB Port Tracking**: Retrieve unique hardware device paths via `GetRawInputDeviceInfoW(RIDI_DEVICENAME)`.
- [x] **100% Offline Local JSON Persistence**: Zero cloud dependencies; configuration stored in `%APPDATA%/LoopStation`.
- [x] **32-Voice Real-Time Safe Sampler**: Zero memory allocations and zero disk I/O in the high-priority audio callback (`getNextAudioBlock`).
- [x] **Built-in Procedural Drum Synthesizer**: Instant out-of-the-box electronic kit (808 Kick, Snare, Hi-Hat, Sub-Bass, Synth Lead).
- [x] **Sample-Accurate Multi-Track Looper**: Timestamped discrete event recording with modulo wrap-around playback.
- [x] **Dual-Tab UI**: Interactive *Learn Key* Mapping Editor and Live Performance Looper.
- [x] **Pro Audio Driver Support**: Direct integration with ASIO, WASAPI Exclusive/Shared, and DirectSound.

---

## 🎯 Phase 2: Rhythmic Precision, BPM Quantization & Adaptive UX (v0.2.0)

### 1. Metronome & Clock Synchronization Engine `[audio / dsp]`
- [ ] **Sample-Accurate BPM Clock**: Variable tempo engine (40 to 300 BPM) with support for 4/4, 3/4, 6/8, and odd time signatures.
- [ ] **Procedural Click Generator**: Accentuated downbeats (high-pitch click) and unaccented subdivisions (low-pitch click).
- [ ] **Input Quantization (Snap-to-Grid)**:
  - Snap physical trigger events to the nearest musical grid line ($1/4$, $1/8$, $1/16$, $1/32$, and Triplet modes).
  - Configurable **Humanize / Swing** factor ($0\%$ to $100\%$) for natural groove feel.

### 2. Adaptive Single-Keyboard Expressivity (Layer Shift) `[ux / input]`
- [ ] **Modifier Shift Layers**:
  - `Shift` Key: Dynamically switches the active key bank from **Drums (Bank A)** to **Bass (Bank B)**.
  - `Space` / `Caps Lock`: Toggles **FX / Modulation (Bank C)**.
  - `Tab` / Number keys: Octave transpose and pitch shift on the fly.
- [ ] **Numpad Compact Drum Pad Mode**: Dedicated optimized layout for separate USB numeric keypads.

### 3. Visual Performance Feedback & Threading Enhancements `[ui / performance]`
- [ ] **Circular / Linear Animated Playheads**: Real-time position visualizers for each loop lane.
- [ ] **Live Audio Waveform Display**: Dynamic mini-oscilloscope and peak meters per track.
- [ ] **Lock-Free SPSC Ring Buffer**: Replace `CriticalSection` locks in `AudioEngine` with `juce::AbstractFifo` for zero-jitter execution on extreme buffer sizes (<32 samples @ 96kHz).

---

## 🎛️ Phase 3: DSP Sound Sculpting & Effects Engine (v0.3.0)

### 1. Per-Lane Audio Effects Rack `[dsp]`
- [ ] **Multi-Mode Resonant Filter**: Low-Pass, High-Pass, and Band-Pass filters with resonance control per track lane.
- [ ] **Stereo Ping-Pong Delay**: Tempo-synced delay with feedback and damping filters.
- [ ] **Algorithmic Reverb**: Lightweight Schroeder/Freeverb implementation for spatial ambiance.
- [ ] **3-Band Parametric Equalizer**: Low Shelf, Mid Bell, and High Shelf tone control.

### 2. Pitch Shifting & Time Stretching `[dsp]`
- [ ] **Real-Time Sample Resampler**: Integration of `juce::LagrangeInterpolator` and `juce::CatmullRomInterpolator`.
- [ ] **Granular Pitch Transpose**: Pitch-shift samples across keyboard keys without altering playback duration.
- [ ] **Dynamic Velocity Emulation**: Velocity calculation based on keystroke repetition interval or modifier pressure.

---

## 📡 Phase 4: Studio Integration, Stem Export & Ableton Link (v0.4.0)

### 1. Export & Recording `[workflow]`
- [ ] **Multitrack Stem Exporter**: Export all recorded loop lanes as separate, synchronized 24-bit WAV/FLAC audio stems.
- [ ] **Standard MIDI File (.mid) Export**: Save recorded trigger patterns as standard MIDI sequences for import into Ableton, FL Studio, or Logic Pro.
- [ ] **Virtual MIDI Output**: Send live MIDI notes to external software synthesizers via virtual MIDI drivers.

### 2. Wireless Jamming & Tempo Sync `[networking]`
- [ ] **Ableton Link Protocol Integration**: Zero-configuration wireless tempo and beat synchronization with other laptops, iPads, hardware drum machines, and DJ gear on the local Wi-Fi network.

---

## 🌍 Phase 5: Cross-Platform Portability & Hardware Ecosystem (v0.5.0+)

### 1. Cross-Platform Input Engines `[core / os]`
- [ ] **macOS Support**: Low-level per-keyboard HID interception using *Apple IOKit HID Manager*.
- [ ] **Linux Support**: Direct hardware device tracking via *libevdev* / *udev* and native support for ALSA, JACK, and PipeWire.

### 2. Plugin Edition (VST3 / CLAP / AU) `[plugin]`
- [ ] **DAW Plugin Wrapper**: Run Armoni Composer as an instrument plugin inside commercial DAWs, routing multi-keyboard raw inputs directly into the host track routing.

### 3. Open Community Preset & Sample Hub `[community]`
- [ ] **Preset Sharing Protocol**: One-click export/import of self-contained `.armoni` archive packs (including JSON mapping and bundled audio samples).
