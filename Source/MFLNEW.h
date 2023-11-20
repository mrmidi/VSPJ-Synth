/*
  ==============================================================================

    MFLNEW.h
    Created: 18 Nov 2023 11:02:03pm
    Author:  Alexander Shabelnikov

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

class MoogLadderFilter {
public:
    MoogLadderFilter()
        : sampleRate(44100.0f),
          frequency(1000.0f),
          cutoff(1.0f) {
        updateCoefficients();
    }

    void setParams(float cutoff, float resonance) {
        // check if values has changed to avoid unnecessary calculations
        if (cutoff == this->cutoff && resonance == this->resonance) {
            return;
        }
        setFrequency(cutoff);
        setResonance(resonance);
    }

    void prepareToPlay(float sampleRate) {
        this->sampleRate = sampleRate;
        // reset state
        state = {0.0f, 0.0f, 0.0f, 0.0f};
        updateCoefficients();
    }

    void setFrequency(float freq) {
        frequency = freq;
        updateCoefficients();
    }

    void setResonance(float resonance) {
        this->resonance = resonance;
        updateCoefficients();
    }

    float processSample(float input) {
    // Calculate input to the first stage with feedback
    float feedback = beta1 * state[0] + beta2 * state[1] + beta3 * state[2] + beta4 * state[3];
    float u = input - K * feedback;

    // Update state variables for each stage
    for (int i = 0; i < 4; ++i) {
        float v = g * (u - state[i]);  // v is the output of each stage
        u = state[i] + v;             // u is the input to the next stage
        //state[i] = state[i] + 2 * v;  // Update state with the bilinear transform
        state.set(i, state[i] + 2 * v);
    }

    // Output of the fourth stage is the filter output
    return state[3];
}


    private:

    double sampleRate;
    float frequency;
    float cutoff;
    float resonance;

    float beta1;
    float beta2;
    float beta3;
    float beta4;

    float K;
    float g;
    float alpha;
    float T;
    float wd;

    juce::Array<float> state = {0.0f, 0.0f, 0.0f, 0.0f};

    void updateCoefficients() {
        // Calculate the coefficients for each stage
         T = 1.0f / sampleRate; // Sample period
         wd = 2 * juce::MathConstants<float>::pi * frequency / 2; // Discrete frequency in Fs/2 (pi radians/sample)
         g = 2 * std::tan(wd * T / 2); // Feedforward coefficient
         alpha = g / (1 + g); // Feedback coefficient
         K = 4.0f * ((resonance - 0.707f)/(25 - 0.707f)) ; // Feedback coefficient normalized in range 0 to 4

        // Calculate the coefficients for each stage
        beta1 = alpha * alpha * alpha / (1.0f + g);
        beta2 = alpha * alpha / (1.0f + g);
        beta3 = alpha / (1.0f + g);
        beta4 = 1.0f / (1.0f + g);


    }


};
