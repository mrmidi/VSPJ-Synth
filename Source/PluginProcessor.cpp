/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
//MidiusAudioProcessor::MidiusAudioProcessor(juce::MidiKeyboardState& state)
//#ifndef JucePlugin_PreferredChannelConfigurations
//     : AudioProcessor (BusesProperties()
//                     #if ! JucePlugin_IsMidiEffect
//                      #if ! JucePlugin_IsSynth
//                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
//                      #endif
//                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
//                     #endif
//                       ),
//                       keyboardState(state), // Initialize keyboardState
//                       synthSource(keyboardState), // Initialize synthSource
//                       parameters (*this, nullptr, "Parameters", createParameterLayout())
//#endif
//{
//
//    FOLEYS_SET_SOURCE_PATH(__FILE__);
//
//}
//
// foleys::MagicProcessor (juce::AudioProcessor::BusesProperties()
//                            .withOutput ("Output", juce::AudioChannelSet::stereo(), true)),
//    treeState (*this, nullptr, ProjectInfo::projectName, createParameterLayout())
//
MidiusAudioProcessor::MidiusAudioProcessor(juce::MidiKeyboardState& state)
     : foleys::MagicProcessor (juce::AudioProcessor::BusesProperties()
                                .withOutput ("Output", juce::AudioChannelSet::stereo(), true)),
     keyboardState(state), // Initialize keyboardState
       synthSource(keyboardState), // Initialize synthSource
       parameters (*this, nullptr, "Parameters", createParameterLayout())
{
    
    FOLEYS_SET_SOURCE_PATH(__FILE__);
    
    DBG("Searching for midius.xml");
    magicState.setGuiValueTree (BinaryData::midius_xml, BinaryData::midius_xmlSize);
    outputMeter = magicState.createAndAddObject<foleys::MagicLevelSource>("output");
    oscilloscope = magicState.createAndAddObject<foleys::MagicOscilloscope>("waveform");
    analyser     = magicState.createAndAddObject<foleys::MagicAnalyser>("analyser");
    magicState.addBackgroundProcessing (analyser);

    magicState.setApplicationSettingsFile (juce::File::getSpecialLocation (juce::File::userApplicationDataDirectory)
                                           .getChildFile (ProjectInfo::companyName)
                                           .getChildFile (ProjectInfo::projectName + juce::String (".settings")));

    magicState.setPlayheadUpdateFrequency (30);

    
}


MidiusAudioProcessor::~MidiusAudioProcessor()
{
}

//==============================================================================
const juce::String MidiusAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool MidiusAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool MidiusAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool MidiusAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double MidiusAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int MidiusAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int MidiusAudioProcessor::getCurrentProgram()
{
    return 0;
}

void MidiusAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String MidiusAudioProcessor::getProgramName (int index)
{
    return {};
}

void MidiusAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void MidiusAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    
    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = samplesPerBlock;
    spec.numChannels = getTotalNumOutputChannels();
    
    masterGain.reset();
    masterGain.prepare(spec);
    masterGain.setGainDecibels(-24.0f);
    
    
    synthSource.synth.setCurrentPlaybackSampleRate(sampleRate);
    
    for (int i = 0; i < synthSource.synth.getNumVoices(); i++) {
        if (auto voice = dynamic_cast<SynthVoice*>(synthSource.synth.getVoice(i)))
            voice->prepareToPlay(samplesPerBlock, sampleRate, getTotalNumOutputChannels());
    }
    std::cout << "MidiusAudioProcessor::prepareToPlay called with sample rate " << sampleRate
                  << " and samples per block " << samplesPerBlock << "\n";
    synthSource.prepareToPlay(samplesPerBlock, sampleRate);

    // magic meters
    outputMeter->setupSource (getTotalNumOutputChannels(), sampleRate, 500);
    oscilloscope->prepareToPlay (sampleRate, samplesPerBlock);
    analyser->prepareToPlay (sampleRate, samplesPerBlock);


    
}

void MidiusAudioProcessor::releaseResources()
{

}

#ifndef JucePlugin_PreferredChannelConfigurations
bool MidiusAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void MidiusAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
     juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // Clear the output buffers.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

//     for (const auto metadata : midiMessages)
//     {
//         const auto message = metadata.getMessage();
        
//         // DBG("MIDI message received: "
//         //     << " Note: " << message.getNoteNumber()
//         //     << " Velocity: " << message.getVelocity()
//         //     << " Timestamp: " << message.getTimeStamp());
        
// //        synthSource.addMidiMessage(message);
//     }

    
    //synthSource.synth.renderNextBlock(buffer, midiMessages, 0, buffer.getNumSamples());
    // Render the next block of data from the synth.
    juce::AudioSourceChannelInfo info (buffer);
    juce::dsp::AudioBlock<float> block (buffer);
    
    // keyboardState.processNextMidiBuffer(midiMessages, info.startSample,
    //                                             info.numSamples, true);

    setVoiceParams();
    synthSource.synth.renderNextBlock(buffer, midiMessages, 0, buffer.getNumSamples());
    
    masterGain.process (juce::dsp::ProcessContextReplacing<float> (block));

    for (int i = 1; i < buffer.getNumChannels(); ++i)
        buffer.copyFrom (i, 0, buffer.getReadPointer (0), buffer.getNumSamples());

    // MAGIC GUI: send the finished buffer to the level meter
    
    // Amplify buffer for oscilloscope
    juce::AudioBuffer<float> oscBuffer = buffer; // Create a copy of the buffer for amplification
    float gainFactor = 15.0; // or any other desired gain value

    for (int channel = 0; channel < oscBuffer.getNumChannels(); ++channel)
    {
        float* channelData = oscBuffer.getWritePointer(channel);
        for (int sample = 0; sample < oscBuffer.getNumSamples(); ++sample)
        {
            channelData[sample] *= gainFactor;
        }
    }

    outputMeter->pushSamples (buffer);
    oscilloscope->pushSamples (oscBuffer);
    analyser->pushSamples (buffer);
    
        
    
}

