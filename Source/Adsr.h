/*
  ==============================================================================

    Adsr.h
    Created: 17 Jun 2023 1:38:43pm
    Author:  Aleksandr Shabelnikov

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "Specs.h"

class Adsr : public juce::ADSR
{
public:
    void updateParams(const float attack, const float decay, const float sustain, const float release);
    void updateAttack(const float attack);
    void updateDecay(const float decay);
    void updateSustain(const float sustain);
    void updateRelease(const float release);

    void setCutoffFreqency(const float cutoffFreq);


private:
    juce::ADSR::Parameters adsrParams;
};
