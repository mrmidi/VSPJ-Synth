/*
  ==============================================================================

    SynthVoice.cpp
    Created: 14 Jul 2023 1:18:09pm
    Author:  Aleksandr Shabelnikov

  ==============================================================================
*/

#include "SynthVoice.h"


// void SynthVoice::renderNextBlock(juce::AudioBuffer<float>& outputBuffer, int startSample, int numSamples) {
//     jassert(isPrepared);
//     if (!isVoiceActive())
//         return;

//     synthBuffer.setSize(
//         outputBuffer.getNumChannels(), // Number of channels
//         numSamples, // Number of samples to render
//         false, // Keep existing content (false = overwrite)
//         false, // ClearExtraSpace
//         true // avoidReallocating
//         );
//     synthBuffer.clear();

//     filterAdsr.applyEnvelopeToBuffer(synthBuffer, 0, synthBuffer.getNumSamples());

//     // context

//     juce::dsp::AudioBlock<float> block(synthBuffer);
//     juce::dsp::ProcessContextReplacing<float> context(block);
//     auto upsampled = oversampler.processSamplesUp(context.getInputBlock());

    
//     // oversample oscillators



//     for (int sample = 0; sample < numSamples; ++sample)
//     {
//         // Get the next sample from the oscillator
//         auto oscSample = osc1.getNextSample();
//         auto osc2Sample = osc2.getNextSample();

//         // Apply the ADSR envelope to the oscillators samples
//         auto envSample = adsr.getNextSample() * oscSample;
//         auto envSample2 = adsr.getNextSample() * osc2Sample;

//         float lfoSample = 1.0f;
//         if (isLFOEnabled) {
//             lfoSample = (tremoloLFO.getNextSample() + 1.0f) * 0.5f;  // Convert LFO range from [-1, 1] to [0, 1]
//         }

//         // Calculate filter modulation using filter ADSR
//         auto filterMod = filterAdsr.getNextSample();
//         // if (dbgCounter % 1000 == 0) {  // Only print every 1000th sample
//         //     DBG("filterAdsr output: " + String(filterMod));
//         // }
        
//         // Modulate the filter cutoff frequency
//         float cutoff = filter.getBaseCutOffFreq() + filterMod * filter.getCutoffFrequency();
//         cutoff = juce::jlimit(Filter::MIN_CUTOFF, Filter::MAX_CUTOFF, cutoff);
//         //  DBG("CUTOFF: " << juce::String(cutoff));

//         filter.setCutOffFreq(cutoff + modControllerCutOffFreq); // Apply the modulated cutoff to the filter
        
//         // Combine oscillator samples and apply LFO
//         float drySample = (envSample + envSample2) * 0.5f * lfoSample;

//         // Filter the dry sample
//         float wetSample = filter.processNextSample(0, drySample);

//         // Calculate the dry/wet mix
//         float mixedSample = drySample * (1.0f - filterAmount) + wetSample * filterAmount;

//         for (int channel = 0; channel < outputBuffer.getNumChannels(); ++channel)
//         {
//             // Add the mixed sample to the output buffer
//             outputBuffer.addSample(channel, startSample + sample, mixedSample);
//         }
//     }
// }

void SynthVoice::renderNextBlock(juce::AudioBuffer<float>& outputBuffer, int startSample, int numSamples) {
    jassert (isPrepared);
    jassert (osc1.isInitialised()); // Check that the oscillator is initialised. If not, it will cause a crash.
    if (!isVoiceActive())
        return;

    synthBuffer.setSize(
        outputBuffer.getNumChannels(),
        numSamples,
        false,
        false,
        true
    );
    synthBuffer.clear();

    juce::dsp::AudioBlock<float> block(synthBuffer);
    juce::dsp::ProcessContextReplacing<float> context(block);
    auto upsampledBlock = oversampler.processSamplesUp(context.getInputBlock());
    DBG("Upsampled block has " << upsampledBlock.getNumSamples() << " samples and " << upsampledBlock.getNumChannels() << " channels. Upsampling factor is " << oversampler.getOversamplingFactor() << ".");

    for (int ch = 0; ch < upsampledBlock.getNumChannels(); ++ch) {
        auto* channelData = upsampledBlock.getChannelPointer(ch);
        for (int i = 0; i < upsampledBlock.getNumSamples(); ++i) {
            // float sample1 = osc1.getNextSample();
            // float sample2 = osc2.getNextSample();
            // channelData[i] = (sample1 + sample2) * 0.5f;  // Combine oscillator samples

            channelData[i] = osc1.getNextSample();
        }
    }

    oversampler.processSamplesDown(context.getOutputBlock());

    // Copy processed samples to outputBuffer
    for (int ch = 0; ch < outputBuffer.getNumChannels(); ++ch) {
        outputBuffer.copyFrom(ch, startSample, synthBuffer, ch, 0, numSamples);
    }
}



void SynthVoice::enableLFO(bool isEnabled) {
    if (isEnabled == isLFOEnabled)
        return;

    isLFOEnabled = isEnabled;

}
