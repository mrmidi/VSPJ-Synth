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
#include "Delay.h"
#include "OSCMidius.h"
// #include "MFLNEW.h"
// #include "OnePoleFilter.h"
#include "MLF2.h"

class SynthVoice : public juce::SynthesiserVoice
{
public:
    SynthVoice() {
        // Initialization of the voice
        // osc1.setWaveform(Oscillator::Sine);
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
        osc1.setGain(velocity);
        osc2.setGain(velocity);
        
        adsr.updateParams(0.1f, 0.2f, 0.8f, 0.5f); // default values will be overriden by APVTS
        adsr.noteOn();
        filterAdsr.updateParams(0.1f, 0.2f, 0.8f, 0.5f); // default values will be overriden by APVTS
        filterAdsr.noteOn();
    }

    void stopNote(float velocity, bool allowTailOff) override {
        // This is called when a note stops
        adsr.noteOff();
        filterAdsr.noteOff();
        filterAdsr.reset();
        
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
        if (controllerNumber == 1) // 1 is the mod wheel's CC number
        {
            modControllerCutOffFreq = mapModWheelToFrequency(newControllerValue);
            // DBG("Mod wheel value: " << newControllerValue << ", mapped frequency: " << modControllerCutOffFreq);
            // DBG("New cutoff frequency: " << filter.getCutoffFrequency());
            
        }
    }
    
    void prepareToPlay(int samplesPerBlockExpected, double sampleRate, int numChannels) {

        juce::dsp::ProcessSpec spec;
        spec.maximumBlockSize = samplesPerBlockExpected;
        spec.sampleRate = sampleRate;
        spec.numChannels = numChannels;
        
        adsr.reset();
        adsr.setSampleRate(sampleRate);

        for (int i = 0; i < numChannels; ++i) {
            DBG("Initializing oscillator " << i);
//            osc1.prepareToPlay(sampleRate, samplesPerBlockExpected, numChannels);
//            osc2.prepareToPlay(sampleRate, samplesPerBlockExpected, numChannels);
            osc1.setSampleRate(sampleRate);
            osc2.setSampleRate(sampleRate);

            DBG("Initializing filter " << i);
            filter.prepareToPlay(sampleRate);

            DBG("Initializing LFO " << i);
            tremoloLFO.prepareToPlay(sampleRate, samplesPerBlockExpected, numChannels);

        }
        
        tremoloLFO.setDepth(0.5f);       // Depth of 50%
        tremoloLFO.setFrequency(5);   // Rate of 5 Hz


        // prepare the filter
        filterAdsr.reset();
        filterAdsr.setSampleRate(sampleRate);

        DBG("Initialization finished");
        isPrepared = true;
    }


    float mapModWheelToFrequency(int modWheelValue)
    {
        // Ensure the modWheelValue is in the range [0, 127]
        modWheelValue = juce::jlimit(0, 127, modWheelValue);
        //DBG("New freq");

        // Map from [0, 127] to [300, 3200]
        float normalizedValue = modWheelValue / 127.0f; // Converts to [0, 1]
        return 300.0f + normalizedValue * (5000.0f - 300.0f);
    }

    
    void renderNextBlock(juce::AudioBuffer<float>& outputBuffer, int startSample, int numSamples) override;


void setOsc1Params(int octave, int cent, float gain, float pulseWidth, int waveformType) {
    // osc1.setWaveform(static_cast<Oscillator::Waveform>(waveformType));
    // osc1.setGain(gain);
    // osc1.setOctave(octave);
    // osc1.setDetune(cent);
    // osc1.setPulseWidth(pulseWidth);

    // // Update the frequency based on the new parameters
    // osc1.setMusicalFrequency(originalFrequency);
    
    osc1.setWaveform(static_cast<Oscillator::Waveform>(waveformType));
    osc1.setGain(gain);
    osc1.setOctave(octave);
    osc1.setDetune(cent);
    // check if PWM is modlated by lfo and if so - do not update the puls width in this code
    if (tremoloLFO.getType() != LFOsc::lfoType::PWM)
        osc1.setPulseWidth(pulseWidth);

    osc1.setMusicalFrequency(originalFrequency);

}

    void setLfoType(int type) {
        
        // check the value has changed
        if (type == tremoloLFO.getType()) {
            return;
        }
        DBG("Switching LFO type to " << type);
        tremoloLFO.setType(static_cast<LFOsc::lfoType>(type));
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
        filter.setParams(cutoff, resonance); 
        // filter.setParams(cutoff); // testing one-pole filter
    }

    void setFilterAdsrParams(float attack, float decay, float sustain, float release, float baseCutoffFreq) {

        float currentAttack, currentDecay, currentSustain, currentRelease;
        filterAdsr.getParams(currentAttack, currentDecay, currentSustain, currentRelease);
        if (currentAttack == attack && currentDecay == decay && currentSustain == sustain && currentRelease == release) {
            return;
        }

        filterAdsr.updateParams(attack, decay, sustain, release);
        //filter.setBaseCutOffFreq(baseCutoffFreq);


    }

    private:

    // The audio programmer youtube Josh Hodge recommends
    // to organize signal chain declaration in actual order of processing
    // (i.e. osc -> filter -> adsr -> output)

    // Oscillator osc1; // oscillators
    // Oscillator osc2;

    MidiusOsc osc1;
    MidiusOsc osc2;


    
    Adsr adsr; // amplitude envelope
 
    LFOsc tremoloLFO; // tremolo LFO
    
    MoogLadderFilter filter; // filter
    // OnePoleFilter filter;
    Adsr filterAdsr;

    juce::AudioBuffer<float> synthBuffer;

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
    
    float modControllerCutOffFreq = 0; // default value. will be overridden by ModWheel

};
