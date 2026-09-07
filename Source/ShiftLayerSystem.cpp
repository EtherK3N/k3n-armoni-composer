#include "ShiftLayerSystem.h"

namespace
{
    // Windows Virtual Key codes
    constexpr int kVkTab     = 0x09;
    constexpr int kVkShift   = 0x10;
    constexpr int kVkCapital = 0x14; // Caps Lock
    constexpr int kVkLShift  = 0xA0;
    constexpr int kVkRShift  = 0xA1;
}

ShiftLayerSystem::ShiftLayerSystem()
{
    resetModifiers();
}

KeyBank ShiftLayerSystem::getActiveBank() const
{
    if (capsLockOn)
        return shiftHeld ? KeyBank::FX : KeyBank::Synth;

    return shiftHeld ? KeyBank::Bass : KeyBank::Drums;
}

int ShiftLayerSystem::getOctaveOffset(KeyBank bank) const
{
    const int idx = static_cast<int>(bank);
    if (idx >= 0 && idx < kNumBanks)
        return octaveOffsets[idx];

    return 0;
}

void ShiftLayerSystem::setOctaveOffset(KeyBank bank, int offset)
{
    const int idx = static_cast<int>(bank);
    if (idx >= 0 && idx < kNumBanks)
    {
        const int clamped = juce::jlimit(-2, 2, offset);
        if (octaveOffsets[idx] != clamped)
        {
            octaveOffsets[idx] = clamped;
            notifyStateChanged();
        }
    }
}

int ShiftLayerSystem::getActiveOctaveOffset() const
{
    return getOctaveOffset(getActiveBank());
}

void ShiftLayerSystem::setShiftHeld(bool held)
{
    if (shiftHeld != held)
    {
        shiftHeld = held;
        notifyStateChanged();
    }
}

void ShiftLayerSystem::setCapsLockOn(bool on)
{
    if (capsLockOn != on)
    {
        capsLockOn = on;
        notifyStateChanged();
    }
}

void ShiftLayerSystem::setNumpadModeEnabled(bool enabled)
{
    numpadMode = enabled;
}

void ShiftLayerSystem::resetModifiers()
{
    shiftHeld = false;
    capsLockOn = false;
    tabHeld = false;
    notifyStateChanged();
}

void ShiftLayerSystem::notifyStateChanged()
{
    if (onStateChanged != nullptr)
        onStateChanged(getActiveBank(), getActiveOctaveOffset());
}

int ShiftLayerSystem::translateNumpadKey(int vkCode) const
{
    // Translates numeric keypad to standard 3x4 MPC pad grid:
    // Row 1 (top):    Pad 7, 8, 9
    // Row 2:          Pad 4, 5, 6
    // Row 3:          Pad 1, 2, 3
    // Row 4 (bottom): Pad 0, Decimal, Enter
    switch (vkCode)
    {
        case 0x67: return 0x101; // Numpad 7 -> Pad 1
        case 0x68: return 0x102; // Numpad 8 -> Pad 2
        case 0x69: return 0x103; // Numpad 9 -> Pad 3
        case 0x64: return 0x104; // Numpad 4 -> Pad 4
        case 0x65: return 0x105; // Numpad 5 -> Pad 5
        case 0x66: return 0x106; // Numpad 6 -> Pad 6
        case 0x61: return 0x107; // Numpad 1 -> Pad 7
        case 0x62: return 0x108; // Numpad 2 -> Pad 8
        case 0x63: return 0x109; // Numpad 3 -> Pad 9
        case 0x60: return 0x10A; // Numpad 0 -> Pad 10
        case 0x6E: return 0x10B; // Numpad . -> Pad 11
        case 0x0D: return 0x10C; // Enter    -> Pad 12
        case 0x6B: return 0x10D; // Numpad + -> Pad 13
        case 0x6D: return 0x10E; // Numpad - -> Pad 14
        case 0x6A: return 0x10F; // Numpad * -> Pad 15
        case 0x6F: return 0x110; // Numpad / -> Pad 16
        default:   return vkCode;
    }
}

bool ShiftLayerSystem::processKeyEvent(const RawKeyEvent& raw, ProcessedKeyEvent& out)
{
    out.deviceId = raw.deviceId;
    out.rawVirtualKeyCode = raw.virtualKeyCode;
    out.isKeyDown = raw.isKeyDown;

    // Shift modifier interception
    if (raw.virtualKeyCode == kVkShift || raw.virtualKeyCode == kVkLShift || raw.virtualKeyCode == kVkRShift)
    {
        setShiftHeld(raw.isKeyDown);
        out.shouldTriggerSound = false;
        return false;
    }

    // CapsLock toggle interception (latched)
    if (raw.virtualKeyCode == kVkCapital)
    {
        if (raw.isKeyDown)
            setCapsLockOn(! capsLockOn);

        out.shouldTriggerSound = false;
        return false;
    }

    // Tab modifier interception
    if (raw.virtualKeyCode == kVkTab)
    {
        tabHeld = raw.isKeyDown;
        out.shouldTriggerSound = false;
        return false;
    }

    // Tab combination: Octave Transposition
    if (tabHeld)
    {
        if (raw.isKeyDown && raw.virtualKeyCode >= 0x31 && raw.virtualKeyCode <= 0x35)
        {
            // '1': -2, '2': -1, '3': 0, '4': +1, '5': +2
            const int targetOctave = (raw.virtualKeyCode - 0x31) - 2;
            setOctaveOffset(getActiveBank(), targetOctave);
        }
        out.shouldTriggerSound = false;
        return false;
    }

    // Regular performance key
    out.activeBank = getActiveBank();
    out.octaveOffset = getActiveOctaveOffset();
    out.effectiveVirtualKeyCode = numpadMode ? translateNumpadKey(raw.virtualKeyCode) : raw.virtualKeyCode;
    out.shouldTriggerSound = true;
    return true;
}
