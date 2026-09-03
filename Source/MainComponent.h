/*
    MainComponent.h
    ---------------
    Root application component:
      - JUCE AudioDeviceManager setup (ASIO / DirectSound / WASAPI)
      - Tabbed layout: Mapping Editor and Live Performance View
      - Hardware raw input routing to editor and audio looper engine
      - Asynchronous GitHub Release Update-Checker
*/

#pragma once

#include <JuceHeader.h>
#include "AudioEngine.h"
#include "DeviceManager.h"
#include "MappingEngine.h"
#include "MappingEditorComponent.h"
#include "PerformanceViewComponent.h"

class MainComponent : public juce::Component,
                      private juce::Button::Listener
{
public:
    MainComponent();
    ~MainComponent() override;

    void paint(juce::Graphics& g) override;
    void resized() override;

    /** Connects native host window handle for WM_INPUT message interception. */
    void setNativeWindowHandle(void* hwnd);

    /** Forwards native WM_INPUT messages to the device manager. */
    void handleRawInput(void* lParam);

    /** Asynchronously checks GitHub Releases for new software updates. */
    void checkForUpdatesAsync();

private:
    void buttonClicked(juce::Button* b) override;
    void showAudioSettingsModal();

    juce::AudioDeviceManager audioDeviceManager;
    juce::AudioSourcePlayer audioSourcePlayer;

    AudioEngine audioEngine;
    DeviceManager deviceManager;
    MappingEngine mappingEngine;

    juce::TextButton audioSettingsButton { "Audio Settings..." };
    juce::TextButton updateButton { "Update Available!" };
    juce::Label titleLabel;

    juce::TabbedComponent tabs { juce::TabbedButtonBar::TabsAtTop };
    MappingEditorComponent editorComponent;
    PerformanceViewComponent performanceComponent;

    juce::String latestReleaseUrl;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
};
