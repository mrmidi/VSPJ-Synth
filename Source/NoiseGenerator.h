/*
  ==============================================================================

    NoiseGenerator.h
    Created: 8 Feb 2024 5:16:29pm
    Author:  Alexander Shabelnikov

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

class NoiseGenerator
{
public:
    NoiseGenerator() {}

    void prepare(const juce::dsp::ProcessSpec& spec)
    {
        filt.prepare(spec); // Prepare the filter with the audio specifications
    }

    juce::String getNoiseType() const
    {
        switch (noiseType)
        {
            case NoiseType::White:
                return "White";
            case NoiseType::Pink:
                return "Pink";
            case NoiseType::Brown:
                return "Brown";
            case NoiseType::Blue:
                return "Blue";
            default:
                return "Unknown";
        }
    }

    void setType(const juce::String& type)
    {
        if (type == "White")
        {
            noiseType = NoiseType::White;
        }
        else if (type == "Pink")
        {
            noiseType = NoiseType::Pink;
        }
        else if (type == "Brown")
        {
            noiseType = NoiseType::Brown;
        }
        else if (type == "Blue")
        {
            noiseType = NoiseType::Blue;
        }

        changeFilterParams(); // Update filter parameters when the noise type changes
    }

    float getNextSample()
    {
        float noiseSample = random.nextFloat() * 2.0f - 1.0f; // Generate white noise

        if (noiseType == NoiseType::White)
        {
            return noiseSample; // No filtering needed for white noise
        }
        else
        {
            juce::dsp::AudioBlock<float> block(&noiseSample, 1, 1);
            juce::dsp::ProcessContextReplacing<float> context(block);
            filt.process(context); // Filter the noise sample
            return noiseSample;
        }
    }

private:
    enum class NoiseType
    {
        White,
        Pink,
        Brown,
        Blue
    };

    juce::dsp::StateVariableFilter::Filter<float> filt;
    juce::Random random; // For generating white noise
    NoiseType noiseType = NoiseType::White; // Default to white noise

    void changeFilterParams()
    {
        // Example settings for different noise colors
        // These settings are illustrative; you'll need to experiment to find what works best for your application
        switch (noiseType)
        {
            case NoiseType::Pink:
                filt.state->type = juce::dsp::StateVariableFilter::Parameters<float>::Type::lowPass;
                filt.state->setCutOffFrequency(44100, 500, 0.707); // 500 Hz cut-off with 0.707 Q-factor as an example
                break;
            case NoiseType::Brown:
                filt.state->type = juce::dsp::StateVariableFilter::Parameters<float>::Type::lowPass;
                filt.state->setCutOffFrequency(44100, 200, 0.707); // Lower cut-off for Brown noise
                break;
            case NoiseType::Blue:
                filt.state->type = juce::dsp::StateVariableFilter::Parameters<float>::Type::highPass;
                filt.state->setCutOffFrequency(44100, 1000, 0.707); // Higher cut-off for Blue noise
                break;
            case NoiseType::White:
            default:
                // No filtering needed for white noise, but you might set up a bypass state or reset the filter
                break;
        }
    }
};
