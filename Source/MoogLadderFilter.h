/*
  ============================================================================

    MoogLadderFilter
    Created: 10 Oct 2023 1:3:851
    Author:  Alexander Shabenlik
  
    This code is based on DaFX 2nd edition
    Moog Ladder filter implementation
  
    "This source code is provided without any warranties as published in
    DAFX book 2nd edition, copyright Wiley & Sons 2011, available at
    http://www.dafx.de. It may be used for educational purposes and not
    for commercial applications without further permission."

  ============================================================================
*/

#include <JuceHeader.h>

class MoogLadderFilter
{
public:
    ~MoogLadderFilter()
    {
        reset();
    }

    void prepare(const juce::dsp::ProcessSpec& spec)
    {
        fs = spec.sampleRate;
    }

    void reset()
    {
        w.fill(0.0f);
        wold.fill(0.0f);
    }

    float processSample(float inputSample)
    {
        float u = inputSample - 4.0f * Gres * (wold[4] - Gcomp * inputSample);
        w.set(0, std::tanh(u));
        w.set(1, h0 * w[0] + h1 * wold[0] + (1.0f - g) * wold[1]);
        w.set(2, h0 * w[1] + h1 * wold[1] + (1.0f - g) * wold[2]);
        w.set(3, h0 * w[2] + h1 * wold[2] + (1.0f - g) * wold[3]);
        w.set(4, h0 * w[3] + h1 * wold[3] + (1.0f - g) * wold[4]);

        wold = w;
        return w[4];
    }

    void setCutoffFrequency(float frequency)
    {
        g = 2.0f * juce::MathConstants<float>::pi * frequency / fs;
        h0 = g / 1.3f;
        h1 = (g * 0.3f) / 1.3f;
    }

    void setResonance(float resonance)
    {
        Gres = resonance;
    }

private:
    float fs = 44100.0f; // fs is the Sampling frequency will be set in prepareToPlay
    juce::Array<float> w{ 0.0f, 0.0f, 0.0f, 0.0f, 0.0f }; // w is the current state of the filter
    juce::Array<float> wold{ 0.0f, 0.0f, 0.0f, 0.0f, 0.0f }; // wold is the previous state of w

    float Gcomp = 0.5f;  // Gcomp is the compensation gain
    float g = 0.0f;      // g is the IIR feedback coefficient
    float h0 = 0.0f;     // h0 and h1 are the filter coefficients
    float h1 = 0.0f;     // 
    float Gres = 0.0f;   // Gres is the Resonance gain
};
