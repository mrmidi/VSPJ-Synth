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
    MoogLadderFilter();

    /**
     * @brief Set filter parameters
     *
     * @param cutoff The cutoff frequency
     * @param resonance The resonance value
     */
    void setParams(float cutoff, float resonance);

    /**
     * @brief Prepare the filter for playback
     *
     * @param sampleRate The sample rate for playback
     */
    void prepareToPlay(float sampleRate);

    /**
     * @brief Set the base frequency
     *
     * @param freq The base frequency to be set
     */
    void setFrequency(float freq);

    /**
     * @brief Set the resonance
     *
     * @param Q The resonance value to be set
     */
    void setResonance(float Q);

    /**
     * @brief Process a single audio sample
     *
     * @param input The input audio sample
     * @return The processed audio sample
     */
    float processSample(float input);

    /**
     * @brief Set modulation from mod wheel
     *
     * @param mod The modulation value from the mod wheel
     */
    void setModCutoff(float mod);

    /**
     * @brief Set modulation from ADSR
     *
     * @param adsrMod The modulation value from ADSR
     */
    void setADSRCutOff(float adsrMod);

    /**
     * @brief Set modulation from LFO
     *
     * @param lfoMod The modulation value from LFO
     */
    void setLFOMod(float lfoMod);

private:
    /**
     * @brief Update the modulated frequency considering base, mod wheel, and ADSR
     */
    void updateModFrequency();

    /**
     * @brief Convert a normalized float value (0.0 to 1.0) to a frequency (0 to 15000 Hz)
     *
     * @param floatVal The normalized float value
     * @return The converted frequency
     */
    float floatToFrequency(float floatVal);

    /**
     * @brief Update filter coefficients
     */
    void updateCoefficients();

    FilterCoeffs coeffs;

    float sampleRate;
    float resonance;
    float frequency;
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
