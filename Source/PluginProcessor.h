/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "SynthAudioSource.h"

#include "Delay.h"
#include "HPF.h"
#include "Chorus.h"
#include "ShelfFilter.h"

//==============================================================================
/**
 */
// class MidiusAudioProcessor  : public juce::AudioProcessor
//                             #if JucePlugin_Enable_ARA
//                              , public juce::AudioProcessorARAExtension
//                             #endif
class MidiusAudioProcessor : public foleys::MagicProcessor
{
public:
  //==============================================================================
  MidiusAudioProcessor(juce::MidiKeyboardState &keyState);

  ~MidiusAudioProcessor() override;

  //==============================================================================
  void prepareToPlay(double sampleRate, int samplesPerBlock) override;
  void releaseResources() override;

#ifndef JucePlugin_PreferredChannelConfigurations
  bool isBusesLayoutSupported(const BusesLayout &layouts) const override;
#endif

  void processBlock(juce::AudioBuffer<float> &, juce::MidiBuffer &) override;

  //==============================================================================
  // juce::AudioProcessorEditor* createEditor() override;
  //    bool hasEditor() const override;

  //==============================================================================
  const juce::String getName() const override;

  bool acceptsMidi() const override;
  bool producesMidi() const override;
  bool isMidiEffect() const override;
  double getTailLengthSeconds() const override;

  //==============================================================================
  int getNumPrograms() override;
  int getCurrentProgram() override;
  void setCurrentProgram(int index) override;
  const juce::String getProgramName(int index) override;
  void changeProgramName(int index, const juce::String &newName) override;

  //==============================================================================
  //    void getStateInformation (juce::MemoryBlock& destData) override;
  //    void setStateInformation (const void* data, int sizeInBytes) override;

