/*
    MappingEditorComponent.h
    ------------------------
    Graphical user interface for mapping audio samples to physical keyboard keys:
      - HID Device selector
      - Audio file importer (WAV, MP3, FLAC, AIFF)
      - Interactive key learning mode ("Learn Key")
      - Live bindings table with in-place Rename / Delete actions
      - Preset saving
*/

#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include "MappingEngine.h"
#include "DeviceManager.h"
#include "AudioEngine.h"

class MappingEditorComponent;

class SoundLibraryListModel : public juce::ListBoxModel
{
public:
    explicit SoundLibraryListModel(MappingEditorComponent& owner) : owner(owner) {}

    int getNumRows() override;
    void paintListBoxItem(int rowNumber, juce::Graphics& g, int width, int height, bool rowIsSelected) override;
    void selectedRowsChanged(int lastRowSelected) override;

private:
    MappingEditorComponent& owner;
};

class BindingsTableModel : public juce::TableListBoxModel
{
public:
    struct Row
    {
        DeviceId deviceId;
        juce::String deviceName;
        int virtualKeyCode = 0;
        juce::String soundLabel;
    };

    explicit BindingsTableModel(MappingEditorComponent& owner) : owner(owner) {}

    void rebuild();

    int getNumRows() override { return rows.size(); }
    void paintRowBackground(juce::Graphics&, int rowNumber, int width, int height, bool rowIsSelected) override;
    void paintCell(juce::Graphics&, int rowNumber, int columnId, int width, int height, bool rowIsSelected) override;
    juce::Component* refreshComponentForCell(int rowNumber, int columnId, bool isRowSelected,
                                              juce::Component* existingComponentToUpdate) override;

    const Row& getRow(int index) const { return rows.getReference(index); }

private:
    MappingEditorComponent& owner;
    juce::Array<Row> rows;
};

class MappingEditorComponent : public juce::Component,
                                private juce::Button::Listener
{
public:
    MappingEditorComponent(MappingEngine& mappingEngine, DeviceManager& deviceManager, AudioEngine& audioEngine);
    ~MappingEditorComponent() override;

    void resized() override;

    /** Captures the key press if in key learning mode. Returns true if consumed. */
    bool captureKeyForAssignment(const RawKeyEvent& event);

    int getNumImportedSounds() const { return importedSoundLabels.size(); }
    juce::String getImportedSoundLabel(int index) const { return importedSoundLabels[index]; }
    void setSelectedSoundIndex(int index) { selectedSoundIndex = index; }
    MappingEngine& getEngine() { return engine; }
    DeviceManager& getDevices() { return devices; }
    AudioEngine& getAudio() { return audio; }

    void deleteBindingAndRefresh(const DeviceId& deviceId, int virtualKeyCode);
    void promptRenameBinding(const DeviceId& deviceId, int virtualKeyCode, const juce::String& currentLabel);

    void refreshDeviceList();
    void refreshBindingsList();

private:
    void buttonClicked(juce::Button*) override;
    void importSoundClicked();
    void assignKeyClicked();
    void saveSetClicked();

    MappingEngine& engine;
    DeviceManager& devices;
    AudioEngine& audio;

    juce::ComboBox deviceSelector;
    juce::ListBox soundLibraryList { "Sample Library", nullptr };
    juce::TextButton importSoundButton { "Import Sample..." };
    juce::TextButton assignKeyButton { "Learn Key" };
    juce::TextButton saveSetButton { "Save Preset" };
    juce::Label statusLabel;

    juce::TableListBox bindingsTable { "Key Mappings", nullptr };

    SoundLibraryListModel soundLibraryModel { *this };
    BindingsTableModel bindingsTableModel { *this };

    juce::StringArray importedSoundLabels;
    juce::StringArray importedSoundPaths;
    int selectedSoundIndex = -1;

    bool waitingForKeyAssignment = false;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MappingEditorComponent)
};
