/*
  ============================================================================
    Osc.h
    Created: 10 Jun 2023 8:30:pm
    Author:  Aleksandr Shabelnikov
  ============================================================================
*/
#pragma once

#include <JuceHeader.h>

class Oscillator
{
public:
    Oscillator();

    enum Waveform
    {
        Sine,
        Sawtooth,
        Square,
        Triangle,
        Pulse,
        // Add more waveforms as needed.
    };

    void setWaveform(Waveform newWaveform);

    float getNextSample();

    void setFrequency(float freq);

    void setGain(float newGain) { gain = newGain; }

    double getGain() { return gain; }
    int getWaveform() { return this->waveform; } // just to check it's actually changing

    void setDetune(float detune);

    void setPulseWidth(float newPulseWidth) {
      printf("SETTING PULSE WIDTH TO %f\n", newPulseWidth);
      pulseWidth = newPulseWidth;
    }


private:
    juce::dsp::Oscillator<float> oscillator;
    Waveform waveform;
    float gain = 1.0f;
    float baseFrequency = 0;
    float detuneAmount = 0;
    float pulseWidth = 0.5f;
};
