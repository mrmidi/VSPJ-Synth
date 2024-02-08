// THIS IS NOT USED!!!!

/*
  ==============================================================================

    MidiusOsc.cpp
    Created: 6 Feb 2024 12:03:41am
    Author:  Alexander Shabelnikov

  ==============================================================================
*/

#include "MidiusOsc.h"

// public methods
MyOscillator::MyOscillator()
{
    phase = 0;
    sampleRate = 44100;
    frequency = 220;
    waveForm = SAWPTR2;  // Ensure SAWPTR2 is defined somewhere or replace with appropriate value
    // waveForm = SINE;  // Uncomment if needed
    // waveForm = SQUARE;  // Uncomment if needed and correct typo "SQUAE" to "SQUARE"
    amplitude = 1.0f;

    // Initialize pulse train variables
    delayBuffer.resize(delayBufferSize, 0.0f);  
    writeIndex = 0;

    updatePhaseIncrement();  // Ensure this function is implemented
}

void MyOscillator::setSampleRate(float newSampleRate)
{
    filter.prepareToPlay(newSampleRate);  
    sampleRate = newSampleRate;
    updatePhaseIncrement();  
}

void MyOscillator::setBaseFrequency(float newFrequency)
{
    // clamp frequency from 20hz
    baseFrequency = std::max<float>(20.0f, std::fabs(newFrequency));
    MyOscillator::setFrequency(); // update frequency
}

void MyOscillator::setOctave(int newOctave)
{
    if (octave == newOctave) return;  // No change
    octave = newOctave;
    MyOscillator::setFrequency(); // update frequency
}

void MyOscillator::setFrequency()
{
    // frequency equals baseFrequency + pitch + octave + detune
    frequency = baseFrequency;
    frequency = std::max<float>(20.0f, std::fabs(frequency)); // self clamp
    CUSTOMDBG("New frequency: " << frequency);
    updatePhaseIncrement();
}

void MyOscillator::setAmplitude(float newAmplitude)
{
    amplitude = newAmplitude; // linear amplitude
}

void MyOscillator::setAmplitudeDB(float newAmplitudeDB)
{
    amplitude = juce::Decibels::decibelsToGain(newAmplitudeDB); // dB amplitude
}

void MyOscillator::renderNextBlock(juce::AudioBuffer<float> &outputBuffer, int startSample, int numSamples)
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
            value = lf_sawpos() * amplitude; // amplitude * (2 * (phase / TWOPI) - 1.0f)
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
        else if (waveForm == SQUARE) 
        {
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

// private methods

float MyOscillator::getSineTick()
{
    float value = sin(phase);
    phase += phaseIncrement;
    wrapPhase();
    return value;
}

float MyOscillator::getSaw2PtrTick() {

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
}

float MyOscillator::diff(float input, float *history)
{
    float derivative = (input - history[1]) / (2.0f / p0n);
    history[1] = history[0]; // Shift history
    history[0] = input;      // Store current input
    return derivative;
}

float MyOscillator::getSaw4Tick()
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

float MyOscillator::getSaw3Tick()
{
    float sawValue = lf_sawpos();
    sawHistory[0] = sawValue * sawValue * sawValue - sawValue; // x^3 - x

    // Perform differentiations
    float firstDerivative = diff(sawHistory[0], diff1History);    // 1st derivative: 3x^2 - 1
    float secondDerivative = diff(firstDerivative, diff2History); // 2nd derivative: 6x

    // Normalize by the scaling factor
    float normalizedValue = secondDerivative / scalingFactor3;

    return normalizedValue;
}

float MyOscillator::getSaw2Tick()
{
    float sawValue = lf_sawpos();
    sawHistory[0] = sawValue * sawValue; // x^2

    // Perform differentiation
    float firstDerivative = diff(sawHistory[0], diff1History); // 1st derivative: 2x

    // Normalize by the scaling factor
    float normalizedValue = firstDerivative / scalingFactor2;

    return normalizedValue;
}

// Function to update and return the current value of the modulo counter
float MyOscillator::phi() {
    float currentPhase = phase; // Save the current phase for return
    phase += phaseIncrement; // Update phase
    if (phase >= 1.0f) {
        phase -= 1.0f; // Wrap phase if it reaches or exceeds 1
    }
    return currentPhase; // Return the phase at the time of function call
}

void MyOscillator::updatePhaseIncrement()
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

float MyOscillator::calculateScalingFactor(int order)
{

    // T = sampling interval (1 / sample rate)
    // omega0 = 2 * pi * f0 (fundamental frequency)
    // formula is pi^n-1/N![2 * sin(omega0*T/2)]

    float omega0 = 2 * M_PI * frequency; // Fundamental angular frequency
    float T = samplingPeriod; // Sampling interval
    float numerator = pow(M_PI, order - 1); // Pi to the power of (order - 1)
    float denumerator = getFactorial(order) * (2 * std::sin(omega0 * T / 2)); // Factorial of order times the sinusoidal component
    float value = numerator / denumerator; // Calculation of the scaling factor
    DBG("Scaling factor: " << value << " for order " << order << " and frequency " << frequency);
    return value;
}

long double MyOscillator::calculateImprovedScalingFactor(int order)
{
    long double P = sampleRate / frequency;
    long double scalingFactor = 0.0;

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

float MyOscillator::getFactorial(int n)
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

void MyOscillator::wrapPhase()
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













