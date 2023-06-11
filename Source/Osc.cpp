/*
  ============================================================================
    Osc.cpp
    Created: 10 Jun 2023 8:30:49
    Author:  Aleksandr Shabelnikov
  ============================================================================
*/
#include "Osc.h"

Oscillator::Oscillator() : waveform(Sine)
{
    oscillator.initialise([] (float x) { return std::sin(x); });
}

void Oscillator::setWaveform(Waveform newWaveform)
{
    waveform = newWaveform;
    std::cout << "SETTING OSC WAVEFORM TO " << waveform << "\n";
    switch (waveform)
    {
        case Sine:
            oscillator.initialise([] (float x) { return std::sin(x); });
            break;

        case Sawtooth:
            oscillator.initialise([] (float x) { return x / juce::MathConstants<float>::pi; });
            break;

        case Triangle:
            oscillator.initialise([] (float x) { return 2.0f * std::abs(2.0f * (x / juce::MathConstants<float>::twoPi) - 1.0f) - 1.0f; });
            break;
           
        case Square:
            oscillator.initialise([] (float x) {
                // calculate square wave by using sine wave and apply sign function to it
                return std::sin(x) > 0 ? 1.0f : -1.0f;
            });
            break;

        case Pulse:
            oscillator.initialise([this] (float x) {
                // calculate pulse wave by using sine wave and apply sign function to it
                return std::sin(x) > pulseWidth ? 1.0f : -1.0f;
            });
            break;
    }
}

float Oscillator::getNextSample()
{
    return oscillator.processSample(1.0f) * gain;
}

void Oscillator::setFrequency(float freq)
{
    baseFrequency = freq;
    oscillator.setFrequency(baseFrequency + detuneAmount);
}

void Oscillator::setDetune(float detune)
{
    detuneAmount = detune;
    oscillator.setFrequency(baseFrequency + detuneAmount);
}
