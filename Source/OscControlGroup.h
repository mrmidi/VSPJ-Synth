/*
  ==============================================================================

    OscControlGroup.h
    Created: 8 Jul 2023 2:35:46pm
    Author:  Aleksandr Shabelnikov

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
/*
*/
class OscControlGroup  : public juce::Component
{
public:
    OscControlGroup(juce::AudioProcessorValueTreeState& parameters, const juce::String& prefix);
    ~OscControlGroup() override;

    void paint (juce::Graphics&) override;
    void resized() override;
    void setTitle(const juce::String& title);

private:
    // apvts
    juce::AudioProcessorValueTreeState& parameters;

    // Group box
    juce::GroupComponent oscControlGroup;

    // Sliders: Octave, Cent, Gain
    juce::Slider octaveSlider;
    juce::Slider centSlider;
    juce::Slider gainSlider;
    juce::Slider pulseWidthSlider;
    // Labels: Octave, Cent, Gain, Pulse Width
    juce::Label octaveLabel;
    juce::Label centLabel;
    juce::Label gainLabel;
    juce::Label pulseWidthLabel;


    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> octaveAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> centAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> gainAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> pulseWidthAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> waveformAttachment;

    // ComboBox: Waveform
    juce::ComboBox waveformComboBox;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (OscControlGroup)
};
