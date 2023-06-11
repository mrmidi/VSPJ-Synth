#pragma once

#include <JuceHeader.h>
#include "SynthSound.h"
#include "SynthVoice.h"
#include "SynthAudioSource.h"


//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/
class MainComponent  : public juce::Slider::Listener,
                       public juce::ComboBox::Listener,
                       public juce::AudioAppComponent
//                       private juce::MidiInputCallback
{
public:
    //==============================================================================
    MainComponent();
    ~MainComponent() override;

    //==============================================================================
    void prepareToPlay (int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill) override;
    void releaseResources() override;

    //==============================================================================
    void paint (juce::Graphics& g) override;
    void resized() override;

    void initGUI();
    
    void sliderValueChanged(juce::Slider* slider) override;
    void comboBoxChanged(juce::ComboBox* comboBox) override;

    //int getNumVoices() const noexcept { return synth.getNumVoices(); }

    int getActiveVoiceCount() {
        int count = 0;
        for (int i = 0; i < synth.getNumVoices(); i++) {
            if (synth.getVoice(i)->isVoiceActive()) {
                count++;
            }
        }
        if (count == 0) {
            return 1;
        }
        return count;
    } 
    
    // handle midi messages
//    void handleIncomingMidiMessage(juce::MidiInput* source, const juce::MidiMessage& message) override;
    
    void setMidiInput (int index)
    {
        auto list = juce::MidiInput::getAvailableDevices();

        deviceManager.removeMidiInputDeviceCallback (list[lastInputIndex].identifier,
                                                     synthAudioSource.getMidiCollector()); // [12]

        auto newInput = list[index];

        if (! deviceManager.isMidiInputDeviceEnabled (newInput.identifier))
            deviceManager.setMidiInputDeviceEnabled (newInput.identifier, true);

        deviceManager.addMidiInputDeviceCallback (newInput.identifier, synthAudioSource.getMidiCollector()); // [13]
        midiInputList.setSelectedId (index + 1, juce::dontSendNotification);

        lastInputIndex = index;
    }

private:
    //==============================================================================
    // Your private member variables go here...
    juce::Synthesiser synth;
    
    // juce::Slider volumeSlider;
    // juce::Label volumeLabel;
    juce::Label gainLabel;

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

    // pulse width
    juce::Slider pulseWidthSlider1;
    juce::Label pulseWidthLabel1;
    juce::Slider pulseWidthSlider2;
    juce::Label pulseWidthLabel2;
    

    // ADSR
    juce::Slider attackSlider;
    juce::Label attackLabel;
    juce::Slider decaySlider;
    juce::Label decayLabel;
    juce::Slider sustainSlider;
    juce::Label sustainLabel;
    juce::Slider releaseSlider;
    juce::Label releaseLabel;

/************ NOT IMPLEMENTED YET 
    // FILTER
    juce::Slider filterCutoff;
    juce::Slider filterResonance;
    juce::ComboBox filterType;

    // LFO
    juce::Slider lfoRate;
    juce::Slider lfoDepth;
    juce::ComboBox lfoWaveform;

    // DELAY
    juce::Slider delayTime;
    juce::Slider delayFeedback;
    juce::Slider delayWetDry;

    // REVERB
    juce::Slider reverbSize;
    juce::Slider reverbDamping;
    juce::Slider reverbWetDry;

    // CHORUS
    juce::Slider chorusRate;
    juce::Slider chorusDepth;
    juce::Slider chorusCentreDelay;
    juce::Slider chorusFeedback;
    juce::Slider chorusWetDry;

    // DISTORTION
    juce::Slider distortionDrive;
    juce::Slider distortionRange;
    juce::Slider distortionBlend;
    juce::Slider distortionWetDry;

    // PHASER
    juce::Slider phaserRate;
    juce::Slider phaserDepth;
    juce::Slider phaserCentreDelay;
    juce::Slider phaserFeedback;
    juce::Slider phaserWetDry;

    // FLANGER
    juce::Slider flangerRate;
    juce::Slider flangerDepth;
    juce::Slider flangerCentreDelay;
    juce::Slider flangerFeedback;
    juce::Slider flangerWetDry;

    // EQ
    juce::Slider eqLowGain;
    juce::Slider eqMidGain;
    juce::Slider eqHighGain;

    // MASTER
    juce::Slider masterVolume;
    juce::Slider masterPan;
    
**********************************/
    

    

    

    
    SynthAudioSource synthAudioSource;
    
//    juce::Label volumeLabel;
    
    float amplitude = 1.0f;

    // Frequency of the sine wave in Hz
    float frequency = 440.0f;

    // Current phase of the sine wave
    float phase = 0.0f;

    // The change in phase for each sample
    float phaseDelta = 0.0f;
    
    double currentSampleRate = 0.0;
    
    int lastInputIndex = 0;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)

};
