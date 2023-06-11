/*
  ==============================================================================

    Mixer.cpp
    Created: 10 Jun 2023 9:19:23pm
    Author:  Aleksandr Shabelnikov

  ==============================================================================
*/

#include "Mixer.h"

Mixer::Mixer() : mix(0.0f), gain(1.0f)
{
}

void Mixer::addSample(float sample)
{
    mix += sample * gain;
}

float Mixer::getNextSample()
{
    float sample = mix;
    mix = 0.0f;
    return sample;
}

void Mixer::setGain(float newGain)
{
    gain = newGain;
}
