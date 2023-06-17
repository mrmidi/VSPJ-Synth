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
    // init specs
    spec.sampleRate = 44100.0;
    spec.maximumBlockSize = 512;
    spec.numChannels = 2;
    


    // Make sure you set the size of the component after
    // you add any child components.
    setSize (1000, 600);

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

    // set ADSR group properties
    adsrGroup1.setColour(juce::GroupComponent::textColourId, juce::Colours::white);
    addAndMakeVisible(adsrGroup1);
    adsrGroup1.setText("ADSR");
    // add listeners
    attackSlider1.addListener(this);
    decaySlider1.addListener(this);
    sustainSlider1.addListener(this);
    releaseSlider1.addListener(this);
    // attack
    adsrGroup1.addAndMakeVisible(attackSlider1);
    attackSlider1.setSliderStyle(juce::Slider::LinearHorizontal);
    attackSlider1.setRange(0.01, 100.0); // set slider range from 0.1 to 100
    attackSlider1.setSkewFactorFromMidPoint(10.0); // mid-point at 10
    attackSlider1.setValue(0.01); // set default value to 1
    attackSlider1.setTextBoxStyle(juce::Slider::TextBoxRight, false, 50, 20);
    adsrGroup1.addAndMakeVisible(attackLabel1);
    // decay
    adsrGroup1.addAndMakeVisible(decaySlider1);
    decaySlider1.setSliderStyle(juce::Slider::LinearHorizontal);
    decaySlider1.setRange(0.1, 10.0, 0.1);
    decaySlider1.setValue(0.1);
    decaySlider1.setTextBoxStyle(juce::Slider::TextBoxRight, false, 50, 20);
    adsrGroup1.addAndMakeVisible(decayLabel1);
    // sustain
    adsrGroup1.addAndMakeVisible(sustainSlider1);
    sustainSlider1.setSliderStyle(juce::Slider::LinearHorizontal);
    sustainSlider1.setRange(0.0, 1.0, 0.01);
    sustainSlider1.setValue(1.0);
    sustainSlider1.setTextBoxStyle(juce::Slider::TextBoxRight, false, 50, 20);
    adsrGroup1.addAndMakeVisible(sustainLabel1);
    // release
    adsrGroup1.addAndMakeVisible(releaseSlider1);
    releaseSlider1.setSliderStyle(juce::Slider::LinearHorizontal);
    releaseSlider1.setRange(0.1, 10.0, 0.1);
    releaseSlider1.setValue(0.1);
    releaseSlider1.setTextBoxStyle(juce::Slider::TextBoxRight, false, 50, 20);
    adsrGroup1.addAndMakeVisible(releaseLabel1);
    // set labels
    attackLabel1.setText("Attack", juce::dontSendNotification);
    attackLabel1.attachToComponent(&attackSlider1, true);
    decayLabel1.setText("Decay", juce::dontSendNotification);
    decayLabel1.attachToComponent(&decaySlider1, true);
    sustainLabel1.setText("Sustain", juce::dontSendNotification);
    sustainLabel1.attachToComponent(&sustainSlider1, true);
    releaseLabel1.setText("Release", juce::dontSendNotification);
    releaseLabel1.attachToComponent(&releaseSlider1, true);


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
    // filter type
    filterType.addItem("Low Pass", 1);
    filterType.addItem("High Pass", 2);
    filterType.addItem("Band Pass", 3);
    filterType.addListener(this);
    filterType.setSelectedId(1);
    filterGroup.addAndMakeVisible(filterType);
    // env amount
    filterEnvAmount.setSliderStyle(juce::Slider::LinearHorizontal);
    filterEnvAmount.setRange(0, 10, 1);
    filterEnvAmount.setValue(0);
    filterEnvAmount.setTextBoxStyle(juce::Slider::TextBoxRight, false, 50, 20);
    filterGroup.addAndMakeVisible(filterEnvAmount);
    filterGroup.addAndMakeVisible(filterEnvAmountLabel);
    filterEnvAmountLabel.setText("Env Amt", juce::dontSendNotification);
    filterEnvAmountLabel.attachToComponent(&filterEnvAmount, true);
    // add listeners
    filterEnvAmount.addListener(this);
    
    



    // setup filterAdsr properties
    filterAdsrGroup.setColour(juce::GroupComponent::textColourId, juce::Colours::white);
    addAndMakeVisible(filterAdsrGroup);
    filterAdsrGroup.setText("Filter ADSR");
    // add listeners
    filterAttack.addListener(this);
    filterDecay.addListener(this);
    filterSustain.addListener(this);
    filterRelease.addListener(this);
    // filter attack
    filterAttack.setSliderStyle(juce::Slider::LinearHorizontal);
    filterAttack.setRange(0.0, 1, 0.01);
    filterAttack.setValue(0.0);
    filterAttack.setTextBoxStyle(juce::Slider::TextBoxRight, false, 50, 20);
    filterAdsrGroup.addAndMakeVisible(filterAttack);
    filterAdsrGroup.addAndMakeVisible(filterAttackLabel);
    filterAttackLabel.setText("Attack", juce::dontSendNotification);
    filterAttackLabel.attachToComponent(&filterAttack, true);
    // filter decay
    filterDecay.setSliderStyle(juce::Slider::LinearHorizontal);
    filterDecay.setRange(0.0, 1, 0.01);
    filterDecay.setValue(0.0);
    filterDecay.setTextBoxStyle(juce::Slider::TextBoxRight, false, 50, 20);
    filterAdsrGroup.addAndMakeVisible(filterDecay);
    filterAdsrGroup.addAndMakeVisible(filterDecayLabel);
    filterDecayLabel.setText("Decay", juce::dontSendNotification);
    filterDecayLabel.attachToComponent(&filterDecay, true);
    // filter sustain
    filterSustain.setSliderStyle(juce::Slider::LinearHorizontal);   
    filterSustain.setRange(0.0, 1.0, 0.01);
    filterSustain.setValue(1.0);
    filterSustain.setTextBoxStyle(juce::Slider::TextBoxRight, false, 50, 20);
    filterAdsrGroup.addAndMakeVisible(filterSustain);
    filterAdsrGroup.addAndMakeVisible(filterSustainLabel);
    filterSustainLabel.setText("Sustain", juce::dontSendNotification);
    filterSustainLabel.attachToComponent(&filterSustain, true);
    // filter release
    filterRelease.setSliderStyle(juce::Slider::LinearHorizontal);
    filterRelease.setRange(0.0, 1.0, 0.01);
    filterRelease.setValue(0.0);
    filterRelease.setTextBoxStyle(juce::Slider::TextBoxRight, false, 50, 20);
    filterAdsrGroup.addAndMakeVisible(filterRelease);
    filterAdsrGroup.addAndMakeVisible(filterReleaseLabel);
    filterReleaseLabel.setText("Release", juce::dontSendNotification);
    filterReleaseLabel.attachToComponent(&filterRelease, true);
    


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
    juce::GroupComponent filterGroup;

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

    // pulse width
    juce::Slider pulseWidthSlider1;
    juce::Label pulseWidthLabel1;
    juce::Slider pulseWidthSlider2;
    juce::Label pulseWidthLabel2;


    // ADSR osc1
    juce::Slider attackSlider1;
    juce::Label attackLabel1;
    juce::Slider decaySlider1;
    juce::Label decayLabel1;
    juce::Slider sustainSlider1;
    juce::Label sustainLabel1;
    juce::Slider releaseSlider1;
    juce::Label releaseLabel1;

    // ADSR osc2
    juce::Slider attackSlider1;
    juce::Label attackLabel1;
    juce::Slider decaySlider1;
    juce::Label decayLabel1;
    juce::Slider sustainSlider1;
    juce::Label sustainLabel1;
    juce::Slider releaseSlider1;
    juce::Label releaseLabel1;

    // FILTER
    juce::Slider filterCutoff;
    juce::Slider filterResonance;
    juce::Label filterCutoffLabel;
    juce::Label filterResonanceLabel;

    // ADSR filter
    juce::Slider filterAttack;
    juce::Label filterAttackLabel;
    juce::Slider filterDecay;
    juce::Label filterDecayLabel;
    juce::Slider filterSustain;
    juce::Label filterSustainLabel;
    juce::Slider filterRelease; */

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

    // ADSR GROUP
    adsrGroup1.setBounds(220, 180, 200, 160);
    // add adsr controls to adsrGroup
    attackSlider1.setBounds(40, 30, 150, 20);
    attackLabel1.setBounds(40, 10, 150, 20);
    decaySlider1.setBounds(40, 60, 150, 20);
    decayLabel1.setBounds(40, 40, 150, 20);
    sustainSlider1.setBounds(40, 90, 150, 20);
    sustainLabel1.setBounds(40, 70, 150, 20);
    releaseSlider1.setBounds(40, 120, 150, 20);
    releaseLabel1.setBounds(40, 100, 150, 20);

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
    filterType.setBounds(40, 70, 150, 20);

    //    juce::Slider filterEnvAmount;
    // juce::Label filterEnvAmountLabel;
    filterEnvAmount.setBounds(40, 90, 150, 20);
    filterEnvAmountLabel.setBounds(40, 90, 150, 20);
    //    juce::ComboBox filterType;

    

    // FILTER ADSR GROUP (put below filter group)
    filterAdsrGroup.setBounds(430, 180, 200, 160);
    // add filter adsr controls to filterGroup
    filterAttack.setBounds(40, 30, 150, 20);
    filterDecay.setBounds(40, 60, 150, 20);
    filterSustain.setBounds(40, 90, 150, 20);
    filterRelease.setBounds(40, 120, 150, 20);
    filterAttackLabel.setBounds(40, 10, 150, 20);
    filterDecayLabel.setBounds(40, 40, 150, 20);
    filterSustainLabel.setBounds(40, 70, 150, 20);
    filterReleaseLabel.setBounds(40, 100, 150, 20);


    



 
    
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
    keyboardComponent.setBounds(10, 500, getWidth() - 20, getHeight() - 510);
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
    // Attack Slider
    if (slider == &attackSlider1) {
        std::cout << "Attack Slider" << std::endl;
        std::cout << slider->getValue() << std::endl;
        synthAudioSource.setAttack(slider->getValue());
    }
    // Decay Slider
    if (slider == &decaySlider1) {
        std::cout << "Decay Slider" << std::endl;
        std::cout << slider->getValue() << std::endl;
        synthAudioSource.setDecay(slider->getValue());
    }
    // Sustain Slider
    if (slider == &sustainSlider1) {
        std::cout << "Sustain Slider" << std::endl;
        std::cout << slider->getValue() << std::endl;
        synthAudioSource.setSustain(slider->getValue());
    }
    // Release Slider
    if (slider == &releaseSlider1) {
        std::cout << "Release Slider" << std::endl;
        std::cout << slider->getValue() << std::endl;
        synthAudioSource.setRelease(slider->getValue());
    }



   // Filter Cutoff Slider
   if (slider == &filterCutoff) {
       std::cout << "Filter Cutoff Slider" << std::endl;
       std::cout << slider->getValue() << std::endl;
       synthAudioSource.setFilterCutoff(slider->getValue());
   }
   // Filter Resonance Slider
   if (slider == &filterResonance) {
       std::cout << "Filter Resonance Slider" << std::endl;
       std::cout << slider->getValue() << std::endl;
       synthAudioSource.setFilterResonance(slider->getValue());
   }

   // Filter Attack Slider
    if (slider == &filterAttack) {
         std::cout << "Filter Attack Slider" << std::endl;
         std::cout << slider->getValue() << std::endl;
         synthAudioSource.setFilterAttack(slider->getValue());
    }
    // Filter Decay Slider
    if (slider == &filterDecay) {
         std::cout << "Filter Decay Slider" << std::endl;
         std::cout << slider->getValue() << std::endl;
         //synthAudioSource.setFilterDecay(slider->getValue());
    }
    // Filter Sustain Slider
    if (slider == &filterSustain) {
         std::cout << "Filter Sustain Slider" << std::endl;
         std::cout << slider->getValue() << std::endl;
         //synthAudioSource.setFilterSustain(slider->getValue());
    }
    // Filter Release Slider
    if (slider == &filterRelease) {
         std::cout << "Filter Release Slider" << std::endl;
         std::cout << slider->getValue() << std::endl;
         //synthAudioSource.setFilterRelease(slider->getValue());
    }
    // Filter Amount Slider
    if (slider == &filterEnvAmount) {
         std::cout << "Filter Amount Slider" << std::endl;
         std::cout << slider->getValue() << std::endl;
         synthAudioSource.setDepth(slider->getValue());
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
