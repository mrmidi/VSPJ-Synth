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

//==============================================================================
/**
*/
//class MidiusAudioProcessor  : public juce::AudioProcessor
//                            #if JucePlugin_Enable_ARA
//                             , public juce::AudioProcessorARAExtension
//                            #endif
class MidiusAudioProcessor : public foleys::MagicProcessor
{
public:
    //==============================================================================
    MidiusAudioProcessor(juce::MidiKeyboardState& keyState);
    
    ~MidiusAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

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
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
//    void getStateInformation (juce::MemoryBlock& destData) override;
//    void setStateInformation (const void* data, int sizeInBytes) override;
    
    juce::MidiKeyboardState& getKeyboardState() { return keyboardState; }
    juce::dsp::ProcessSpec spec;
    juce::AudioProcessorValueTreeState parameters;
private:
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MidiusAudioProcessor)


         juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout()
            {
                std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

                params.push_back(std::make_unique<juce::AudioParameterInt>(juce::ParameterID("osc1Octave", 6), "Octave 1", -3, 3, 0));
                params.push_back(std::make_unique<juce::AudioParameterInt>(juce::ParameterID("osc1Cent", 7), "Cent 1", -100, 100, 0));
                params.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("osc1Gain", 8), "Gain 1", 0.0f, 1.0f, 0.5f));
                params.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("osc1PulseWidth", 9), "PW 1", 0.0f, 1.0f, 0.5f));
                params.push_back(std::make_unique<juce::AudioParameterChoice>(juce::ParameterID("osc1WaveformType", 10), "Waveform 1", juce::StringArray  {"Sine", "Square", "Saw", "SawBL2", "SawBL3", "SawBL4", "SawPTR", "SawPTR2", "Triangle"}, 0));

                params.push_back(std::make_unique<juce::AudioParameterInt>(juce::ParameterID("osc2Octave", 11), "Octave 2", -3, 3, 0));
                params.push_back(std::make_unique<juce::AudioParameterInt>(juce::ParameterID("osc2Cent", 12), "Cent 2", -100, 100, 0));
                params.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("osc2Gain", 13), "Gain 2", 0.0, 1.0, 0.5));
                params.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("osc2PulseWidth", 14), "PW 2", 0.0, 1.0, 0.5));
                //params.push_back(std::make_unique<juce::AudioParameterChoice>(juce::ParameterID("osc2WaveformType", 15), "Waveform 2", juce::StringArray {"Sine", "Sawtooth", "Triangle", "Square"}, 0));
                params.push_back(std::make_unique<juce::AudioParameterChoice>(juce::ParameterID("osc2WaveformType", 15), "Waveform 2", juce::StringArray {"Sine", "Square", "Saw", "SawBL2", "SawBL3", "SawBL4", "SawPTR", "SawPTR2", "Triangle"}, 7));

                // Noise
                params.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("noiseGain", 16), "Noise Gain", 0.0, 1.0, 0.0));

                // LFO

                params.push_back(std::make_unique<juce::AudioParameterChoice>(juce::ParameterID("sourceComboBox", 17), "Source", juce::StringArray { "keyboard", "modwheel", "lfo" }, 2));
                params.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("depthSlider", 18), "Depth", 0.0, 1.0, 0.5));
                params.push_back(std::make_unique<juce::AudioParameterInt>(juce::ParameterID("rateSlider", 19), "Rate", 1, 20, 5));
                params.push_back(std::make_unique<juce::AudioParameterChoice>(juce::ParameterID("typeComboBox", 20), "Type", juce::StringArray { "tremolo", "pwm", "filter" }, 0));

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
             
              // Filter Amount (using this to mix the dry/wet signal) or maybe it's better to use depth?
              params.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("filterAmount", 29), "Filter Amount", 0.0, 1.0, 0.5));

              // Cutoff Frequency (usually in Hz, but you can use a normalized value if you prefer)
              params.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("cutoff", 30), "Cutoff", 20.0, 20000.0, 1000.0));

              // Resonance (or Q Factor)
              params.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("resonance", 31), "Resonance", 0.1, 25, 1.0));

              // Filter Type
              //params.push_back(std::make_unique<juce::AudioParameterChoice>(juce::ParameterID("filterType", 32), "Filter Type", juce::StringArray { "High Pass", "Low Pass", "Band Pass" }, 1)); // default to "Low Pass"

              // Delay Parameters
              params.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("delayTime", 29), "Delay Time", 0.0, 2000.0, 500.0)); // in milliseconds, from 0ms to 2000ms with a default of 500ms
              params.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("feedback", 30), "Feedback", 0.0, 1.0, 0.5)); // normalized from 0 to 1
              params.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("delayMix", 31), "Delay Mix", 0.0, 1.0, 0.5)); // dry/wet mix, normalized from 0 to 1

              // Distortion Parameters
              params.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("drive", 32), "Drive", 1.0, 100.0, 1.0)); // from 1 (no gain) to 100 (a lot of gain)
              params.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("tone", 33), "Tone", 20.0, 20000.0, 1000.0)); // in Hz, from 20Hz to 20kHz with a default of 1kHz
              params.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("distortionMix", 34), "Distortion Mix", 0.0, 1.0, 0.5)); // dry/wet mix, normalized from 0 to 1

              // master gain
              params.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("masterGain", 35), "Master Gain", 0.0, 1.0, 0.5)); // normalized from 0 to 1 TODO: convert to decibels

              // osc zoom
              params.push_back(std::make_unique<juce::AudioParameterInt>(juce::ParameterID("oscZoom", 36), "Osc Zoom", 1, 10, 5)); // min 1, max 10, default 1 

              // hpf
              params.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("hpfCutoff", 37), "HP", 20.0, 25.0, 20.0)); // in Hz, from 20Hz to 20kHz with a default of 1kHz

                return {params.begin(), params.end()};
            }
    
    juce::MidiKeyboardState& keyboardState; // Declare this before synthSource
    SynthAudioSource synthSource;
    
    juce::dsp::Gain<float> masterGain;


    

    
    void setVoiceParams();

    StereoDelay delayEffect;

    HighPassFilter hpf; // i've noticed a lot of low-end rumble in the audio, so let's add a high-pass filter to remove it

    float hpfCutoffFreq = 20.0f; // default to 20Hz

    foleys::MagicLevelSource*   outputMeter  = nullptr;
    foleys::MagicPlotSource*    oscilloscope = nullptr;
    int oscZoomVal; // 1 to 10 multiplier for oscilloscope zoom
    foleys::MagicPlotSource*    analyser     = nullptr;

    
};
