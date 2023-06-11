#include "MainComponent.h"
#include "SynthAudioSource.h"


#define VOICES 10


SynthAudioSource::SynthAudioSource(juce::MidiKeyboardState& keyState, juce::MidiKeyboardComponent::Orientation orientation)
    : keyboardState(keyState), keyboardOrientation(orientation)
{
    for (int i = 0; i < VOICES; ++i)
        synth.addVoice(new SynthVoice());

    synth.addSound(new SynthSound());
    adsrParams.attack = 0.1;
    adsrParams.decay = 0.2;
    adsrParams.sustain = 0.8;
    adsrParams.release = 0.3;

    adsr.setParameters(adsrParams);
}


void SynthAudioSource::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    synth.setCurrentPlaybackSampleRate(sampleRate);
    printf("\nsetting audio sample rate to %f\n", sampleRate);
    midiCollector.reset(sampleRate);
    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = samplesPerBlockExpected;
    spec.numChannels = 2; // You need to provide the number of channels

    filter.prepare(spec, sampleRate);
    
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
    
    // Process the block with your filter
    //juce::dsp::AudioBlock<float> block(bufferToFill.buffer->getArrayOfWritePointers(), bufferToFill.buffer->getNumChannels(), bufferToFill.numSamples);

        // Process the block with your filter
    juce::dsp::AudioBlock<float> block(bufferToFill.buffer->getArrayOfWritePointers(), bufferToFill.buffer->getNumChannels(), bufferToFill.numSamples);
    juce::dsp::ProcessContextReplacing<float> context(block);
    filter.process(context);
    // Set the cutoff frequency for each sample.
//    for (int sample = 0; sample < bufferToFill.numSamples; ++sample)
//    {
//        float envValue = adsr.getNextSample();
//        float cutoffFreq = getCutoffFreq() + 2000.0f * envValue;
//        float resonance = getResonance() * envValue;
//        // get cutoff frequency from slider
//        filter.setState(*juce::dsp::IIR::Coefficients<float>::makeLowPass(currentSampleRate, cutoffFreq, resonance));
//        juce::dsp::ProcessContextReplacing<float> context(block);
//        filter.process(context);
//    }
//    for (int sample = 0; sample < bufferToFill.numSamples; ++sample)
//    {
//        float envValue = adsr.getNextSample();
//        float cutoffFreq = filterCutoff.getValue() * envValue;
//
//        // Set the cutoff frequency for each sample.
//        filter.state = *juce::dsp::IIR::Coefficients<float>::makeLowPass(lastSampleRate, cutoffFreq);
//
//        juce::dsp::ProcessContextReplacing<float> context(block.getSubBlock(sample, 1));
//        filter.process(context);
//    }
//


    //filter.process(context);
   
    float volume = 1.0f / VOICES; // Adjust volume per number of active voices

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
    setSize (1000, 400);

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

    // init GUI
    initGUI();
    
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
    
//    addAndMakeVisible(volumeLabel);
//    volumeLabel.setText("Volume: 0 dB", juce::dontSendNotification);

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
    synthAudioSource.setCurrentSampleRate(44100);
    
    

    
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
    synthAudioSource.setCurrentSampleRate(sampleRate);
    
}

