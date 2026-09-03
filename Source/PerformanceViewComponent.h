/*
    PerformanceViewComponent.h
    --------------------------
    Live performance user interface:
      - Real-time multitrack looper lane monitoring and control
      - Zero-latency sample triggering via pre-cached RAM buffers
      - Sample-accurate loop overdubbing, muting, and clearing
*/

#pragma once

#include <JuceHeader.h>
#include "AudioEngine.h"
#include "MappingEngine.h"
#include "DeviceManager.h"

class PerformanceViewComponent : public juce::Component,
                                  private juce::Button::Listener,
                                  private juce::Timer
{
public:
    PerformanceViewComponent(AudioEngine& audioEngine, MappingEngine& mappingEngine, DeviceManager& deviceManager);
    ~PerformanceViewComponent() override;

    void resized() override;

    /** Processes incoming key events: plays one-shot sample and overdubs onto active loop track if recording. */
    void handleKeyEvent(const RawKeyEvent& event, KeyboardRole role);

private:
    static constexpr int kNumTracks = 5;

    struct TrackRow
    {
        KeyboardRole role = KeyboardRole::Unassigned;
        juce::String label;
        juce::Label statusLabel;
        juce::TextButton recordButton { "Rec" };
        juce::TextButton muteButton { "Mute" };
        juce::TextButton clearButton { "Clear" };
        LoopTrack track;
        juce::int64 recordingStartMs = 0;
    };

    void buttonClicked(juce::Button*) override;
    void timerCallback() override;
    void updateStatusLabels();
    TrackRow* findRow(KeyboardRole role);

    AudioEngine& audio;
    MappingEngine& mapping;
    DeviceManager& devices;

    juce::OwnedArray<TrackRow> tracks;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PerformanceViewComponent)
};
