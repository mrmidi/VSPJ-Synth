/*
  ==============================================================================

    Adsr.cpp
    Created: 15 Jul 2023 8:10:13pm
    Author:  Alexander Shabelnikov

  ==============================================================================
*/

#include "Adsr.h"

void Adsr::updateParams (const float attack, const float decay, const float sustain, const float release)
{
    adsrParams.attack = attack;
    adsrParams.decay = decay;
    adsrParams.sustain = sustain;
    adsrParams.release = release;
    setParameters(adsrParams);
}