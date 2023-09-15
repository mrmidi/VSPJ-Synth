/*
  ==============================================================================

    SynthAudioSource.cpp
    Created: 14 Jul 2023 12:56:06pm
    Author:  Aleksandr Shabelnikov

  ==============================================================================
*/
#pragma once
#include <JuceHeader.h>
#include "SynthSound.h"
#include "SynthVoice.h"


class SynthAudioSource : public juce::AudioSource
{
public:

    SynthAudioSource(juce::MidiKeyboardState& keyState);


    void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override {
        // This is where you'll prepare your synth to start producing sound.
        // synth.setCurrentPlaybackSampleRate(sampleRate);
        midiCollector.reset(sampleRate);
        DBG("MidiCollector reset");
        synth.setCurrentPlaybackSampleRate(sampleRate);
        DBG("SynthAudioSource::prepareToPlay called with sample rate " << sampleRate
                << " and samples per block expected " << samplesPerBlockExpected);
        // prepare to play voices
        int numVoices = synth.getNumVoices();
        DBG("Synth has " << numVoices << " voices");
        for (int i = 0; i < numVoices; ++i) {
            auto* voice = synth.getVoice(i);
            voice->setCurrentPlaybackSampleRate(sampleRate);
            DBG("Voice " << i << " Sample rate is " << voice->getSampleRate());
        }
    }
    
    juce::MidiMessageCollector* getMidiCollector()
    {
        return &midiCollector;
    }

    void releaseResources() override {
        // This is where you would deallocate any resources you no longer need.
    }


    void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill) override {

    }

    
    juce::Synthesiser synth;
private:
    juce::MidiKeyboardState& keyboardState;
    juce::MidiMessageCollector midiCollector;
};
