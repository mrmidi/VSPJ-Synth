/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
MidiusAudioProcessor::MidiusAudioProcessor(juce::MidiKeyboardState& state)
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       ),
                       keyboardState(state), // Initialize keyboardState
                       synthSource(keyboardState) // Initialize synthSource
#endif
{
    
    
    
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
    
}

void MidiusAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool MidiusAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
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
    
    for (const auto metadata : midiMessages)
    {
        const auto message = metadata.getMessage();
        
        DBG("MIDI message received: "
            << " Note: " << message.getNoteNumber()
            << " Velocity: " << message.getVelocity()
            << " Timestamp: " << message.getTimeStamp());
        
//        synthSource.addMidiMessage(message);
    }

    
    //synthSource.synth.renderNextBlock(buffer, midiMessages, 0, buffer.getNumSamples());
    // Render the next block of data from the synth.
    juce::AudioSourceChannelInfo info (buffer);
    juce::dsp::AudioBlock<float> block (buffer);
    
    // keyboardState.processNextMidiBuffer(midiMessages, info.startSample,
    //                                             info.numSamples, true);
    synthSource.synth.renderNextBlock(buffer, midiMessages, 0, buffer.getNumSamples());
    
    masterGain.process (juce::dsp::ProcessContextReplacing<float> (block));
    
        
    
}

//==============================================================================
bool MidiusAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* MidiusAudioProcessor::createEditor()
{
    return new MidiusAudioProcessorEditor (*this);
}

//==============================================================================
void MidiusAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void MidiusAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    static juce::MidiKeyboardState keyboardState;
    return new MidiusAudioProcessor(keyboardState);
}
