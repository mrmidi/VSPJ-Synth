/*
 ==============================================================================
 
 OSCMidius.cpp
 Created: 7 Feb 2024 2:19:05pm
 Author:  Alexander Shabelnikov
 
 ==============================================================================
 */

#include "OSCMidius.h"

// public methods
MidiusOsc::MidiusOsc()
{
    phase = 0;
    sampleRate = 44100;
    frequency = 220;
    waveForm = SAWPTR;
    amplitude = 1.0f;
    
    // pulse train variables (cirular buffer for delay line)
    delayBuffer.resize(delayBufferSize, 0.0f);
    writeIndex = 0;
    
    pitchBendMultiplierSmoothed.reset(sampleRate);              // Set the sample rate if known, otherwise set it in setSampleRate
    pitchBendMultiplierSmoothed.setTargetValue(1.0f);           // Start with no pitch bend
    pitchBendMultiplierSmoothed.setCurrentAndTargetValue(1.0f); // Ensure the current value is also initialized
    
    
    updatePhaseIncrement();
}

void MidiusOsc::getFrequency() 
{
//    DBG("CURRENT FREQUECNY: " << newFrequency);
}

void MidiusOsc::setSampleRate(float newSampleRate)
{
    sampleRate = newSampleRate;
    pitchBendMultiplierSmoothed.reset(sampleRate); // update the sample rate
    updatePhaseIncrement();
}

void MidiusOsc::setFrequency(float newFrequency)
{
    // clamp frequency from 20hz
    frequency = std::max<float>(20.0f, std::fabs(newFrequency));
    CUSTOMDBG("New frequency: " << frequency);

    updatePhaseIncrement();
}

void MidiusOsc::setAmplitude(float newAmplitude)
{
    amplitude = newAmplitude; // linear amplitude
}

void MidiusOsc::setAmplitudeDB(float newAmplitudeDB)
{
    amplitude = juce::Decibels::decibelsToGain(newAmplitudeDB); // dB amplitude
}

void MidiusOsc::renderNextBlock(juce::AudioBuffer<float> &outputBuffer, int startSample, int numSamples)
{
    for (int sample = 0; sample < numSamples; ++sample)
    {
        float value = 0;
        // float amplitude = 0.5f;
        if (waveForm == SINE)
        {
            value = getSineTick() * amplitude * velocity; // amplitude * sin(phase)
        }
        else if (waveForm == SAW)
        {
            value = getSawTick() * amplitude * velocity; // aplitude * (2 * (phase / TWOPI) - 1.0f)
            CUSTOMDBG("Sawtooth value: " << value);
        }
        else if (waveForm == SAWBL2)
        {
            value = getSaw2Tick() * amplitude * velocity;
        }
        else if (waveForm == SAWBL4)
        {
            value = getSaw4Tick() * amplitude * velocity;
        }
        else if (waveForm == SAWBL3)
        {
            value = getSaw3Tick() * amplitude * velocity;
        }
        else if (waveForm == SAWPTR2)
        {
            value = getSaw2PtrTick() * amplitude * velocity;
        }
        else if (waveForm == SQUARE)
        {
            value = getPulseTick() * amplitude * velocity;
        }
        else if (waveForm == SAWPTR)
        {
            DBG("SAWPTR waveform detected");
            value = getSawPtrTick() * amplitude * velocity;
        }
        else
        {
            value = 0.0f; // no waveform detecter or not implemented, so should'n be here
        }
        CUSTOMDBG("Sample value: " << value);
        for (int channel = 0; channel < outputBuffer.getNumChannels(); ++channel)
        {
            outputBuffer.addSample(channel, startSample + sample, value);
        }
    }
}

float MidiusOsc::getNextSample()
{
    // depending on waveform type, return the next sample
    switch (waveForm)
    {
        case SINE:
            return getSineTick() * amplitude * velocity;
        case SAW:
            return getSawTick() * amplitude * velocity;
        case SAWBL2:
            return getSaw2Tick() * amplitude * velocity;
        case SAWBL3:
            return getSaw3Tick() * amplitude * velocity;
        case SAWBL4:
            return getSaw4Tick() * amplitude * velocity;
        case SAWPTR2:
            return getSaw2PtrTick() * amplitude * velocity;
        case SQUARE:
            return getPulseTick() * amplitude * velocity;
        case SAWPTR:
            return getSawPtrTick() * amplitude * velocity;
        case TRIANGLE:
            return getTriangleTick() * amplitude * velocity;
        default:
            jassert(false); // unknown waveform type
    }
    return 0;
}

