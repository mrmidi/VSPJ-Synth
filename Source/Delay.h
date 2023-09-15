/*
  ==============================================================================

    Delay.h
    Created: 14 Sep 2023 12:56:35pm
    Author:  Alexander Shabelnikov

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>



class StereoDelay
{
public:
    StereoDelay();
    ~StereoDelay();

    void prepare(const juce::dsp::ProcessSpec& spec);
    void process(juce::AudioBuffer<float>& buffer);
    void reset();

    void setDelayTime(float delayTimeMs);
    void setFeedback(float feedbackAmount);
    void setWetLevel(float wetLevelAmount);

    void setDelayParams(float delayTimeMs, float feedbackAmount, float wetLevelAmount);

    void setPan(float newPanValue);

    void setStereoSpread(float newSpreadValue);

    

private:
    float delayTimeMs = 500.0f;  // Default to 500ms
    float feedback = 0.5f;      // Default to 50% feedback
    float wetLevel = 0.5f;      // Default to 50% wet level

    double currentSampleRate = 44100.0; // Default sample rate

    juce::dsp::DelayLine<float, juce::dsp::DelayLineInterpolationTypes::Linear> delayLineLeft;
    juce::dsp::DelayLine<float, juce::dsp::DelayLineInterpolationTypes::Linear> delayLineRight;

    float panValue = 0.0f;

    float stereoSpread = 0.3f;  // Default to full stereo

    
};
