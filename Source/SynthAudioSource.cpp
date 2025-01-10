/*
  ==============================================================================

    SynthAudioSource.cpp
    Created: 14 Jul 2023 12:56:06pm
    Author:  Aleksandr Shabelnikov

  ==============================================================================
*/

#include <JuceHeader.h>
#include "SynthAudioSource.h"

#define VOICES 10

SynthAudioSource::SynthAudioSource(juce::MidiKeyboardState& keyState) : keyboardState(keyState) 
{
        // Initialize your synth here.
        std::cout << "SynthAudioSource constructor called\n";
        for (int i = 0; i < VOICES; ++i) {
            synth.addVoice(new SynthVoice());
            std::cout << "Added voice " << i << "\n";
        }

        synth.addSound(new SynthSound());
        std::cout << "Added sound\n";

    }
