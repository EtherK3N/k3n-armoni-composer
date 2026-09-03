#include "MappingEngine.h"

MappingEngine::MappingEngine()
{
    loadActiveSetFromDisk();
}

MappingEngine::~MappingEngine()
{
    saveActiveSetToDisk();
}

void MappingEngine::assignKey(const DeviceId& deviceId, int virtualKeyCode,
                             const juce::String& soundLabel, const juce::String& soundPath)
{
    DeviceMapping* targetDeviceMapping = nullptr;
    for (auto& dm : activeSet.deviceMappings)
    {
        if (dm.deviceId == deviceId)
        {
            targetDeviceMapping = &dm;
            break;
        }
    }

    if (targetDeviceMapping == nullptr)
    {
        DeviceMapping newDm;
        newDm.deviceId = deviceId;
        activeSet.deviceMappings.add(newDm);
        targetDeviceMapping = &activeSet.deviceMappings.getReference(activeSet.deviceMappings.size() - 1);
    }

    bool found = false;
    for (auto& b : targetDeviceMapping->bindings)
    {
        if (b.virtualKeyCode == virtualKeyCode)
        {
            b.soundLabel = soundLabel;
            b.soundPath = soundPath;
            found = true;
            break;
        }
    }

    if (! found)
    {
        KeyBinding b;
        b.virtualKeyCode = virtualKeyCode;
        b.soundLabel = soundLabel;
        b.soundPath = soundPath;
        targetDeviceMapping->bindings.add(b);
    }

    if (onMappingChanged != nullptr)
        onMappingChanged();
}

void MappingEngine::removeBinding(const DeviceId& deviceId, int virtualKeyCode)
{
    for (auto& dm : activeSet.deviceMappings)
    {
        if (dm.deviceId == deviceId)
        {
            for (int i = dm.bindings.size() - 1; i >= 0; --i)
            {
                if (dm.bindings[i].virtualKeyCode == virtualKeyCode)
                {
                    dm.bindings.remove(i);
                    if (onMappingChanged != nullptr)
                        onMappingChanged();
                    return;
                }
            }
        }
    }
}

void MappingEngine::renameBinding(const DeviceId& deviceId, int virtualKeyCode, const juce::String& newLabel)
{
    for (auto& dm : activeSet.deviceMappings)
    {
        if (dm.deviceId == deviceId)
        {
            for (auto& b : dm.bindings)
            {
                if (b.virtualKeyCode == virtualKeyCode)
                {
                    b.soundLabel = newLabel;
                    if (onMappingChanged != nullptr)
                        onMappingChanged();
                    return;
                }
            }
        }
    }
}

const KeyBinding* MappingEngine::findBinding(const DeviceId& deviceId, int virtualKeyCode) const
{
    for (const auto& dm : activeSet.deviceMappings)
    {
        if (dm.deviceId == deviceId)
        {
            for (const auto& b : dm.bindings)
            {
                if (b.virtualKeyCode == virtualKeyCode)
                    return &b;
            }
        }
    }
    return nullptr;
}

juce::File MappingEngine::getMappingConfigFile() const
{
    auto dir = juce::File::getSpecialLocation(juce::File::userApplicationDataDirectory)
                   .getChildFile("LoopStation");
    dir.createDirectory();
    return dir.getChildFile("mappings.json");
}

void MappingEngine::loadActiveSetFromDisk()
{
    auto file = getMappingConfigFile();
    if (! file.existsAsFile())
        return;

    auto json = juce::JSON::parse(file);
    if (! json.isObject())
        return;

    activeSet.deviceMappings.clear();
    activeSet.name = json.getProperty("name", "Default Preset").toString();

    if (auto* devArr = json.getProperty("devices", juce::var()).getArray())
    {
        for (auto& devItem : *devArr)
        {
            if (! devItem.isObject())
                continue;

            DeviceMapping dm;
            dm.deviceId = devItem.getProperty("deviceId", "").toString();

            if (auto* bindArr = devItem.getProperty("bindings", juce::var()).getArray())
            {
                for (auto& bindItem : *bindArr)
                {
                    if (! bindItem.isObject())
                        continue;

                    KeyBinding kb;
                    kb.virtualKeyCode = static_cast<int>(bindItem.getProperty("vkey", 0));
                    kb.soundLabel = bindItem.getProperty("label", "").toString();
                    kb.soundPath = bindItem.getProperty("path", "").toString();

                    if (kb.virtualKeyCode > 0)
                        dm.bindings.add(kb);
                }
            }

            if (dm.deviceId.isNotEmpty())
                activeSet.deviceMappings.add(dm);
        }
    }
}

void MappingEngine::saveActiveSetToDisk()
{
    auto rootObj = std::make_unique<juce::DynamicObject>();
    rootObj->setProperty("name", activeSet.name);

    juce::Array<juce::var> devArray;
    for (const auto& dm : activeSet.deviceMappings)
    {
        auto devObj = std::make_unique<juce::DynamicObject>();
        devObj->setProperty("deviceId", dm.deviceId);

        juce::Array<juce::var> bindArray;
        for (const auto& b : dm.bindings)
            {
            auto bindObj = std::make_unique<juce::DynamicObject>();
            bindObj->setProperty("vkey", b.virtualKeyCode);
            bindObj->setProperty("label", b.soundLabel);
            bindObj->setProperty("path", b.soundPath);
            bindArray.add(juce::var(bindObj.release()));
        }

        devObj->setProperty("bindings", juce::var(bindArray));
        devArray.add(juce::var(devObj.release()));
    }

    rootObj->setProperty("devices", juce::var(devArray));

    auto file = getMappingConfigFile();
    file.replaceWithText(juce::JSON::toString(juce::var(rootObj.release())));
}
