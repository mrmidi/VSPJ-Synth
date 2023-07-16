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
//    SynthAudioSource() {
//        // Here you can initialize your synth.
//        // For example:
//        // synth.clearVoices();
//        // for (int i = 0; i < 5; ++i)
//        //    synth.addVoice(new SynthVoice());
//        // synth.clearSounds();
//        // synth.addSound(new SynthSound());
//    }

    SynthAudioSource(juce::MidiKeyboardState& keyState)
    : keyboardState(keyState) {
        // Initialize your synth here.
        std::cout << "SynthAudioSource constructor called\n";
        for (int i = 0; i < 5; ++i) {
            synth.addVoice(new SynthVoice());
            std::cout << "Added voice " << i << "\n";
        }

        synth.addSound(new SynthSound());
        std::cout << "Added sound\n";

    }

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
    
//    void addMidiMessage(const juce::MidiMessage& message) {
//        midiCollector.addMessageToQueue(message);
//    }

    void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill) override {
        // Here is where you actually produce the sound.
        // You'll want to clear the buffer and then let your synth write into it.
//        bufferToFill.clearActiveBufferRegion();
//        juce::MidiBuffer incomingMidi;
//        midiCollector.removeNextBlockOfMessages(incomingMidi, bufferToFill.numSamples);
//        keyboardState.processNextMidiBuffer(incomingMidi, bufferToFill.startSample,
//                                            bufferToFill.numSamples, true);
//
//        synth.renderNextBlock(*bufferToFill.buffer, incomingMidi, bufferToFill.startSample, bufferToFill.numSamples);
    }

    
    juce::Synthesiser synth;
private:
    //juce::MidiKeyboardState& keyboardState;
    //juce::MidiKeyboardComponent::Orientation keyboardOrientation;
    // Your synth goes here.
    juce::MidiKeyboardState& keyboardState;

    juce::MidiMessageCollector midiCollector;
};
