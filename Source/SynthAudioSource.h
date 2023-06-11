/*
  ==============================================================================

    SynthAudioSource.h
    Created: 10 Jun 2023 2:54:11pm
    Author:  Aleksandr Shabelnikov

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "Filter.h"

class SynthAudioSource : public juce::AudioSource
{
public:
    SynthAudioSource(juce::MidiKeyboardState& keyState, juce::MidiKeyboardComponent::Orientation orientation);

    void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;
    void releaseResources() override;
    void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill) override;


    int getActiveVoiceCount() {
        int count = 0;
        for (int i = 0; i < synth.getNumVoices(); i++) {
            if (synth.getVoice(i)->isVoiceActive()) {
                count++;
            }
        }
        if (count == 0) {
            return 1;
        }
        return count;
    } 

    void setOscType(int oscType, int oscIndex) {
        for (int i = 0; i < synth.getNumVoices(); i++) {
            if (auto voice = dynamic_cast<SynthVoice*>(synth.getVoice(i))) {
                voice->setOscType(oscType, oscIndex);
            }
        }
    }

    void setOscGain(int oscIndex, float gain) {
        for (int i = 0; i < synth.getNumVoices(); i++) {
            if (auto voice = dynamic_cast<SynthVoice*>(synth.getVoice(i))) {
                voice->setOscGain(oscIndex, gain);
            }
        }
    }

    void setDetune(int oscIndex, float detune) {
        for (int i = 0; i < synth.getNumVoices(); i++) {
            if (auto voice = dynamic_cast<SynthVoice*>(synth.getVoice(i))) {
                voice->setDetune(oscIndex, detune);
            }
        }
    }

    void setPulseWidth(int oscIndex, float pw) {
        for (int i = 0; i < synth.getNumVoices(); i++) {
            if (auto voice = dynamic_cast<SynthVoice*>(synth.getVoice(i))) {
                voice->setPulseWidth(oscIndex, pw);
            }
        }
    }

    int getOscType(int oscIndex) {
      return dynamic_cast<SynthVoice*>(synth.getVoice(0))->getWaveform(oscIndex);
    }

    juce::MidiMessageCollector* getMidiCollector()
    {
        return &midiCollector;
    }

private:
    juce::MidiKeyboardState& keyboardState;
    juce::Synthesiser synth;
    juce::MidiKeyboardComponent::Orientation keyboardOrientation;
    juce::MidiMessageCollector midiCollector;
    Filter filter;
};
