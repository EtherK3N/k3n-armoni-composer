#include "PerformanceViewComponent.h"

namespace
{
    juce::String roleToLabel(KeyboardRole role)
    {
        switch (role)
        {
            case KeyboardRole::Drums:    return "Drums Track";
            case KeyboardRole::Bass:     return "Bass Track";
            case KeyboardRole::FX:       return "FX Track";
            case KeyboardRole::Melody:   return "Melody Track";
            case KeyboardRole::Adaptive: return "Adaptive Track";
            case KeyboardRole::Unassigned:
            default: return "Free Track";
        }
    }
}

PerformanceViewComponent::PerformanceViewComponent(AudioEngine& audioEngine, MappingEngine& mappingEngine, DeviceManager& deviceManager)
    : audio(audioEngine), mapping(mappingEngine), devices(deviceManager)
{
    juce::ignoreUnused(devices);

    const KeyboardRole roles[kNumTracks] = {
        KeyboardRole::Drums, KeyboardRole::Bass, KeyboardRole::FX,
        KeyboardRole::Melody, KeyboardRole::Adaptive
    };

    for (auto role : roles)
    {
        auto* row = new TrackRow();
        row->role = role;
        row->label = roleToLabel(role);

        addAndMakeVisible(row->statusLabel);
        addAndMakeVisible(row->recordButton);
        addAndMakeVisible(row->muteButton);
        addAndMakeVisible(row->clearButton);

        row->recordButton.addListener(this);
        row->muteButton.addListener(this);
        row->clearButton.addListener(this);

        // Register loop track with the audio engine
        audio.registerLoopTrack(&row->track);

        tracks.add(row);
    }

    updateStatusLabels();
    startTimerHz(15);
}

PerformanceViewComponent::~PerformanceViewComponent()
{
    stopTimer();
    for (auto* row : tracks)
    {
        if (row != nullptr)
            audio.unregisterLoopTrack(&row->track);
    }
}

void PerformanceViewComponent::resized()
{
    auto area = getLocalBounds().reduced(12);
    const int rowHeight = 44;

    for (auto* row : tracks)
    {
        auto rowArea = area.removeFromTop(rowHeight);
        row->statusLabel.setBounds(rowArea.removeFromLeft(rowArea.getWidth() - 3 * 95));
        row->recordButton.setBounds(rowArea.removeFromLeft(95).reduced(4));
        row->muteButton.setBounds(rowArea.removeFromLeft(95).reduced(4));
        row->clearButton.setBounds(rowArea.removeFromLeft(95).reduced(4));
        area.removeFromTop(8);
    }
}

PerformanceViewComponent::TrackRow* PerformanceViewComponent::findRow(KeyboardRole role)
{
    for (auto* row : tracks)
        if (row->role == role)
            return row;
    return nullptr;
}

void PerformanceViewComponent::buttonClicked(juce::Button* b)
{
    for (auto* row : tracks)
    {
        if (b == &row->recordButton)
        {
            if (row->track.isRecording())
            {
                row->track.stopRecordingAndStartLoop();
            }
            else
            {
                row->track.startRecording();
                row->recordingStartMs = juce::Time::currentTimeMillis();
            }
            updateStatusLabels();
            return;
        }
        if (b == &row->muteButton)
        {
            row->track.setMuted(! row->track.isMuted());
            updateStatusLabels();
            return;
        }
        if (b == &row->clearButton)
        {
            row->track.clear();
            updateStatusLabels();
            return;
        }
    }
}

void PerformanceViewComponent::timerCallback()
{
    updateStatusLabels();
}

void PerformanceViewComponent::updateStatusLabels()
{
    for (auto* row : tracks)
    {
        juce::String status = row->label + ": ";

        if (row->track.isRecording())
        {
            auto elapsedMs = juce::Time::currentTimeMillis() - row->recordingStartMs;
            status << "[RECORDING (" << juce::String(elapsedMs / 1000.0, 1) << "s)]";
            row->recordButton.setButtonText("Stop Rec");
        }
        else if (row->track.isEmpty())
        {
            status << "[Empty Track]";
            row->recordButton.setButtonText("Rec");
        }
        else
        {
            status << (row->track.isMuted() ? "[Looping - MUTED]" : "[Looping - ACTIVE]");
            row->recordButton.setButtonText("Overdub");
        }

        row->muteButton.setButtonText(row->track.isMuted() ? "Unmute" : "Mute");
        row->statusLabel.setText(status, juce::dontSendNotification);
    }
}

void PerformanceViewComponent::handleKeyEvent(const RawKeyEvent& event, KeyboardRole role)
{
    if (! event.isKeyDown)
        return;

    const auto* binding = mapping.findBinding(event.deviceId, event.virtualKeyCode);
    if (binding == nullptr || binding->soundPath.isEmpty())
        return;

    // Fast handle resolution from RAM cache
    int handle = audio.findSampleHandle(binding->soundPath);
    if (handle < 0)
        handle = audio.loadSample(juce::File(binding->soundPath));

    if (handle >= 0)
        audio.triggerSample(handle);

    // Synchronous recording to active track
    if (auto* row = findRow(role))
    {
        if (row->track.isRecording())
        {
            const auto offsetMs = juce::Time::currentTimeMillis() - row->recordingStartMs;
            const double sRate = audio.getSampleRate() > 0 ? audio.getSampleRate() : 44100.0;
            const auto offsetSamples = static_cast<juce::int64>((offsetMs / 1000.0) * sRate);
            row->track.recordTrigger(handle, offsetSamples);
        }
    }
}
