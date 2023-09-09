/*
  ==============================================================================

    AdsrControlGroup.cpp
    Created: 9 Sep 2023 12:12:36pm
    Author:  Alexander Shabelnikov

  ==============================================================================
*/

#include <JuceHeader.h>
#include "AdsrControlGroup.h"

AdsrControlGroup::AdsrControlGroup(juce::AudioProcessorValueTreeState& parameters, const juce::String& prefix)
 : parameters(parameters)
{
    // Configure and add group component
    adsrControlGroup.setText("ADSR");
    addAndMakeVisible(adsrControlGroup);

    // Configure and add the sliders
    attackSlider.setSliderStyle(juce::Slider::LinearVertical);
    attackSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 20);
    addAndMakeVisible(attackSlider);

    decaySlider.setSliderStyle(juce::Slider::LinearVertical);
    decaySlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 20);
    addAndMakeVisible(decaySlider);

    sustainSlider.setSliderStyle(juce::Slider::LinearVertical);
    sustainSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 20);
    addAndMakeVisible(sustainSlider);

    releaseSlider.setSliderStyle(juce::Slider::LinearVertical);
    releaseSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 20);
    addAndMakeVisible(releaseSlider);


    // Attack
    juce::NormalisableRange<double> attackRange(0.001, 3.0); // 1ms to 3s
    attackRange.setSkewForCentre(0.01); // skew towards shorter times
    attackSlider.setNormalisableRange(attackRange);

    // Decay
    juce::NormalisableRange<double> decayRange(0.001, 3.0); // 1ms to 3s
    decayRange.setSkewForCentre(0.01); // skew towards shorter times
    decaySlider.setNormalisableRange(decayRange);

    // Sustain - remains linear, between 0 and 1
    sustainSlider.setRange(0.0, 1.0, 0.01);

    // Release
    juce::NormalisableRange<double> releaseRange(0.001, 5.0); // 1ms to 5s
    releaseRange.setSkewForCentre(0.01); // skew towards shorter times
    releaseSlider.setNormalisableRange(releaseRange);


    // Configure and add the labels
    attackLabel.setText("A", juce::dontSendNotification);
    addAndMakeVisible(attackLabel);

    decayLabel.setText("D", juce::dontSendNotification);
    addAndMakeVisible(decayLabel);

    sustainLabel.setText("S", juce::dontSendNotification);
    addAndMakeVisible(sustainLabel);

    releaseLabel.setText("R", juce::dontSendNotification);
    addAndMakeVisible(releaseLabel);

    // Attachments
    attackAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(parameters, prefix + "attack", attackSlider);
    decayAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(parameters, prefix + "decay", decaySlider);
    sustainAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(parameters, prefix + "sustain", sustainSlider);
    releaseAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(parameters, prefix + "release", releaseSlider);
}

AdsrControlGroup::~AdsrControlGroup()
{
}

void AdsrControlGroup::paint (juce::Graphics& g)
{
    // Clear the background
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
}

void AdsrControlGroup::resized()
{
    // Set up group box bounds
    adsrControlGroup.setBounds(getLocalBounds());

    const int sliderSpacing = 5;
    const int sliderWidth = 30;
    const int sliderHeight = 100;
    const int labelWidth = 30;

    // Calculate total width of all sliders and spacings combined
    int totalWidth = 4 * sliderWidth + 3 * sliderSpacing;

    // Calculate starting x position to center the sliders
    int startX = (getWidth() - totalWidth) / 2;

    // Calculate starting y position to center the sliders and labels vertically
    const int labelHeight = 20;
    int totalHeight = sliderHeight + labelHeight;
    int startY = (getHeight() - totalHeight) / 2;

    // Set up label bounds
    attackLabel.setBounds(startX, startY, labelWidth, labelHeight);
    decayLabel.setBounds(attackLabel.getRight() + sliderSpacing, startY, labelWidth, labelHeight);
    sustainLabel.setBounds(decayLabel.getRight() + sliderSpacing, startY, labelWidth, labelHeight);
    releaseLabel.setBounds(sustainLabel.getRight() + sliderSpacing, startY, labelWidth, labelHeight);

    // Adjust startY for slider positioning
    startY += labelHeight;

    // Set up slider bounds
    attackSlider.setBounds(startX, startY, sliderWidth, sliderHeight);
    decaySlider.setBounds(attackSlider.getRight() + sliderSpacing, startY, sliderWidth, sliderHeight);
    sustainSlider.setBounds(decaySlider.getRight() + sliderSpacing, startY, sliderWidth, sliderHeight);
    releaseSlider.setBounds(sustainSlider.getRight() + sliderSpacing, startY, sliderWidth, sliderHeight);
}

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
