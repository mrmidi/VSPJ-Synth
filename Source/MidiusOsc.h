// THIS IS NOT USED!!!!

/*
  ==============================================================================

    OSC.h
    Created: 3 Nov 2023 12:49:39pm
    Author:  Alexander Shabelnikov

  ==============================================================================
*/

#pragma once

#define TWOPI (2 * M_PI)

#include <JuceHeader.h>
#include "MoogLadderFilter.h"

// Set DEBUG to 1 to enable custom debug messages
#define MIDIUSDEBUG 0

// Define CUSTOMDBG based on whether DEBUG is enabled
#if MIDIUSDEBUG
#define CUSTOMDBG(...) \
  {                    \
    DBG(__VA_ARGS__);  \
  }
#else
#define CUSTOMDBG(...) \
  {                    \
  }
#endif

class MyOscillator
/**
 * @class MyOscillator
 * @brief A class representing an oscillator for audio synthesis.
 *
 * This class provides various methods to set parameters such as sample rate, base frequency,
 * amplitude, and velocity. It also includes methods to render audio samples and generate
 * different types of waveforms including sine, sawtooth, and pulse waves.
 */
{
public:
  MyOscillator();  // Constructor declaration


  /**
   * @brief Sets the sample rate and updates the phase increment.
   *
   * This function sets the sample rate to the new value provided and then calls
   * the updatePhaseIncrement() function to update the phase increment based on
   * the new sample rate. Also updates other required variables.
   *
   * @param newSampleRate The new sample rate to be set.
   */
  void setSampleRate(float newSampleRate);

  // this method is called on noteOn event
  // to set the base frequency of the oscillator
  /**
   * @brief Sets the base frequency for the oscillator.
   * 
   * @param newFrequency The new base frequency to be set, in Hertz (Hz).
   */
  void setBaseFrequency(float newFrequency);

  /**
   * @brief Sets the octave for the oscillator.
   * 
   * This function adjusts the oscillator to the specified octave.
   * 
   * @param newOctave The new octave value to set.
   */
  void setOctave(int newOctave);

  // this method is used to update frequency
  // based on detune, pitch bend and octave
  void setFrequency();

  void setAmplitude(float newAmplitude);

  void setAmplitudeDB(float newAmplitudeDB);

  void setVelocity(float newVelocity);

  /**
   * @brief Renders the next block of samples for the oscillator.
   *
   * This function calculates the next block of samples for the oscillator based on the
   * current waveform type and amplitude. The calculated samples are then added to the
   * output buffer.
   *
   * @param outputBuffer The buffer to which the calculated samples are added.
   * @param startSample The index of the first sample to be calculated.
   * @param numSamples The number of samples to be calculated.
   */
  void renderNextBlock(juce::AudioBuffer<float> &outputBuffer, int startSample, int numSamples);



private:
  /**
   * @brief Calculates and returns the value of a sine wave.
   *
   * This function calculates the value of a sine wave at the current phase,
   * then increments the phase by the phase increment and wraps the phase
   * to the range [0, TWOPI).
   *
   * @return The value of the sine wave at the current phase.
   */
  float getSineTick();

  float getSaw2PtrTick();

  /**
   * @brief Calculates the derivative of a given input.
   *
   * This function calculates the derivative of a given input using the formula:
   * (x - x') / (2.0f / p0n), where x is the current input, x' is the previous input,
   * and p0n is the number of samples per cycle of the sawtooth wave.
   *
   * @param input The input for which to calculate the derivative.
   * @param history The history array for the input. The previous input is stored in history[1].
   * @return The calculated derivative.
   */
  float diff(float input, float *history);

  /**
   * @brief Calculates and returns the normalized value of a sawtooth wave.
   *
   * This function calculates the value of a sawtooth wave at a given point in time,
   * then differentiates it three times, and finally normalizes it by dividing by the
   * scaling factor. The sawtooth wave is defined by the equation x^4 - 2x^2.
   *
   * @return The normalized value of the sawtooth wave at the current point in time.
   */
  float getSaw4Tick();

  /**
   * @brief Calculates and returns the normalized value of a cubic sawtooth wave.
   *
   * This function calculates the value of a cubic sawtooth wave at a given point in time,
   * then differentiates it twice, and finally normalizes it by dividing by the
   * scaling factor. The cubic sawtooth wave is defined by the equation x^3 - x.
   *
   * @return The normalized value of the cubic sawtooth wave at the current point in time.
   */
  float getSaw3Tick();

  /**
   * @brief Calculates and returns the normalized value of a quadratic sawtooth wave.
   *
   * This function calculates the value of a quadratic sawtooth wave at a given point in time,
   * then differentiates it once, and finally normalizes it by dividing by the
   * scaling factor. The quadratic sawtooth wave is defined by the equation x^2.
   *
   * @return The normalized value of the quadratic sawtooth wave at the current point in time.
   */
  float getSaw2Tick();

  /**
   * @brief Calculates and returns the value of a linear sawtooth wave.
   *
   * This function calculates the value of a linear sawtooth wave at a given point in time,
   * then normalizes it to the range [-1, 1]. The phase is then incremented and wrapped
   * to the range [0, TWOPI).
   *
   * @return The value of the linear sawtooth wave at the current point in time.
   */
  float lf_sawpos();

    // Function to update and return the current value of the modulo counter
    float phi();

    // Modulo counter method
    float getSawModTick() {
        // transfer to bipolar singal from unipolar modulo counter;
        return 2.0f * phi() - 1.0f;
    }

  /**
   * @brief Updates the phase increment and scaling factors.
   *
   * This function updates the phase increment and scaling factors based on the current
   * frequency and sample rate. It also calculates the sampling period and the number
   * of samples per cycle (p0n). The scaling factors are calculated for orders 2, 3, and 4.
   */
  void updatePhaseIncrement();

  /**
   * @brief Calculates the scaling factor for a given order.
   *
   * This function calculates the scaling factor for a given order using the formula:
   * pi^(n-1) / N! * [2 * sin(omega0 * T / 2)], where T is the sampling interval (1 / sample rate),
   * omega0 is 2 * pi * f0 (fundamental frequency), and N is the order.
   *
   * @param order The order for which to calculate the scaling factor.
   * @return The calculated scaling factor.
   */
  float calculateScalingFactor(int order);

  long double calculateImprovedScalingFactor(int order);

  /**
   * @brief Calculates the factorial of a small integer.
   *
   * This function calculates the factorial of an integer between 2 and 4.
   * The factorials are precalculated for efficiency. If the input is not
   * in this range, the function asserts false and returns 0.0f.
   *
   * @param n The integer of which to calculate the factorial. Must be between 2 and 4.
   * @return The factorial of the input number as a float. If the input is not between 2 and 4, returns 0.0f.
   */
  float getFactorial(int n);

  /**
   * @brief Wraps the phase to the range [0, TWOPI).
   *
   * This function checks if the phase is outside the range [0, TWOPI) and
   * wraps it back into that range by adding or subtracting TWOPI as necessary.
   */
  void wrapPhase();

  // Pulse train methods (differential delay)
  int intPart(float value)
  {
    return static_cast<int>(value); // cast value to int
  }

  float fracPart(float value)
  {
    return value - static_cast<float>(intPart(value)); // substract integer part from value
  }

  // Function to perform the differential delay
  float diffDel(float x, float del) {
        int intDel = intPart(del); // integer part of delay
        float fracDel = fracPart(del); // fractional part of delay

        // get delayed values from delay buffer where
        // delayedX1 is the delayed value at intDel
        // delayedX2 is the delayed value at intDel + 1
        // the delay buffer is a circular buffer
        // the write index is the current index
        // the read index is the current index - intDel
        // interpolate between the two delayed values
        float delayedX1 = delayBuffer[(writeIndex - intDel + delayBufferSize) % delayBufferSize];
        float delayedX2 = delayBuffer[(writeIndex - intDel - 1 + delayBufferSize) % delayBufferSize];
        return x - delayedX1 * (1.0f - fracDel) - delayedX2 * fracDel;
  }

  float getPulseTick() {
        // Calculate the period and desired delay based on frequency and duty cycle
        // float period = sampleRate / frequency;
        // float ddel = dutyCycle * period;

        // del is delay size
        float del = std::fmax(0.0f, std::fmin(static_cast<float>(delayBufferSize - 1), ddel)); // clamp delay to buffer size

        // Generate the next sample of the bandlimited sawtooth wave
        // based on PTR method
        float sawSample = getSaw2PtrTick();

        // Store the current sawtooth sample in the delay buffer
        delayBuffer[writeIndex] = sawSample;

        // Increment the write index and wrap if necessary
        writeIndex = (writeIndex + 1) % delayBufferSize;

        // Apply differential delay to the sawtooth wave to generate the pulse train
        return diffDel(sawSample, del);
    }

    // VARIABLES DECLARATION
    private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MyOscillator)

                    enum WaveForm {
                      SINE,
                      SQUARE,
                      SAW,
                      SAWBL2,
                      SAWBL3,
                      SAWBL4,
                      SAWPTR2,
                      TRIANGLE
                    };



  float amplitude; // amplitude of the oscillator

  float velocity; // velocity of the oscillator

  float baseFrequency;      // base frequency of the oscillator
  int octave;               // octave of the oscillator
  int detune;             // detune of the oscillator

  float frequency;         // frequency in Hz
  float sampleRate;        // sample rate in Hz
  float phase;             // current phase of the oscillator
  float phaseIncrement;    // phase increment per sample
  float p0n;               // period of the sawtooth wave in samples
  float lastSample = 0.0f; // last sample of the oscillator
  float samplingPeriod;    // sampling period (1 / sample rate)
  float scalingFactor2;    // scaling factor for 2nd order sawtooth wave
  float scalingFactor3;    // scaling factor for 3rd order sawtooth wave
  float scalingFactor4;    // scaling factor for 4th order sawtooth wave

  long double improvedScalingFactor2; // improved scaling factor for 2nd order sawtooth wave
  long double improvedScalingFactor3; // improved scaling factor for 3rd order sawtooth wave
  long double improvedScalingFactor4; // improved scaling factor for 4th order sawtooth wave
  WaveForm waveForm;                  // waveform of the oscillator

  // History arrays
  float sawHistory[4] = {0.0f};   // History for sawtooth values
  float diff1History[2] = {0.0f}; // History for first derivative
  float diff2History[2] = {0.0f}; // History for second derivative
  float diff3History[2] = {0.0f}; // History for third derivative

  // Pulse train variables
  float ddel = 0.0f;                    // differential delay
  float dutyCycle = 0.5f;               // duty cycle of the pulse train
  int delayBufferSize = 2048;            // size of the delay buffer
  std::vector<float> delayBuffer;       // delay buffer
  int writeIndex = 0;                   // write index for the delay buffer


};

