/*
  ==============================================================================

    Specs.h
    Created: 17 Jun 2023 12:37:20pm
    Author:  Aleksandr Shabelnikov

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>


class Specs
{
  public:
  Specs() 
  {
    specs.sampleRate = 44100.0f;
    specs.maximumBlockSize = 512;
    specs.numChannels = 2;
  };

  void setSampleRate(float sampleRate) { specs.sampleRate = sampleRate; }
  void setBlockSize(int blockSize) { specs.maximumBlockSize = blockSize; }
  void setNumChannels(int numChannels) { specs.numChannels = numChannels; }
  void setAll(float sampleRate, int blockSize, int numChannels) 
  {
    specs.sampleRate = sampleRate;
    specs.maximumBlockSize = blockSize;
    specs.numChannels = numChannels;
  }

  juce::dsp::ProcessSpec getSpecs() { return specs; }
  private:
  juce::dsp::ProcessSpec specs;
};
