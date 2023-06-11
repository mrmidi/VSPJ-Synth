/*
  ==============================================================================

    Filter.h
    Created: 11 Jun 2023 4:45:01pm
    Author:  Aleksandr Shabelnikov

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

class Filter {
public:
    Filter()
    : lowPassFilter(juce::dsp::IIR::Coefficients<float>::makeLowPass(44100, 20000.0f, 0.1)) {}

    void prepare(const juce::dsp::ProcessSpec& spec) {
        lowPassFilter.prepare(spec);
        lowPassFilter.reset();
    }

    void process(juce::dsp::ProcessContextReplacing<float>& context) {
        lowPassFilter.process(context);
    }

    void updateCoefficients(double sampleRate, float freq, float res) {
        *lowPassFilter.state = *juce::dsp::IIR::Coefficients<float>::makeLowPass(sampleRate, freq, res);
    }

private:
    juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>, juce::dsp::IIR::Coefficients<float>> lowPassFilter;
};




