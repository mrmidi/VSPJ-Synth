/*
  ==============================================================================

    LFOControlGroup.cpp
    Created: 9 Sep 2023 7:50:01am
    Author:  Alexander Shabelnikov

  ==============================================================================
*/

#include "LFOControlGroup.h"

// LFOControlGroup(juce::AudioProcessorValueTreeState& parameters);
LFOControlGroup::LFOControlGroup(juce::AudioProcessorValueTreeState& state) : state(state) 
{
    // Set up group component
    lfoControlGroup.setText("LFO Control");
    addAndMakeVisible(lfoControlGroup);

    // Set up source combo box
    sourceComboBox.addItem("Keyboard", 1);
    sourceComboBox.addItem("ModWheel", 2);
    sourceComboBox.addItem("LFO", 3);
//    sourceComboBox.addItem("Aftertouch", 4);
    sourceComboBox.setSelectedId(1);
    addAndMakeVisible(sourceComboBox);

    // Set up depth slider
    depthSlider.setRange(0.0, 1.0);
    depthSlider.setValue(0.5);
    depthSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 50, 20);
    addAndMakeVisible(depthSlider);

    // Set up rate slider
    rateSlider.setRange(1, 20);  // Example range, adjust as needed
    rateSlider.setValue(5);
    rateSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 50, 20);
    addAndMakeVisible(rateSlider);

    // Set up type combo box
    typeComboBox.addItem("Tremolo", 1);
    typeComboBox.addItem("Vibrato", 2);
    typeComboBox.addItem("Filter", 3);
    typeComboBox.setSelectedId(1);
    addAndMakeVisible(typeComboBox);

    // Set up enabled toggle button
    enabledToggle.setButtonText("Enabled");
    addAndMakeVisible(enabledToggle);

    // Set up depth label
    depthLabel.setText("D", juce::dontSendNotification);
    addAndMakeVisible(depthLabel);

    // Set up rate label
    rateLabel.setText("R", juce::dontSendNotification);
    addAndMakeVisible(rateLabel);

    // Attachments
    depthAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(state, "depthSlider", depthSlider);
    rateAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(state, "rateSlider", rateSlider);
    sourceAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(state, "sourceComboBox", sourceComboBox);
    typeAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(state, "typeComboBox", typeComboBox);
    enabledAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(state, "enabledToggle", enabledToggle);
    
}

LFOControlGroup::~LFOControlGroup()
{
    // Destructor content, if needed
}

void LFOControlGroup::paint(juce::Graphics& g)
{
    // Fill the background, if needed
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
}

void LFOControlGroup::resized()
{

    depthLabel.setText("D", juce::dontSendNotification);
    rateLabel.setText("R", juce::dontSendNotification);
    lfoControlGroup.setText("LFO Control");
    
    // Set up group box bounds
    lfoControlGroup.setBounds(getLocalBounds());

    // Constants for positioning
    const int sliderSpacing = 5;
    const int sliderWidth = 140;
    const int sliderHeight = 20;
    const int sliderMargin = 20;
    const int labelWidth = 30;
    const int labelHeight = 20;
    const int comboBoxWidth = 150;
    const int comboBoxHeight = 20;

    // Position each component

    // Enabled Toggle
    enabledToggle.setBounds(lfoControlGroup.getX() + 15 + sliderMargin, lfoControlGroup.getY() + sliderMargin, sliderWidth, sliderHeight);

    // Source ComboBox
    sourceComboBox.setBounds(15, enabledToggle.getBottom() + 10, comboBoxWidth, comboBoxHeight);

    // Depth Label and Slider
    depthLabel.setBounds(5, sourceComboBox.getBottom() + sliderSpacing, labelWidth, labelHeight);
    depthSlider.setBounds(depthLabel.getRight() + sliderSpacing, depthLabel.getY(), sliderWidth, sliderHeight);

    // Rate Label and Slider
    rateLabel.setBounds(5, depthSlider.getBottom() + sliderSpacing, labelWidth, labelHeight);
    rateSlider.setBounds(rateLabel.getRight() + sliderSpacing, rateLabel.getY(), sliderWidth, sliderHeight);

    // Type ComboBox
    typeComboBox.setBounds(15, rateSlider.getBottom() + 10, comboBoxWidth, comboBoxHeight);
}


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
