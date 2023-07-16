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
    keyboardComponent(audioProcessor.getKeyboardState(), juce::MidiKeyboardComponent::horizontalKeyboard)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (800, 600);
    addAndMakeVisible(keyboardComponent); // Add the keyboardComponent to the visible components
    addAndMakeVisible (oscControlGroup);
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

    oscControlGroup.setBounds(10, 10, 200, 150);
    
    
    // KEYBOARD
    const int keyboardHeight = 100;
    keyboardComponent.setBounds (0, getHeight() - keyboardHeight, getWidth(), keyboardHeight);
}
