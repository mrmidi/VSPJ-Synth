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
    DBG("Prepare called for StereoDelay");
    int numChannels = spec.numChannels; 
    DBG("Current channel count: " << numChannels);
    // if (numChannels < 2) {
    //     DBG("I suppose i will crash now");
    // }
    currentSampleRate = spec.sampleRate; // Update this first

    DBG("Preparing delay with sample rate: " << spec.sampleRate);

    delayLineLeft.setMaximumDelayInSamples(currentSampleRate * 3.0f); // 3 second maximum delay
    delayLineRight.setMaximumDelayInSamples(currentSampleRate * 3.0f); // 3 second maximum delay
    
    delayLineLeft.prepare(spec);
    delayLineRight.prepare(spec);
    delayLineLeft.reset();
    delayLineRight.reset();

//    currentSampleRate = spec.sampleRate; moved to fix validation error

    setDelayTime(delayTimeMs);
    setPan(-0.3f);
}

void StereoDelay::setStereoSpread(float newSpreadValue)
{
    stereoSpread = juce::jlimit(0.0f, 1.0f, newSpreadValue); // Ensure the value stays between 0 and 1
}

void StereoDelay::process(juce::AudioBuffer<float>& buffer)
{
    const int numChannels = buffer.getNumChannels();
    const int numSamples = buffer.getNumSamples();

    for (int i = 0; i < numSamples; ++i)
    {
        float inSampleLeft = buffer.getSample(0, i);
        float inSampleRight = numChannels > 1 ? buffer.getSample(1, i) : inSampleLeft;

        float delaySampleLeft = delayLineLeft.popSample(0);
        float delaySampleRight = numChannels > 1 ? delayLineRight.popSample(0) : delaySampleLeft;

        // Calculate the spread values
        float mixedDelayLeft = (1.0f - stereoSpread) * 0.5f * (delaySampleLeft + delaySampleRight) + stereoSpread * delaySampleLeft;
        float mixedDelayRight = numChannels > 1 ? ((1.0f - stereoSpread) * 0.5f * (delaySampleLeft + delaySampleRight) + stereoSpread * delaySampleRight) : mixedDelayLeft;

        buffer.setSample(0, i, inSampleLeft + mixedDelayLeft * wetLevel);
        if (numChannels > 1) {
            buffer.setSample(1, i, inSampleRight + mixedDelayRight * wetLevel);
        }

        delayLineLeft.pushSample(0, inSampleLeft + delaySampleLeft * feedback);
        if (numChannels > 1) {
            delayLineRight.pushSample(0, inSampleRight + delaySampleRight * feedback);
        } else {
            // For mono processing, feedback the mixed delay back into the left delay line
            delayLineLeft.pushSample(0, mixedDelayLeft * feedback);
        }
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


