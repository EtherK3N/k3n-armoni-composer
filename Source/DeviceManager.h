/*
    DeviceManager.h
    ---------------
    High-level hardware device management:
      - DeviceId -> Friendly Name and Functional Role (Drums, Bass, FX, Melody...)
      - 100% Offline Local JSON Persistence (%APPDATA%/LoopStation)
      - Notification when new unconfigured keyboards are connected
*/

#pragma once

#include <juce_core/juce_core.h>
#include "RawInputHandler.h"

enum class KeyboardRole
{
    Unassigned,
    Drums,
    Bass,
    FX,
    Melody,
    Adaptive
};

struct KnownDevice
{
    DeviceId id;
    juce::String friendlyName;
    KeyboardRole role = KeyboardRole::Unassigned;
};

class DeviceManager
{
public:
    DeviceManager();
    ~DeviceManager();

    /** Initializes the hook with the native host window handle for Raw Input. */
    void initialise(void* nativeWindowHandle);

    /** Forwards the captured WM_INPUT message. */
    void handleRawInputMessage(void* lParam);

    /** Returns all registered keyboards. */
    juce::Array<KnownDevice> getDevices() const;

    /** Rename or assign a role to a device. */
    void renameDevice(const DeviceId& id, const juce::String& newName);
    void setDeviceRole(const DeviceId& id, KeyboardRole role);

    /** Local hardware configuration persistence. */
    void loadFromDisk();
    void saveToDisk() const;

    /** Notification when an unrecognized device needs naming. */
    std::function<void(const DeviceId&)> onNewDeviceNeedsNaming;

    /** Key event callback enriched with the device's functional role. */
    std::function<void(const RawKeyEvent&, KeyboardRole)> onKeyEvent;

private:
    juce::File getConfigFile() const;

    RawInputHandler rawInput;
    std::unordered_map<std::string, KnownDevice> devices;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DeviceManager)
};
