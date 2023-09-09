/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "OscControlGroup.h"
#include "LFOControlGroup.h"
#include "AdsrControlGroup.h"

//==============================================================================
/**
*/
class MidiusAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    MidiusAudioProcessorEditor (MidiusAudioProcessor&);
    ~MidiusAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;



private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    MidiusAudioProcessor& audioProcessor;

    // Oscillator control group
    OscControlGroup oscControlGroup1;
    OscControlGroup oscControlGroup2;
    
    // LFO Control group
    LFOControlGroup lfoControlGroup1;

    // ADSR For oscillators
    AdsrControlGroup adsrControlGroup1;

    // ADSR for filter
    AdsrControlGroup adsrControlGroup2;

    // Filter control group


    // Keyboard
    juce::MidiKeyboardState keyboardState;
    juce::MidiKeyboardComponent keyboardComponent;


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MidiusAudioProcessorEditor)



        
};
