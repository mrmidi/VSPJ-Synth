/*
  ==============================================================================

    Delay.cpp
    Created: 14 Sep 2023 12:56:35pm
    Author:  Alexander Shabelnikov

  ==============================================================================
*/

#include "Delay.h"

StereoDelay::StereoDelay()
{
    // Constructor
}

StereoDelay::~StereoDelay()
{
    // Destructor
}

void StereoDelay::prepare(const juce::dsp::ProcessSpec& spec)
{
    DBG("Preparing delay with sample rate: " << spec.sampleRate);
    delayLineLeft.setMaximumDelayInSamples(currentSampleRate * 2.0f); // 2 second maximum delay
    delayLineRight.setMaximumDelayInSamples(currentSampleRate * 2.0f); // 2 second maximum delay
    
    delayLineLeft.prepare(spec);
    delayLineRight.prepare(spec);
    delayLineLeft.reset();
    delayLineRight.reset();

    currentSampleRate = spec.sampleRate;

    setDelayTime(delayTimeMs);
    setPan(-0.3f);
}

void StereoDelay::setStereoSpread(float newSpreadValue)
{
    stereoSpread = juce::jlimit(0.0f, 1.0f, newSpreadValue); // Ensure the value stays between 0 and 1
}

void StereoDelay::process(juce::AudioBuffer<float>& buffer)
{
    const int numSamples = buffer.getNumSamples();

    for (int i = 0; i < numSamples; ++i)
    {
        float inSampleLeft = buffer.getSample(0, i);
        float inSampleRight = buffer.getSample(1, i);

        float delaySampleLeft = delayLineLeft.popSample(0);
        float delaySampleRight = delayLineRight.popSample(0);

        // Calculate the spread values
        float mixedDelayLeft = (1.0f - stereoSpread) * 0.5f * (delaySampleLeft + delaySampleRight) + stereoSpread * delaySampleLeft;
        float mixedDelayRight = (1.0f - stereoSpread) * 0.5f * (delaySampleLeft + delaySampleRight) + stereoSpread * delaySampleRight;

        buffer.setSample(0, i, inSampleLeft + mixedDelayLeft * wetLevel);
        buffer.setSample(1, i, inSampleRight + mixedDelayRight * wetLevel);

        delayLineLeft.pushSample(0, inSampleLeft + delaySampleLeft * feedback);
        delayLineRight.pushSample(0, inSampleRight + delaySampleRight * feedback);
    }
}

void StereoDelay::setPan(float newPanValue)
{
    panValue = juce::jlimit(-1.0f, 1.0f, newPanValue); // Ensure the pan value stays between -1 and 1
}

void StereoDelay::reset()
{
    delayLineLeft.reset();
    delayLineRight.reset();
}

void StereoDelay::setDelayTime(float newDelayTimeMs)
{
    delayTimeMs = newDelayTimeMs;
    float delaySamples = newDelayTimeMs * 0.001f * currentSampleRate;
    //DBG("New delay time in samples: " << delaySamples);
    int leftSamples = (int)delaySamples * 0.7;  // factor of 0.5f for stereo
    int rightSamples = (int)delaySamples * 1.3; // factor of 0.5f for stereo
    delayLineLeft.setDelay(leftSamples);
    delayLineRight.setDelay(rightSamples);
}

void StereoDelay::setFeedback(float newFeedback)
{
    feedback = newFeedback;
}

void StereoDelay::setWetLevel(float newWetLevel)
{
    wetLevel = newWetLevel;
}

void StereoDelay::setDelayParams(float newDelayTimeMs, float newFeedback, float newWetLevel)
{
    setDelayTime(newDelayTimeMs);
    setFeedback(newFeedback);
    setWetLevel(newWetLevel);
}


