/*
  ==============================================================================

    SynthVoice.cpp
    Created: 14 Jul 2023 1:18:09pm
    Author:  Aleksandr Shabelnikov

  ==============================================================================
*/

#include "SynthVoice.h"

void SynthVoice::renderNextBlock(juce::AudioBuffer<float>& outputBuffer, int startSample, int numSamples) {
    jassert(isPrepared);
    if (!isVoiceActive())
        return;

    for (int sample = 0; sample < numSamples; ++sample)
    {
        // Get the next sample from the oscillator
        auto oscSample = osc1.getNextSample();
        auto osc2Sample = osc2.getNextSample();

        // Apply the ADSR envelope to the oscillators samples
        auto envSample = adsr.getNextSample() * oscSample;
        auto envSample2 = adsr.getNextSample() * osc2Sample;

        float lfoSample = 1.0f;
        if (isLFOEnabled) {
            lfoSample = (tremoloLFO.getNextSample() + 1.0f) * 0.5f;  // Convert LFO range from [-1, 1] to [0, 1]

        }
        
        for (int channel = 0; channel < outputBuffer.getNumChannels(); ++channel)
        {
            // Mix the processed samples of osc1 and osc2, modulate with the LFO, and add to the output buffer
            outputBuffer.addSample(channel, startSample + sample, (envSample + envSample2) * 0.5f * lfoSample);
        }
    }
}

void SynthVoice::enableLFO(bool isEnabled) {
    if (isEnabled == isLFOEnabled)
        return;

    isLFOEnabled = isEnabled;
    // DBG("LFO is toggled");
    // if (isLFOEnabled) {
    //     DBG("LFO IS ENABLED");
    //     //tremoloLFO.reset();  // Resetting the LFO when enabled
    // } else {
    //     DBG("LFO IS DISABLED");
    // }
}
