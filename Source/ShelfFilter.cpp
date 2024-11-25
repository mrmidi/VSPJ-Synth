/*
  ==============================================================================

    ShelfFilter.cpp
    Created: 24 Nov 2024 5:24:10pm
    Author:  Alexander Shabelnikov

  ==============================================================================
*/

#include "ShelfFilter.h"

// void ShelfFilter::prepare(const juce::dsp::ProcessSpec& spec)
// {
//     this->spec = spec;
//     filter.prepare(spec);

//     // Initialize the filter with default frequency and gain
//     setCutoffFrequency(frequency);
//     setGain(gainDB);
// }

// void ShelfFilter::setCutoffFrequency(double frequency)
// {
//     if (this->frequency == frequency)
//         return; // No need to update if the frequency hasn't changed

//     this->frequency = static_cast<float>(frequency);

//     // Recalculate the filter coefficients with the current gain
//     auto coefficients = juce::dsp::IIR::Coefficients<float>::makeLowShelf(
//         spec.sampleRate,
//         this->frequency,
//         0.707f, // Quality factor (Q). 0.707 is typical for a Butterworth filter
//         juce::Decibels::decibelsToGain(gainDB)
//     );

//     // Apply the new coefficients to the filter
//     filter.coefficients = coefficients;
// }

// void ShelfFilter::setGain(double gainDB)
// {
//     if (this->gainDB == gainDB)
//         return; // No need to update if the gain hasn't changed

//     this->gainDB = static_cast<float>(gainDB);

//     // Recalculate the filter coefficients with the current frequency
//     auto coefficients = juce::dsp::IIR::Coefficients<float>::makeLowShelf(
//         spec.sampleRate,
//         frequency,
//         0.707f, // Quality factor (Q). 0.707 is typical for a Butterworth filter
//         juce::Decibels::decibelsToGain(this->gainDB)
//     );

//     // Apply the new coefficients to the filter
//     filter.coefficients = coefficients;
// }

// float ShelfFilter::process(float inputSample)
// {
//     return filter.processSample(inputSample);
// }

// void ShelfFilter::prepareToPlay(double sampleRate, int samplesPerBlock, int numChannels)
// {
//     juce::dsp::ProcessSpec spec;
//     spec.sampleRate = sampleRate;
//     spec.maximumBlockSize = samplesPerBlock;
//     spec.numChannels = numChannels;

//     prepare(spec);
// }

// float ShelfFilter::getFrequency() const
// {
//     return frequency;
// }

// float ShelfFilter::getGain() const
// {
//     return gainDB;
// }
