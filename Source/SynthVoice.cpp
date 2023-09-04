/*
  ==============================================================================

    SynthVoice.cpp
    Created: 14 Jul 2023 1:18:09pm
    Author:  Aleksandr Shabelnikov

  ==============================================================================
*/

#include "SynthVoice.h"


SynthVoice::SynthVoice() {
    // Initialization of the voice
    osc1.setWaveform(Oscillator::Sine);
    // print current params
        //DBG("Parameter waveform: " << *params.getRawParameterValue("osc1WaveformType"));

    // osc1.setDetune(0.0f);
    // osc1.setGain(0.5f);
}
