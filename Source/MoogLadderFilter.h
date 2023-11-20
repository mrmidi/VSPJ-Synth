/*
  ==============================================================================

    MoogLadderFilter.h
    Created: 9 Nov 2023 9:37:58pm
    Author:  Alexander Shabelnikov

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

#include <cmath>
#include <vector>
#include <algorithm>

class MoogLadderFilter {
public:
    // MoogLadderFilter(float sampleRate) : m_sampleRate(sampleRate) {}
    MoogLadderFilter() {
        sampleRate = 44100.0f;
        normalizedFrequency = 0.02f;
        Q = 5.0f;
        updateCoefficients();
    }

    //        filter.setParams(cutoff, resonance);
    void setParams(float cutoff, float resonance) {
        setFrequency(cutoff);
        setResonance(resonance);
    }

    void prepareToPlay(float sampleRate) {
        this->sampleRate = sampleRate;
        // DBG("New sample rate: " << sampleRate);
        updateCoefficients();
    }

    void setFrequency(float freq) {
        // Normalize the frequency to the range [0.0, 1.0]. This is the range that the bilinear transform expects.
        normalizedFrequency = freq / sampleRate; // e.g. 1000 / 44100 = 0.02267573696145124716553287981859
        // DBG("Original frequency: " << freq);
        // DBG("Normalized frequency: " << normalizedFrequency);
        // 12000 / 44100 = 0.27210884353741496598639455782313
        // this->frequency = freq;
        updateCoefficients();
    }

    void setResonance(float Q) {
        this->Q = Q;
        updateCoefficients();
    }

/*
FAUST GENERATED CODE
	virtual void compute(int count, FAUSTFLOAT** RESTRICT inputs, FAUSTFLOAT** RESTRICT outputs) {
		FAUSTFLOAT* input0 = inputs[0];
		FAUSTFLOAT* output0 = outputs[0];
		for (int i0 = 0; i0 < count; i0 = i0 + 1) {
			float fTemp0 = 1.0f * (float(input0[i0]) - 3.2931297e-05f * (0.0005885344f * fRec0[1] + 0.006814578f * fRec1[1] + 0.078905284f * fRec2[1] + 0.91363597f * fRec3[1])) - fRec0[1];
			fRec0[0] = fRec0[1] + 0.17272805f * fTemp0;
			float fTemp1 = fRec0[1] + 0.08636402f * fTemp0 - fRec1[1];
			fRec1[0] = fRec1[1] + 0.17272805f * fTemp1;
			float fTemp2 = fRec1[1] + 0.08636402f * fTemp1 - fRec2[1];
			fRec2[0] = fRec2[1] + 0.17272805f * fTemp2;
			float fTemp3 = fRec2[1] + 0.08636402f * fTemp2 - fRec3[1];
			fRec3[0] = fRec3[1] + 0.17272805f * fTemp3;
			float fRec4 = fRec3[1] + 0.08636402f * fTemp3;
			output0[i0] = FAUSTFLOAT(fRec4);
			fRec0[1] = fRec0[0];
			fRec1[1] = fRec1[0];
			fRec2[1] = fRec2[0];
			fRec3[1] = fRec3[0];
		}
	}

*/

//     float processSample(float input) {
//     // Compute the input to the first stage with feedback
//     // input = 1.0f * input - k * S;
//     // old u calculation
//     // recalculate S with previous stage outputs
//     // S = g * g * g * (s1 * (1 - g)) + g * g * (s2 * (1 - g)) + g * (s3 * (1 - g)) + (s4 * (1 - g)); // ladder's S in generalised form y = G * xi + S
    
//     // Update the feedback path (summing stage outputs) 
//     // S = g^3 * s1 + g^2 * s2 + g * s3 + s4 (Zavalishin, Figure 5.5)
//     // DBG("INPUT: " << input);
//     float stmp1 = g * g * g * (stage1[1] * (1.0f - g));
//     float stmp2 = g * g * (stage2[1] * (1.0f - g));
//     float stmp3 = g * (stage3[1] * (1.0f - g));
//     float stmp4 = stage4[1] * (1.0f - g);
//     S = stmp1 + stmp2 + stmp3 + stmp4;

