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

//     synthBuffer.setSize(outputBuffer.getNumChannels(), numSamples, false, false, true);
//     synthBuffer.clear();

//     // 1. Fill the synthBuffer with oscillator samples
//     for (int ch = 0; ch < synthBuffer.getNumChannels(); ++ch)
//     {
//         float* buffer = synthBuffer.getWritePointer(ch);
//         for (int s = 0; s < numSamples; ++s)
//         {
//             buffer[s] = osc1.getNextSample() + osc2.getNextSample();
//         }
//     }

//     // 2. Apply the ADSR
//     adsr.applyEnvelopeToBuffer(synthBuffer, 0, numSamples);
    
//     // 3. Apply LFO if required
//     if (isLFOEnabled) {
//         for (int ch = 0; ch < synthBuffer.getNumChannels(); ++ch)
//         {
//             float* buffer = synthBuffer.getWritePointer(ch);
//             for (int s = 0; s < numSamples; ++s)
//             {
//                 float lfoSample = (tremoloLFO.getNextSample() + 1.0f) * 0.5f;
//                 buffer[s] *= lfoSample;
//             }
//         }
//     }

//     // 4. Use the filter to process the synthBuffer
//     for (int ch = 0; ch < synthBuffer.getNumChannels(); ++ch)
//     {
//         float* buffer = synthBuffer.getWritePointer(ch);
//         for (int s = 0; s < numSamples; ++s)
//         {
//             // Calculate filter modulation using filter ADSR
//             auto filterMod = filterAdsr.getNextSample();
                
//             // Modulate the filter cutoff frequency
//             float cutoff = filter.getBaseCutOffFreq() + filterMod * filter.getDepth() * filter.getBaseCutOffFreq(); 
//             filter.setCutOffFreq(cutoff); 

//             // Filter the sample
//             buffer[s] = filter.processNextSample(ch, buffer[s]);
//         }
//     }

//     // 5. Mix the dry and wet samples in the synthBuffer
//     for (int ch = 0; ch < synthBuffer.getNumChannels(); ++ch)
//     {
//         float* buffer = synthBuffer.getWritePointer(ch);
//         for (int s = 0; s < numSamples; ++s)
//         {
//             float drySample = buffer[s];
//             float wetSample = filter.processNextSample(ch, drySample);
//             buffer[s] = drySample * (1.0f - filterAmount) + wetSample * filterAmount;
//         }
//     }

//     // 6. Copy the processed samples from synthBuffer to the outputBuffer
//     for (int ch = 0; ch < outputBuffer.getNumChannels(); ++ch)
//     {
//         outputBuffer.addFrom(ch, startSample, synthBuffer, ch, 0, numSamples);
//     }
// }




void SynthVoice::renderNextBlock(juce::AudioBuffer<float>& outputBuffer, int startSample, int numSamples) {
    jassert(isPrepared);
    if (!isVoiceActive())
        return;

    // static int dbgCounter = 0;
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
        float cutoff = filter.getBaseCutOffFreq() + filterMod * filter.getBaseCutOffFreq();
        //DBG("CUTOFF: " << juce::String(cutoff));

        filter.setCutOffFreq(cutoff); // Apply the modulated cutoff to the filter

        // Combine oscillator samples and apply LFO
        float drySample = (envSample + envSample2) * 0.5f * lfoSample;

        // Filter the dry sample
        float wetSample = filter.processNextSample(0, drySample);

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
    // DBG("LFO is toggled");
    // if (isLFOEnabled) {
    //     DBG("LFO IS ENABLED");
    //     //tremoloLFO.reset();  // Resetting the LFO when enabled
    // } else {
    //     DBG("LFO IS DISABLED");
    // }
}
