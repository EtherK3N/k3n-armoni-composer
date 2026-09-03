/*
    BpmQuantizer.h
    --------------
    Snap-to-grid input quantizer for the K3N Armoni Composer loop engine.

    Problem this solves:
      When a performer hits a key slightly before or after the grid beat, the
      recorded event has a timing offset (human error). Without quantization,
      loops drift out of sync across overdub layers. With quantization, events
      are rounded to the nearest musical grid line, producing tight, locked loops.

    Design:
      - The quantizer is stateless per-event: it receives an event timestamp (in
        samples) and returns the corrected timestamp. No buffering, no latency.
      - Grid resolution is expressed as a fraction of a beat (1/1, 1/2, 1/4, etc.)
      - A humanize factor [0.0, 1.0] blends between fully quantized (0.0) and
        the original unquantized position (1.0), enabling swing and natural feel.

    Thread Safety:
      - All methods are stateless or use only atomic reads/writes.
      - Safe to call from the audio thread without locks.
*/

#pragma once

#include <JuceHeader.h>
#include <atomic>
#include <cmath>
#include <algorithm>

//==============================================================================
enum class GridResolution
{
    Quarter,        // 1/4  note (1 beat)
    Eighth,         // 1/8  note (half beat)
    Sixteenth,      // 1/16 note (quarter beat)
    ThirtySecond,   // 1/32 note
    EighthTriplet,  // 1/8  note triplet (swing/hip-hop)
    SixteenthTriplet, // 1/16 note triplet
};

//==============================================================================
class BpmQuantizer
{
public:
    BpmQuantizer() = default;
    ~BpmQuantizer() = default;

    //==========================================================================
    // Configuration — safe from message thread, atomics protect audio reads

    void setTempo(double bpm) noexcept { currentBpm.store(juce::jlimit(40.0, 300.0, bpm)); }
    void setSampleRate(double sr) noexcept { sampleRate.store(sr); }

    /** Quantization resolution grid. Default: sixteenth notes. */
    void setGridResolution(GridResolution res) noexcept;
    GridResolution getGridResolution() const noexcept { return resolution; }

    /**
     * Humanize factor: 0.0 = full quantization (robotic), 1.0 = no quantization (raw).
     * At 0.5 a note lands halfway between its raw position and the grid, creating groove.
     */
    void setHumanizeFactor(float factor) noexcept { humanize.store(juce::jlimit(0.0f, 1.0f, factor)); }
    float getHumanizeFactor() const noexcept { return humanize.load(); }

    //==========================================================================
    // Core quantization — safe to call from audio thread

    /**
     * Quantizes a sample offset within a loop to the nearest grid line.
     *
     * @param rawOffsetSamples  Recorded sample offset (possibly uneven due to human timing)
     * @param loopLengthSamples Total loop length in samples (for wrap-around awareness)
     * @return                  Corrected offset, snapped to the nearest grid line
     */
    juce::int64 quantize(juce::int64 rawOffsetSamples, juce::int64 loopLengthSamples) const noexcept;

    /**
     * Returns the grid step size in samples for the current BPM and resolution.
     * Useful for drawing grid lines in the UI.
     */
    double getGridStepSamples() const noexcept;

    /**
     * Returns the number of grid steps that fit inside the given loop length.
     * Useful for aligning loop end to the nearest bar.
     */
    int countGridStepsInLoop(juce::int64 loopLengthSamples) const noexcept;

    /**
     * Snaps a raw loop length (from recording) to the nearest full bar boundary.
     * This is the "auto bar snap" feature: press STOP and the loop cleanly aligns.
     */
    juce::int64 snapLoopLengthToNearestBar(juce::int64 rawLengthSamples,
                                            int beatsPerBar = 4) const noexcept;

private:
    std::atomic<double> currentBpm   { 120.0 };
    std::atomic<double> sampleRate   { 44100.0 };
    std::atomic<float>  humanize     { 0.0f };
    GridResolution      resolution   = GridResolution::Sixteenth;

    double gridFractionOfBeat() const noexcept;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(BpmQuantizer)
};
