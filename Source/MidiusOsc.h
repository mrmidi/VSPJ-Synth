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
{
public:
  MyOscillator()
  {
    phase = 0;
    sampleRate = 44100;
    frequency = 220;
    waveForm = SAWPTR2;
    // waveForm = SINE;
      // waveForm = SQUAE;
    amplitude = 1.0f;

    // pulse train variables
    delayBuffer.resize(delayBufferSize, 0.0f);
    writeIndex = 0;

    updatePhaseIncrement();
  }

  /**
   * @brief Sets the sample rate and updates the phase increment.
   *
   * This function sets the sample rate to the new value provided and then calls
   * the updatePhaseIncrement() function to update the phase increment based on
   * the new sample rate. Also updates other required variables.
   *
   * @param newSampleRate The new sample rate to be set.
   */
  void setSampleRate(float newSampleRate)
  {
    filter.prepareToPlay(newSampleRate);
    sampleRate = newSampleRate;
    updatePhaseIncrement();
  }

  void setFrequency(float newFrequency)
  {
    // clamp frequency from 20hz
    frequency = std::max<float>(20.0f, std::fabs(newFrequency));
    CUSTOMDBG("New frequency: " << frequency);
    updatePhaseIncrement();
  }

  void setAmplitude(float newAmplitude)
  {
    amplitude = newAmplitude; // linear amplitude
  }

  void setAmplitudeDB(float newAmplitudeDB)
  {
    amplitude = juce::Decibels::decibelsToGain(newAmplitudeDB); // dB amplitude
  }

  /**
   * @brief Sets the waveform type for the oscillator.
   *
   * This function sets the waveform type for the oscillator to the new value provided.
   *
   * @param newWaveForm The new waveform type to be set.
   */
  // void setWaveForm(WaveForm newWaveForm) {
  //   waveForm = newWaveForm;
  // }

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
  void renderNextBlock(juce::AudioBuffer<float> &outputBuffer, int startSample, int numSamples)
  {
    for (int sample = 0; sample < numSamples; ++sample)
    {
      float value = 0;
      float amplitude = 0.5f;
      if (waveForm == SINE)
      {
        value = getSineTick() * amplitude; // amplitude * sin(phase)
      }
      else if (waveForm == SAW)
      {
        value = lf_sawpos() * amplitude; // aplitude * (2 * (phase / TWOPI) - 1.0f)
        CUSTOMDBG("Sawtooth value: " << value);
      }
      else if (waveForm == SAWBL2)
      {
        value = getSaw2Tick() * amplitude;
      }
      else if (waveForm == SAWBL4)
      {
        value = getSaw4Tick() * amplitude;
      }
      else if (waveForm == SAWBL3)
      {
        value = getSaw3Tick() * amplitude;
      }
      else if (waveForm == SAWPTR2)
      {
        value = getSaw2PtrTick() * amplitude;
      }
      else if (waveForm == SQUARE) {
        value = getPulseTick();

      }
      CUSTOMDBG("Square value: " << value);
      // float value = getSineTick();
      // filter value
      value = filter.processSample(value);
      for (int channel = 0; channel < outputBuffer.getNumChannels(); ++channel)
      {
        outputBuffer.addSample(channel, startSample + sample, value);
      }
    }
  }
  MoogLadderFilter filter;
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
  float getSineTick()
  {
    float value = sin(phase);
    phase += phaseIncrement;
    wrapPhase();
    return value;
  }

float getSaw2PtrTick() {

  // T0 = f0/fs = fundamental frequency / sample rate
  // it's better to update it in setFrequency method in the future

  /*
  Original Python code:
  def PTR1(T0):
	h = 1										# f0 update for h=1:
	cdc = T0									# m
	DC = 1 + cdc
	# -- coefficients (5 operations)
	a1 = 2 - 2*h*P0		# am			# am
	a0 = 2*h - DC			# maa			# a
	#
	p = phi(T0,0.5)
	y = zeros(L)
	for n in range(0,L):
		if p[n] >= T0:		y[n] =  2*p[n] - DC	# outside	MA
		else:				y[n] = a1*p[n] + a0	# inside		MA
	return y
  */


  // Update phase and wrap if necessary
  phase += phaseIncrement;
  wrapPhase();

  // Calculate the normalized phase position for the sawtooth wave, ranging from 0 to 1
  float normalizedPhase = phase / TWOPI;
  
  // Constants for the PTR1 algorithm
  float h = 1.0f; 
  float T0 = frequency / sampleRate;
  float cdc = T0 * h;
  float DC = 1 + cdc;

  // Coefficients for the inside and outside conditions
  float a1 = 2 - 2 * h / p0n;
  float a0 = 2 * h - DC;
  
  // Calculate the sawtooth value based on the phase position
  if (normalizedPhase >= T0) {
    // Outside
    return 2 * normalizedPhase - DC;
  } else {
    // Inside
    return a1 * normalizedPhase + a0;
  }

  
  return 0.0f;
}


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
  float diff(float input, float *history)
  {
    float derivative = (input - history[1]) / (2.0f / p0n);
    history[1] = history[0]; // Shift history
    history[0] = input;      // Store current input
    return derivative;
  }

  /**
   * @brief Calculates and returns the normalized value of a sawtooth wave.
   *
   * This function calculates the value of a sawtooth wave at a given point in time,
   * then differentiates it three times, and finally normalizes it by dividing by the
   * scaling factor. The sawtooth wave is defined by the equation x^4 - 2x^2.
   *
   * @return The normalized value of the sawtooth wave at the current point in time.
   */
  float getSaw4Tick()
  {
    float sawValue = lf_sawpos();
    sawHistory[0] = sawValue * sawValue * (sawValue * sawValue - 2.0f); // x^4 - 2x^2

    // Perform differentiations
    float firstDerivative = diff(sawHistory[0], diff1History);    // 1st derivative: 4x^3 - 4x
    float secondDerivative = diff(firstDerivative, diff2History); // 2nd derivative 12x^2 - 4
    float thirdDerivative = diff(secondDerivative, diff3History); // 3rd derivative 24x

    // Apply scaling factor
    float normalizedValue = thirdDerivative / scalingFactor4;

    return normalizedValue;
  }

  /**
   * @brief Calculates and returns the normalized value of a cubic sawtooth wave.
   *
   * This function calculates the value of a cubic sawtooth wave at a given point in time,
   * then differentiates it twice, and finally normalizes it by dividing by the
   * scaling factor. The cubic sawtooth wave is defined by the equation x^3 - x.
   *
   * @return The normalized value of the cubic sawtooth wave at the current point in time.
   */
  float getSaw3Tick()
  {
    float sawValue = lf_sawpos();
    sawHistory[0] = sawValue * sawValue * sawValue - sawValue; // x^3 - x

    // Perform differentiations
    float firstDerivative = diff(sawHistory[0], diff1History);    // 1st derivative: 3x^2 - 3
    float secondDerivative = diff(firstDerivative, diff2History); // 2nd derivative: 6x

    // Normalize by the scaling factor
    float normalizedValue = secondDerivative / scalingFactor3;

    return normalizedValue;
  }

  /**
   * @brief Calculates and returns the normalized value of a quadratic sawtooth wave.
   *
   * This function calculates the value of a quadratic sawtooth wave at a given point in time,
   * then differentiates it once, and finally normalizes it by dividing by the
   * scaling factor. The quadratic sawtooth wave is defined by the equation x^2.
   *
   * @return The normalized value of the quadratic sawtooth wave at the current point in time.
   */
  float getSaw2Tick()
  {
    float sawValue = lf_sawpos();
    sawHistory[0] = sawValue * sawValue; // x^2

    // Perform differentiation
    float firstDerivative = diff(sawHistory[0], diff1History); // 1st derivative: 2x

    // Normalize by the scaling factor
    float normalizedValue = firstDerivative / scalingFactor2;

    return normalizedValue;
  }

  /**
   * @brief Calculates and returns the value of a linear sawtooth wave.
   *
   * This function calculates the value of a linear sawtooth wave at a given point in time,
   * then normalizes it to the range [-1, 1]. The phase is then incremented and wrapped
   * to the range [0, TWOPI).
   *
   * @return The value of the linear sawtooth wave at the current point in time.
   */
  float lf_sawpos()
  {
    //   saw1l = 2*lf_sawpos(clippedFreq) - 1;
    float value = 2.0f * (phase / TWOPI) - 1.0f;

    phase += phaseIncrement;
    wrapPhase();
    return value;
  }

    // Функция для обновления и возврата текущего значения счетчика модуло
    float phi() {
        float currentPhase = phase; // Сохраняем текущую фазу для возврата
        phase += phaseIncrement; // Обновляем фазу
        if (phase >= 1.0f) {
            phase -= 1.0f; // Оборачиваем фазу, если она достигла или превысила 1
        }
        return currentPhase; // Возвращаем фазу на момент вызова функции
    }

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
  void updatePhaseIncrement()
  {
    samplingPeriod = 1.0f / sampleRate;
    phaseIncrement = frequency * TWOPI / sampleRate;
    p0n = sampleRate / frequency;

    scalingFactor2 = calculateScalingFactor(2);
    scalingFactor3 = calculateScalingFactor(3);
    scalingFactor4 = calculateScalingFactor(4);

    improvedScalingFactor2 = calculateImprovedScalingFactor(2);
    improvedScalingFactor3 = calculateImprovedScalingFactor(3);
    improvedScalingFactor4 = calculateImprovedScalingFactor(4);

    DBG("New phase increment: " << phaseIncrement);
    DBG(frequency << " * " << TWOPI << " / " << sampleRate);
    DBG("waveform: " << waveForm);
    DBG("p0n: " << p0n);

    DBG("Scaling factor 2: " << scalingFactor2);
    DBG("Scaling factor 3: " << scalingFactor3);
    DBG("Scaling factor 4: " << scalingFactor4);

    // update Pulse train variables
    ddel = dutyCycle * p0n; // differential delay calculated by multiplying duty cycle by samples per cycle
    DBG("Differential delay: " << ddel);
  }

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
  float calculateScalingFactor(int order)
  {

    // T = sampling interval (1 / sample rate)
    // omega0 = 2 * pi * f0 (fundamental frequency)
    // formula is pi^n-1/N![2 * sin(omega0*T/2)]

    float omega0 = 2 * M_PI * frequency;
    float T = samplingPeriod;
    float numerator = pow(M_PI, order - 1);
    float denumerator = getFactorial(order) * (2 * std::sin(omega0 * T / 2));
    float value = numerator / denumerator;
    DBG("Scaling factor: " << value << " for order " << order << " and frequency " << frequency);
    return value;
  }

  long double calculateImprovedScalingFactor(int order)
  {
    long double P = sampleRate / frequency;
    long double scalingFactor = 0.0;

    // Check the order and apply the respective formula
    switch (order)
    {
    case 1:
      scalingFactor = 1;
      break;
    case 2:
      scalingFactor = M_PI / (4 * sin(M_PI / P));
      break;
    case 3:
      scalingFactor = pow(M_PI, 2) / (pow(2 * sin(M_PI / P), 2) * 6);
      break;
    case 4:
      scalingFactor = pow(M_PI, 3) / (pow(24 * sin(M_PI / P), 3));
      break;
    case 5:
      scalingFactor = pow(M_PI, 4) / (pow(120 * sin(M_PI / P), 4));
      break;
    case 6:
      scalingFactor = pow(M_PI, 5) / (pow(720 * sin(M_PI / P), 5));
      break;
    default:
      std::cerr << "Order not supported." << std::endl;
      return -1; // Or some other error value
    }

    return scalingFactor;
  }

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
  float getFactorial(int n)
  {
    // precalculated factorials for N = 2 to N = 4
    if (n == 2)
    {
      return 2.0f;
    }
    else if (n == 3)
    {
      return 6.0f;
    }
    else if (n == 4)
    {
      return 24.0f;
    }
    else
    {
      jassert(false); // you should not be here
    }
    return 0.0f;
  }

  /**
   * @brief Wraps the phase to the range [0, TWOPI).
   *
   * This function checks if the phase is outside the range [0, TWOPI) and
   * wraps it back into that range by adding or subtracting TWOPI as necessary.
   */
  void wrapPhase()
  {
    if (phase >= TWOPI)
    {
      phase -= TWOPI;
    }
    if (phase < 0)
    {
      phase += TWOPI;
    }
  }

  // Pulse train methods
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
        float sawSample = getSaw2PtrTick();

        // Store the current sawtooth sample in the delay buffer
        delayBuffer[writeIndex] = sawSample;

        // Increment the write index and wrap if necessary
        writeIndex = (writeIndex + 1) % delayBufferSize;

        // Apply differential delay to the sawtooth wave to generate the pulse train
        return diffDel(sawSample, del);
    }

      // filter


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

  // Sawtooth generator variables

  float amplitude; // amplitude of the oscillator

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

