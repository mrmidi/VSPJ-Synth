/*
  ==============================================================================

    MLF2.h
    Created: 25 Nov 2023 6:08:16pm
    Author:  Alexander Shabelnikov

  ==============================================================================
*/

// Custom debug macro is used to print debug messages only when MIDIUSDEBUG is set to 1
#define MIDIUSDEBUG 0
// Define CUSTOMDBG based on whether DEBUG is enabled
#if MIDIUSDEBUG
#define CUSTOMDBG(...) \
  {                    \
    DBG(__VA_ARGS__);  \
  }
#else
#define CUSTOMDBG(...) \
  {                    \
  }
#endif

#pragma once
#include <JuceHeader.h>
#include "FilterCoeffs.h"
#include "OnePoleFilter.h"

class MoogLadderFilter {
public:
    MoogLadderFilter() {
        // empty constructor
    }

    // Set filter parameters
    void setParams(float cutoff, float resonance) {
        // Check if values have changed to avoid unnecessary calculations
        if (cutoff == this->Fc && Q == resonance) {
            return;
        }
        CUSTOMDBG("Cutoff: " + String(cutoff) + " Resonance: " + String(resonance));
        setFrequency(cutoff);
        setResonance(resonance);
    }

    // Prepare the filter for playback
    void prepareToPlay(float sampleRate) {
        this->sampleRate = sampleRate;
        stage1.reset(); // Reset filter stages
        stage2.reset();
        stage3.reset();
        stage4.reset();
        updateCoefficients();
    }

    // Set the base frequency
    void setFrequency(float freq) {
        baseFrequency = freq; // Store the base frequency
        updateModFrequency(); // Update the frequency considering mod wheel and ADSR
    }

    // Set the resonance
    void setResonance(float Q) {
        this->Q = Q;
        updateCoefficients();
    }

    // Process a single audio sample
    float processSample(float input) {
        float g = coeffs.getG();
        float GAMMA = g * g * g * g;
        float S1 = stage1.getS() / (1.0 + g);
        float S2 = stage2.getS() / (1.0 + g);
        float S3 = stage3.getS() / (1.0 + g);
        float S4 = stage4.getS() / (1.0 + g);

        float SIGMA = g * g * g * S1 + g * g * S2 + g * S3 + S4;

        // u is the input to the first stage
        float u = (input - K * SIGMA) / (1.0 + K * GAMMA);

        // Process through all stages
        float out = stage4.processSample(stage3.processSample(stage2.processSample(stage1.processSample(u))));

        return out;
    }

    // Set modulation from mod wheel
    void setModCutoff(float mod) {
        modWheelMod = mod;
        updateModFrequency(); // Recalculate the effective frequency
    }

    // Set modulation from ADSR
    void setADSRCutOff(float adsrMod) {
        this->adsrMod = adsrMod; // Corrected to use this->adsrMod
        updateModFrequency(); // Recalculate the effective frequency
    }

    void setLFOMod(float lfoMod) {
        this->lfoMod = lfoMod;
        updateModFrequency(); // Recalculate the effective frequency
    }

private:
    // Update the modulated frequency considering base, mod wheel, and ADSR
    void updateModFrequency() {
      float a = 0; // adsr controlled frequency
      float l = 0; // lfo controlled frequency
      float m = 0; // mod wheel controlled frequency
        if (adsrEnabled) {
            a = floatToFrequency(adsrMod); // Convert ADSR value to frequency
        } else {
            a = 0.0f;
        }
      if (lfoMod > 0.0f) {
        l = floatToFrequency(lfoMod);
      }
      if (modWheelMod > 0.0f) {
        m = floatToFrequency(modWheelMod);
      }
        modulatedFrequency = baseFrequency + m + a + l;

        // Clamp frequency to 20-20000 Hz
        if (modulatedFrequency < 20.0f) {
            modulatedFrequency = 20.0f;
        } else if (modulatedFrequency > 20000.0f) {
            modulatedFrequency = 20000.0f;
        }

        Fc = 20.0f + (modulatedFrequency - 20.0f) * (10000.0f - 20.0f) / (20000.0f - 20.0f); // Scale to 20-10000 Hz
        updateCoefficients();
    }

    // Convert a normalized float value (0.0 to 1.0) to a frequency (0 to 5000 Hz)
    float floatToFrequency(float floatVal) {
        return floatVal * 15000.0f;
    }

    // Update filter coefficients
    void updateCoefficients() {
        coeffs.setParams(Fc, sampleRate); // Update filter coefficients
        stage1.setG(coeffs.getG());
        stage2.setG(coeffs.getG());
        stage3.setG(coeffs.getG());
        stage4.setG(coeffs.getG());
        K = 4.0f * (Q - 0.707f) / (25.0f - 0.707f); // if Q = 25, K = 4
    }

    FilterCoeffs coeffs;

    float sampleRate;
    float resonance;
    float frequency; // Not scaled
    float Q;
    float K;
    float Fc;

    float baseFrequency = 0.0f;
    float modulatedFrequency = 0.0f;

    float adsrMod = 0.0f;
    float modWheelMod = 0.0f;
    float lfoMod = 0.0f;

    bool adsrEnabled = true;

    OnePoleFilter stage1;
    OnePoleFilter stage2;
    OnePoleFilter stage3;
    OnePoleFilter stage4;
};
