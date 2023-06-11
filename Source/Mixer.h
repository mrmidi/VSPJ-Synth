/*
  ==============================================================================

    Mixer.h
    Created: 10 Jun 2023 9:19:13pm
    Author:  Aleksandr Shabelnikov

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

class Mixer
{
public:
    Mixer();

    void addSample(float sample);

    float getNextSample();

    void setGain(float newGain);

private:
    float mix;
    float gain;
};
