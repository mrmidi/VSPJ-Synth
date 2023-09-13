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
#include "LFOsc.h"
#include "Filter.h"

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
        originalFrequency = juce::MidiMessage::getMidiNoteInHertz(midiNoteNumber);

        osc1.setMusicalFrequency(originalFrequency);
        osc2.setMusicalFrequency(originalFrequency);


        // Apply pitch bend if the pitch wheel is not centered
        //DBG("Starting note with original frequency: " << originalFrequency);
        if (currentPitchWheelPosition != 8192) {
            applyPitchBend(currentPitchWheelPosition);
            //DBG("Applied pitch bend. New frequency for osc1: " << osc1.getOriginalFrequency());
        }

        // Set the level of the oscillator based on the velocity.
        osc1.setVelocity(velocity);
        osc2.setVelocity(velocity);
        
        adsr.updateParams(0.1f, 0.2f, 0.8f, 0.5f); // default values will be overriden by APVTS
        adsr.noteOn();
        filterAdsr.updateParams(0.1f, 0.2f, 0.8f, 0.5f); // default values will be overriden by APVTS
        filterAdsr.noteOn();
    }

    void stopNote(float velocity, bool allowTailOff) override {
        // This is called when a note stops
        adsr.noteOff();
        filterAdsr.noteOff();
        
        if (! allowTailOff || ! adsr.isActive())
        //osc1.reset();
            clearCurrentNote();
    }

    void pitchWheelMoved(int newPitchWheelValue) override {
        // Handle pitch wheel changes
         applyPitchBend(newPitchWheelValue);
        
    }

    void applyPitchBend(int pitchWheelValue) {
       //DBG("Pitch wheel raw value: " << pitchWheelValue);

        // Assuming the pitch bend range is ±2 semitones
        float bendRangeInSemitones = 2.0f;

        // Calculate how far the pitch wheel is from the center
        float bendAmount = (pitchWheelValue - 8192.0f) / 8192.0f;
        //DBG("Bend amount: " << bendAmount);

        // Calculate the frequency multiplier
        float frequencyMultiplier = std::pow(2.0f, bendRangeInSemitones * bendAmount / 12.0f);
        //DBG("Frequency multiplier: " << frequencyMultiplier);

        // Apply the frequency multiplier to your oscillators
        osc1.setPitchBendMultiplier(frequencyMultiplier);
        osc2.setPitchBendMultiplier(frequencyMultiplier);
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
        tremoloLFO.prepareToPlay(sampleRate, samplesPerBlockExpected, numChannels);
        tremoloLFO.setDepth(0.5f);       // Depth of 50%
        tremoloLFO.setFrequency(5);   // Rate of 5 Hz

        // prepare the filter
        filterAdsr.reset();
        filterAdsr.setSampleRate(sampleRate);

        filter.prepareToPlay(sampleRate, samplesPerBlockExpected, numChannels);

        DBG("Initialization finished");
        isPrepared = true;
    }

    
    void renderNextBlock(juce::AudioBuffer<float>& outputBuffer, int startSample, int numSamples) override;
    //voice->setOsc1Params(osc1Octave.load(), osc1Cent.load(), osc1Gain.load(), osc1PulseWidth.load(), osc1WaveformType.load());

void setOsc1Params(int octave, int cent, float gain, float pulseWidth, int waveformType) {
    osc1.setWaveform(static_cast<Oscillator::Waveform>(waveformType));
    osc1.setGain(gain);
    osc1.setOctave(octave);
    osc1.setDetune(cent);
    osc1.setPulseWidth(pulseWidth);

    // Update the frequency based on the new parameters
    osc1.setMusicalFrequency(originalFrequency);
}

    void enableLFO(bool isEnabled);
    
void setOsc2Params(int octave, int cent, float gain, float pulseWidth, int waveformType) {
    osc2.setWaveform(static_cast<Oscillator::Waveform>(waveformType));
    osc2.setGain(gain);
    osc2.setOctave(octave);
    osc2.setDetune(cent);
    osc2.setPulseWidth(pulseWidth);

    // Update the frequency based on the new parameters
    osc2.setMusicalFrequency(originalFrequency);
}

    void setLFOParams(float depth, float frequency, int source, int type) {
        tremoloLFO.setDepth(depth);
        tremoloLFO.setFrequency(frequency);
    }

    // voice->setADSRParams(adsrAttack.load(), adsrDecay.load(), adsrSustain.load(), adsrRelease.load());
    void setADSRParams(float attack, float decay, float sustain, float release) {
        adsr.updateParams(attack, decay, sustain, release);
    }

    void setFilterParams(int type, float cutoff, float resonance, float amount) {
        filterAmount = amount;
        filter.setParams(type, cutoff, resonance);
    }

    void setFilterAdsrParams(float attack, float decay, float sustain, float release, float baseCutoffFreq) {

        float currentAttack, currentDecay, currentSustain, currentRelease;
        filterAdsr.getParams(currentAttack, currentDecay, currentSustain, currentRelease);
        if (currentAttack == attack && currentDecay == decay && currentSustain == sustain && currentRelease == release) {
            return;
        }
        DBG("Updating filter ADSR params to attack: " << attack << ", decay: " << decay << ", sustain: " << sustain << ", release: " << release);
        DBG("Current values: attack: " << currentAttack << ", decay: " << currentDecay << ", sustain: " << currentSustain << ", release: " << currentRelease);
        filterAdsr.updateParams(attack, decay, sustain, release);
        filter.setBaseCutOffFreq(baseCutoffFreq);

    }

    private:
    LFOsc tremoloLFO;
    juce::AudioBuffer<float> synthBuffer;
    Oscillator osc1;
    Oscillator osc2;
    Adsr adsr;
    Adsr filterAdsr;
    Filter filter;

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

    float filterAmount = 1.0f; // will be overriden by APVTS

    bool isLFOEnabled = true;

};
