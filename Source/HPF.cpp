/*
  ==============================================================================

    HPF.cpp
    Created: 15 Sep 2023 2:59:13pm
    Author:  Alexander Shabelnikov

  ==============================================================================
*/

#include "HPF.h"

void HighPassFilter::prepare(const juce::dsp::ProcessSpec& spec)
{
    this->spec = spec;
    filter.prepare(spec);
}

void HighPassFilter::setCutoffFrequency(double frequency)
{
  if (this->frequency == frequency)
        return; // no need to update the filter
    auto coefficients = juce::dsp::IIR::Coefficients<float>::makeHighPass(spec.sampleRate, frequency);
    filter.coefficients = coefficients;

    this->frequency = frequency;
    //DBG("Setting cutoff frequency to: " << frequency);
}

float HighPassFilter::process(float inputSample)
{
    return filter.processSample(inputSample);
}



void HighPassFilter::prepareToPlay(double sampleRate, int samplesPerBlock, int numChannels)
{
    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = samplesPerBlock;
    spec.numChannels = numChannels;

    prepare(spec);
}

float HighPassFilter::getFrequency() {
    return frequency;
}
