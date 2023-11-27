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
        : sampleRate(44100.0),  // Changed to double
          frequency(1000.0),   // Changed to double
          cutoff(1.0) {        // Changed to double
        updateCoefficients();
    }

    void setParams(float cutoff, float resonance) {
        if (cutoff == this->cutoff && resonance == this->resonance) {
            return;
        }
        setFrequency(cutoff);
        setResonance(resonance);
    }

    void prepareToPlay(double sampleRate) {  // Changed argument type to double
        this->sampleRate = sampleRate;
        state = {0.0f, 0.0f, 0.0f, 0.0f};
        updateCoefficients();
    }

    void setFrequency(double freq) {  // Changed argument type to double
        frequency = freq;
        updateCoefficients();
    }

    void setResonance(double resonance) {  // Changed argument type to double
        this->resonance = resonance;
        updateCoefficients();
    }

    float processSample(float input) {
        float feedback = beta1 * state[0] + beta2 * state[1] + beta3 * state[2] + beta4 * state[3];
        float u = input - K * feedback;

        for (int i = 0; i < 4; ++i) {
            float v = g * (u - state[i]);
            u = state[i] + v;
            state.set(i, state[i] + 2 * v);
        }

        return state[3];
    }

private:
    double sampleRate;
    double frequency;  
    double cutoff;     
    double resonance;  

    double beta1;      
    double beta2;      
    double beta3;      
    double beta4;      

    double K;          
    double g;          
    double alpha;      
    double T;          
    double wd;         

    juce::Array<float> state = {0.0f, 0.0f, 0.0f, 0.0f};

    void updateCoefficients() {
        T = 1.0 / sampleRate;
        wd = 2 * juce::MathConstants<double>::pi * frequency / 2;  // Changed to double
        g = 2 * std::tan(wd * T / 2);
        alpha = g / (1 + g);
        K = 4.0 * ((resonance - 0.707)/(25 - 0.707));  // Changed to double

        beta1 = alpha * alpha * alpha / (1.0 + g);
        beta2 = alpha * alpha / (1.0 + g);
        beta3 = alpha / (1.0 + g);
        beta4 = 1.0 / (1.0 + g);
    }
};
