/*
  ==============================================================================

    MLF.h
    Created: 18 Nov 2023 2:46:42pm
    Author:  Alexander Shabelnikov

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>


class MoogLadderFilter {
public:
    MoogLadderFilter()
        : sampleRate(44100.0f),
          normalizedFrequency(0.02f),
          Q(0.7072f) {
        updateCoefficients();
    }

    void setParams(float cutoff, float resonance) {
        // check if values has changed to avoid unnecessary calculations
        if (cutoff == this->cutoff && resonance == this->Q) {
            return;
        }
        // if (cutoff == normalizedFrequency * sampleRate / 2 && resonance == Q) {
        //     return;
        // }
        setFrequency(cutoff);
        setResonance(resonance);
    }

    void prepareToPlay(float sampleRate) {
        this->sampleRate = sampleRate;
        updateCoefficients();
    }

    void setFrequency(float freq) {
        normalizedFrequency = freq / (sampleRate);
        updateCoefficients();
    }

    void setResonance(float Q) {
        this->Q = Q;
        updateCoefficients();
    }

    float processSample(float input) {
        // Calculate input to the first stage with feedback
        float u = input - K * (beta1 * state[0] + beta2 * state[1] + beta3 * state[2] + beta4 * state[3]);
        // float u = input - K * (beta1 * state_old[0] + beta2 * state_old[1] + beta3 * state_old[2] + beta4 * state_old[3]);

        // Update state variables for each stage
        for (int i = 0; i < 4; ++i) {
            float v = g * (u - state[i]);  // v is the output of each stage
            u = state[i] + v;              // u is the input to the next stage
            // state[i] += 2 * v;             // Update state with the bilinear transform
            state[i] += g * v;             // Update state with the bilinear transform
        }
        // update the state_old vector
//        state_old = state;
        // Output of the fourth stage is the filter output
        return state[3];
    }

private:
    void updateCoefficients() {
        float T = 1.0f / sampleRate;
        float wd = 2.0f * M_PI * (normalizedFrequency * sampleRate / 2); // cutoff frequency in radians (normalizedFrequency * sampleRate / 2)
        g = tan(wd * T / 2.0f); // Frequency warping
        alpha = g / (1.0f + g);
        // K = Q; // Feedback coefficient 4.0 * (Q - 0.707) / (25.0 - 0.707) so Q is normalized from 0 to 4 with 0.707 to 25 range
        K = 4.0f * ((Q - 0.707f)/(25 - 0.707f)) ; // Feedback coefficient
        // DBG("K: " << K << ", g: " << g << ", alpha: " << alpha);
        // Calculate beta coefficients
        beta1 = alpha * alpha * alpha / (1.0f + g);
        beta2 = alpha * alpha / (1.0f + g);
        beta3 = alpha / (1.0f + g);
        beta4 = 1.0f / (1.0f + g);
        // DBG("beta1: " << beta1 << ", beta2: " << beta2 << ", beta3: " << beta3 << ", beta4: " << beta4);
        // Reset state variables
        state.assign(4, 0.0f);
        state_old.assign(4, 0.0f);
    }

    float sampleRate;
    float normalizedFrequency;
    float Q;
    float g;
    float alpha;
    float K;
    float beta1, beta2, beta3, beta4;
    float cutoff;
    std::vector<float> state; // Holds the state of the filter
    std::vector<float> state_old; // Holds the state of the filter
};
