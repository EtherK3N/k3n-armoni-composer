#include "MappingEditorComponent.h"

//==============================================================================
// SoundLibraryListModel

int SoundLibraryListModel::getNumRows()
{
    return owner.getNumImportedSounds();
}

void SoundLibraryListModel::paintListBoxItem(int rowNumber, juce::Graphics& g, int width, int height, bool rowIsSelected)
{
    if (rowIsSelected)
        g.fillAll(juce::Colours::darkslateblue);

    g.setColour(juce::Colours::white);
    g.setFont(14.0f);

    if (rowNumber >= 0 && rowNumber < owner.getNumImportedSounds())
        g.drawText(owner.getImportedSoundLabel(rowNumber), 8, 0, width - 16, height,
                    juce::Justification::centredLeft, true);
}

void SoundLibraryListModel::selectedRowsChanged(int lastRowSelected)
{
    owner.setSelectedSoundIndex(lastRowSelected);
}

//==============================================================================
// BindingsTableModel

namespace
{
    class RowActionsComponent : public juce::Component
    {
    public:
        RowActionsComponent(MappingEditorComponent& ownerIn, BindingsTableModel& modelIn)
            : owner(ownerIn), model(modelIn)
        {
            addAndMakeVisible(renameButton);
            addAndMakeVisible(deleteButton);

            renameButton.onClick = [this]
            {
                if (rowIndex >= 0 && rowIndex < model.getNumRows())
                {
                    auto& row = model.getRow(rowIndex);
                    owner.promptRenameBinding(row.deviceId, row.virtualKeyCode, row.soundLabel);
                }
            };

            deleteButton.onClick = [this]
            {
                if (rowIndex >= 0 && rowIndex < model.getNumRows())
                {
                    auto& row = model.getRow(rowIndex);
                    owner.deleteBindingAndRefresh(row.deviceId, row.virtualKeyCode);
                }
            };
        }

        void setRow(int newRowIndex) { rowIndex = newRowIndex; }

        void resized() override
        {
            auto area = getLocalBounds().reduced(2);
            renameButton.setBounds(area.removeFromLeft(area.getWidth() / 2 - 2));
            area.removeFromLeft(4);
            deleteButton.setBounds(area);
        }

    private:
        MappingEditorComponent& owner;
        BindingsTableModel& model;
        int rowIndex = -1;

        juce::TextButton renameButton { "Rename" };
        juce::TextButton deleteButton { "Delete" };
    };

    enum ColumnIds
    {
        colDevice = 1,
        colKey = 2,
        colSound = 3,
        colActions = 4
    };

    juce::String describeVirtualKeyCode(int vkey)
    {
        if (vkey >= '0' && vkey <= '9') return juce::String::charToString((juce::juce_wchar) vkey);
        if (vkey >= 'A' && vkey <= 'Z') return juce::String::charToString((juce::juce_wchar) vkey);
        if (vkey >= 0x70 && vkey <= 0x87) return "F" + juce::String(vkey - 0x70 + 1); // VK_F1 .. VK_F24

        switch (vkey)
        {
            case 0x20: return "Space";
            case 0x0D: return "Enter";
            case 0x09: return "Tab";
            case 0x10: return "Shift";
            case 0x11: return "Ctrl";
            case 0x12: return "Alt";
            case 0x1B: return "Esc";
            case 0x08: return "Backspace";
            default: break;
        }

        return "0x" + juce::String::toHexString(vkey).toUpperCase();
    }
}

void BindingsTableModel::rebuild()
{
    rows.clear();

    for (const auto& known : owner.getDevices().getDevices())
    {
        for (const auto& dm : owner.getEngine().getActiveSet().deviceMappings)
        {
            if (dm.deviceId != known.id)
                continue;

            for (const auto& b : dm.bindings)
            {
                Row r;
                r.deviceId = known.id;
                r.deviceName = known.friendlyName;
                r.virtualKeyCode = b.virtualKeyCode;
                r.soundLabel = b.soundLabel;
                rows.add(r);
            }
        }
    }
}

void BindingsTableModel::paintRowBackground(juce::Graphics& g, int rowNumber, int width, int height, bool rowIsSelected)
{
    juce::ignoreUnused(width, height);
    g.fillAll(rowIsSelected ? juce::Colours::darkslateblue
                             : (rowNumber % 2 == 0 ? juce::Colours::black.withAlpha(0.15f) : juce::Colours::transparentBlack));
}

