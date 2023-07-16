

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

//    void setFrequency(float newFrequency)
//    {
//        frequency = newFrequency;
////        setFrequency(frequency);
//        DBG("OSC FREQUENCY: " << frequency);
//    }

    void setGain(float newGain)
    {
        gain = newGain;
    }

    void setWaveformToSine()
    {
        initialise([] (float x) { return std::sin(x); });
    }

    // Add methods to set the waveform to other shapes as desired

    // Process the next sample

    void setWaveform(Waveform newWaveform);

private:
//    float frequency = 440.0f;  // Default to A4
    float gain = 0.5f;         // Default to half amplitude
    float pulseWidth = 0.5f;   // Same as square
    Waveform waveform;
};

//
//#pragma once
//#include <JuceHeader.h>
//
//class Oscillator : public juce::dsp::Oscillator<float>
//{
//public:
//    Oscillator();
//
//    enum Waveform
//    {
//        Sine,
//        Sawtooth,
//        Square,
//        Triangle,
//        Pulse,
//        // Add more waveforms as needed.
//    };
//
//    void setWaveform(Waveform newWaveform);
//
//    void updateOscillatorFrequency();
//
//    float getNextSample(float sample);
//
//    void setFrequency(float freq);
//
//    void setGain(float newGain) { gain = newGain; }
//
//    double getGain() { return gain; }
//    int getWaveform() { return this->waveform; } // just to check it's actually changing
//
//    void setDetune(float detune);
//
//    void setPulseWidth(float newPulseWidth) {
//      printf("SETTING PULSE WIDTH TO %f\n", newPulseWidth);
//      pulseWidth = newPulseWidth;
//    }
//
//    void setSampleRate(double sampleRate);
//
//    void prepare(double sampleRate, int samplesPerBlock);
//
//    void prepare(juce::dsp::ProcessSpec& spec);
//
//    void renderNextBlock(juce::dsp::AudioBlock<float>& block);
//
//
//private:
//    juce::dsp::Oscillator<float> oscillator;
//    Waveform waveform;
//
//    float gain = 1.0f;
//    float baseFrequency = 0;
//    float detuneAmount = 0;
//    float pulseWidth = 0.5f;
//};
