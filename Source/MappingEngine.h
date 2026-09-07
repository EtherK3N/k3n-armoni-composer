/*
    MappingEngine.h
    ---------------
    Manages physical key bindings: (DeviceId + VirtualKey) -> Audio Sample.
    Supports saving and loading presets (Sets) in 100% offline local JSON format.
*/

#pragma once

#include <juce_core/juce_core.h>
#include "RawInputHandler.h"

struct KeyBinding
{
    int virtualKeyCode = 0;
    juce::String soundLabel;
    juce::String soundPath;
    int bank = 0; // 0 = Drums, 1 = Bass, 2 = Synth, 3 = FX
};

struct DeviceMapping
{
    DeviceId deviceId;
    juce::Array<KeyBinding> bindings;
};

struct MappingSet
{
    juce::String name = "Default Preset";
    juce::Array<DeviceMapping> deviceMappings;
};

class MappingEngine
{
public:
    MappingEngine();
    ~MappingEngine();

    /** Assigns or updates a binding for a given device, virtual key code, and bank. */
    void assignKey(const DeviceId& deviceId, int virtualKeyCode,
                   const juce::String& soundLabel, const juce::String& soundPath,
                   int bank = 0);

    /** Removes a key binding for a specific bank (or all banks if bank == -1). */
    void removeBinding(const DeviceId& deviceId, int virtualKeyCode, int bank = 0);

    /** Renames the descriptive label for a key binding. */
    void renameBinding(const DeviceId& deviceId, int virtualKeyCode, const juce::String& newLabel, int bank = 0);

    /** Looks up a binding for a given device, key code, and bank.
        Falls back to default bank (0) if no bank-specific binding is found. */
    const KeyBinding* findBinding(const DeviceId& deviceId, int virtualKeyCode, int bank = 0) const;

    /** Access active preset in memory. */
    MappingSet& getActiveSet() { return activeSet; }
    const MappingSet& getActiveSet() const { return activeSet; }

    /** Local JSON preset persistence. */
    void saveActiveSetToDisk();
    void loadActiveSetFromDisk();

    /** Callback invoked whenever bindings are modified. */
    std::function<void()> onMappingChanged;

private:
    juce::File getMappingConfigFile() const;

    MappingSet activeSet;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MappingEngine)
};
