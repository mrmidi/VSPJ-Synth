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

    static long dbgCounter = 0;
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

        // Calculate filter modulation using filter ADSR
        auto filterMod = filterAdsr.getNextSample();
        // if (dbgCounter % 1000 == 0) {  // Only print every 1000th sample
        //     DBG("filterAdsr output: " + String(filterMod));
        // }
        
        // Modulate the filter cutoff frequency
        //float cutoff = filter.getBaseCutOffFreq() + filterMod * filter.getCutoffFrequency();
        //cutoff = juce::jlimit(Filter::MIN_CUTOFF, Filter::MAX_CUTOFF, cutoff);
        //  DBG("CUTOFF: " << juce::String(cutoff));

        // filter.setCutOffFreq(cutoff + modControllerCutOffFreq); // Apply the modulated cutoff to the filter
        
        // Combine oscillator samples and apply LFO
        float drySample = (envSample + envSample2) * 0.5f * lfoSample;

        // Filter the dry sample
        float wetSample = filter.processSample(drySample);

        // Calculate the dry/wet mix
        float mixedSample = drySample * (1.0f - filterAmount) + wetSample * filterAmount;

        for (int channel = 0; channel < outputBuffer.getNumChannels(); ++channel)
        {
            // Add the mixed sample to the output buffer
            outputBuffer.addSample(channel, startSample + sample, mixedSample);
        }
    }
}


void SynthVoice::enableLFO(bool isEnabled) {
    if (isEnabled == isLFOEnabled)
        return;

    isLFOEnabled = isEnabled;

}
