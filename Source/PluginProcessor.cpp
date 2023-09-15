/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"

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

    delayEffect.prepare(spec);

    // magic meters
    outputMeter->setupSource (getTotalNumOutputChannels(), sampleRate, 500);
    oscilloscope->prepareToPlay (sampleRate, samplesPerBlock);
    analyser->prepareToPlay (sampleRate, samplesPerBlock);

    hpf.prepareToPlay(sampleRate, samplesPerBlock, getTotalNumOutputChannels());


    
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

    

    // Render the next block of data from the synth.
    juce::AudioSourceChannelInfo info (buffer);
    juce::dsp::AudioBlock<float> block (buffer);
    
    // Iterate over all MIDI messages
    for (const auto midiMetaData : midiMessages)
    {
        const auto& midiMessage = midiMetaData.getMessage();

        // Check if the MIDI message is a controller message
        if (midiMessage.isController())
        {
            // DBG("Controller number: " << midiMessage.getControllerNumber()); -- make sure it's the right controller number
            // Check if the controller number is the mod wheel (1)
            if (midiMessage.getControllerNumber() == 1)
            {
                // DBG("Modwheel value: " << midiMessage.getControllerValue());
                for (int i = 0; i < synthSource.synth.getNumVoices(); ++i)
                {
                    if (auto voice = dynamic_cast<SynthVoice*>(synthSource.synth.getVoice(i)))
                    {
                        // Map the mod wheel value to a frequency
                        voice->controllerMoved(midiMessage.getControllerNumber(), midiMessage.getControllerValue());
                    }
                }
            }
        }
    }
    setVoiceParams();
    synthSource.synth.renderNextBlock(buffer, midiMessages, 0, buffer.getNumSamples());
    


    for (int i = 1; i < buffer.getNumChannels(); ++i)
        buffer.copyFrom (i, 0, buffer.getReadPointer (0), buffer.getNumSamples());
    


    delayEffect.process(buffer);

    // Amplify buffer for oscilloscope
    juce::AudioBuffer<float> oscBuffer (buffer.getNumChannels(), buffer.getNumSamples());
    oscBuffer.makeCopyOf(buffer, false); // copy buffer to oscBuffer
    float gainFactor = oscZoomVal;

    for (int channel = 0; channel < oscBuffer.getNumChannels(); ++channel)
    {
        float* channelData = oscBuffer.getWritePointer(channel);
        for (int sample = 0; sample < oscBuffer.getNumSamples(); ++sample)
        {
            channelData[sample] *= gainFactor;
        }
    }


    // process with HPF if required
    if (hpf.getFrequency() > 21.0f) { 
 //           DBG("HPF frequency: " + juce::String(hpf.getFrequency()));
            for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
            {
                float* channelData = buffer.getWritePointer(channel);
                for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
                {
                    channelData[sample] = hpf.process(channelData[sample]);
                    //DBG("Processing HPF at frequency: " + juce::String(hpf.getFrequency()));
                }
            }
    } 

        // apply masterGain
    masterGain.process (juce::dsp::ProcessContextReplacing<float> (block));
    analyser->pushSamples (buffer);
    oscilloscope->pushSamples (oscBuffer);
    outputMeter->pushSamples (buffer);


    //masterGain.process(buffer);
    
        
    
}


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
            // auto& filterType = *parameters.getRawParameterValue ("filterType"); // everything than lowpass sounds bad!
            auto& filterCutoff = *parameters.getRawParameterValue ("cutoff");
            auto& filterResonance = *parameters.getRawParameterValue ("resonance");
            auto& filterAmount = *parameters.getRawParameterValue ("filterAmount");

            // Delay
            auto& delayTime = *parameters.getRawParameterValue ("delayTime");
            auto& delayFeedback = *parameters.getRawParameterValue ("feedback");
            auto& delayWetDry = *parameters.getRawParameterValue ("delayMix");

            // Master
            auto& masterVolume = *parameters.getRawParameterValue ("masterGain");

            // Osc Zoom
            auto& oscZoom = *parameters.getRawParameterValue ("oscZoom");

            // HPF
            auto& hpfCutoffFreq = *parameters.getRawParameterValue ("hpfCutoff");
                      

            //DBG("Retrieved waveform type: " << osc1WaveformType.load());
//            DBG("Waveform type: " << osc1WaveformType.load());
            voice->setOsc1Params(osc1Octave.load(), osc1Cent.load(), osc1Gain.load(), osc1PulseWidth.load(), osc1WaveformType.load());
            voice->setOsc2Params(osc2Octave.load(), osc2Cent.load(), osc2Gain.load(), osc2PulseWidth.load(), osc2WaveformType.load());
            voice->setLFOParams(depthSlider.load(), rateSlider.load(), sourceComboBox.load(), typeComboBox.load());
            voice->setADSRParams(adsrAttack.load(), adsrDecay.load(), adsrSustain.load(), adsrRelease.load());
            voice->setFilterAdsrParams(adsr2AttackFilter.load(), adsr2DecayFilter.load(), adsr2SustainFilter.load(), adsr2ReleaseFilter.load(), filterCutoff.load()); // baseCutoffFreq
            voice->setFilterParams(0, filterCutoff.load(), filterResonance.load(), filterAmount.load());
            delayEffect.setDelayParams(delayTime.load(), delayFeedback.load(), delayWetDry.load());



            masterGain.setGainDecibels(juce::Decibels::gainToDecibels(masterVolume.load())); // decibels control
            //voice->setDelayParams(delayTime.load(), delayFeedback.load(), delayWetDry.load());

            oscZoomVal = oscZoom.load();
            hpf.setCutoffFrequency(hpfCutoffFreq.load());
            
            
            
            //adsr.update (osc 1 octave.load(), osc 1 cent.load(), osc 1 gain.load(), osc 1 pulse width.load());
        }
    }
}
