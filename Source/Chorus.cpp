/*
  ==============================================================================

    Chorus.cpp
    Created: 6 Jun 2024 8:01:29pm
    Author:  Alexander Shabelnikov

  ==============================================================================
*/

#include "Chorus.h"

Chorus::Chorus()
{
    // Constructor
}

Chorus::~Chorus()
{
    // Destructor
}

void Chorus::prepare(const juce::dsp::ProcessSpec& spec)
{
    // Prepare the chorus effect with the given process specification
    juce::dsp::Chorus<float>::prepare(spec);
    currentSampleRate = spec.sampleRate;
}

void Chorus::process(juce::AudioBuffer<float>& buffer)
{
    // Process the audio buffer through the chorus effect
    juce::dsp::AudioBlock<float> block(buffer);
    juce::dsp::ProcessContextReplacing<float> context(block);
    juce::dsp::Chorus<float>::process(context);
}

void Chorus::reset()
{
    // Reset the chorus effect state
    juce::dsp::Chorus<float>::reset();
}

void Chorus::setRate(float newRate)
{
    rate = newRate;
    juce::dsp::Chorus<float>::setRate(rate);
}

void Chorus::setDepth(float newDepth)
{
    depth = newDepth;
    juce::dsp::Chorus<float>::setDepth(depth);
}

void Chorus::setCentreDelay(float newCentreDelay)
{
    centreDelay = newCentreDelay;
    juce::dsp::Chorus<float>::setCentreDelay(centreDelay);
}

void Chorus::setFeedback(float newFeedback)
{
    feedback = newFeedback;
    juce::dsp::Chorus<float>::setFeedback(feedback);
}

void Chorus::setMix(float newMix)
{
    mix = newMix;
    juce::dsp::Chorus<float>::setMix(mix);
}

void Chorus::setChorusParams(float newRate, float newDepth, float newCentreDelay, float newFeedback, float newMix)
{
    setRate(newRate);
    setDepth(newDepth);
    setCentreDelay(newCentreDelay);
    setFeedback(newFeedback);
    setMix(newMix);
}
