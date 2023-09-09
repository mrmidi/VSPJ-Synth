/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "SynthAudioSource.h"

//==============================================================================
/**
*/
class MidiusAudioProcessor  : public juce::AudioProcessor
                            #if JucePlugin_Enable_ARA
                             , public juce::AudioProcessorARAExtension
                            #endif
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
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

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
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;
    
    juce::MidiKeyboardState& getKeyboardState() { return keyboardState; }
    juce::dsp::ProcessSpec spec;
    juce::AudioProcessorValueTreeState parameters;
private:
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MidiusAudioProcessor)


     juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout()
        {
            std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

            params.push_back(std::make_unique<juce::AudioParameterInt>(juce::ParameterID("osc1Octave", 6), "Osc 1 Octave", -3, 3, 0));
            params.push_back(std::make_unique<juce::AudioParameterInt>(juce::ParameterID("osc1Cent", 7), "Osc 1 Cent", -50, 50, 0));
            params.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("osc1Gain", 8), "Osc 1 Gain", 0.0f, 1.0f, 0.5f));
            params.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("osc1PulseWidth", 9), "Osc 1 Pulse Width", 0.0f, 1.0f, 0.5f));
            params.push_back(std::make_unique<juce::AudioParameterChoice>(juce::ParameterID("osc1WaveformType", 10), "Osc 1 Waveform Type", juce::StringArray {"Sine", "Triangle", "Sawtooth", "Square" }, 0));
            params.push_back(std::make_unique<juce::AudioParameterInt>(juce::ParameterID("osc2Octave", 11), "Osc 2 Octave", -3, 3, 0));
            params.push_back(std::make_unique<juce::AudioParameterInt>(juce::ParameterID("osc2Cent", 12), "Osc 2 Cent", -50, 50, 0));
            params.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("osc2Gain", 13), "Osc 2 Gain", 0.0, 1.0, 0.5));
            params.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("osc2PulseWidth", 14), "Osc 2 Pulse Width", 0.0, 1.0, 0.5));
            params.push_back(std::make_unique<juce::AudioParameterChoice>(juce::ParameterID("osc2WaveformType", 15), "Osc 2 Waveform Type", juce::StringArray { "Sine", "Triangle", "Sawtooth", "Square" }, 0));
            
            // LFO
            params.push_back(std::make_unique<juce::AudioParameterInt>(juce::ParameterID("enabledToggle", 16), "Enabled Toggle", 0, 1, 1));
            params.push_back(std::make_unique<juce::AudioParameterChoice>(juce::ParameterID("sourceComboBox", 17), "Source Combo Box", juce::StringArray { "keyboard", "modwheel", "lfo" }, 2));
            params.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("depthSlider", 18), "Depth Slider", 0.0, 1.0, 0.5));
            params.push_back(std::make_unique<juce::AudioParameterInt>(juce::ParameterID("rateSlider", 19), "Rate Slider", 1, 20, 5));
            params.push_back(std::make_unique<juce::AudioParameterChoice>(juce::ParameterID("typeComboBox", 20), "Type Combo Box", juce::StringArray { "tremolo", "vibrato", "filter" }, 0));

          // ADSR voice
          params.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("adsr1attack", 21), "ADSR Attack", juce::NormalisableRange<float>(0.001f, 3.0f, 0.001f, 0.25f), 0.1f));
          params.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("adsr1decay", 22), "ADSR Decay", juce::NormalisableRange<float>(0.001f, 3.0f, 0.001f, 0.25f), 0.5));
          params.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("adsr1sustain", 23), "ADSR Sustain", 0.0f, 1.0f, 0.5f));
          params.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("adsr1release", 24), "ADSR Release", juce::NormalisableRange<float>(0.001f, 3.0f, 0.001f, 0.25f), 0.2f));

          // ADSR filter
          params.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("adsr2attack", 25), "ADSR Attack", juce::NormalisableRange<float>(0.001f, 3.0f, 0.001f, 0.25f), 0.1f));
          params.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("adsr2decay", 26), "ADSR Decay", juce::NormalisableRange<float>(0.001f, 3.0f, 0.001f, 0.25f), 0.5));
          params.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("adsr2sustain", 27), "ADSR Sustain", 0.0f, 1.0f, 0.5f));
          params.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("adsr2release", 28), "ADSR Release", juce::NormalisableRange<float>(0.001f, 3.0f, 0.001f, 0.25f), 0.2f));

            return {params.begin(), params.end()};
        }
    
    juce::MidiKeyboardState& keyboardState; // Declare this before synthSource
    SynthAudioSource synthSource;
    
    juce::dsp::Gain<float> masterGain;

    void setVoiceParams();

    
};
