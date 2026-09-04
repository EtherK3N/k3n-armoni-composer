/*
    MetronomeClock.h
    ----------------
    Sample-accurate BPM clock for the K3N Armoni Composer audio engine.

    Responsibilities:
      - Generates a continuous clock signal aligned to the audio sample grid
      - Supports variable BPM (40-300) and time signatures (4/4, 3/4, 6/8, user-defined)
      - Emits beat callbacks on the message thread (safe to update UI)
      - Provides the master phase reference for BpmQuantizer

    Thread Safety:
      - setTempo() and setTimeSignature() are safe to call from the message thread
      - processBlock() is called exclusively from the audio thread (getNextAudioBlock)
      - Internal sync uses an atomic double for the tempo — no locks in the hot path
*/

#pragma once

#include <juce_core/juce_core.h>
#include <atomic>
#include <functional>

//==============================================================================
enum class BeatType
{
    Downbeat,       // Beat 1 (strong accent)
    Beat,           // Beats 2, 3, 4 (normal)
    Subdivision     // Subdivisions (1/8, 1/16 etc.)
};

struct BeatEvent
{
    BeatType type;
    int beatIndex;          // 0-based index within bar
    int subdivisionIndex;   // 0-based subdivision within beat
    juce::int64 samplePosition; // Absolute sample position in the host stream
};

//==============================================================================
class MetronomeClock
{
public:
    MetronomeClock();
    ~MetronomeClock() = default;

    //==========================================================================
    // Configuration — safe to call from message thread

    /** Sets tempo in BPM. Applied at the next processBlock() call. */
    void setTempo(double bpm) noexcept;

    /** Returns current tempo in BPM. */
    double getTempo() const noexcept { return currentBpm.load(); }

    /** Time signature numerator and denominator (e.g. 4, 4 for 4/4; 3, 4 for 3/4). */
    void setTimeSignature(int numerator, int denominator);

    /** Subdivision grid (2 = eighth notes, 4 = sixteenth notes, 3 = triplets). */
    void setSubdivisions(int subdivisionsPerBeat);

    /** Mutes the metronome click without stopping the clock. */
    void setClickEnabled(bool enabled) noexcept { clickEnabled.store(enabled); }
    bool isClickEnabled() const noexcept { return clickEnabled.load(); }

    /** Resets the phase to beat 1 of bar 1. */
    void reset() noexcept;

    //==========================================================================
    // Audio thread — call from getNextAudioBlock()

    void prepareToPlay(double sampleRate, int samplesPerBlock);
    void releaseResources();

    /**
     * Processes one audio block. Emits beat events synchronously.
     * onBeat is called on the AUDIO THREAD — use async dispatch for UI updates.
     * numSamples: size of the current audio block.
     */
    void processBlock(int numSamples, std::function<void(const BeatEvent&)> onBeat);

    //==========================================================================
    // Query — safe to call from any thread

    /** Current phase within the bar, [0.0, 1.0). */
    double getBarPhase() const noexcept { return barPhase.load(); }

    /** Current phase within the beat, [0.0, 1.0). */
    double getBeatPhase() const noexcept { return beatPhase.load(); }

    /** Returns the period of one beat in samples at the current tempo and sample rate. */
    double getSamplesPerBeat() const noexcept;

    /** Returns the period of one bar in samples. */
    double getSamplesPerBar() const noexcept;

private:
    std::atomic<double> currentBpm  { 120.0 };
    std::atomic<bool>   clickEnabled { true };
    std::atomic<double> barPhase    { 0.0 };
    std::atomic<double> beatPhase   { 0.0 };

    double sampleRate       = 44100.0;
    double phaseAccumulator = 0.0; // Absolute phase in beats (not bounded)

    int timeSignatureNum    = 4;   // 4/4 default
    int timeSignatureDen    = 4;
    int subdivisionsPerBeat = 2;   // Eighth notes by default

    int   lastBeatIndex = -1;      // To detect beat crossing
    int   lastSubIndex  = -1;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MetronomeClock)
};
