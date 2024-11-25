#pragma once

#include <JuceHeader.h>

class LowShelfFilter
{

  public:
  void setGainDB(float gain) {
    if (gain == prevGainDB) {
      return;
    }

    prevGainDB = gain;

    auto coeffs = juce::dsp::IIR::Coefficients<float>::makeLowShelf(
        sampleRate,
        200,
        0.707f, // Quality factor (Q). 0.707 is typical for a Butterworth filter
        juce::Decibels::decibelsToGain(gain)
    );

    *filter.state = *coeffs;
    DBG("Setting low shelf gain to: " << juce::Decibels::decibelsToGain(gain));
  }

  float getGainDB() const {
    return prevGainDB;
  }

  // Helper function to compare floating-point numbers
  bool approximatelyEqual(float a, float b, float epsilon = 0.001f) const
  {
      return std::abs(a - b) <= epsilon;
  }  
  float sampleRate = 44100.0f;
  float gainDB = 0.0f;
  juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>, juce::dsp::IIR::Coefficients<float>> filter;
  private:
  float prevGainDB = 0.0f;
};