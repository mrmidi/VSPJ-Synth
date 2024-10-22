/*
  ==============================================================================

    MLF2.cpp
    Created: 25 Nov 2023 6:08:16pm
    Author:  Alexander Shabelnikov

  ==============================================================================
*/

#include "MLF2.h"

MoogLadderFilter::MoogLadderFilter() {
    // empty constructor
}

void MoogLadderFilter::setParams(float cutoff, float resonance) {
    // Check if values have changed to avoid unnecessary calculations
    if (cutoff == this->Fc && Q == resonance) {
        return;
    }
    CUSTOMDBG("Cutoff: " + String(cutoff) + " Resonance: " + String(resonance));
    setFrequency(cutoff);
    setResonance(resonance);
}

void MoogLadderFilter::prepareToPlay(float sampleRate) {
    this->sampleRate = sampleRate;
    stage1.reset(); // Reset filter stages
    stage2.reset();
    stage3.reset();
    stage4.reset();
    updateCoefficients();
}

void MoogLadderFilter::setFrequency(float freq) {
    baseFrequency = freq; // Store the base frequency
    updateModFrequency(); // Update the frequency considering mod wheel and ADSR
}

void MoogLadderFilter::setResonance(float Q) {
    this->Q = Q;
    updateCoefficients();
}

float MoogLadderFilter::processSample(float input) {
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

void MoogLadderFilter::setModCutoff(float mod) {
    modWheelMod = mod;
    updateModFrequency(); // Recalculate the effective frequency
}

void MoogLadderFilter::setADSRCutOff(float adsrMod) {
    this->adsrMod = adsrMod; // Corrected to use this->adsrMod
    updateModFrequency(); // Recalculate the effective frequency
}

void MoogLadderFilter::setLFOMod(float lfoMod) {
    this->lfoMod = lfoMod;
    updateModFrequency(); // Recalculate the effective frequency
}

void MoogLadderFilter::updateModFrequency() {
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

float MoogLadderFilter::floatToFrequency(float floatVal) {
    return floatVal * 15000.0f;
}

void MoogLadderFilter::updateCoefficients() {
    coeffs.setParams(Fc, sampleRate); // Update filter coefficients
    stage1.setG(coeffs.getG());
    stage2.setG(coeffs.getG());
    stage3.setG(coeffs.getG());
    stage4.setG(coeffs.getG());
    K = 4.0f * (Q - 0.707f) / (25.0f - 0.707f); // if Q = 25, K = 4
}
