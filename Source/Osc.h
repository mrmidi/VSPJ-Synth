

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
            // Add more waveforms as needed.
        };


    void setGain(float newGain)
    {
        if (newGain != gain) {
            DBG("SETTING OSC GAIN TO " << newGain);
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

void setFrequency(float newFrequency)
{
        originalFrequency = newFrequency;
        frequency = newFrequency * pow(2, octave) + detune / 100.0f;
        
        // This line actually sets the frequency of the base oscillator
        juce::dsp::Oscillator<float>::setFrequency(frequency);
        
        // DBG("SETTING OSC FREQUENCY TO " << frequency);
    
}



    void setDetune(int newDetune)
    {
        if (newDetune != detune) {
            detune = newDetune;
            DBG("SETTING OSC DETUNE TO " << detune);
        }

    }

    void setPulseWidth(float newPulseWidth)
    {
        if (newPulseWidth != pulseWidth) {
            pulseWidth = newPulseWidth;
            DBG("SETTING OSC PULSE WIDTH TO " << pulseWidth);
        }
    }

    void setOctave(int newOctave)
    {
        if (newOctave != octave) {
            octave = newOctave;
            DBG("SETTING OSC OCTAVE TO " << octave);
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

    Waveform waveform;
};
