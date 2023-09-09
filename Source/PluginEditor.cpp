/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
MidiusAudioProcessorEditor::MidiusAudioProcessorEditor (MidiusAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p),
    oscControlGroup1(p.parameters, "osc1"),
    oscControlGroup2(p.parameters, "osc2"),
    lfoControlGroup1(p.parameters),
    keyboardComponent(audioProcessor.getKeyboardState(), juce::MidiKeyboardComponent::horizontalKeyboard)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (800, 600);
    addAndMakeVisible(keyboardComponent); // Add the keyboardComponent to the visible components
    addAndMakeVisible (oscControlGroup1);
    addAndMakeVisible (oscControlGroup2);
    addAndMakeVisible (lfoControlGroup1);
}


MidiusAudioProcessorEditor::~MidiusAudioProcessorEditor()
{
}

//==============================================================================
void MidiusAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

}

void MidiusAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..

    oscControlGroup1.setBounds(10, 10, 200, 160);
    oscControlGroup2.setBounds(10, 170, 200, 160);

    lfoControlGroup1.setBounds(210, 10, 200, 160); // X, Y, Width, Height
    
    
    // KEYBOARD
    const int keyboardHeight = 100;
    keyboardComponent.setBounds (0, getHeight() - keyboardHeight, getWidth(), keyboardHeight);
}
