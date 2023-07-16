/*
  ==============================================================================

    SynthSound.h
    Created: 14 Jul 2023 1:17:28pm
    Author:  Aleksandr Shabelnikov

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

class SynthSound : public juce::SynthesiserSound
{
public:
    SynthSound() {}

    bool appliesToNote(int midiNoteNumber) override {
        // Return true if this sound should be used for the given MIDI note number.
        return true;
    }

    bool appliesToChannel(int midiChannel) override {
        // Return true if this sound should be used on the given MIDI channel.
        return true;
    }
};

