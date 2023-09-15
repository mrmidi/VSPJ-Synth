/*
  ==============================================================================

    Filter.h
    Created: 9 Sep 2023 7:31:01pm
    Author:  Alexander Shabelnikov

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

class Filter : public juce::dsp::StateVariableTPTFilter<float>
{
public:
    static constexpr float MIN_CUTOFF = 20.0f;    // 20Hz, lowest frequency humans can hear
    static constexpr float MAX_CUTOFF = 20000.0f; // 20kHz, Nyquist frequency for 44.1kHz sample rate

    Filter();

    /** Prepare the filter for playback. */
    void prepareToPlay(double sampleRate, int samplesPerBlock, int numChannels);

    /** Set the filter's parameters. */
    void setParams(int filterType, float filterCutoff, float filterResonance);

    /** Process the given audio buffer through the filter. */
    void processNextBlock(juce::AudioBuffer<float>& buffer);

    /** Process a single sample through the filter. */
    float processNextSample(int channel, float inputValue);

    /** Reset the filter state. */
    void resetAll();

    /** Set the LFO parameters for the filter. */
    void setLfoParams(const float freq, const float depth);

    /** Setters and getters for various properties. */
    void setDepth(int depth);
    int getDepth() const;

    void setCurrentSampleRate(float sampleRate);
    
    void setBaseCutOffFreq(float baseCutoffFreq);
    float getBaseCutOffFreq() const;

        /** Set the cutoff frequency with checks for min and max values. */
    void setCutOffFreq(float filterCutoff);

    void getCurrentCutoffFreq(float& filterCutoff) const;


private:


    /** Select the filter type based on an integer value. */
    void selectFilterType(int filterType);

    juce::dsp::Oscillator<float> lfo;
    float currentSampleRate = 44100.0f; // Default to 44.1kHz
    float lfoGain = 0.0f;
    int depth = 0;
    float baseCutoffFreq = 1000.0f; // Default to 1kHz
};
