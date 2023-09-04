/*
  ===========================================================================
    Osc.cpp
    Created: 10 Jun 2023 8:3:49
    Author:  Aleksandr Shabelnikov
  ===========================================================================
*/
#include "Osc.h"


void Oscillator::setWaveform(Waveform newWaveform)
{
    if (newWaveform == waveform)
        return;
    waveform = newWaveform;
    std::cout << "SETTING OSC WAVEFORM TO " << waveform << "\n";
    switch (waveform)
    {
        case Sine:
            initialise([] (float x) { return std::sin(x); });
            break;
            
        case Sawtooth:
            initialise([] (float x) { return x / juce::MathConstants<float>::pi; });
            break;
            
        case Triangle:
            initialise([] (float x) { return 2.0f * std::abs(2.0f * (x / juce::MathConstants<float>::twoPi) - 1.0f) - 1.0f; });
            break;
            
        case Square:
            initialise([] (float x) {
                // calculate square wave by using sine wave and apply sign function to it
                return std::sin(x) > 0 ? 1.0f : -1.0f;
            });
            break;
            
        case Pulse:
            initialise([this] (float x) {
                // calculate pulse wave by using sine wave and apply sign function to it
                return std::sin(x) > pulseWidth ? 1.0f : -1.0f;
            });
            break;
    }
    
}

void Oscillator::prepareToPlay(double sampleRate, int samplesPerBlock, int numChannels)
{
    // todo: improove this
    DBG("PREPARING OSCILLATOR");
    DBG("SAMPLE RATE: " << sampleRate << " SAMPLES PER BLOCK: " << samplesPerBlock << " NUM CHANNELS: " << numChannels);
    reset();

    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = samplesPerBlock;
    spec.numChannels = numChannels;

    prepare(spec);

}

void Oscillator::setGain(float newGain)
{
    if (newGain == gain)
        return;
    this->gain = newGain;
}
