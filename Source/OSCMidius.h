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

class MidiusOsc
{
public:
    MidiusOsc();
    
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
    
    void setFrequency(float newFrequency);
    
    void setAmplitude(float newAmplitude);
    
    void setAmplitudeDB(float newAmplitudeDB);
    
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
    
    float getNextSample();
    
    void setWaveform(int waveformType);
    
    void setPulseWidth(float pulseWidth);
    
    void setGain(float gain);
    
    void setOctave(int octave);
    
    void setDetune(int cent);
    
    void setMusicalFrequency(float freq);
    
    void setPitchBendMultiplier(float multiplier);
    
    void updateFrequency();
    
    void getFrequency(); // helper to debug actual frequency

    void setVelocity(float newVelocity);
    
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
    
    float getSawPtrTick();
    
    float getSaw2PtrTick();

    float getTriangleTick();
    
    // Unipolar modulo counter approach as described in papers
    float phi();
    
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
    float getSawTick();
    
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
    
    // Pulse train methods
    int intPart(float value);
    
    float fracPart(float value);
    
    // Function to perform the differential delay
    float diffDel(float x, float del);
    
    float getPulseTick();


    
    
private:
    // Variable declarations
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MidiusOsc)
    
    enum WaveForm
    {
        SINE,    // 0
        SQUARE,  // 1
        SAW,     // 2
        SAWBL2,  // 3
        SAWBL3,  // 4
        SAWBL4,  // 5
        SAWPTR,  // 6
        SAWPTR2, // 7
        TRIANGLE // 8
    };
    
    juce::String getWaveformType()
    {
        switch (waveForm)
        {
            case SINE:
                return "Sine";
            case SQUARE:
                return "Square";
            case SAW:
                return "Saw";
            case SAWBL2:
                return "SawBL2";
            case SAWBL3:
                return "SawBL3";
            case SAWBL4:
                return "SawBL4";
            case SAWPTR:
                return "SawPTR";
            case SAWPTR2:
                return "SawPTR2";
            case TRIANGLE:
                return "Triangle";
            default:
                return "Unknown";
        }
    }
    
    // Oscillator variables
    float pole = 0.999f;         // Pole value for the filter (for triangle wave)
    float prevOutput = 0.0f;     // Previous output for filter (for triangle wave)
    
    int octave = 0; // 0 on the initial state
    int detune = 0; // 0 on the initial state
    float pitchBendMultiplier = 1.0f;
    juce::SmoothedValue<float> pitchBendMultiplierSmoothed;
    
    float amplitude; // amplitude of the oscillator
    float velocity;
    
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
    
    // PTR variables
    float T0; // frequency / sampling rate (fundamental frequency)
    float P0; //
    
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
    float ddel = 0.0f;              // differential delay
    float dutyCycle = 0.5f;         // duty cycle of the pulse train
    int delayBufferSize = 2048;     // size of the delay buffer
    std::vector<float> delayBuffer; // delay buffer
    int writeIndex = 0;             // write index for the delay buffer
};
