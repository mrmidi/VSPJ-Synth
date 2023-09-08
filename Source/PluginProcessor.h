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
            return {params.begin(), params.end()};
        }
    
    juce::MidiKeyboardState& keyboardState; // Declare this before synthSource
    SynthAudioSource synthSource;
    
    juce::dsp::Gain<float> masterGain;

    void setVoiceParams();

    
};
