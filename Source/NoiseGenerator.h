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
  NoiseGenerator()
  {
    // set the default sample rate
    sampleRate = 44100.0f;
    // change noise to pink
    setType(1); // 1 is pink noise
    calcCoeffs();
  }

  void prepare(const juce::dsp::ProcessSpec &spec)
  {
    // check if the sample rate has changed
    if (spec.sampleRate == sampleRate)
      return;
    DBG("NoiseGenerator::prepare() - sample rate changed to " << spec.sampleRate);
    sampleRate = spec.sampleRate; // Store the sample rate
    sampleRateChanged = true;     // Set the flag to indicate the sample rate has changed
    calcCoeffs();                 // Recalculate coefficients
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

  // set type with int
  void setType(int type)
  {
    // check if type has changed and return if not
    if (type == noiseType)
      return;
      
    switch (type)
    {
    case 0:
      noiseType = NoiseType::White;
      DBG("NoiseGenerator::setType() - type: " << type << " - White");
      break;
    case 1:
      noiseType = NoiseType::Pink;
      DBG("NoiseGenerator::setType() - type: " << type << " - White");
      break;
    case 2:
      noiseType = NoiseType::Brown;
      break;
    case 3:
      noiseType = NoiseType::Blue;
      break;
    default:
      noiseType = NoiseType::White;
      break;
    }
    // recalculate coeffs if needed
    calcCoeffs();
  }

  // void setType(const juce::String &type)
  // {
  //   if (type == "White")
  //   {
  //     noiseType = NoiseType::White;
  //   }
  //   else if (type == "Pink")
  //   {
  //     noiseType = NoiseType::Pink;
  //   }
  //   else if (type == "Brown")
  //   {
  //     noiseType = NoiseType::Brown;
  //   }
  //   else if (type == "Blue")
  //   {
  //     noiseType = NoiseType::Blue;
  //   }
  //   // recalculate coeffs if needed
  //   calcCoeffs();
  // }

  void calcCoeffs()
  {
    if (!sampleRateChanged)
      return; // If the sample rate has not changed, return

    DBG("NoiseGenerator::calcCoeffs() - sample rate: " << sampleRate);

    // Frequency values for different sample rate ranges
    float f[7]; // Array to hold frequency values

    // Choose frequency values based on the sample rate
    if (sampleRate <= 48100.0f)
    {
      f[0] = 4752.456f;
      f[1] = 4030.961f;
      f[2] = 2784.711f;
      f[3] = 1538.461f;
      f[4] = 357.681f;
      f[5] = 70.0f;
      f[6] = 30.0f;
    }
    else if (sampleRate <= 96000.0f)
    {
      f[0] = 8227.219f;
      f[1] = 8227.219f;
      f[2] = 6388.570f;
      f[3] = 3302.754f;
      f[4] = 479.412f;
      f[5] = 151.070f;
      f[6] = 54.264f;
    }
    else if (sampleRate < 192000.0f)
    {
      f[0] = 9211.912f;
      f[1] = 8621.096f;
      f[2] = 8555.228f;
      f[3] = 8292.754f;
      f[4] = 518.334f;
      f[5] = 163.712f;
      f[6] = 240.241f;
    }
    else // if (sampleRate >= 192000.0f)
    {
      f[0] = 10000.0f;
      f[1] = 10000.0f;
      f[2] = 10000.0f;
      f[3] = 10000.0f;
      f[4] = 544.948f;
      f[5] = 142.088f;
      f[6] = 211.616f;
    }

    // Calculate coefficients
    for (int i = 0; i < 7; ++i)
    {
      k[i] = exp(-2 * M_PI * f[i] / sampleRate); // k[] is an array member variable to store coefficients
    }
    sampleRateChanged = false; // Reset the flag to indicate the sample rate has not changed
  }

  float getNextSample()
  {
    float noiseSample = random.nextFloat() * 2.0f - 1.0f; // Generate white noise
    switch (noiseType)
    {
    case NoiseType::White:
      return noiseSample;
    case NoiseType::Pink:
      return generatePinkNoise(noiseSample);
    default:
      return 0.0f;
    }
  }

float generatePinkNoise(float noiseSample)
{
    // Apply each filter to the white noise sample using the coefficients
    // SIMD instructions could be used here to GREATLY speed up the process
    b[0] = k[0] * (noiseSample + b[0]);
    b[1] = k[1] * (noiseSample + b[1]);
    b[2] = k[2] * (noiseSample + b[2]);
    b[3] = k[3] * (noiseSample + b[3]);
    b[4] = k[4] * (noiseSample + b[4]);
    b[5] = k[5] * (noiseSample + b[5]);
    b[6] = k[6] * (noiseSample - b[6]);

    // Sum the outputs of all filters, including the current white noise sample
    // to generate the pink noise sample
    float pinkNoise = b[0] + b[1] + b[2] + b[3] + b[4] + b[5] + b[6] + noiseSample;

    float compensation = 0.11f; // Compensation factor to normalize the pink noise

    return pinkNoise * compensation;
}


private:

  // this macro is used to prevent copying of the object
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(NoiseGenerator)

  enum NoiseType
  {
    White,
    Pink,
    Brown, // N O T
    Blue   // I M P L E M E N T E D
           // because it's INSANE!!! (integrating white noise)
           // or maybe later
  };

  juce::Random random;                    // For generating white noise
  NoiseType noiseType = NoiseType::White; // Default to white noise

  float sampleRate = 44100.0f; // Default sample rate
  float k[7];                  // Coefficients for pink noise
  float b[7] = {0.0f};         // Initialize all elements to 0.0f
  bool sampleRateChanged = true; // Flag to indicate if the sample rate has changed
};