//     // S = g * g * g * (stage1[1] * (1.0f - g)) + g * g * (stage2[1] * (1.0f - g)) +
//     //     g * (stage3[1] * (1.0f - g)) + (stage4[1] * (1.0f - g));
//     // DBG("S: " << S);
    
// //    float fTemp0 = gainFactor * (float(input0[i0]) - feedbackFactor * (fRec3[1] + g * (fRec2[1] + g * (fRec1[1] + g * fRec0[1])))) - fRec0[1];
//     // S=fRec3[1]+g×(fRec2[1]+g×(fRec1[1]+g×fRec0[1]))
//     // S = stage4[1] + g * (stage3[1] + g * (stage2[1] + g * stage1[1])); // it will be 0 for the first iteration
//     u = ((input - k * S) / (1.0f + k * G)) - stage1[1];
//     // DBG("u: " << u);

//     // First stage
//     // s1 = v1 + lp1 where v1 = g * (u - s1) and lp1 = v1 + s1
//     v1 = g * (u - stage1[1]);
//     lp1 = v1 + stage1[1];
//     stage1[0] = lp1;

//     // Second stage
//     // s2 = v2 + lp2 where v2 = g * (lp1 - s2) and lp2 = v2 + s2
//     v2 = g * (lp1 - stage2[1]);
//     lp2 = v2 + stage2[1];
//     stage2[0] = lp2;

//     // Third stage
//     // s3 = v3 + lp3 where v3 = g * (lp2 - s3) and lp3 = v3 + s3
//     v3 = g * (lp2 - stage3[1]);
//     lp3 = v3 + stage3[1];
//     stage3[0] = lp3;

//     // Fourth stage
//     // s4 = v4 + lp4 where v4 = g * (lp3 - s4) and lp4 = v4 + s4
//     v4 = g * (lp3 - stage4[1]);
//     lp4 = v4 + stage4[1];
//     stage4[0] = lp4;

//     // Update the feedback path (summing stage outputs)
//     // S = g * g * g * (stage1[0] * (1.0f - g)) + g * g * (stage2[0] * (1.0f - g)) +
//     //     g * (stage3[0] * (1.0f - g)) + (stage4[0] * (1.0f - g));

//     // The output of the fourth stage is the filter output
//     float output = lp4;

//     // Update historical values
//     stage1[1] = stage1[0];
//     stage2[1] = stage2[0];
//     stage3[1] = stage3[0];
//     stage4[1] = stage4[0];

//     return output;
// }

// float processSample(float input) {
//     // Update the feedback path (summing stage outputs)
//     S = g * g * g * (stage1 * (1.0f - g)) + g * g * (stage2 * (1.0f - g)) +
//         g * (stage3 * (1.0f - g)) + (stage4 * (1.0f - g));

//     // Compute the input to the first stage with feedback
//     u = (input - k * S) / (1.0f + k * G);

//     // First stage
//     float v1 = g * (u - stage1);
//     float lp1 = v1 + stage1;
//     stage1 = lp1; // Store previous stage value

//     // Second stage
//     float v2 = g * (lp1 - stage2);
//     float lp2 = v2 + stage2;
//     stage2 = lp2; // Store previous stage value

//     // Third stage
//     float v3 = g * (lp2 - stage3);
//     float lp3 = v3 + stage3;
//     stage3 = lp3; // Store previous stage value

//     // Fourth stage
//     float v4 = g * (lp3 - stage4);
//     float lp4 = v4 + stage4;
//     stage4 = lp4; // Store previous stage value

//     // The output of the fourth stage is the filter output
//     return lp4;
// }


