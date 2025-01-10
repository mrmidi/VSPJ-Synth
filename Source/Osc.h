

/*
  ===========================================================================
    Osc.h
    Created: 10 Jun 2023 8:30:00
    Author:  Aleksandr Shabelnikov
  ===========================================================================
*/

#pragma once
#include <JuceHeader.h>

class Oscillator : public juce::dsp::Oscillator<float>
{
public:
    Oscillator()
    {
        // By default, use a sine wave
        initialise([] (float x) { return std::sin(x); });
    }
    
        enum Waveform
        {
            Sine,
            Sawtooth,
            Square,
            Triangle,
            Pulse,
        };


    void setGain(float newGain)
    {
        if (newGain != gain) {
            // DBG("SETTING OSC GAIN TO " << newGain);
            gain = newGain;
        }
            
    }
    float getOriginalFrequency() {
        return originalFrequency;
    }

    void setWaveformToSine()
    {
        initialise([] (float x) { return std::sin(x); });
    }

    // Add methods to set the waveform to other shapes as desired

    // Process the next sample

    void setWaveform(Waveform newWaveform);

    void prepareToPlay (double sampleRate, int samplesPerBlock, int outputChannels);

    float getNextSample()
    {
        return juce::dsp::Oscillator<float>::processSample(1.0f) * velocity * gain;
    }

    void setVelocity(float newVelocity)
    {
        if (newVelocity != velocity) {

            velocity = newVelocity;
        }
    }

    void setRawFrequency(float rawFrequency) {
        frequency = rawFrequency;
        juce::dsp::Oscillator<float>::setFrequency(frequency);
    }

    // Set the musical frequency, considering octave and detune adjustments
    // void setMusicalFrequency(float baseFrequency) {
    //     originalFrequency = baseFrequency;
    //     frequency = originalFrequency * std::pow(2, octave) + detune / 100.0f;
    //     juce::dsp::Oscillator<float>::setFrequency(frequency);
    //     //DBG("SETTING OSC FREQUENCY TO " << frequency);
    // }

    void setMusicalFrequency(float baseFrequency) {
        originalFrequency = baseFrequency;
        frequency = baseFrequency * pow(2, octave) * pitchBendMultiplier + detune;
        juce::dsp::Oscillator<float>::setFrequency(frequency);
    }   


    void setPitchBendMultiplier(float multiplier) {
        pitchBendMultiplier = multiplier;
        setMusicalFrequency(originalFrequency);
    }

    void setDetune(int newDetune)
    {
        if (newDetune != detune) {
            detune = newDetune;
            setMusicalFrequency(originalFrequency);
        }
    }

    void setOctave(int newOctave)
    {
        if (newOctave != octave) {
            octave = newOctave;
            setMusicalFrequency(originalFrequency);
        }
    }
    void setPulseWidth(float newPulseWidth)
    {
        if (newPulseWidth != pulseWidth) {
            pulseWidth = newPulseWidth;
            DBG("SETTING OSC PULSE WIDTH TO " << pulseWidth);
        }
    }



private:
//    float frequency = 440.0f;  // Default to A4
    float gain = 0.5f;         // Default to half amplitude
    float pulseWidth = 0.5f;   // Same as square
    float velocity;     // Default to half velocity
    int detune = 0;    // Default to no detune
    int octave = 0;    // Default to no octave change
    float frequency = 440.0f;  // Default to A4
    float originalFrequency = 440.0f;   // Default to A4
    bool allowPWMOnAllWaveforms = false;
    float pitchBendMultiplier = 1.0f;

    Waveform waveform;
};
