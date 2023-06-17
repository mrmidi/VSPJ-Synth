/*
  ==============================================================================

    Adsr.cpp
    Created: 17 Jun 2023 1:38:52pm
    Author:  Aleksandr Shabelnikov

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

void Adsr::updateAttack (const float attack)
{
    adsrParams.attack = attack;
    setParameters(adsrParams);
}

void Adsr::updateDecay (const float decay)
{
    adsrParams.decay = decay;
    setParameters(adsrParams);
}

void Adsr::updateSustain (const float sustain)
{
    adsrParams.sustain = sustain;
    setParameters(adsrParams);
}

void Adsr::updateRelease (const float release)
{
    adsrParams.release = release;
    setParameters(adsrParams);
}