float processSample(float input) {
    // Compute the input to the first stage with feedback
    S = g * g * g * (stage1[1] * (1.0f - g)) + g * g * (stage2[1] * (1.0f - g)) +
        g * (stage3[1] * (1.0f - g)) + (stage4[1] * (1.0f - g));

    u = (input - k * S) / (1.0f + k * G);

    // First stage
    v1 = g * (u - stage1[1]);
    stage1[0] = v1 + stage1[1];

    // Second stage
    v2 = g * (stage1[0] - stage2[1]);
    stage2[0] = v2 + stage2[1];

    // Third stage
    v3 = g * (stage2[0] - stage3[1]);
    stage3[0] = v3 + stage3[1];

    // Fourth stage
    v4 = g * (stage3[0] - stage4[1]);
    stage4[0] = v4 + stage4[1];

    // Shift previous stage outputs
    stage1[1] = stage1[0];
    stage2[1] = stage2[0];
    stage3[1] = stage3[0];
    stage4[1] = stage4[0];

    // The output of the fourth stage is the filter output
    return stage4[0];
}



    void renderNextBlock(juce::AudioBuffer<float> &outputBuffer, int startSample, int numSamples) {
        for (int sample = 0; sample < numSamples; ++sample) {
            for (int channel = 0; channel < outputBuffer.getNumChannels(); ++channel) {
                float value = processSample(outputBuffer.getSample(channel, startSample + sample));
                outputBuffer.addSample(channel, startSample + sample, value);
            }
        }
    }


    void updateCoefficients() {
        // Calculate the sample period (T)
        T = 1.0f / sampleRate;

        // Calculate the cutoff frequency. The normalized frequency is assumed to be in the range [0.0, 1.0].
        // We use it to calculate the actual cutoff frequency as a fraction of the Nyquist frequency (sampleRate / 2).
        cutOff = normalizedFrequency * 0.5f * sampleRate;

        // Calculate the feedback coefficient (k)
        k = 4.0f * (Q - 0.707f) / (24.7f - 0.707f);

        // Calculate the warped frequency (omegaWrap) for the bilinear transform
        omegaWrap = std::tan(M_PI * cutOff * T);

        // Calculate the per-stage filter coefficient (g)
        g = omegaWrap / (1.0f + omegaWrap);

        // Calculate the ladder's total gain (G)
        G = g * g * g * g;
        // DBG("Updated coefficients");
        // DBG("T: " << T);
        // DBG("cutOff: " << cutOff);
        // DBG("k: " << k);
        // DBG("omegaWarp: " << omegaWrap);
        // DBG("g: " << g);
        // DBG("G: " << G);
    }

    void process(juce::AudioBuffer<float>& buffer) {
        for (int i = 0; i < buffer.getNumSamples(); i++) {
            float input = buffer.getSample(0, i);
            float output = processSample(input);
            buffer.setSample(0, i, output);
        }
    }

private:
    float sampleRate = 0.0f;
    float normalizedFrequency; // normalized frequency range 0.0 - 1.0
    float Q = 0.7072f; // Default resonance (0.7072 - 24.7)
   float stage1[2] = {0, 0}, stage2[2] = {0, 0}, stage3[2] = {0, 0}, stage4[2] = {0, 0}; // Stage outputs
    // float stage1, stage2, stage3, stage4 = 0.0f; // Stage outputs   
    // Coefficients
    float T = 0.0f; // 1.0/sampleRate (sample period)
    float cutOff = 0.0f ; // frequenct * .5 * sampleRate. normalized frequecny range
    float k = 0.0f; // 4.0 * (Q - 0.707) / (25.0 - 0.707) feedback coeff
    float omegaWrap = 0.0f; // tan(ma.PI * cf * T); 
    float g = 0.0f; // omegaWarp / (1.0 + omegaWarp);
    float G = 0.0f; // G = g * g * g * g; // ladder's G in generalised form y = G * xi + S
    float S = 0.0f; // S = g * g * g * (s1 * (1 - g)) + g * g * (s2 * (1 - g)) + g * (s3 * (1 - g)) + (s4 * (1 - g)); // ladder's S in generalised form y = G * xi +
    float u = 0.0f; // u = (x - k * S) / (1.0 + k * G); // input to the first LP stage: u = (x - kS) / (1 + kG) zavalishin, formula 5.8
    float v1 = 0.0f; // = g * (u - s1); // v-signals in TPT integrator (Zavalishin, Figure 3.30)
    float v2 = 0.0f; // = g * (lp1 - s2); // second stage
    float v3 = 0.0f;// = g * (lp2 - s3); // third stage
    float v4 = 0.0f;//  = g * (lp3 - s4); // fourth stage
    float lp1 = 0.0f;//  = v1 + s1; // define outputs
    float lp2 = 0.0f;// = v2 + s2;
    float lp3 = 0.0f;// = v3 + s3;
    float lp4 = 0.0f;//  = v4 + s4;
};


