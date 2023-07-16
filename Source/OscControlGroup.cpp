/*
  ==============================================================================

    OscControlGroup.cpp
    Created: 8 Jul 2023 2:35:47pm
    Author:  Aleksandr Shabelnikov

  ==============================================================================
*/

#include <JuceHeader.h>
#include "OscControlGroup.h"

//==============================================================================
OscControlGroup::OscControlGroup()
{
    // In your constructor, you should add any child components, and
    // initialise any special settings that your component needs.

       // Set up group box
    addAndMakeVisible(oscControlGroup);
    oscControlGroup.setText("Oscillator Controls");

    // Set up sliders
    addAndMakeVisible(octaveSlider);
    octaveSlider.setSliderStyle(juce::Slider::SliderStyle::LinearHorizontal);
    octaveSlider.setTextBoxStyle(juce::Slider::TextBoxRight, true, 50, 20);
    octaveSlider.setRange(-3, 3, 1);
    octaveSlider.setValue(0);

    addAndMakeVisible(centSlider);
    centSlider.setSliderStyle(juce::Slider::SliderStyle::LinearHorizontal);
    centSlider.setTextBoxStyle(juce::Slider::TextBoxRight, true, 50, 20);
    centSlider.setRange(-50, 50, 1);
    centSlider.setValue(0);

    addAndMakeVisible(gainSlider);
    gainSlider.setSliderStyle(juce::Slider::SliderStyle::LinearHorizontal);
    gainSlider.setTextBoxStyle(juce::Slider::TextBoxRight, true, 50, 20);
    gainSlider.setRange(0, 1, 0.01);
    gainSlider.setValue(0.5);

    addAndMakeVisible(pulseWidthSlider);
    pulseWidthSlider.setSliderStyle(juce::Slider::SliderStyle::LinearHorizontal);
    pulseWidthSlider.setTextBoxStyle(juce::Slider::TextBoxRight, true, 50, 20);
    pulseWidthSlider.setRange(0, 1, 0.01);
    pulseWidthSlider.setValue(0.5);

    // Set up labels
    addAndMakeVisible(octaveLabel);
    octaveLabel.setText("O", juce::NotificationType::dontSendNotification);
    octaveLabel.attachToComponent(&octaveSlider, false);

    addAndMakeVisible(centLabel);
    centLabel.setText("D", juce::NotificationType::dontSendNotification);
    centLabel.attachToComponent(&centSlider, false);

    addAndMakeVisible(gainLabel);
    gainLabel.setText("G", juce::NotificationType::dontSendNotification);
    gainLabel.attachToComponent(&gainSlider, false);

    addAndMakeVisible(pulseWidthLabel);
    pulseWidthLabel.setText("PW", juce::NotificationType::dontSendNotification);
    pulseWidthLabel.attachToComponent(&pulseWidthSlider, false);

    // Set up waveform combo box
    addAndMakeVisible(waveformComboBox);
    waveformComboBox.addItem("Sine", 1);
    waveformComboBox.addItem("Triangle", 2);
    waveformComboBox.addItem("Sawtooth", 3);
    waveformComboBox.addItem("Square", 4);
    waveformComboBox.setSelectedId(1);

}

OscControlGroup::~OscControlGroup()
{
}

void OscControlGroup::paint (juce::Graphics& g)
{
    /* This demo code just fills the component's background and
       draws some placeholder text to get you started.

       You should replace everything in this method with your own
       drawing code..
    */

    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));   // clear the background

    //g.setColour (juce::Colours::grey);
    //g.drawRect (getLocalBounds(), 1);   // draw an outline around the component

    g.setColour (juce::Colours::white);
    g.setFont (14.0f);

}

void OscControlGroup::resized()
{
    // This method is where you should set the bounds of any child
    // components that your component contains..

       // Set up group box bounds
    oscControlGroup.setBounds(getLocalBounds());
    const int sliderSpacing = 5;

    // Set up slider bounds
    const int sliderWidth = 140;
    const int sliderHeight = 20;
    const int sliderMargin = 20;
    octaveSlider.setBounds(oscControlGroup.getX() + 15 + sliderMargin, oscControlGroup.getY() + sliderMargin, sliderWidth, sliderHeight);
    centSlider.setBounds(octaveSlider.getX(), octaveSlider.getBottom() + sliderSpacing, sliderWidth, sliderHeight);
    gainSlider.setBounds(centSlider.getX(), centSlider.getBottom() + sliderSpacing, sliderWidth, sliderHeight);
    pulseWidthSlider.setBounds(gainSlider.getX(), gainSlider.getBottom() + sliderSpacing, sliderWidth, sliderHeight);

    // Set up label bounds
    const int labelWidth = 30;
    const int labelHeight = 20;
    octaveLabel.setBounds(5, octaveSlider.getY(), labelWidth, labelHeight);
    centLabel.setBounds(5, centSlider.getY(), labelWidth, labelHeight);
    gainLabel.setBounds(5, gainSlider.getY(), labelWidth, labelHeight);
    pulseWidthLabel.setBounds(5, pulseWidthSlider.getY(), labelWidth, labelHeight);

    // Set up combo box bounds
    const int comboBoxWidth = 150;
    const int comboBoxHeight = 20;
    waveformComboBox.setBounds(15, pulseWidthSlider.getBottom() + 10, comboBoxWidth, comboBoxHeight);

}
