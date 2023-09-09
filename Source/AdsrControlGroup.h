/*
  ==============================================================================

    AdsrControlGroup.h
    Created: 9 Sep 2023 12:12:36pm
    Author:  Alexander Shabelnikov

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
/*
    A component that groups controls related to an ADSR envelope.
*/
class AdsrControlGroup  : public juce::Component
{
public:
    AdsrControlGroup(juce::AudioProcessorValueTreeState& parameters, const juce::String& prefix);
    ~AdsrControlGroup() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AdsrControlGroup)

    juce::AudioProcessorValueTreeState& parameters;

    juce::GroupComponent adsrControlGroup; // Group box
    juce::Slider attackSlider;
    juce::Slider decaySlider;
    juce::Slider sustainSlider;
    juce::Slider releaseSlider;

    juce::Label attackLabel;
    juce::Label decayLabel;
    juce::Label sustainLabel;
    juce::Label releaseLabel;
    
    
    // attachments
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> attackAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> decayAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> sustainAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> releaseAttachment;
    
};

/*
 ----------------------------
 Parameter ID: adsrAttack
 Component Type: Slider
 Declaration: std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> adsrAttackAttachment;
 Implementation: adsrAttackAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(state, prefix + "adsrAttack", adsrAttackSlider, juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f), 0.1f);
 ----------------------------
 Parameter ID: adsrDecay
 Component Type: Slider
 Declaration: std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> adsrDecayAttachment;
 Implementation: adsrDecayAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(state, prefix + "adsrDecay", adsrDecaySlider, juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f), 0.5f);
 ----------------------------
 Parameter ID: adsrSustain
 Component Type: Slider
 Declaration: std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> adsrSustainAttachment;
 Implementation: adsrSustainAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(state, prefix + "adsrSustain", adsrSustainSlider, juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f), 0.5f);
 ----------------------------
 Parameter ID: adsrRelease
 Component Type: Slider
 Declaration: std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> adsrReleaseAttachment;
 Implementation: adsrReleaseAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(state, prefix + "adsrRelease", adsrReleaseSlider, juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f), 0.2f);
 ----------------------------
 
 */