//==============================================================================
//bool MidiusAudioProcessor::hasEditor() const
//{
//    return true; // (change this to false if you choose to not supply an editor)
//}

//juce::AudioProcessorEditor* MidiusAudioProcessor::createEditor()
//{
//    return new MidiusAudioProcessorEditor (*this);
//}

// juce::AudioProcessorEditor* MidiusAudioProcessor::createEditor()
// {
//    return new MidiusAudioProcessorEditor (*this);
// }


//==============================================================================
//void MidiusAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
//{
//    // You should use this method to store your parameters in the memory block.
//    // You could do that either as raw data, or use the XML or ValueTree classes
//    // as intermediaries to make it easy to save and load complex data.
//}

//void MidiusAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
//{
//    // You should use this method to restore your parameters from this memory block,
//    // whose contents will have been created by the getStateInformation() call.
//}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    static juce::MidiKeyboardState keyboardState;
    return new MidiusAudioProcessor(keyboardState);
}

void MidiusAudioProcessor::setVoiceParams()
{
    for (int i = 0; i < synthSource.synth.getNumVoices(); ++i)

    {
        if (auto voice = dynamic_cast<SynthVoice*>(synthSource.synth.getVoice(i)))
        {
            // Oscillator 1
            auto& osc1Octave = *parameters.getRawParameterValue ("osc1Octave");
            auto& osc1Cent = *parameters.getRawParameterValue ("osc1Cent");
            auto& osc1Gain = *parameters.getRawParameterValue ("osc1Gain");
            auto& osc1PulseWidth = *parameters.getRawParameterValue ("osc1PulseWidth");
            auto& osc1WaveformType = *parameters.getRawParameterValue ("osc1WaveformType");
            // Oscillator 2
            auto& osc2Octave = *parameters.getRawParameterValue ("osc2Octave");
            auto& osc2Cent = *parameters.getRawParameterValue ("osc2Cent");
            auto& osc2Gain = *parameters.getRawParameterValue ("osc2Gain");
            auto& osc2PulseWidth = *parameters.getRawParameterValue ("osc2PulseWidth");
            auto& osc2WaveformType = *parameters.getRawParameterValue ("osc2WaveformType");
            // LFO
            //auto& enabledToggle = *parameters.getRawParameterValue ("enabledToggle");
            auto& sourceComboBox = *parameters.getRawParameterValue ("sourceComboBox");
            auto& depthSlider = *parameters.getRawParameterValue ("depthSlider");
            auto& rateSlider = *parameters.getRawParameterValue ("rateSlider");
            auto& typeComboBox = *parameters.getRawParameterValue ("typeComboBox");
            // ADSR voice
            auto& adsrAttack = *parameters.getRawParameterValue ("adsr1attack");
            auto& adsrDecay = *parameters.getRawParameterValue ("adsr1decay");
            auto& adsrSustain = *parameters.getRawParameterValue ("adsr1sustain");
            auto& adsrRelease = *parameters.getRawParameterValue ("adsr1release");
            // ADSR filter
            auto& adsr2AttackFilter = *parameters.getRawParameterValue ("adsr2attack");
            auto& adsr2DecayFilter = *parameters.getRawParameterValue ("adsr2decay");
            auto& adsr2SustainFilter = *parameters.getRawParameterValue ("adsr2sustain");
            auto& adsr2ReleaseFilter = *parameters.getRawParameterValue ("adsr2release");
            // Filter
            auto& filterType = *parameters.getRawParameterValue ("filterType");
            auto& filterCutoff = *parameters.getRawParameterValue ("cutoff");
            auto& filterResonance = *parameters.getRawParameterValue ("resonance");
            auto& filterAmount = *parameters.getRawParameterValue ("filterAmount");
                      

            //DBG("Retrieved waveform type: " << osc1WaveformType.load());
//            DBG("Waveform type: " << osc1WaveformType.load());
            voice->setOsc1Params(osc1Octave.load(), osc1Cent.load(), osc1Gain.load(), osc1PulseWidth.load(), osc1WaveformType.load());
            voice->setOsc2Params(osc2Octave.load(), osc2Cent.load(), osc2Gain.load(), osc2PulseWidth.load(), osc2WaveformType.load());
            voice->setLFOParams(depthSlider.load(), rateSlider.load(), sourceComboBox.load(), typeComboBox.load());
            voice->setADSRParams(adsrAttack.load(), adsrDecay.load(), adsrSustain.load(), adsrRelease.load());
            voice->setFilterAdsrParams(adsr2AttackFilter.load(), adsr2DecayFilter.load(), adsr2SustainFilter.load(), adsr2ReleaseFilter.load(), filterCutoff.load()); // baseCutoffFreq
            voice->setFilterParams(filterType.load(), filterCutoff.load(), filterResonance.load(), filterAmount.load());
            
            
            //adsr.update (osc 1 octave.load(), osc 1 cent.load(), osc 1 gain.load(), osc 1 pulse width.load());
        }
    }
}

/* 
LFO Control Group

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
