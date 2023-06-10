#include "MainComponent.h"
#include "SynthAudioSource.h"


SynthAudioSource::SynthAudioSource(juce::MidiKeyboardState& keyState, juce::MidiKeyboardComponent::Orientation orientation)
    : keyboardState(keyState), keyboardOrientation(orientation)
{
    for (int i = 0; i < 10; ++i)
        synth.addVoice(new SynthVoice());

    synth.addSound(new SynthSound());
}


void SynthAudioSource::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    synth.setCurrentPlaybackSampleRate(sampleRate);
    printf("\nsetting audio sample rate to %f\n", sampleRate);
    midiCollector.reset(sampleRate);
}

void SynthAudioSource::releaseResources() {}

void SynthAudioSource::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill)
{
    bufferToFill.clearActiveBufferRegion();

    juce::MidiBuffer incomingMidi;
    midiCollector.removeNextBlockOfMessages (incomingMidi, bufferToFill.numSamples); 

    keyboardState.processNextMidiBuffer (incomingMidi, bufferToFill.startSample,
                                             bufferToFill.numSamples, true);

    // Debug output
    for (const auto metadata : incomingMidi)
    {
        const auto message = metadata.getMessage();
        juce::String messageString;
        messageString << "Time: " << metadata.samplePosition << " Message: " << message.getDescription();
        DBG(messageString);
    }

    synth.renderNextBlock (*bufferToFill.buffer, incomingMidi,
                           bufferToFill.startSample, bufferToFill.numSamples);
    
    float volume = 0.3;  // Set volume factor between 0 and 1.
    
    for (int channel = 0; channel < bufferToFill.buffer->getNumChannels(); ++channel)
    {
        auto* channelData = bufferToFill.buffer->getWritePointer(channel, bufferToFill.startSample);

        for (int sample = 0; sample < bufferToFill.numSamples; ++sample)
        {
            channelData[sample] *= volume;
        }
    }
    
}



//==============================================================================
MainComponent::MainComponent()
    : keyboardComponent(keyboardState, juce::MidiKeyboardComponent::horizontalKeyboard),
    synthAudioSource(keyboardState, juce::MidiKeyboardComponent::horizontalKeyboard)
{
    // Make sure you set the size of the component after
    // you add any child components.
    setSize (800, 200);

    // Some platforms require permissions to open input channels so request that here
    if (juce::RuntimePermissions::isRequired (juce::RuntimePermissions::recordAudio)
        && ! juce::RuntimePermissions::isGranted (juce::RuntimePermissions::recordAudio))
    {
        juce::RuntimePermissions::request (juce::RuntimePermissions::recordAudio,
                                           [&] (bool granted) { setAudioChannels (granted ? 2 : 0, 2); });
    }
    else
    {
        // Specify the number of input and output channels that we want to open
        setAudioChannels (2, 2);
    }
    
    
    
    setAudioChannels (0, 2);
    
    // Setup volumeSlider properties
    volumeSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    volumeSlider.setRange(0.0, 1.0, 0.01);
    volumeSlider.setValue(0.1); // start at max volume
    volumeSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 50, 20);
    addAndMakeVisible(&volumeSlider); // Add the slider to the component

    // Setup volumeLabel properties
    //volumeLabel.setText("Volume", juce::dontSendNotification);
    gainLabel.setText("Volume: 0 dB", juce::dontSendNotification);
