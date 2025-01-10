/*
  ==============================================================================

    OnePoleFilter.h
    Created: 26 Nov 2023 4:46:13pm
    Author:  Alexander Shabelnikov

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
// #include "FilterCoeffs.h"

class OnePoleFilter {
private:
    float s; // This will hold the previous output value (delay element) (z^-1)
    float G; // This is the filter coefficient (G)


public:
    // Constructor
    OnePoleFilter() : s(0), G(0) {}

    void reset() {
        s = 0;
    }

    void setG(float G) {
        this->G = G;
    }

    // Method to process an individual sample
    float processSample(float input) {
        float nonLinearInput = tanh(input);
        float nonLinearFeedback = tanh(s); // Assuming 's' is the state variable holding feedback

        // Your new filter equation here, using nonLinearInput and nonLinearFeedback
        float v = (nonLinearInput - nonLinearFeedback) * G;
        float outputLPF = v + s;

        // Update the delay element with the current output
        s = outputLPF + v;

        return outputLPF;
    }

    float getS() {
      return s;
    }
};