void MidiusOsc::setWaveform(int waveformType)
{
    if (this->waveForm == waveformType)
    {
        return;
    }
    // check the new waferom in range [0, 8], set 0 if out of range
    if (waveformType < 0 || waveformType > 8)
    {
        waveformType = 0;
    }
    waveForm = static_cast<WaveForm>(waveformType);
    DBG("New waveform: " << getWaveformType());
}

void MidiusOsc::setPulseWidth(float pulseWidth)
{
    // osc1.setPulseWidth(pulseWidth);
    dutyCycle = pulseWidth; // from 0 to 1
}

// void MidiusOsc::setGain(int gain)
// {
//     // osc1.setGain(gain);
//     if (amplitude == gain)
//         return; // No change
//     // convert to 0-1 range
//     amplitude = gain / 100.0f;
//     // convert to db scale
//     amplitude = juce::Decibels::decibelsToGain(gain);
    
//     DBG("New amplitude: " << amplitude);
// }

void MidiusOsc::setGain(int gain)
{
    if (prevGain == gain)
        return; // No change

    if (gain < 0 || gain > 100)
    {
        DBG("Gain percentage out of range: " << gain);
        return; // Ensure gain stays within 0-100%
    }
    prevGain = gain;

    // Map 0-100% to a decibel range (-60 dB to 0 dB)
    float dbGain = juce::jmap(static_cast<float>(gain), 0.0f, 100.0f, -60.0f, 0.0f);

    // Convert decibel value to linear gain
    float linearGain = juce::Decibels::decibelsToGain(dbGain);

    if (amplitude == linearGain)
        return; // No change

    amplitude = linearGain;

    DBG("New amplitude (linear): " << amplitude << ", dB: " << dbGain);
}


void MidiusOsc::setVelocity(float velocity)
{
    this->velocity = velocity;
    DBG("New velocity: " << velocity);
}

void MidiusOsc::setMusicalFrequency(float freq)
{
    // osc1.setMusicalFrequency(freq);
    // clamp frequency from 20hz
    frequency = std::max<float>(20.0f, std::fabs(freq));
//    DBG("OSC frequency: " << frequency);
    updateFrequency();
}

void MidiusOsc::updateFrequency() {
    // Calculate frequency with octave and detune
    float newFrequency = frequency * std::pow(2, octave); // Apply octave
    newFrequency += detune / 100.0f; // Apply detune
    
    // Apply smoothed pitch bend
    // newFrequency *= pitchBendMultiplierSmoothed.getNextValue();
    newFrequency *= pitchBendMultiplier; // freq * 1 if no pitch bend
    
    // Clamp frequency to avoid inaudible ranges
    newFrequency = std::max<float>(20.0f, std::fabs(newFrequency));
    frequency = newFrequency;
    updatePhaseIncrement();
}

void MidiusOsc::setOctave(int newOctave)
{
    if (octave == newOctave)
        return; // No change
    octave = newOctave;
}

void MidiusOsc::setDetune(int newDetune)
{
    if (detune == newDetune)
        return; // No change
    detune = newDetune;
}

void MidiusOsc::setPitchBendMultiplier(float multiplier)
{
    // DBG("Setting pitch bend multiplier to: " << multiplier);
    //  check if it's acutally changed
    if (pitchBendMultiplier == multiplier)
        return;
    DBG("Setting pitch bend multiplier to: " << multiplier);
    pitchBendMultiplier = multiplier;
    pitchBendMultiplierSmoothed.setTargetValue(multiplier);
    
}

// private methods

float MidiusOsc::getSineTick()
{
    float value = sin(phase);
    phase += phaseIncrement;
    wrapPhase();
    return value;
}

float MidiusOsc::getSawPtrTick()
{
    CUSTOMDBG("Method getSawPtrTick() called");
    float h = 1.0f;        // Coefficient for update
    float cdc = T0 * h;    // DC offset
    float DC = 1.0f + cdc; // Total DC offset
    
    // Coefficients for the transition polynomial
    float a1 = 2.0f - 2.0f * h / P0;
    float a0 = 2.0f * h - DC;
    
    // Get the current phase value
    float currentPhase = phi();
    
    // Compute the sawtooth waveform value
    if (currentPhase >= T0)
    {
        // Outside the transition region
        return 2.0f * currentPhase - DC; // Bipolar transform
    }
    else
    {
        // Inside the transition region
        // DBG("Transition region detected at phase: " << currentPhase);
        return a1 * currentPhase + a0;
    }
}