void MainComponent::getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill)
{
    synthAudioSource.getNextAudioBlock(bufferToFill);
    // int voices = getActiveVoiceCount();
    // divide output to the number of voices

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

void MainComponent::initGUI() {

   
    // setup osc1Group properties
    osc1Group.setText("Oscillator 1");
    osc1Group.setColour(juce::GroupComponent::outlineColourId, juce::Colours::white);
    osc1Group.setColour(juce::GroupComponent::textColourId, juce::Colours::white);
    addAndMakeVisible(osc1Group);

    // setup osc2Group properties
    osc2Group.setText("Oscillator 2");
    osc2Group.setColour(juce::GroupComponent::outlineColourId, juce::Colours::white);
    osc2Group.setColour(juce::GroupComponent::textColourId, juce::Colours::white);
    addAndMakeVisible(osc2Group);

    // setup oscLevel1 properties
    oscLevel1.setSliderStyle(juce::Slider::LinearHorizontal);
    oscLevel1.setRange(0.0, 1.0, 0.01);
    oscLevel1.setValue(0.5);
    oscLevel1.setTextBoxStyle(juce::Slider::TextBoxRight, false, 50, 20);
    osc1Group.addAndMakeVisible(oscLevel1);
    osc1Group.addAndMakeVisible(oscLevelLabel1);
    oscLevelLabel1.setText("Volume", juce::dontSendNotification);
    oscLevelLabel1.attachToComponent(&oscLevel1, true);
    // detune (pitch)
    oscPitch1.setSliderStyle(juce::Slider::LinearHorizontal);
    oscPitch1.setRange(-12.0, 12.0, 0.01);
    oscPitch1.setValue(0.0);
    oscPitch1.setTextBoxStyle(juce::Slider::TextBoxRight, false, 50, 20);
    osc1Group.addAndMakeVisible(oscPitch1);
    osc1Group.addAndMakeVisible(oscPitchLabel1);
    oscPitchLabel1.setText("Detune", juce::dontSendNotification);
    oscPitchLabel1.attachToComponent(&oscPitch1, true);
    // waveform
    osc1Group.addAndMakeVisible(waveformSelector1);
    waveformSelector1.addItem("Sine", 1);
    waveformSelector1.addItem("Sawtooth", 2);
    waveformSelector1.addItem("Square", 3);
    waveformSelector1.addItem("Triangle", 4);
    waveformSelector1.addItem("Pulse", 5);
    // pulse width
    osc1Group.addAndMakeVisible(pulseWidthSlider1);
    pulseWidthSlider1.setSliderStyle(juce::Slider::LinearHorizontal);
    pulseWidthSlider1.setRange(0.01, 0.99, 0.01);
    pulseWidthSlider1.setValue(0.3);
    pulseWidthSlider1.setTextBoxStyle(juce::Slider::TextBoxRight, false, 50, 20);
    pulseWidthLabel1.setText("Pulse Width", juce::dontSendNotification);
    pulseWidthLabel1.attachToComponent(&pulseWidthSlider1, true);
    osc1Group.addAndMakeVisible(pulseWidthLabel1);
    // add listeners
    oscLevel1.addListener(this);
    oscPitch1.addListener(this);
    waveformSelector1.addListener(this);
    pulseWidthSlider1.addListener(this);
    // set default waveform
    waveformSelector1.setSelectedId(2);

    

    // setup oscLevel2 properties
    oscLevel2.setSliderStyle(juce::Slider::LinearHorizontal);
    oscLevel2.setRange(0.0, 1.0, 0.01);
    oscLevel2.setValue(0.5);
    oscLevel2.setTextBoxStyle(juce::Slider::TextBoxRight, false, 50, 20);
    osc2Group.addAndMakeVisible(oscLevel2);
    osc2Group.addAndMakeVisible(oscLevelLabel2);
    oscLevelLabel2.setText("Volume", juce::dontSendNotification);
    oscLevelLabel2.attachToComponent(&oscLevel2, true);
    // detune (pitch)
    oscPitch2.setSliderStyle(juce::Slider::LinearHorizontal);
    oscPitch2.setRange(-12.0, 12.0, 0.01);
    oscPitch2.setValue(0.0);
    oscPitch2.setTextBoxStyle(juce::Slider::TextBoxRight, false, 50, 20);
    osc2Group.addAndMakeVisible(oscPitch2);
    osc2Group.addAndMakeVisible(oscPitchLabel2);
    oscPitchLabel2.setText("Detune", juce::dontSendNotification);
    oscPitchLabel2.attachToComponent(&oscPitch2, true);
    // waveform
    osc2Group.addAndMakeVisible(waveformSelector2);
    waveformSelector2.addItem("Sine", 1);
    waveformSelector2.addItem("Sawtooth", 2);
    waveformSelector2.addItem("Square", 3);
    waveformSelector2.addItem("Triangle", 4);
    waveformSelector2.addItem("Pulse", 5);
    // pulse width
    osc2Group.addAndMakeVisible(pulseWidthSlider2);
    pulseWidthSlider2.setSliderStyle(juce::Slider::LinearHorizontal);
    pulseWidthSlider2.setRange(0.01, 0.99, 0.01);
    pulseWidthSlider2.setValue(0.3);
    pulseWidthSlider2.setTextBoxStyle(juce::Slider::TextBoxRight, false, 50, 20);
    pulseWidthLabel2.setText("Pulse Width", juce::dontSendNotification);
    pulseWidthLabel2.attachToComponent(&pulseWidthSlider2, true);
    osc2Group.addAndMakeVisible(pulseWidthLabel2);
    // add listeners
    oscLevel2.addListener(this);
    oscPitch2.addListener(this);
    waveformSelector2.addListener(this);
    pulseWidthSlider2.addListener(this);
    // set default waveform
    waveformSelector2.setSelectedId(1);

    // setup filterGroup properties
    filterGroup.setColour(juce::GroupComponent::textColourId, juce::Colours::white);
    addAndMakeVisible(filterGroup);
    filterGroup.setText("Filter");
    // add listeners
    filterCutoff.addListener(this);
    filterResonance.addListener(this);
    // filter cutoff
    filterCutoff.setSliderStyle(juce::Slider::LinearHorizontal);
    filterCutoff.setRange(20.0, 20000.0, 0.01);
    filterCutoff.setValue(6000);
    filterCutoff.setTextBoxStyle(juce::Slider::TextBoxRight, false, 50, 20);
    filterGroup.addAndMakeVisible(filterCutoff);
    filterGroup.addAndMakeVisible(filterCutoffLabel);
    filterCutoffLabel.setText("Cutoff", juce::dontSendNotification);
    filterCutoffLabel.attachToComponent(&filterCutoff, true);
    // filter resonance
    filterResonance.setSliderStyle(juce::Slider::LinearHorizontal);
    filterResonance.setRange(1.0, 5.0, 0.01);
    filterResonance.setValue(3.0);
    filterResonance.setTextBoxStyle(juce::Slider::TextBoxRight, false, 50, 20);
    filterGroup.addAndMakeVisible(filterResonance);
    filterGroup.addAndMakeVisible(filterResonanceLabel);
    filterResonanceLabel.setText("Resonance", juce::dontSendNotification);
    filterResonanceLabel.attachToComponent(&filterResonance, true);


    // add keyboard component
    addAndMakeVisible(keyboardComponent);




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
    /*
    PRIVATE VARIABLES DECLARED:
    // MIDI Input
    juce::MidiKeyboardState keyboardState;
    juce::TextEditor midiMessagesBox; // log midi messages
    juce::ComboBox midiInputList; // midi input list dropdown
    
    // GUI Keyboard
    juce::MidiKeyboardComponent keyboardComponent;

    ////// OSC Controls
    // Groups
    juce::GroupComponent osc1Group;
    juce::GroupComponent osc2Group;

    // VOLUME
    juce::Slider oscLevel1;
    juce::Slider oscLevel2;
    juce::Label oscLevelLabel1;
    juce::Label oscLevelLabel2;

    // DETUNE
    juce::Slider oscPitch1;
    juce::Slider oscPitch2;
    juce::Label oscPitchLabel1;
    juce::Label oscPitchLabel2;

    // WAVEFORM
    juce::ComboBox waveformSelector1;
    juce::ComboBox waveformSelector2;

    // ADSR
    juce::Slider attackSlider;
    juce::Label attackLabel;
    juce::Slider decaySlider;
    juce::Label decayLabel;
    juce::Slider sustainSlider;
    juce::Label sustainLabel;
    juce::Slider releaseSlider;
    juce::Label releaseLabel;
*/

    auto area = getLocalBounds();
    // create groups for osc1 and osc2. 300 px high and 200 px wide
    osc1Group.setBounds(10, 10, 200, 160);
    // add osc1 controls to osc1Group
    oscLevel1.setBounds(40, 30, 150, 20);
    oscPitch1.setBounds(40, 60, 150, 20);
    oscLevelLabel1.setBounds(40, 10, 150, 20);
    oscPitchLabel1.setBounds(40, 40, 150, 20);
    waveformSelector1.setBounds(40, 90, 150, 20);

    // pulse width
    pulseWidthSlider1.setBounds(40, 120, 150, 20);
    pulseWidthLabel1.setBounds(40, 120, 150, 20);

    
    //osc1Group.setBounds (area.removeFromTop(300).removeFromLeft(200));
    osc2Group.setBounds(220, 10, 200, 160);
    // add osc2 controls to osc2Group
    oscLevel2.setBounds(40, 30, 150, 20);
    oscPitch2.setBounds(40, 60, 150, 20);
    oscLevelLabel2.setBounds(40, 10, 150, 20);
    oscPitchLabel2.setBounds(40, 40, 150, 20);
    waveformSelector2.setBounds(40, 90, 150, 20);

    // pulse width
    pulseWidthSlider2.setBounds(40, 120, 150, 20);
    pulseWidthLabel2.setBounds(40, 120, 150, 20);

    // FILTER GROUP
    filterGroup.setBounds(430, 10, 200, 160);
    // add filter controls to filterGroup
    //     juce::Slider filterCutoff;
    //     juce::Slider filterResonance;
    //     juce::Label filterCutoffLabel;
    //     juce::Label filterResonanceLabel;
    filterCutoff.setBounds(40, 30, 150, 20);
    filterResonance.setBounds(40, 60, 150, 20);
    filterCutoffLabel.setBounds(40, 10, 150, 20);
    filterResonanceLabel.setBounds(40, 40, 150, 20);

    



 
    
    // volumeSlider is 150 pixels wide and 20 pixels tall placed in the top left corner
//    volumeLabel.setBounds (10, 10, 100, 20);
//    // second volume slider is 100 pixels wide and 20 pixels tall placed next to the first volume slider
//    volumeSlider.setBounds (volumeLabel.getBounds().translated(100, 0));

    //volumeSlider.setBounds (controlArea.removeFromLeft(150));
//    waveformSelector1.setBounds (volumeSlider.getBounds().translated(0, volumeSlider.getHeight() + 10)); // Position below volumeSlider with a margin of 10
//    
//    midiInputList.setBounds (controlArea.removeFromLeft(200));
//    waveformSelector2.setBounds (midiInputList.getBounds().translated(0, midiInputList.getHeight() + 10)); // Position below midiInputList with a margin of 10
//
//    gainLabel.setBounds (500, 10, getWidth() - 20, 20);
    
    // set the height for the midiMessagesBox
    // set the height for the midiMessagesBox
    auto messageArea = area.removeFromRight(200);
    midiMessagesBox.setBounds(messageArea);
    
    // put keyboard to bottom of screen
    keyboardComponent.setBounds(10, 300, getWidth() - 20, getHeight() - 310);
}


void MainComponent::sliderValueChanged(juce::Slider* slider) {
    //std::cout << "Slider Value Changed" << std::endl;
    //std::cout << slider->getValue() << std::endl;
    
    // OSC 1 Volume Slider
    if (slider == &oscLevel1) {
        std::cout << "OSC 1 Volume Slider" << std::endl;
        std::cout << slider->getValue() << std::endl;
        synthAudioSource.setOscGain(1, slider->getValue());
    }
    // OSC 1 Detune Slider
    if (slider == &oscPitch1) {
        std::cout << "OSC 1 Detune Slider" << std::endl;
        std::cout << slider->getValue() << std::endl;
        synthAudioSource.setDetune(1, slider->getValue());
    }
    // OSC 1 Pulse Width Slider
    if (slider == &pulseWidthSlider1) {
        std::cout << "OSC 1 Pulse Width Slider" << std::endl;
        std::cout << slider->getValue() << std::endl;
        synthAudioSource.setPulseWidth(1, slider->getValue());
    }
    // OSC 2 Volume Slider
    if (slider == &oscLevel2) {
        std::cout << "OSC 2 Volume Slider" << std::endl;
        std::cout << slider->getValue() << std::endl;
        synthAudioSource.setOscGain(2, slider->getValue());
    }
    // OSC 2 Detune Slider
    if (slider == &oscPitch2) {
        std::cout << "OSC 2 Detune Slider" << std::endl;
        std::cout << slider->getValue() << std::endl;
        synthAudioSource.setDetune(2, slider->getValue());
    }
    // OSC 2 Pulse Width Slider
    if (slider == &pulseWidthSlider2) {
        std::cout << "OSC 2 Pulse Width Slider" << std::endl;
        std::cout << slider->getValue() << std::endl;
        synthAudioSource.setPulseWidth(2, slider->getValue());
    }
    // Filter Cutoff Slider
    if (slider == &filterCutoff) {
        std::cout << "Filter Cutoff Slider" << std::endl;
        std::cout << slider->getValue() << std::endl;
        synthAudioSource.setCutoffFreq(slider->getValue());
    }
    // Filter Resonance Slider
    if (slider == &filterResonance) {
        std::cout << "Filter Resonance Slider" << std::endl;
        std::cout << slider->getValue() << std::endl;
        synthAudioSource.setResonance(slider->getValue());
    }
    
}

void MainComponent::comboBoxChanged(juce::ComboBox* comboBox) {
    // OSC 1 Waveform Selector
    if (comboBox == &waveformSelector1) {
        std::cout << "OSC 1 Waveform Selector" << std::endl;
        std::cout << comboBox->getSelectedItemIndex() << std::endl;
        synthAudioSource.setOscType(1, comboBox->getSelectedItemIndex());
        std::cout << "New OSC 1 Waveform: " << synthAudioSource.getOscType(1) << std::endl;
    }
    if (comboBox == &waveformSelector2) {
        std::cout << "OSC 2 Waveform Selector" << std::endl;
        std::cout << comboBox->getSelectedItemIndex() << std::endl;
        synthAudioSource.setOscType(2, comboBox->getSelectedItemIndex());
    }
}