void BindingsTableModel::paintCell(juce::Graphics& g, int rowNumber, int columnId, int width, int height, bool rowIsSelected)
{
    juce::ignoreUnused(rowIsSelected);
    if (rowNumber < 0 || rowNumber >= rows.size())
        return;

    const auto& row = rows.getReference(rowNumber);
    g.setColour(juce::Colours::white);
    g.setFont(13.0f);

    juce::String text;
    switch (columnId)
    {
        case colDevice: text = row.deviceName; break;
        case colKey:    text = describeVirtualKeyCode(row.virtualKeyCode); break;
        case colSound:  text = row.soundLabel; break;
        default: return;
    }

    g.drawText(text, 6, 0, width - 12, height, juce::Justification::centredLeft, true);
}

juce::Component* BindingsTableModel::refreshComponentForCell(int rowNumber, int columnId, bool isRowSelected,
                                                               juce::Component* existingComponentToUpdate)
{
    juce::ignoreUnused(isRowSelected);

    if (columnId != colActions)
    {
        delete existingComponentToUpdate;
        return nullptr;
    }

    auto* rowComponent = dynamic_cast<RowActionsComponent*>(existingComponentToUpdate);
    if (rowComponent == nullptr)
    {
        delete existingComponentToUpdate;
        rowComponent = new RowActionsComponent(owner, *this);
    }

    rowComponent->setRow(rowNumber);
    return rowComponent;
}

//==============================================================================
// MappingEditorComponent

MappingEditorComponent::MappingEditorComponent(MappingEngine& mappingEngine, DeviceManager& deviceManager, AudioEngine& audioEngine)
    : engine(mappingEngine), devices(deviceManager), audio(audioEngine)
{
    addAndMakeVisible(deviceSelector);
    addAndMakeVisible(soundLibraryList);
    addAndMakeVisible(importSoundButton);
    addAndMakeVisible(assignKeyButton);
    addAndMakeVisible(saveSetButton);
    addAndMakeVisible(statusLabel);
    addAndMakeVisible(bindingsTable);

    importSoundButton.addListener(this);
    assignKeyButton.addListener(this);
    saveSetButton.addListener(this);

    statusLabel.setText("Select a sample from the library, then click 'Learn Key'.",
                         juce::dontSendNotification);

    // Populate built-in zero-dependency starter kit so the app is immediately playable
    importedSoundLabels.add("Built-in Kick (808)");
    importedSoundPaths.add("starter://kick");
    importedSoundLabels.add("Built-in Snare Drum");
    importedSoundPaths.add("starter://snare");
    importedSoundLabels.add("Built-in Closed Hi-Hat");
    importedSoundPaths.add("starter://hihat");
    importedSoundLabels.add("Built-in 808 Sub Bass");
    importedSoundPaths.add("starter://bass808");
    importedSoundLabels.add("Built-in Synth Melodic Stab");
    importedSoundPaths.add("starter://synth");

    soundLibraryList.setModel(&soundLibraryModel);

    bindingsTable.setModel(&bindingsTableModel);
    bindingsTable.getHeader().addColumn("Keyboard / Device", colDevice, 150);
    bindingsTable.getHeader().addColumn("Key", colKey, 90);
    bindingsTable.getHeader().addColumn("Assigned Sample", colSound, 200);
    bindingsTable.getHeader().addColumn("Actions", colActions, 160);

    refreshDeviceList();
    refreshBindingsList();
}

MappingEditorComponent::~MappingEditorComponent()
{
    soundLibraryList.setModel(nullptr);
    bindingsTable.setModel(nullptr);
}

void MappingEditorComponent::refreshDeviceList()
{
    deviceSelector.clear();
    int itemId = 1;
    for (const auto& d : devices.getDevices())
        deviceSelector.addItem(d.friendlyName + " (" + d.id.substring(0, 10) + "...)", itemId++);

    if (deviceSelector.getNumItems() > 0)
        deviceSelector.setSelectedItemIndex(0);
}

