/*
  ==============================================================================

    Adsr.h
    Created: 15 Jul 2023 8:10:13pm
    Author:  Alexander Shabelnikov

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

class Adsr : public juce::ADSR
{
public:
    void updateParams(const float attack, const float decay, const float sustain, const float release);
    void updateAttack(const float attack);
    void updateDecay(const float decay);
    void updateSustain(const float sustain);
    void updateRelease(const float release);

    void setCutoffFreqency(const float cutoffFreq);

    bool isFilterEnabled();

    void getParams(float& attack, float& decay, float& sustain, float& release) {
        attack = adsrParams.attack;
        decay = adsrParams.decay;
        sustain = adsrParams.sustain;
        release = adsrParams.release;
    }


private:
    juce::ADSR::Parameters adsrParams;
};
