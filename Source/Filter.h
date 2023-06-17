/*
  ==============================================================================

    Filter.h
    Created: 17 Jun 2023 10:31:31pm
    Author:  Aleksandr Shabelnikov

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

class Filter : public juce::dsp::StateVariableTPTFilter<float>
{
public:
    Filter() 
    {
        // set default values
        setParams(0, 1000.0f, 0.1f);
        // set default sample rate
        setCurrentSampleRate(44100.0f);
        // set default depth
        setDepth(0);
    };
    void setParams (const int filterType, const float filterCutoff, const float filterResonance);
    void setDepth (int depth) { this->depth = depth; }
    void setLfoParams (const float freq, const float depth);
    void prepareToPlay (double sampleRate, int samplesPerBlock, int outputChannels);
    void selectFilterType (const int filterType);
    void processNextBlock(juce::AudioBuffer<float>& buffer);
    float processNextSample (int channel, float inputValue);
    void setCurrentSampleRate(float sampleRate) { currentSampleRate = sampleRate; }
    void setCutOffFreq (float filterCutoff);
    void resetAll();
    int getDepth() { return depth; }
    void setBaseCutOffFreq(float baseCutoffFreq) { this->baseCutoffFreq = baseCutoffFreq; }
    float getBaseCutOffFreq() { return baseCutoffFreq; }
    
private:
    juce::dsp::Oscillator<float> lfo;
    float currentSampleRate;
    float lfoGain;
    int depth;
    float baseCutoffFreq;
};
