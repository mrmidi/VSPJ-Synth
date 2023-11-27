/*
  ==============================================================================

    LFOsc.h
    Created: 8 Sep 2023 10:44:55pm
    Author:  Alexander Shabelnikov

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

class LFOsc
{
public:
    LFOsc();

    // Set the frequency (rate) of the LFO
    void setFrequency(float freq);

    // Set the depth (amplitude) of the LFO
    void setDepth(float depth);

    // Process and get the next sample value from the LFO
    float getNextSample();

    // Prepare the LFO for playback
    void prepareToPlay (double sampleRate, int samplesPerBlock, int outputChannels);

    enum lfoType {
        TREMOLO,
        PWM,
        FILTER
    };

    // Set the type of the LFO
    void setType(lfoType type);

    // Get the current type of the LFO
    lfoType getType() const;

private:
    juce::dsp::Oscillator<float> oscillator;
    float frequency = 1.0f;  // 1 Hz by default
    float depth = 1.0f;      // Full depth by default



    lfoType currentType = TREMOLO;
};