void MappingEditorComponent::resized()
{
    auto area = getLocalBounds().reduced(12);

    auto topRow = area.removeFromTop(32);
    deviceSelector.setBounds(topRow.removeFromLeft(280));

    area.removeFromTop(10);
    auto soundRow = area.removeFromTop(140);
    soundLibraryList.setBounds(soundRow.removeFromLeft(area.getWidth() / 2 - 8));
    soundRow.removeFromLeft(16);
    importSoundButton.setBounds(soundRow.removeFromTop(32));

    area.removeFromTop(10);
    auto buttonRow = area.removeFromTop(32);
    assignKeyButton.setBounds(buttonRow.removeFromLeft(160));
    buttonRow.removeFromLeft(12);
    saveSetButton.setBounds(buttonRow.removeFromLeft(160));

    area.removeFromTop(8);
    statusLabel.setBounds(area.removeFromTop(24));

    area.removeFromTop(8);
    bindingsTable.setBounds(area);
}

void MappingEditorComponent::buttonClicked(juce::Button* b)
{
    if (b == &importSoundButton)      importSoundClicked();
    else if (b == &assignKeyButton)   assignKeyClicked();
    else if (b == &saveSetButton)     saveSetClicked();
}

void MappingEditorComponent::importSoundClicked()
{
    auto chooser = std::make_shared<juce::FileChooser>(
        "Select Audio File",
        juce::File::getSpecialLocation(juce::File::userMusicDirectory),
        "*.wav;*.mp3;*.flac;*.aiff");

    auto flags = juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles;

    chooser->launchAsync(flags, [this, chooser](const juce::FileChooser& fc)
    {
        auto file = fc.getResult();
        if (file.existsAsFile())
        {
            importedSoundLabels.add(file.getFileNameWithoutExtension());
            importedSoundPaths.add(file.getFullPathName());

            // Pre-cache sample directly into RAM
            audio.loadSample(file);

            soundLibraryList.updateContent();
            statusLabel.setText("Imported: " + file.getFileName(), juce::dontSendNotification);
        }
    });
}

void MappingEditorComponent::assignKeyClicked()
{
    if (selectedSoundIndex < 0 || selectedSoundIndex >= importedSoundLabels.size())
    {
        statusLabel.setText("Please select a sample from the library on the left.", juce::dontSendNotification);
        return;
    }

    waitingForKeyAssignment = true;
    statusLabel.setText("Listening... Press the physical key on your keyboard now!",
                         juce::dontSendNotification);
}

bool MappingEditorComponent::captureKeyForAssignment(const RawKeyEvent& event)
{
    if (! waitingForKeyAssignment || ! event.isKeyDown)
        return false;

    if (selectedSoundIndex >= 0 && selectedSoundIndex < importedSoundLabels.size())
    {
        engine.assignKey(event.deviceId, event.virtualKeyCode,
                          importedSoundLabels[selectedSoundIndex],
                          importedSoundPaths[selectedSoundIndex]);

        audio.loadSample(juce::File(importedSoundPaths[selectedSoundIndex]));

        statusLabel.setText("Key successfully mapped to: " + importedSoundLabels[selectedSoundIndex],
                             juce::dontSendNotification);
        refreshBindingsList();
    }

    waitingForKeyAssignment = false;
    return true;
}

void MappingEditorComponent::refreshBindingsList()
{
    bindingsTableModel.rebuild();
    bindingsTable.updateContent();
}

void MappingEditorComponent::deleteBindingAndRefresh(const DeviceId& deviceId, int virtualKeyCode)
{
    engine.removeBinding(deviceId, virtualKeyCode);
    refreshBindingsList();
}

void MappingEditorComponent::promptRenameBinding(const DeviceId& deviceId, int virtualKeyCode, const juce::String& currentLabel)
{
    auto* aw = new juce::AlertWindow("Rename Key Binding", "Enter new descriptive label:", juce::AlertWindow::NoIcon);
    aw->addTextEditor("newName", currentLabel);
    aw->addButton("OK", 1, juce::KeyPress(juce::KeyPress::returnKey));
    aw->addButton("Cancel", 0, juce::KeyPress(juce::KeyPress::escapeKey));

    aw->enterModalState(true, juce::ModalCallbackFunction::create([this, aw, deviceId, virtualKeyCode](int result)
    {
        if (result == 1)
        {
            auto newName = aw->getTextEditorContents("newName");
            if (newName.isNotEmpty())
            {
                engine.renameBinding(deviceId, virtualKeyCode, newName);
                refreshBindingsList();
            }
        }
        delete aw;
    }), false);
}

void MappingEditorComponent::saveSetClicked()
{
    engine.saveActiveSetToDisk();
    statusLabel.setText("Preset saved successfully: " + engine.getActiveSet().name, juce::dontSendNotification);
}
