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

class SynthVoice : public juce::SynthesiserVoice
{
public:
    SynthVoice() {
        // Initialization of the voice
        osc1.setWaveform(Oscillator::Sine);
    }

    bool canPlaySound(juce::SynthesiserSound* sound) override {
        // Return true if this voice can play the given sound
        return dynamic_cast<SynthSound*>(sound) != nullptr;
    }


    void startNote(int midiNoteNumber, float velocity,
                juce::SynthesiserSound* sound, int currentPitchWheelPosition) override {
        // Convert the MIDI note number to a frequency.
        originalFrequency = juce::MidiMessage::getMidiNoteInHertz(midiNoteNumber);

        DBG("Starting note with midi note number " << midiNoteNumber << " with frequency " << originalFrequency);

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
        
        //adsr.updateParams(0.1f, 0.2f, 0.8f, 0.5f); // default values will be overriden by APVTS
        adsr.noteOn();
        //filterAdsr.updateParams(0.1f, 0.2f, 0.8f, 0.5f); // default values will be overriden by APVTS
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


                // oversampling
        int overSamplingFactor;

        if (sampleRate < 48000) {
            overSamplingFactor = 2;
            upSampledBufferSize = samplesPerBlockExpected * overSamplingFactor;
            upSamledSampleRate = sampleRate * static_cast<float>(overSamplingFactor);
        }
        else if (sampleRate >= 48000 && sampleRate < 80000) {
            overSamplingFactor = 2;
            upSampledBufferSize = samplesPerBlockExpected * overSamplingFactor;
            upSamledSampleRate = sampleRate * static_cast<float>(overSamplingFactor);
        }
        else if (sampleRate < 80000) {
            overSamplingFactor = 1;
            upSampledBufferSize = samplesPerBlockExpected * overSamplingFactor;
            upSamledSampleRate = sampleRate * static_cast<float>(overSamplingFactor);
        }
        else {
            overSamplingFactor = 1;
            upSampledBufferSize = samplesPerBlockExpected * overSamplingFactor;
            upSamledSampleRate = sampleRate * static_cast<float>(overSamplingFactor);
        }

        oversampler.reset();
        oversampler.factorOversampling = overSamplingFactor;
        oversampler.initProcessing(samplesPerBlockExpected);

        
        
        adsr.reset();
        adsr.setSampleRate(sampleRate);

        for (int i = 0; i < numChannels; ++i) {
            //DBG("Initializing oscillator " << i);
            DBG("Initializing oscillator 1, channel " << i << " with sample rate " << (String)upSamledSampleRate << " and buffer size " << (String)upSampledBufferSize);
            osc1.prepareToPlay(upSamledSampleRate, upSampledBufferSize, numChannels);
            DBG("Initializing oscillator 2, channel " << i << " with sample rate " << (String)upSamledSampleRate << " and buffer size " << (String)upSampledBufferSize);
            osc2.prepareToPlay(upSamledSampleRate, upSamledSampleRate, numChannels);

            // osc1.prepareToPlay(sampleRate, samplesPerBlockExpected, numChannels);
            // osc2.prepareToPlay(sampleRate, samplesPerBlockExpected, numChannels);

            DBG("Initializing filter " << i);
            filter.prepareToPlay(sampleRate, samplesPerBlockExpected, numChannels);

            DBG("Initializing LFO " << i);
            tremoloLFO.prepareToPlay(sampleRate, samplesPerBlockExpected, numChannels);

        }

        // for (int ch = 0; ch < numChannels; ch++) {
        //     DBG("Initializing oscillator 1, channel " << ch << " with sample rate " << (String)upSamledSampleRate << " and buffer size " << (String)upSampledBufferSize);
        //     oscArray1[ch].prepareToPlay(upSamledSampleRate, upSampledBufferSize, numChannels);
        //     DBG("Initializing oscillator 2, channel " << ch);
        //     oscArray2[ch].prepareToPlay(upSamledSampleRate, upSamledSampleRate, numChannels);
        // }

 
        
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

        filterAdsr.updateParams(attack, decay, sustain, release);
        filter.setBaseCutOffFreq(baseCutoffFreq);


    }

    private:

    // The audio programmer youtube Josh Hodge recommends
    // to organize signal chain declaration in actual order of processing
    // (i.e. osc -> filter -> adsr -> output)

    dsp::Oversampling<float> oversampler{ 
        2, // num channels
        2, // factor
        dsp::Oversampling<float>::filterHalfBandPolyphaseIIR, // filter type
        false, // isMaxQuality
        true // useIntegerLatency
        };

    Oscillator osc1; // oscillators
    Oscillator osc2;

    std::array<Oscillator, 2> oscArray1;
    std::array<Oscillator, 2> oscArray2;

    Adsr adsr; // amplitude envelope
 
    LFOsc tremoloLFO; // tremolo LFO
    
    Filter filter;
    Adsr filterAdsr;

    juce::AudioBuffer<float> synthBuffer;

    int upSampledBufferSize { 0 };
    float upSamledSampleRate { 0 };

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