//    volumeLabel.setText("Volume: 0 dB", juce::dontSendNotification);
    addAndMakeVisible(&gainLabel); // Add the label to the component
    
    addAndMakeVisible(keyboardComponent);
    
    midiMessagesBox.setMultiLine(true);
    midiMessagesBox.setReturnKeyStartsNewLine(true);
    midiMessagesBox.setReadOnly(true);
    midiMessagesBox.setScrollbarsShown(true);
    midiMessagesBox.setCaretVisible(false);
    midiMessagesBox.setPopupMenuEnabled(true);
    midiMessagesBox.setColour(juce::TextEditor::backgroundColourId, juce::Colour::greyLevel(0.2f));
    midiMessagesBox.setColour(juce::TextEditor::outlineColourId, juce::Colour::greyLevel(0.8f));

    //addAndMakeVisible(midiMessagesBox);
    addAndMakeVisible (&midiMessagesBox);
    
    // In your MainComponent constructor add:
    addAndMakeVisible(midiInputList);
    midiInputList.setTextWhenNoChoicesAvailable ("No MIDI Inputs Enabled");
    
    addAndMakeVisible(volumeLabel);
    volumeLabel.setText("Volume: 0 dB", juce::dontSendNotification);

    auto midiInputs = juce::MidiInput::getAvailableDevices();
    juce::StringArray midiInputNames;

    for (auto input : midiInputs) {
        midiInputNames.add(input.name);
    }

    midiInputList.addItemList(midiInputNames, 1);
    midiInputList.onChange = [this] { setMidiInput (midiInputList.getSelectedItemIndex()); };

    for (auto input : midiInputs)
    {
        if (deviceManager.isMidiInputDeviceEnabled (input.identifier))
        {
            setMidiInput (midiInputs.indexOf (input));
            break;
        }
    }

    if (midiInputList.getSelectedId() == 0)
        setMidiInput (0);
    
    
    phaseDelta = (float)(2.0 * juce::MathConstants<double>::pi * frequency / currentSampleRate);
    
    synthAudioSource.prepareToPlay(512, 44100);
    
    
    
    
}

MainComponent::~MainComponent()
{
    // This shuts down the audio device and clears the audio source.
    shutdownAudio();
}

//==============================================================================
void MainComponent::prepareToPlay (int samplesPerBlockExpected, double sampleRate)
{

    currentSampleRate = sampleRate;
    synth.setCurrentPlaybackSampleRate(sampleRate);
    synthAudioSource.prepareToPlay(samplesPerBlockExpected, sampleRate);
    
}

void MainComponent::getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill)
{
//    juce::MidiBuffer incomingMidi;
//    keyboardState.processNextMidiBuffer (incomingMidi, bufferToFill.startSample, bufferToFill.numSamples, true);
//
//    for (const auto metadata : incomingMidi)
//    {
//        auto message = metadata.getMessage();
//        handleIncomingMidiMessage(nullptr, message);
//    }
//
    //synth.renderNextBlock(*bufferToFill.buffer, incomingMidi, bufferToFill.startSample, bufferToFill.numSamples);
    synthAudioSource.getNextAudioBlock(bufferToFill);
    float maxGain = 0.0f;
    for (auto sample = 0; sample < bufferToFill.numSamples; ++sample)
    {
        const float gain = std::abs(bufferToFill.buffer->getSample(0, sample));
        maxGain = std::max(gain, maxGain);
    }

    // Convert gain to decibels
    float gainInDb = juce::Decibels::gainToDecibels(maxGain, -100.0f);

    // Update the volume label text on the message thread
    juce::MessageManager::callAsync([this, gainInDb] {
        gainLabel.setText("Volume: " + juce::String(gainInDb, 2) + " dB", juce::dontSendNotification);
    });
}


void MainComponent::releaseResources()
{
}

//==============================================================================
void MainComponent::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    // You can add your drawing code here!
}


void MainComponent::resized()
{
    auto area = getLocalBounds();
    
    // set the height for the top control area
    auto controlArea = area.removeFromTop(100);
    
    volumeSlider.setBounds (controlArea.removeFromLeft(150));
    midiInputList.setBounds (controlArea.removeFromLeft(200));
    gainLabel.setBounds (500, 10, getWidth() - 20, 20);
    
    // set the height for the midiMessagesBox
    auto messageArea = area.removeFromRight(200);
    midiMessagesBox.setBounds(messageArea);
    
    // rest of the area goes to keyboardComponent
    keyboardComponent.setBounds(area);
}

//void MainComponent::handleIncomingMidiMessage(juce::MidiInput* source, const juce::MidiMessage& message)
//{
//    juce::MessageManager::callAsync([this, message] {
//        midiMessagesBox.insertTextAtCaret(message.getDescription() + "\n");
//        // print note value
//        midiMessagesBox.insertTextAtCaret("Note number: " +  juce::String(message.getNoteNumber()) + "\n");
//        // print frequency in Hz
//        midiMessagesBox.insertTextAtCaret("Frequency: " + juce::String( message.getMidiNoteInHertz(message.getNoteNumber())) + "\n");
//    });
//
//
//}
