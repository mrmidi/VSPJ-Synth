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
    originalFrequency = frequency;



    osc1.setFrequency(frequency);
    osc2.setFrequency(frequency);
    
    // You might also want to set the level of the oscillator based on the velocity.
    osc1.setVelocity(velocity);
    osc2.setVelocity(velocity);
    
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
         applyPitchBend(newPitchWheelValue);
        
    }

    void applyPitchBend(int pitchWheelValue) {
        // Assuming the pitch bend range is ±2 semitones
        float bendRangeInSemitones = 2.0f;

        // Calculate how far the pitch wheel is from the center
        float bendAmount = (pitchWheelValue - 8192.0f) / 8192.0f;

        // Calculate the frequency multiplier
        float frequencyMultiplier = std::pow(2.0f, bendRangeInSemitones * bendAmount / 12.0f);



        // Apply the frequency multiplier to your oscillators
        osc1.setFrequency(originalFrequency * frequencyMultiplier);
        osc2.setFrequency(originalFrequency * frequencyMultiplier);
    }


    void controllerMoved(int controllerNumber, int newControllerValue) override {
        // Handle controller movements
    }
    
    void prepareToPlay(int samplesPerBlockExpected, double sampleRate, int numChannels) {
        // This is where you can do the pre-playback calculations..
        //reset();
        //osc1.setSampleRate(sampleRate);
        // todo complete reset
        
        juce::dsp::ProcessSpec spec;
        spec.maximumBlockSize = samplesPerBlockExpected;
        spec.sampleRate = sampleRate;
        spec.numChannels = numChannels;
        
        adsr.reset();
        adsr.setSampleRate(sampleRate);

        for (int i = 0; i < numChannels; ++i) {
            DBG("Initializing oscillator " << i);
            osc1.prepareToPlay(sampleRate, samplesPerBlockExpected, numChannels);
            osc2.prepareToPlay(sampleRate, samplesPerBlockExpected, numChannels);
        }
        DBG("Initialization of oscillators finished");
        isPrepared = true;
        
        
        
    }

    
    void renderNextBlock(juce::AudioBuffer<float>& outputBuffer, int startSample, int numSamples) override {
        
        
        jassert(isPrepared);
        if (!isVoiceActive())
            return;

        for (int sample = 0; sample < numSamples; ++sample)
        {
            // Get the next sample from the oscillator
            auto oscSample = osc1.getNextSample();
            auto osc2Sample = osc2.getNextSample();

            // Apply the ADSR envelope to the oscillators samples
            auto envSample = adsr.getNextSample() * oscSample;
            auto envSample2 = adsr.getNextSample() * osc2Sample;

        for (int channel = 0; channel < outputBuffer.getNumChannels(); ++channel)
        {
            // Mix the processed samples of osc1 and osc2 and add to the output buffer
            outputBuffer.addSample(channel, startSample + sample, (envSample + envSample2) * 0.5f);
        }

        }
    }
    //voice->setOsc1Params(osc1Octave.load(), osc1Cent.load(), osc1Gain.load(), osc1PulseWidth.load(), osc1WaveformType.load());

    void setOsc1Params(int octave, int cent, float gain, float pulseWidth, int waveformType) {

        osc1.setWaveform(static_cast<Oscillator::Waveform>(waveformType));
        osc1.setGain(gain);
        osc1.setOctave(octave);
        osc1.setDetune(cent);
        osc1.setPulseWidth(pulseWidth);



    }

    void setOsc2Params(int octave, int cent, float gain, float pulseWidth, int waveformType) {

        osc2.setWaveform(static_cast<Oscillator::Waveform>(waveformType));
        osc2.setGain(gain);
        osc2.setOctave(octave);
        osc2.setDetune(cent);
        osc2.setPulseWidth(pulseWidth);


    }

    private:
    juce::AudioBuffer<float> synthBuffer;
    Oscillator osc1;
    Oscillator osc2;
    Adsr adsr;
    bool isPrepared = false;
    int osc1detune = 0;
    int osc1octave = 0;;
    float osc1gain = 0.5f;
    float osc1pulsewidth = 0.5f;
    int osc2detune = 0;
    int osc2octave = 0;
    float osc2gain = 0.5f;
    float osc2pulsewidth = 0.5f;
    float originalFrequency = 440.0f;

};