/*

#include <cmath>
#include <vector>

class PulseTrainGenerator {
private:
    float sampleRate;
    float frequency;
    float dutyCycle;
    int delayBufferSize;
    std::vector<float> delayBuffer;
    int writeIndex;

    // Function to calculate the integer part of a float
    int intPart(float value) {
        return static_cast<int>(value);
    }

    // Function to calculate the fractional part of a float
    float fracPart(float value) {
        return value - static_cast<float>(intPart(value));
    }

    // Function to perform the differential delay
    float diffDel(float x, float del) {
        int intDel = intPart(del);
        float fracDel = fracPart(del);
        float delayedX1 = delayBuffer[(writeIndex - intDel + delayBufferSize) % delayBufferSize];
        float delayedX2 = delayBuffer[(writeIndex - intDel - 1 + delayBufferSize) % delayBufferSize];
        return x - delayedX1 * (1.0f - fracDel) - delayedX2 * fracDel;
    }

public:
    PulseTrainGenerator(float sr, float freq, float duty, int bufferSize)
        : sampleRate(sr), frequency(freq), dutyCycle(duty), delayBufferSize(bufferSize) {
        delayBuffer.resize(bufferSize, 0.0f);
        writeIndex = 0;
    }

    // Call this function to generate the next sample of the pulse train
    float nextSample() {
        // Calculate the period and desired delay based on frequency and duty cycle
        float period = sampleRate / frequency;
        float ddel = dutyCycle * period;
        float del = std::fmax(0.0f, std::fmin(static_cast<float>(delayBufferSize - 1), ddel));

        // Generate the next sample of the bandlimited sawtooth wave
        float sawSample = saw2(frequency, writeIndex, sampleRate);

        // Store the current sawtooth sample in the delay buffer
        delayBuffer[writeIndex] = sawSample;

        // Increment the write index and wrap if necessary
        writeIndex = (writeIndex + 1) % delayBufferSize;

        // Apply differential delay to the sawtooth wave to generate the pulse train
        return diffDel(sawSample, del);
    }
};

// Assume a saw2 function is defined elsewhere, which generates a bandlimited sawtooth wave
float saw2(float frequency, int phase, float sampleRate);

// Usage example
int main() {
    float sampleRate = 48000.0f; // Sample rate in Hz
    float frequency = 440.0f; // Frequency of the sawtooth wave in Hz
    float dutyCycle = 0.5f; // Duty cycle of the pulse train (0.5 for square wave)
    int bufferSize = 2048; // Size of the delay buffer

    PulseTrainGenerator generator(sampleRate, frequency, dutyCycle, bufferSize);

    // Generate a buffer of samples
    std::vector<float> outputBuffer(512);
    for (auto& sample : outputBuffer) {
        sample = generator.nextSample();
    }

    // Output buffer would now contain the pulse train samples
    // ...

    return 0;
}


*/