/*
Original faust implementation

moogLadder(normFreq, Q, x) = loop ~ si.bus(4) : (!,!,!,!,_)
with {
    loop(s1, s2, s3, s4) =  v1 + lp1 , // define s1
                            v2 + lp2 , // define s2
                            v3 + lp3 , // define s3
                            v4 + lp4 , // define s4
                            lp4 // system output
        with {
            T = 1.0 / ma.SR;
            cf = normFreq * .5 * ma.SR;
            k = 4.0 * (Q - 0.707) / (25.0 - 0.707);
            omegaWarp = tan(ma.PI * cf * T);
            g = omegaWarp / (1.0 + omegaWarp);
            G = g * g * g * g; // ladder's G in generalised form y = G * xi + S
            S = g * g * g * (s1 * (1 - g)) + g * g * (s2 * (1 - g)) + g * (s3 * (1 - g)) + (s4 * (1 - g)); // ladder's S in generalised form y = G * xi + S
            u = (x - k * S) / (1.0 + k * G); // input to the first LP stage: u = (x - kS) / (1 + kG)
            v1 = g * (u - s1); // v-signals in TPT integrator (Zavalishin, Figure 3.30)
            v2 = g * (lp1 - s2); // second stage
            v3 = g * (lp2 - s3); // third stage
            v4 = g * (lp3 - s4); // fourth stage
            lp1 = v1 + s1; // define outputs
            lp2 = v2 + s2;
            lp3 = v3 + s3;
            lp4 = v4 + s4;
        };
};

*/

/*
C++ Generated code

virtual void compute(int count, FAUSTFLOAT** RESTRICT inputs, FAUSTFLOAT** RESTRICT outputs) {
    FAUSTFLOAT* input0 = inputs[0];
    FAUSTFLOAT* output0 = outputs[0];
    float omegaWrap = std::tan(1.5707964f * float(fHslider0)); // fSlow0
    float fSlow1 = omegaWrap + 1.0f;
    float g = omegaWrap / fSlow1; // fSlow2
    float kFactor = float(fHslider1) + -0.707f; // fSlow3
    float feedbackFactor = 0.16465649f * kFactor * (1.0f - g); // fSlow4
    float gainFactor = 1.0f / (0.16465649f * (mydsp_faustpower4_f(omegaWrap) * kFactor / mydsp_faustpower4_f(fSlow1)) + 1.0f); // fSlow5
    float updateCoefficient = 2.0f * g; // fSlow6

    for (int i0 = 0; i0 < count; i0++) {
        float fTemp0 = gainFactor * (float(input0[i0]) - feedbackFactor * (fRec3[1] + g * (fRec2[1] + g * (fRec1[1] + g * fRec0[1])))) - fRec0[1];
        fRec0[0] = fRec0[1] + updateCoefficient * fTemp0;
        float fTemp1 = fRec0[1] + g * fTemp0 - fRec1[1];
        fRec1[0] = fRec1[1] + updateCoefficient * fTemp1;
        float fTemp2 = fRec1[1] + g * fTemp1 - fRec2[1];
        fRec2[0] = fRec2[1] + updateCoefficient * fTemp2;
        float fTemp3 = fRec2[1] + g * fTemp2 - fRec3[1];
        fRec3[0] = fRec3[1] + updateCoefficient * fTemp3;
        float fRec4 = fRec3[1] + g * fTemp3;
        output0[i0] = FAUSTFLOAT(fRec4);
        fRec0[1] = fRec0[0];
        fRec1[1] = fRec1[0];
        fRec2[1] = fRec2[0];
        fRec3[1] = fRec3[0];
    }
}



*/
