/*
  ==============================================================================

    MLF2.h
    Created: 25 Nov 2023 6:08:16pm
    Author:  Alexander Shabelnikov

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "FilterCoeffs.h"
#include "OnePoleFilter.h"

class MoogLadderFilter {
public:
    // MoogLadderFilter()
    //     : sampleRate(44100.0f),
    //       Fc(1000.0f),
    //       Q(0.7072f) {
    //     updateCoefficients();
    // }

    MoogLadderFilter() {
        // Initialization of the voice
        // osc1.setWaveform(Oscillator::Sine);
    } 

    void setParams(float cutoff, float resonance) {
        // check if values has changed to avoid unnecessary calculations
        if (cutoff == this->Fc && Q == resonance) {
            return;
        }
        DBG("Cutoff: " + String(cutoff) + " Resonance: " + String(resonance));
        setFrequency(cutoff);
        setResonance(resonance);
    }

    void prepareToPlay(float sampleRate) {
        this->sampleRate = sampleRate;
        stage1.reset(); // reset filter stage
        stage2.reset();
        stage3.reset();
        stage4.reset();
        updateCoefficients();
    }

    void setFrequency(float freq) {
        frequency = freq;
        // scale to 20-10000 Hz because filter is blowing up at frequencies more than 10000 Hz)
        Fc = 20.0f + (frequency - 20.0f) * (10000.0f - 20.0f) / (20000.0f - 20.0f);
        updateCoefficients();
    }

    void setResonance(float Q) {
        this->Q = Q;
        updateCoefficients();
    }

    float processSample(float input) {
      float g = coeffs.getG();
      float GAMMA = g * g * g * g;
      float S1 = stage1.getS()/(1.0 + g);
      float S2 = stage2.getS()/(1.0 + g);
      float S3 = stage3.getS()/(1.0 + g);
      float S4 = stage4.getS()/(1.0 + g);

      float SIGMA = g*g*g * S1 + g*g * S2 + g * S3 + S4;

      // u is the input to the first stage
      float u = (input - K * SIGMA) / (1.0 + K * GAMMA);

      float out = stage4.processSample(stage3.processSample(stage2.processSample(stage1.processSample(u))));

      return out;
    }

private:
    void updateCoefficients() {
      coeffs.setParams(Fc, sampleRate); // update filter coefficients
      stage1.setG(coeffs.getG());
      stage2.setG(coeffs.getG());
      stage3.setG(coeffs.getG());
      stage4.setG(coeffs.getG());
      K = 4.0f * (Q - 0.707f) / (25.0f - 0.707f); // if Q = 25, K = 4
    }

    FilterCoeffs coeffs;

    float sampleRate;
    float resonance;
    float frequency; // not scaled
    float Q;
    float K;
    float Fc;
    

    OnePoleFilter stage1;
    OnePoleFilter stage2;
    OnePoleFilter stage3;
    OnePoleFilter stage4;
};