/*
Original Faust code:
declare sawN author "Julius O. Smith III"; // Metadata declaration for the author
declare sawN license "STK-4.3"; // Metadata declaration for the license

// The maximum order of sawtooth wave to be generated is set to 4.
MAX_SAW_ORDER = 4;
// A power of two greater than MAX_SAW_ORDER for internal calculations.
MAX_SAW_ORDER_NEXTPOW2 = 8;

// The saw4 function generates a fourth-order sawtooth wave at a given frequency.
saw4(freq) = saw1l : poly(4) : D(3) : gate(3)
with {
  // Ensure the frequency is at least 20 Hz to avoid inaudible frequencies.
  clippedFreq = max(20.0, abs(freq));
  // Generate a basic sawtooth wave, adjust its mean to zero and scale to +/-1 amplitude.
  saw1l = 2*lf_sawpos(clippedFreq) - 1;
  // Fourth order polynomial shaping of the sawtooth wave.
  poly(4,x) = x*x*(x*x - 2.0);
  // Calculate the period of the sawtooth wave in samples.
  p0n = float(ma.SR)/clippedFreq;
  // First-order difference function, used to approximate the derivative.
  diff1(x) = (x - x')/(2.0/p0n);
  // Recursively apply the difference function N times for Nth order differentiation.
  diff(N) = seq(n,N,diff1);
  // Factorial function needed for normalization in the D function.
  factorial(0) = 1;
  factorial(i) = i * factorial(i-1);
  // Base case for the D function, returns input unchanged.
  D(0) = _;
  // Apply Nth order differentiation and normalize by the factorial of (N+1).
  D(i) = diff(i)/factorial(i+1);
  // Gate function to handle initial conditions and suppress startup transients.
  gate(N) = *(1@(N));
};
*/

/*
pulsetrainN(N,freq,duty) = diffdel(sawN(N,freqC),del) with {
 // non-interpolated-delay version: diffdel(x,del) = x - x@int(del+0.5);
 // linearly interpolated delay version (sounds good to me):
    diffdel(x,del) = x-x@int(del)*(1-ma.frac(del))-x@(int(del)+1)*ma.frac(del);
 // Third-order Lagrange interpolated-delay version (see filters.lib):
 // diffdel(x,del) = x - fdelay3(DELPWR2,max(1,min(DELPWR2-2,ddel)));
 DELPWR2 = 2048; // Needs to be a power of 2 when fdelay*() used above.
 delmax = DELPWR2-1; // arbitrary upper limit on diff delay (duty=0.5)
 SRmax = 96000.0; // assumed upper limit on sampling rate
 fmin = SRmax / float(2.0*delmax); // 23.4 Hz (audio freqs only)
 freqC = max(freq,fmin); // clip frequency at lower limit
 period = (float(ma.SR) / freqC); // actual period
 ddel = duty * period; // desired delay
 del = max(0,min(delmax,ddel));
};

*/
