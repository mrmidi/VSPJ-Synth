/*
  ==============================================================================

    LFOsc.cpp
    Created: 8 Sep 2023 10:44:55pm
    Author:  Alexander Shabelnikov

  ==============================================================================
*/

#include "LFOsc.h"

LFOsc::LFOsc()
{
    // Initialize with a sine waveform
    oscillator.initialise([](float x) { return std::sin(x); });
}

void LFOsc::setFrequency(float freq)
{
    if (frequency == freq) {
        return;
    }
    frequency = freq;
    oscillator.setFrequency(frequency);
    DBG("SETTING LFO FREQUENCY TO " << frequency);
}

void LFOsc::setDepth(float newDepth)
{
    if (depth != newDepth) {
        depth = newDepth;
        DBG("SETTING LFO DEPTH TO " << depth);
    }
    
}

float LFOsc::getNextSample()
{
    return oscillator.processSample(1.0f) * depth;
}



void LFOsc::prepareToPlay (double sampleRate, int samplesPerBlock, int outputChannels)
{
    juce::dsp::ProcessSpec spec;
    spec.maximumBlockSize = samplesPerBlock;
    spec.sampleRate = sampleRate;
    spec.numChannels = outputChannels;

    oscillator.prepare(spec);

}


void LFOsc::setType(lfoType type)
{
    currentType = type;
}

LFOsc::lfoType LFOsc::getType() const
{
    return currentType;
}

float LFOsc::getDepth() const
{
    return depth;
}