/*
  ==============================================================================

    SynthVoice.h
    Created: 14 Jul 2023 1:18:09pm
    Author:  Aleksandr Shabelnikov

  ==============================================================================
*/

// Custom debug macro is used to print debug messages only when MIDIUSDEBUG is set to 1
#define MIDIUSDEBUG 1
// Define CUSTOMDBG based on whether DEBUG is enabled
#if MIDIUSDEBUG
#define CUSTOMDBG(...)    \
    {                     \
        DBG(__VA_ARGS__); \
    }
#else
#define CUSTOMDBG(...) \
    {                  \
    }
#endif

#pragma once
#include <JuceHeader.h>
#include "Osc.h"
#include "Adsr.h"
#include "SynthSound.h"
#include "LFOsc.h"
#include "Filter.h"
#include "Delay.h"
#include "OSCMidius.h"
#include "MLF2.h"
#include "NoiseGenerator.h"

class SynthVoice : public juce::SynthesiserVoice
{
public:
    SynthVoice();

    bool canPlaySound(juce::SynthesiserSound *sound) override;

    void startNote(int midiNoteNumber, float velocity,
                   juce::SynthesiserSound *sound, int currentPitchWheelPosition) override;

    void stopNote(float velocity, bool allowTailOff) override;

    void pitchWheelMoved(int newPitchWheelValue) override;

    void applyPitchBend(int pitchWheelValue);

    void controllerMoved(int controllerNumber, int newControllerValue) override;

    void prepareToPlay(int samplesPerBlockExpected, double sampleRate, int numChannels);

    float mapModWheelToFrequency(int modWheelValue);

    void renderNextBlock(juce::AudioBuffer<float> &outputBuffer, int startSample, int numSamples) override;

    void setOsc1Params(int octave, int cent, float gain, float pulseWidth, int waveformType);

    void setLfoType(int type);

    void enableLFO(bool isEnabled);

    void setOsc2Params(int octave, int cent, float gain, float pulseWidth, int waveformType);

    void setLFOParams(float depth, float frequency, int source, int type);

    void setADSRParams(float attack, float decay, float sustain, float release);

    void setFilterParams(int type, float cutoff, float resonance, float amount);

    void setFilterAdsrParams(float attack, float decay, float sustain, float release, float baseCutoffFreq);
    
    void setNoiseLevel(float level);

    void setNoiseType(int type);

    void enableFilterADSR(int state);

    void setFilterLFOParams(float depth, float frequency);

private:
    // METHODS
    bool isFilterADSREnabled();

private:
    // VARS
    MidiusOsc osc1;
    MidiusOsc osc2;
    Adsr adsr; // amplitude envelope
    LFOsc tremoloLFO; // tremolo LFO
    LFOsc filterLFO; // filter LFO
    MoogLadderFilter filter; // filter
    Adsr filterAdsr; // filter envelope
    juce::AudioBuffer<float> synthBuffer;
    bool isPrepared = false;
    bool isFilterADSR = true;
    int osc1detune = 0;
    int osc1octave = 0;
    float osc1gain = 0.5f;
    float osc1pulsewidth = 0.5f;
    int osc2detune = 0;
    int osc2octave = 0;
    float osc2gain = 0.5f;
    float osc2pulsewidth = 0.5f;
    float originalFrequency = 440.0f;
    float filterAmount = 1.0f; // will be overridden by APVTS
    bool isLFOEnabled = true;
    float modControllerCutOffFreq = 0; // default value. will be overridden by ModWheel
    float noiseLevel = 0.0f;
    juce::Random noiseSource;
    NoiseGenerator noiseGen; // noise generator
};
