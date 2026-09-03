#include "BpmQuantizer.h"

void BpmQuantizer::setGridResolution(GridResolution res) noexcept
{
    resolution = res;
}

double BpmQuantizer::gridFractionOfBeat() const noexcept
{
    // Returns what fraction of a quarter-note beat one grid step represents.
    switch (resolution)
    {
        case GridResolution::Quarter:          return 1.0;
        case GridResolution::Eighth:           return 0.5;
        case GridResolution::Sixteenth:        return 0.25;
        case GridResolution::ThirtySecond:     return 0.125;
        case GridResolution::EighthTriplet:    return 1.0 / 3.0;
        case GridResolution::SixteenthTriplet: return 1.0 / 6.0;
        default:                               return 0.25;
    }
}

double BpmQuantizer::getGridStepSamples() const noexcept
{
    const double bpm = currentBpm.load();
    const double sr  = sampleRate.load();
    if (bpm <= 0.0 || sr <= 0.0)
        return 0.0;

    const double samplesPerQuarterNote = (sr * 60.0) / bpm;
    return samplesPerQuarterNote * gridFractionOfBeat();
}

int BpmQuantizer::countGridStepsInLoop(juce::int64 loopLengthSamples) const noexcept
{
    const double step = getGridStepSamples();
    if (step <= 0.0 || loopLengthSamples <= 0)
        return 0;

    return static_cast<int>(std::round(static_cast<double>(loopLengthSamples) / step));
}

juce::int64 BpmQuantizer::snapLoopLengthToNearestBar(juce::int64 rawLengthSamples,
                                                       int beatsPerBar) const noexcept
{
    const double bpm = currentBpm.load();
    const double sr  = sampleRate.load();
    if (bpm <= 0.0 || sr <= 0.0 || rawLengthSamples <= 0)
        return rawLengthSamples;

    const double samplesPerBeat = (sr * 60.0) / bpm;
    const double samplesPerBar  = samplesPerBeat * beatsPerBar;

    if (samplesPerBar <= 0.0)
        return rawLengthSamples;

    // Number of bars — round to nearest integer
    const double bars = std::round(static_cast<double>(rawLengthSamples) / samplesPerBar);
    const juce::int64 snappedLength = static_cast<juce::int64>(std::max(1.0, bars) * samplesPerBar);

    return snappedLength;
}

juce::int64 BpmQuantizer::quantize(juce::int64 rawOffsetSamples,
                                    juce::int64 loopLengthSamples) const noexcept
{
    const double step = getGridStepSamples();
    if (step <= 0.0)
        return rawOffsetSamples;

    // Nearest grid point
    const double raw    = static_cast<double>(rawOffsetSamples);
    const double ratio  = raw / step;
    const double snapped = std::round(ratio) * step;

    // Blend with humanize factor: 0.0 = full snap, 1.0 = raw
    const float h = humanize.load();
    const double result = snapped + (raw - snapped) * static_cast<double>(h);

    // Clamp to loop bounds and wrap if necessary
    juce::int64 quantized = static_cast<juce::int64>(std::round(result));
    if (loopLengthSamples > 0)
        quantized = ((quantized % loopLengthSamples) + loopLengthSamples) % loopLengthSamples;

    return quantized;
}