float MidiusOsc::getSaw2PtrTick()
{
    float h = 1.0f;     // Coefficient for update
    float T2 = T0 + T0; // Double T0
    float cdc = T2;
    float DC = 1 + cdc;
    
    // Coefficients for the polynomial
    float a21 = -h;
    float a11 = T2;
    float a01 = 2 * h - DC;
    float a22 = h;
    float a12 = T2 - 4 * h;
    float a02 = 4 * h - DC;
    
    // Get the current phase value
    float currentPhase = phi(); // Assuming phi() is defined elsewhere
    
    // Calculate the output sample based on the phase value
    if (currentPhase >= T2)
    {
        return 2 * currentPhase - DC;
    }
    else if (currentPhase >= T0)
    {
        float D = currentPhase * P0;
        return (a22 * D + a12) * D + a02;
    }
    else
    {
        float D = currentPhase * P0;
        return (a21 * D + a11) * D + a01;
    }
}

// Unipolar modulo counter approach as described in papers
float MidiusOsc::phi()
{
    float currentPhase = phase;
    phase += T0;
    if (phase >= 1.0f)
    {
        phase -= 1.0f;
    }
    return currentPhase;
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
float MidiusOsc::diff(float input, float *history)
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
float MidiusOsc::getSaw4Tick()
{
    float sawValue = getSawTick();
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
float MidiusOsc::getSaw3Tick()
{
    float sawValue = getSawTick();
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
float MidiusOsc::getSaw2Tick()
{
    float sawValue = getSawTick();
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
float MidiusOsc::getSawTick()
{
    float value = 2.0f * (phase / TWOPI) - 1.0f;
    
    phase += phaseIncrement;
    wrapPhase();
    return value;
}

/**
 * @brief Updates the phase increment and scaling factors.
 *
 * This function updates the phase increment and scaling factors based on the current
 * frequency and sample rate. It also calculates the sampling period and the number
 * of samples per cycle (p0n). The scaling factors are calculated for orders 2, 3, and 4.
 */
void MidiusOsc::updatePhaseIncrement()
{
    samplingPeriod = 1.0f / sampleRate;
    phaseIncrement = frequency * TWOPI / sampleRate;
    p0n = sampleRate / frequency;
    
    T0 = frequency / sampleRate;
    P0 = sampleRate / frequency;
    
    // DBG("Frequency: " << frequency << ", sample rate: " << sampleRate << ", sampling period: " << samplingPeriod << ", phase increment: " << phaseIncrement << ", p0n: " << p0n << ", T0: " << T0 << ", P0: " << P0);
    
    scalingFactor2 = calculateScalingFactor(2);
    scalingFactor3 = calculateScalingFactor(3);
    scalingFactor4 = calculateScalingFactor(4);
    
    // update Pulse train variables
    ddel = dutyCycle * p0n; // differential delay calculated by multiplying duty cycle by samples per cycle
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
float MidiusOsc::calculateScalingFactor(int order)
{
    
    // T = sampling interval (1 / sample rate)
    // omega0 = 2 * pi * f0 (fundamental frequency)
    // formula is pi^n-1/N![2 * sin(omega0*T/2)]
    
    float omega0 = 2 * M_PI * frequency;
    float T = samplingPeriod;
    float numerator = pow(M_PI, order - 1);
    float denumerator = getFactorial(order) * (2 * std::sin(omega0 * T / 2));
    float value = numerator / denumerator;
    // DBG("Scaling factor: " << value << " for order " << order << " and frequency " << frequency);
    return value;
}

long double MidiusOsc::calculateImprovedScalingFactor(int order)
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
float MidiusOsc::getFactorial(int n)
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
void MidiusOsc::wrapPhase()
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
int MidiusOsc::intPart(float value)
{
    return static_cast<int>(value); // cast value to int
}

float MidiusOsc::fracPart(float value)
{
    return value - static_cast<float>(intPart(value)); // substract integer part from value
}

// Function to perform the differential delay
float MidiusOsc::diffDel(float x, float del)
{
    int intDel = intPart(del);     // integer part of delay
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

float MidiusOsc::getPulseTick()
{
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

float MidiusOsc::getTriangleTick()
{
    // Generate the next sample of the pulse (square) wave
    float squareSample = getPulseTick();
    // Calculate the gain factor for the triangle wave
    float gain = 4.0f * frequency / sampleRate; 
    // Apply the one-pole filter to the square wave
    float triangleSample = squareSample * gain + prevOutput * pole;
    prevOutput = triangleSample; // Update previous output
    // Ensure the normalized sample is within the [-1, 1] range
    if (triangleSample > 1.0f)
        triangleSample = 1.0f;
    else if (triangleSample < -1.0f)
        triangleSample = -1.0f;
    return triangleSample;
}