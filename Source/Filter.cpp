/*
  ==============================================================================

    Filter.cpp
    Created: 9 Sep 2023 7:31:01pm
    Author:  Alexander Shabelnikov

  ==============================================================================
*/

#include "Filter.h"

Filter::Filter() {}; // empty constructor

void Filter::prepareToPlay(double sampleRate, int samplesPerBlock, int numChannels)
{
    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = samplesPerBlock;
    spec.numChannels = numChannels;

    for (int i = 0; i < numChannels; ++i)
    {
        lfo.prepare(spec);
    }
}

void Filter::setBaseCutOffFreq(float baseCutoffFreq) {
    this->baseCutoffFreq = baseCutoffFreq;
}

void Filter::setParams(int filterType, float filterCutoff, float filterResonance)
{
    selectFilterType(filterType);
    setCutoffFrequency(filterCutoff);
    setResonance(filterResonance);

    setBaseCutOffFreq(filterCutoff);

}

void Filter::setCutOffFreq(float filterCutoff)
{
    if (filterCutoff > 20000.0f) // 20kHz, Nyquist frequency for 44.1kHz sample rate
        filterCutoff = 20000.0f;
    else if (filterCutoff < 20.0f) // 20Hz, lowest frequency humans can hear
        filterCutoff = 20.0f;
    setCutoffFrequency(filterCutoff);
}

void Filter::selectFilterType (const int filterType)
{
    switch (filterType)
    {
        case 0:
            setType (juce::dsp::StateVariableTPTFilterType::lowpass);
            break;
            
        case 1:
            setType (juce::dsp::StateVariableTPTFilterType::bandpass);
            break;
            
        case 2:
            setType (juce::dsp::StateVariableTPTFilterType::highpass);
            break;
            
        default:
            setType (juce::dsp::StateVariableTPTFilterType::lowpass);
            break;
    }
}

float Filter::getBaseCutOffFreq() const {
    return baseCutoffFreq;
}

int Filter::getDepth() const {
    return depth;
}

void Filter::processNextBlock(juce::AudioBuffer<float>& buffer)
{
    juce::dsp::AudioBlock<float> block { buffer };
    process (juce::dsp::ProcessContextReplacing<float>(block));
}

float Filter::processNextSample (int channel, float inputValue)
{
    return processSample (channel, inputValue);
}

void Filter::resetAll()
{
    reset();
    lfo.reset(); // todo??
}
