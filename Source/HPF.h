/*
  ==============================================================================

    HPF.h
    Created: 15 Sep 2023 2:59:13pm
    Author:  Alexander Shabelnikov

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

class HighPassFilter
{
public:
    HighPassFilter() = default;

    // Prepare the filter with a given sample rate
    void prepare(const juce::dsp::ProcessSpec& spec);

    // Set the cutoff frequency
    void setCutoffFrequency(double frequency);

    // Process a sample through the filter
    float process(float inputSample);

    void prepareToPlay(double sampleRate, int samplesPerBlock, int numChannels);

    float getFrequency();

private:
    juce::dsp::IIR::Filter<float> filter;
    juce::dsp::ProcessSpec spec;

    float frequency = 20.0; // will be set in prepare() 
};