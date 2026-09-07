/*
    ShiftLayerSystem.h
    ------------------
    Single-Keyboard Shift Layer System:
      - Modal modifier tracking (Shift momentary, CapsLock latched toggle)
      - 4 Logical Banks: Drums (Base), Bass (Shift), Synth (CapsLock), FX (CapsLock + Shift)
      - Quick Octave Transposition via Tab + Number keys (1..5: -2, -1, 0, +1, +2)
      - Numpad Mode for dedicated USB numeric keypads (MPC-style 3x4 pad matrix)
*/

#pragma once

#include <juce_core/juce_core.h>
#include "RawInputHandler.h"

enum class KeyBank : int
{
    Drums = 0,
    Bass = 1,
    Synth = 2,
    FX = 3
};

inline juce::String getBankName(KeyBank bank)
{
    switch (bank)
    {
        case KeyBank::Drums: return "DRUMS";
        case KeyBank::Bass:  return "BASS";
        case KeyBank::Synth: return "SYNTH";
        case KeyBank::FX:    return "FX";
        default:             return "UNKNOWN";
    }
}

struct ProcessedKeyEvent
{
    DeviceId deviceId;
    int rawVirtualKeyCode = 0;
    int effectiveVirtualKeyCode = 0;
    KeyBank activeBank = KeyBank::Drums;
    int octaveOffset = 0; // -2 to +2
    bool isKeyDown = false;
    bool shouldTriggerSound = true;
};

class ShiftLayerSystem
{
public:
    ShiftLayerSystem();
    ~ShiftLayerSystem() = default;

    /** Returns current active bank based on CapsLock (toggle) and Shift (momentary). */
    KeyBank getActiveBank() const;

    /** Gets the octave offset for a given bank (-2 to +2). */
    int getOctaveOffset(KeyBank bank) const;

    /** Sets the octave offset for a given bank (-2 to +2). */
    void setOctaveOffset(KeyBank bank, int offset);

    /** Returns the octave offset for current active bank. */
    int getActiveOctaveOffset() const;

    /** Modifier states. */
    bool isShiftHeld() const { return shiftHeld; }
    bool isCapsLockOn() const { return capsLockOn; }
    bool isTabHeld() const { return tabHeld; }

    /** Direct toggles for UI / testing. */
    void setShiftHeld(bool held);
    void setCapsLockOn(bool on);

    /** Numpad mode toggle. */
    bool isNumpadModeEnabled() const { return numpadMode; }
    void setNumpadModeEnabled(bool enabled);

    /** Resets all modifier keys. */
    void resetModifiers();

    /**
        Processes incoming raw key event.
        Returns true if the event produces a playable musical trigger (shouldTriggerSound = true).
        Returns false if intercepted/consumed as a layer modifier or command.
    */
    bool processKeyEvent(const RawKeyEvent& raw, ProcessedKeyEvent& out);

    /** Invoked whenever bank, octave, or modifier state changes. */
    std::function<void(KeyBank currentBank, int currentOctave)> onStateChanged;

private:
    static constexpr int kNumBanks = 4;
    int octaveOffsets[kNumBanks] = { 0, 0, 0, 0 };

    bool shiftHeld = false;
    bool capsLockOn = false;
    bool tabHeld = false;
    bool numpadMode = false;

    void notifyStateChanged();
    int translateNumpadKey(int vkCode) const;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ShiftLayerSystem)
};
