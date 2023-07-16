/*
  ==============================================================================

    SynthVoice.h
    Created: 14 Jul 2023 1:18:09pm
    Author:  Aleksandr Shabelnikov

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "Osc.h"
#include "Adsr.h"
#include "SynthSound.h"

class SynthVoice : public juce::SynthesiserVoice
{
public:
    SynthVoice() {
        // Initialization of the voice
        osc1.setWaveform(Oscillator::Sine);
//        osc1.setDetune(0.0f);
//        osc1.setGain(0.5f);
    }

    bool canPlaySound(juce::SynthesiserSound* sound) override {
        // Return true if this voice can play the given sound
        return dynamic_cast<SynthSound*>(sound) != nullptr;
    }

    void startNote(int midiNoteNumber, float velocity,
                   juce::SynthesiserSound* sound, int currentPitchWheelPosition) override {
        // Convert the MIDI note number to a frequency.
        auto frequency = juce::MidiMessage::getMidiNoteInHertz(midiNoteNumber);

        osc1.setFrequency(frequency);
        DBG("START NOTE FREQUENCY IS " << frequency);
        // You might also want to set the level of the oscillator based on the velocity.
        osc1.setGain(velocity);
        adsr.updateParams(0.1f, 0.2f, 0.8f, 0.5f);
        adsr.noteOn();
    }
    
    void stopNote(float velocity, bool allowTailOff) override {
        // This is called when a note stops
        adsr.noteOff();
//        filterAdsr.noteOff();
        
        if (! allowTailOff || ! adsr.isActive())
        //osc1.reset();
            clearCurrentNote();
    }

    void pitchWheelMoved(int newPitchWheelValue) override {
        // Handle pitch wheel changes
    }

    void controllerMoved(int controllerNumber, int newControllerValue) override {
        // Handle controller movements
    }
    
    void prepareToPlay(int samplesPerBlockExpected, double sampleRate, int numChannels) {
        // This is where you can do the pre-playback calculations..
        //reset();
        //osc1.setSampleRate(sampleRate);
        
        juce::dsp::ProcessSpec spec;
        spec.maximumBlockSize = samplesPerBlockExpected;
        spec.sampleRate = sampleRate;
        spec.numChannels = numChannels;
        
        adsr.reset();
        adsr.setSampleRate(sampleRate);
        osc1.prepare(spec);
//        osc1.setGain(1.0f);
//        osc1.setDetune(0.0f);
        
        
        
        
    }
//void renderNextBlock(juce::AudioBuffer<float>& outputBuffer, int startSample, int numSamples) override {
//    // Here is where you will generate the sound for this voice
//    // Generate the sound by processing the oscillator.
//    if (!isVoiceActive())
//        return;
//
//    synthBuffer.setSize(outputBuffer.getNumChannels(), numSamples, false, false, true);
//
//    synthBuffer.clear();
//    // for (int channel = 0; channel < synthBuffer.getNumChannels(); ++channel) {
//    //     auto buffer = synthBuffer.getWritePointer(channel);
//    //     for (int sample = 0; sample < synthBuffer.getNumSamples(); ++sample) {
//    //         buffer[sample] = osc1.getNextSample(buffer[sample]);
//    //     }
//    // }
//
//    //juce::dsp::AudioBlock<float> block { synthBuffer };
//
//    // Create a context for the oscillator to write into
//    juce::dsp::AudioBlock<float> block (outputBuffer);
//    juce::dsp::ProcessContextReplacing<float> context (block);
//
//    // Process the oscillator
//    osc1.process(context);
//
//    adsr.applyEnvelopeToBuffer(outputBuffer, 0, synthBuffer.getNumSamples());
//
//    for (int channel = 0; channel < outputBuffer.getNumChannels(); ++channel) {
//        auto output = outputBuffer.getWritePointer(channel);
//        auto s = synthBuffer.getReadPointer(channel);
//        for (int sample = 0; sample < outputBuffer.getNumSamples(); ++sample) {
//            output[sample] += s[sample];
//        }
//    }
//}
    
    void renderNextBlock(juce::AudioBuffer<float>& outputBuffer, int startSample, int numSamples) override {
        if (!isVoiceActive())
            return;

        // Clear the output buffer
//        outputBuffer.clear();

        for (int sample = 0; sample < numSamples; ++sample)
        {
            // Get the next sample from the oscillator
            auto oscSample = osc1.processSample(1.f);

            // Apply the ADSR envelope to the oscillator sample
            auto envSample = adsr.getNextSample() * oscSample;

            for (int channel = 0; channel < outputBuffer.getNumChannels(); ++channel)
            {
                // Add the processed sample to the output buffer
                outputBuffer.addSample(channel, startSample + sample, envSample);
            }
        }
    }


    private:
    juce::AudioBuffer<float> synthBuffer;
    Oscillator osc1;
    Adsr adsr;
};
