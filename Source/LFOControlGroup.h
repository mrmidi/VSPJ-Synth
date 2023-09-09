/*
  ==============================================================================

    LFOControlGroup.h
    Created: 9 Sep 2023 7:50:01am
    Author:  Alexander Shabelnikov

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
/*
*/
class LFOControlGroup  : public juce::Component
{
public:
// OscControlGroup(juce::AudioProcessorValueTreeState& parameters, const juce::String& prefix);
    LFOControlGroup(juce::AudioProcessorValueTreeState& parameters);
    ~LFOControlGroup() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LFOControlGroup)
    juce::AudioProcessorValueTreeState& state;

    juce::GroupComponent lfoControlGroup; // Group box
    juce::ComboBox sourceComboBox; // Keyboard, ModWheel, LFO, Aftertouch?
    juce::Slider depthSlider;  // amplitude
    juce::Slider rateSlider;  // frequency
    juce::ComboBox typeComboBox; // tremolo, vibrato, filter
    juce::ToggleButton enabledToggle; // on/off
    juce::Label depthLabel;
    juce::Label rateLabel;

    // attachments
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> depthAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> rateAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> sourceAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> typeAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> enabledAttachment;

};


/*

----------------------------
Parameter ID: enabledToggle
Component Type: Slider
Declaration: std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> enabledToggleAttachment;
Implementation: enabledToggleAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(parameters, "enabledToggle", enabledToggleSlider);
----------------------------
Parameter ID: sourceComboBox
Component Type: ComboBox
Declaration: std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> sourceComboBoxAttachment;
Implementation: sourceComboBoxAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(parameters, "sourceComboBox", sourceComboBoxComboBox);
----------------------------
Parameter ID: depthSlider
Component Type: Slider
Declaration: std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> depthSliderAttachment;
Implementation: depthSliderAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(parameters, "depthSlider", depthSliderSlider);
----------------------------
Parameter ID: rateSlider
Component Type: Slider
Declaration: std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> rateSliderAttachment;
Implementation: rateSliderAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(parameters, "rateSlider", rateSliderSlider);
----------------------------
Parameter ID: typeComboBox
Component Type: ComboBox
Declaration: std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> typeComboBoxAttachment;
Implementation: typeComboBoxAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(parameters, "typeComboBox", typeComboBoxComboBox);
----------------------------


*/