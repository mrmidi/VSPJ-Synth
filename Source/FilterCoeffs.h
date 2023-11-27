/*
  ==============================================================================

    FilterCoeffs.h
    Created: 26 Nov 2023 6:02:57pm
    Author:  Alexander Shabelnikov

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

class FilterCoeffs {

public:
// constructior
FilterCoeffs() : Fc(1000.0f), sampleRate(44100.0f) {
  setParams(Fc, sampleRate); // initialize default values
}


void setParams(float cutoff, float sampleRate) {
  setCutoff(cutoff);
  setSampleRate(sampleRate);
}

void setCutoff(float cutoff) {
  Fc = cutoff;
  update();
}

void setSampleRate(float sampleRate) {
  this->sampleRate = sampleRate;
  update();
}

// Getter methods for each coefficient
float getWd() const { return wd; }
float getT() const { return T; }
float getWa() const { return wa; }
float getG() const { return g; }
float getBigG() const { return G; }

private:
void update() {
  // update coefficients
      wd = 2 * M_PI * Fc;
      T  = 1 / static_cast<float>(sampleRate);
      wa = (2 / T) * tan(wd * T / 2);
      g  = wa * T / 2;
      G  = g / (1.0 + g);
}
// filter coefficients
    float Fc; // Cutoff frequency
    float sampleRate; // Sample rate

    float wd;
    float T;
    float wa;
    float g;
    float G;
};