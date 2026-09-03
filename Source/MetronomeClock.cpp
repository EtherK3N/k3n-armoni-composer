#include "MetronomeClock.h"

MetronomeClock::MetronomeClock() = default;

void MetronomeClock::setTempo(double bpm) noexcept
{
    currentBpm.store(juce::jlimit(40.0, 300.0, bpm));
}

void MetronomeClock::setTimeSignature(int numerator, int denominator)
{
    jassert(numerator > 0 && denominator > 0);
    timeSignatureNum = numerator;
    timeSignatureDen = denominator;
}

void MetronomeClock::setSubdivisions(int subdivisionsPerBeat_)
{
    jassert(subdivisionsPerBeat_ >= 1);
    subdivisionsPerBeat = subdivisionsPerBeat_;
}

void MetronomeClock::reset() noexcept
{
    phaseAccumulator = 0.0;
    barPhase.store(0.0);
    beatPhase.store(0.0);
    lastBeatIndex = -1;
    lastSubIndex  = -1;
}

void MetronomeClock::prepareToPlay(double newSampleRate, int /*samplesPerBlock*/)
{
    sampleRate = newSampleRate;
    reset();
}

void MetronomeClock::releaseResources()
{
    reset();
}

double MetronomeClock::getSamplesPerBeat() const noexcept
{
    const double bpm = currentBpm.load();
    if (bpm <= 0.0)
        return 0.0;
    return (sampleRate * 60.0) / bpm;
}

double MetronomeClock::getSamplesPerBar() const noexcept
{
    // One bar = timeSignatureNum quarter notes (adjusted for denominator).
    // e.g. 4/4 = 4 beats per bar; 6/8 = 6 eighth-note beats per bar.
    // We normalise: beats per bar = numerator * (4.0 / denominator)
    const double beatsPerBar = timeSignatureNum * (4.0 / timeSignatureDen);
    return getSamplesPerBeat() * beatsPerBar;
}

void MetronomeClock::processBlock(int numSamples, std::function<void(const BeatEvent&)> onBeat)
{
    if (sampleRate <= 0.0 || numSamples <= 0)
        return;

    const double bpm            = currentBpm.load();
    const double spb            = getSamplesPerBeat();
    if (spb <= 0.0)
        return;

    const double phasePerSample = 1.0 / spb; // advance in beats per sample
    const double beatsPerBar    = timeSignatureNum * (4.0 / timeSignatureDen);

    for (int i = 0; i < numSamples; ++i)
    {
        phaseAccumulator += phasePerSample;

        // Absolute beat index within the current bar
        const double barBeats     = std::fmod(phaseAccumulator, beatsPerBar);
        const int    beatIdx      = static_cast<int>(barBeats);
        const double localBPhase  = barBeats - beatIdx;

        // Subdivision index within the current beat
        const int subIdx = static_cast<int>(localBPhase * subdivisionsPerBeat);

        // Detect beat crossing
        if (beatIdx != lastBeatIndex)
        {
            lastBeatIndex = beatIdx;
            lastSubIndex  = subIdx;

            BeatEvent ev;
            ev.type            = (beatIdx == 0) ? BeatType::Downbeat : BeatType::Beat;
            ev.beatIndex       = beatIdx;
            ev.subdivisionIndex = 0;
            ev.samplePosition  = i; // offset within this block

            barPhase.store(barBeats / beatsPerBar);
            beatPhase.store(localBPhase);

            if (onBeat)
                onBeat(ev);
        }
        else if (subIdx != lastSubIndex && subIdx > 0)
        {
            // Subdivision crossing (e.g. eighth note, sixteenth note)
            lastSubIndex = subIdx;

            BeatEvent ev;
            ev.type             = BeatType::Subdivision;
            ev.beatIndex        = beatIdx;
            ev.subdivisionIndex = subIdx;
            ev.samplePosition   = i;

            if (onBeat)
                onBeat(ev);
        }
    }

    // Update atomic phases for UI polling
    const double barBeats = std::fmod(phaseAccumulator, beatsPerBar);
    barPhase.store(barBeats / beatsPerBar);
    beatPhase.store(std::fmod(barBeats, 1.0));
}
