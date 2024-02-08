/*
  ==============================================================================

    SynthVoice.cpp
    Created: 14 Jul 2023 1:18:09pm
    Author:  Aleksandr Shabelnikov

  ==============================================================================
*/

#include "SynthVoice.h"

void SynthVoice::renderNextBlock(juce::AudioBuffer<float> &outputBuffer, int startSample, int numSamples)
{
    jassert(isPrepared);
    if (!isVoiceActive())
        return;

    static long dbgCounter = 0;
    for (int sample = 0; sample < numSamples; ++sample)
    {
        // Get the next sample from the oscillator
        auto oscSample = osc1.getNextSample();
        auto osc2Sample = osc2.getNextSample();

        // noise
        float noiseSample = noiseSource.nextFloat() * noiseLevel;

        // DBG("Osc 2 sample value: " << osc2Sample);

        // Apply the ADSR envelope to the oscillators samples
        auto envSample = adsr.getNextSample() * oscSample;
        auto envSample2 = adsr.getNextSample() * osc2Sample;
        auto noiseEnvSample = adsr.getNextSample() * noiseSample;

        float lfoSample = 1.0f;
        if (isLFOEnabled)
        {
            lfoSample = (tremoloLFO.getNextSample() + 1.0f) * 0.5f; // Convert LFO range from [-1, 1] to [0, 1]
        }

        // Calculate filter modulation using filter ADSR
        auto filterMod = filterAdsr.getNextSample();
        // if (dbgCounter % 1000 == 0) {  // Only print every 1000th sample
        //     DBG("filterAdsr output: " + String(filterMod));
        // }

        // Modulate the filter cutoff frequency
        // float cutoff = filter.getBaseCutOffFreq() + filterMod * filter.getCutoffFrequency();
        // cutoff = juce::jlimit(Filter::MIN_CUTOFF, Filter::MAX_CUTOFF, cutoff);
        //  DBG("CUTOFF: " << juce::String(cutoff));

        // filter.setCutOffFreq(cutoff + modControllerCutOffFreq); // Apply the modulated cutoff to the filter

        // Combine oscillator samples and apply LFO
        // if lfo type is tremolo, multiply the samples by the LFO
        LFOsc::lfoType type = tremoloLFO.getType();
        float drySample;
        if (type == LFOsc::lfoType::TREMOLO)
        {
            drySample = (envSample + envSample2) * 0.5f * lfoSample;
        }
        else if (type == LFOsc::lfoType::PWM)
        {
            CUSTOMDBG("PWM LFO DETECTED");
            lfoSample = (lfoSample + 1.0f) / 2.0f;
            // osc1.setPulseWidth(lfoSample);
            // // DBG("LFO sample: " << lfoSample);
            osc2.setPulseWidth(lfoSample);
            drySample = (envSample + envSample2 + noiseEnvSample) * 0.5f;
        }
        else
        {
            // not implemented yet
            // TODO
            drySample = (envSample + envSample + noiseEnvSample) * 0.5f;
        }
        // float drySample = (envSample + envSample2) * 0.5f * lfoSample;

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

void SynthVoice::enableLFO(bool isEnabled)
{
    if (isEnabled == isLFOEnabled)
        return;

    isLFOEnabled = isEnabled;
}