  juce::MidiKeyboardState &getKeyboardState() { return keyboardState; }
  juce::dsp::ProcessSpec spec;
  juce::AudioProcessorValueTreeState parameters;

private:
  //==============================================================================
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MidiusAudioProcessor)

  // helper function to convert percentage to linear
  float percentToLinear(int percentage) {
    return static_cast<float>(percentage) / 100.0f;
  }

  juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout()
  {
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    params.push_back(std::make_unique<juce::AudioParameterInt>(juce::ParameterID("osc1Octave", 6), "Octave 1", -3, 3, 0));
    params.push_back(std::make_unique<juce::AudioParameterInt>(juce::ParameterID("osc1Cent", 7), "Cent 1", -100, 100, 0));
//    params.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("osc1Gain", 8), "Gain 1", 0.0f, 1.0f, 0.5f));
    params.push_back(std::make_unique<juce::AudioParameterInt>(juce::ParameterID("osc1Gain", 8), "Gain 1", 0, 100, 100)); // refactor float to percentage
    params.push_back(std::make_unique<juce::AudioParameterInt>(juce::ParameterID("osc1PulseWidth", 9), "PW 1", 0, 100, 50));
    params.push_back(std::make_unique<juce::AudioParameterChoice>(juce::ParameterID("osc1WaveformType", 10), "Waveform 1", juce::StringArray{"Sine", "Square", "Saw", "SawBL2", "SawBL3", "SawBL4", "SawPTR", "SawPTR2", "Triangle"}, 0));

    params.push_back(std::make_unique<juce::AudioParameterInt>(juce::ParameterID("osc2Octave", 11), "Octave 2", -3, 3, 0));
    params.push_back(std::make_unique<juce::AudioParameterInt>(juce::ParameterID("osc2Cent", 12), "Cent 2", -100, 100, 0));
    // params.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("osc2Gain", 13), "Gain 2", 0.0, 1.0, 0.5));
    params.push_back(std::make_unique<juce::AudioParameterInt>(juce::ParameterID("osc2Gain", 13), "Gain 2", 0, 100, 100)); // refactor float to percentage
    params.push_back(std::make_unique<juce::AudioParameterInt>(juce::ParameterID("osc2PulseWidth", 14), "PW 2", 0, 100, 50));
    // params.push_back(std::make_unique<juce::AudioParameterChoice>(juce::ParameterID("osc2WaveformType", 15), "Waveform 2", juce::StringArray {"Sine", "Sawtooth", "Triangle", "Square"}, 0));
    params.push_back(std::make_unique<juce::AudioParameterChoice>(juce::ParameterID("osc2WaveformType", 15), "Waveform 2", juce::StringArray{"Sine", "Square", "Saw", "SawBL2", "SawBL3", "SawBL4", "SawPTR", "SawPTR2", "Triangle"}, 7));

    // Noise level
    params.push_back(std::make_unique<juce::AudioParameterInt>(juce::ParameterID("noiseGain", 16), "Noise Gain", 0, 100, 0));
    // Noise type (white and pink for now)
    params.push_back(std::make_unique<juce::AudioParameterChoice>(juce::ParameterID("noiseType", 38), "Noise Type", juce::StringArray{"White", "Pink"}, 0));

    // LFO
    params.push_back(std::make_unique<juce::AudioParameterChoice>(juce::ParameterID("sourceComboBox", 17), "Source", juce::StringArray{"keyboard", "modwheel", "lfo"}, 2));
    params.push_back(std::make_unique<juce::AudioParameterInt>(juce::ParameterID("depthSlider", 18), "Depth", 0, 100, 50));
    params.push_back(std::make_unique<juce::AudioParameterInt>(juce::ParameterID("rateSlider", 19), "Rate", 1, 20, 5));
    params.push_back(std::make_unique<juce::AudioParameterChoice>(juce::ParameterID("typeComboBox", 20), "Type", juce::StringArray{"tremolo", "pwm"}, 0));

    // ADSR voice
    params.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("adsr1attack", 21), "ADSR Attack", juce::NormalisableRange<float>(0.001f, 3.0f, 0.001f, 0.25f), 0.1f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("adsr1decay", 22), "ADSR Decay", juce::NormalisableRange<float>(0.001f, 3.0f, 0.001f, 0.25f), 0.5));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("adsr1sustain", 23), "ADSR Sustain", 0.0f, 1.0f, 0.5f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("adsr1release", 24), "ADSR Release", juce::NormalisableRange<float>(0.001f, 3.0f, 0.001f, 0.25f), 0.2f));

    // ADSR filter
    params.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("adsr2attack", 25), "Filter Attack", juce::NormalisableRange<float>(0.001f, 3.0f, 0.001f, 0.25f), 0.1f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("adsr2decay", 26), "Filter Decay", juce::NormalisableRange<float>(0.001f, 3.0f, 0.001f, 0.25f), 0.5));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("adsr2sustain", 27), "Filter Sustain", 0.0f, 1.0f, 0.5f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("adsr2release", 28), "Filter Release", juce::NormalisableRange<float>(0.001f, 3.0f, 0.001f, 0.25f), 0.2f));
    // disable adsr for filter combo box
    params.push_back(std::make_unique<juce::AudioParameterChoice>(juce::ParameterID("adsrFilterEnabled", 29), "Filter ADSR Enabled", juce::StringArray{"Disabled", "Enabled"}, 1));
    

    // Filter Amount (using this to mix the dry/wet signal) or maybe it's better to use depth?
    params.push_back(std::make_unique<juce::AudioParameterInt>(juce::ParameterID("filterAmount", 29), "Filter Amount", 0, 100, 50));

    // Cutoff Frequency
    params.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("cutoff", 30), "Cutoff", 20.0, 20000.0, 1000.0));

    // Resonance (or Q Factor)
    params.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("resonance", 31), "Resonance", 0.1, 25, 1.0));

    // Filter Type
    // params.push_back(std::make_unique<juce::AudioParameterChoice>(juce::ParameterID("filterType", 32), "Filter Type", juce::StringArray { "High Pass", "Low Pass", "Band Pass" }, 1)); // default to "Low Pass"

    // Delay Parameters
    params.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("delayTime", 29), "Delay Time", 0.0, 2000.0, 500.0)); // in milliseconds, from 0ms to 2000ms with a default of 500ms
    params.push_back(std::make_unique<juce::AudioParameterInt>(juce::ParameterID("feedback", 30), "Feedback", 0, 100, 50));         // normalized from 0 to 1
    params.push_back(std::make_unique<juce::AudioParameterInt>(juce::ParameterID("delayMix", 31), "Delay Mix", 0, 100, 50));        // dry/wet mix, normalized from 0 to 1

    // Distortion Parameters (NOT USED)
    params.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("drive", 32), "Drive", 1.0, 100.0, 1.0));                // from 1 (no gain) to 100 (a lot of gain)
    params.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("tone", 33), "Tone", 20.0, 20000.0, 1000.0));            // in Hz, from 20Hz to 20kHz with a default of 1kHz
    params.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("distortionMix", 34), "Distortion Mix", 0.0, 1.0, 0.5)); // dry/wet mix, normalized from 0 to 1

    // Chorus Parameters
    params.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("chorusRate", 35), "Chorus Rate", 0.0, 10.0, 0.5)); // in Hz, from 0Hz to 10Hz with a default of 0.5Hz
    params.push_back(std::make_unique<juce::AudioParameterInt>(juce::ParameterID("chorusDepth", 36), "Chorus Depth", 0, 100, 50)); // normalized from 0 to 1
    params.push_back(std::make_unique<juce::AudioParameterInt>(juce::ParameterID("chorusMix", 37), "Chorus Mix", 0, 100, 50));        
    params.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("chorusCentreDelay", 38), "Chorus Centre Delay", 0.0, 100.0, 0.0)); // in milliseconds, from 0ms to 100ms with a default of 0ms
    params.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("chorusFeedback", 39), "Chorus Feedback", -1.0, 1.0, 0.0));

    // Filter LFO params
    params.push_back(std::make_unique<juce::AudioParameterInt>(juce::ParameterID("filterLFOdepth", 32), "Filter LFO Depth", 0, 100, 50));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("filterLFOfrequency", 33), "Filter LFO Rate", 0.0f, 40.0f, 0.5f));

    
    // master gain
    params.push_back(std::make_unique<juce::AudioParameterInt>(juce::ParameterID("masterGain", 35), "Master Gain", 0, 100, 80)); // normalized from 0 to 1 TODO: convert to decibels

    // osc zoom
    params.push_back(std::make_unique<juce::AudioParameterInt>(juce::ParameterID("oscZoom", 36), "Osc Zoom", 1, 10, 5)); // min 1, max 10, default 1

    // hpf (not used anymore)
    params.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("hpfCutoff", 37), "HP", 20.0, 25.0, 20.0)); // in Hz, from 20Hz to 20kHz with a default of 1kHz
    // shelf filter instead of HPF (gain only)
    params.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("lowShelfGain", 38), "Low Shelf", -6.0, 6.0, 0)); // normalized from 0 to 1

    return {params.begin(), params.end()};
  }

  juce::MidiKeyboardState &keyboardState; // Declare this before synthSource
  SynthAudioSource synthSource;

  juce::dsp::Gain<float> masterGain;

  void setVoiceParams();

  StereoDelay delayEffect;

  Chorus chorusEffect;

  LowShelfFilter lowShelfFilter;

  HighPassFilter hpf; // i've noticed a lot of low-end rumble in the audio, so let's add a high-pass filter to remove it

  float hpfCutoffFreq = 20.0f; // default to 20Hz

  foleys::MagicLevelSource *outputMeter = nullptr;
  foleys::MagicPlotSource *oscilloscope = nullptr;
  int oscZoomVal; // 1 to 10 multiplier for oscilloscope zoom
  foleys::MagicPlotSource *analyser = nullptr;
};
