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

    chorusEffect.prepare(spec);

    // magic meters
    outputMeter->setupSource (getTotalNumOutputChannels(), sampleRate, 500);
    oscilloscope->prepareToPlay (sampleRate, samplesPerBlock);
    analyser->prepareToPlay (sampleRate, samplesPerBlock);

    hpf.prepareToPlay(sampleRate, samplesPerBlock, getTotalNumOutputChannels());
    // lowShelfFilter.prepareToPlay(sampleRate, samplesPerBlock, getTotalNumOutputChannels());
    lowShelfFilter.filter.reset();
    lowShelfFilter.filter.prepare(spec);
    lowShelfFilter.setGainDB(0.0f);
    setVoiceParams(); // reset voice parameters

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
            // Check if the controller number is the mod wheel (1)
            if (midiMessage.getControllerNumber() == 1)
            {
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
    
    // process with chorus
    chorusEffect.process(buffer);

    // process with delay
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


    if (hpf.getFrequency() > 21.0f) { 
            for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
            {
                float* channelData = buffer.getWritePointer(channel);
                for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
                {
                    channelData[sample] = hpf.process(channelData[sample]);
                }
            }
    } 

    if (lowShelfFilter.getGainDB() != 0.0f) {
        lowShelfFilter.filter.process(juce::dsp::ProcessContextReplacing<float> (block));
    }
        // apply masterGain
    masterGain.process (juce::dsp::ProcessContextReplacing<float> (block));
    analyser->pushSamples (buffer);
    oscilloscope->pushSamples (oscBuffer);
    outputMeter->pushSamples (buffer);

        
    
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
            auto& osc1Octave = *parameters.getRawParameterValue("osc1Octave");
            auto& osc1Cent = *parameters.getRawParameterValue("osc1Cent");
            auto& osc1Gain = *parameters.getRawParameterValue("osc1Gain");
            auto& osc1PulseWidth = *parameters.getRawParameterValue("osc1PulseWidth");
            auto& osc1WaveformType = *parameters.getRawParameterValue("osc1WaveformType");
            // Oscillator 2
            auto& osc2Octave = *parameters.getRawParameterValue("osc2Octave");
            auto& osc2Cent = *parameters.getRawParameterValue("osc2Cent");
            auto& osc2Gain = *parameters.getRawParameterValue("osc2Gain");
            auto& osc2PulseWidth = *parameters.getRawParameterValue("osc2PulseWidth");
            auto& osc2WaveformType = *parameters.getRawParameterValue("osc2WaveformType");
            // Noise
            auto& noiseGain = *parameters.getRawParameterValue("noiseGain");
            auto& noiseType = *parameters.getRawParameterValue("noiseType");

            // LFO
            auto& sourceComboBox = *parameters.getRawParameterValue("sourceComboBox");
            auto& depthSlider = *parameters.getRawParameterValue("depthSlider");
            auto& rateSlider = *parameters.getRawParameterValue("rateSlider");
            auto& typeComboBox = *parameters.getRawParameterValue("typeComboBox");

            // ADSR voice
            auto& adsrAttack = *parameters.getRawParameterValue("adsr1attack");
            auto& adsrDecay = *parameters.getRawParameterValue("adsr1decay");
            auto& adsrSustain = *parameters.getRawParameterValue("adsr1sustain");
            auto& adsrRelease = *parameters.getRawParameterValue("adsr1release");

            // ADSR filter
            auto& adsr2AttackFilter = *parameters.getRawParameterValue("adsr2attack");
            auto& adsr2DecayFilter = *parameters.getRawParameterValue("adsr2decay");
            auto& adsr2SustainFilter = *parameters.getRawParameterValue("adsr2sustain");
            auto& adsr2ReleaseFilter = *parameters.getRawParameterValue("adsr2release");
            auto& adsr2Enabled = *parameters.getRawParameterValue("adsrFilterEnabled");

            // Filter
            auto& filterCutoff = *parameters.getRawParameterValue("cutoff");
            auto& filterResonance = *parameters.getRawParameterValue("resonance");
            auto& filterAmount = *parameters.getRawParameterValue("filterAmount");

            // Delay
            auto& delayTime = *parameters.getRawParameterValue("delayTime");
            auto& delayFeedback = *parameters.getRawParameterValue("feedback");
            auto& delayWetDry = *parameters.getRawParameterValue("delayMix");

            // Master
            auto& masterVolume = *parameters.getRawParameterValue("masterGain");

            // Osc Zoom
            auto& oscZoom = *parameters.getRawParameterValue("oscZoom");

            // HPF
            auto& hpfCutoffFreq = *parameters.getRawParameterValue("hpfCutoff");

            // Low Shelf Filter
            auto& lowShelfGain = *parameters.getRawParameterValue("lowShelfGain");

            // LFO Type
            auto& lfoType = *parameters.getRawParameterValue("typeComboBox");          

            // Set parameters
            voice->setOsc1Params(osc1Octave.load(), osc1Cent.load(), osc1Gain.load(), percentToLinear(osc1PulseWidth.load()), osc1WaveformType.load());
            voice->setOsc2Params(osc2Octave.load(), osc2Cent.load(), osc2Gain.load(), percentToLinear(osc2PulseWidth.load()), osc2WaveformType.load());
            voice->setLFOParams(percentToLinear(depthSlider.load()), rateSlider.load(), sourceComboBox.load(), typeComboBox.load());
            voice->setADSRParams(adsrAttack.load(), adsrDecay.load(), adsrSustain.load(), adsrRelease.load());
            voice->setFilterAdsrParams(adsr2AttackFilter.load(), adsr2DecayFilter.load(), adsr2SustainFilter.load(), adsr2ReleaseFilter.load(), filterCutoff.load());
            voice->enableFilterADSR(adsr2Enabled.load());
            voice->setFilterParams(0, filterCutoff.load(), filterResonance.load(), percentToLinear(filterAmount.load()));

            // N O I S E    C O N T R O L
            voice->setNoiseLevel(noiseGain.load());
            voice->setNoiseType(noiseType.load());

            // D E L A Y    C O N T R O L
            delayEffect.setDelayParams(delayTime.load(), percentToLinear(delayFeedback.load()), percentToLinear(delayWetDry.load()));

            // L F O   C O N T R O L
            voice->setLfoType(lfoType.load());

            // Filter LFO params
            auto& filterLfoDepth = *parameters.getRawParameterValue("filterLFOdepth");
            auto& filterLfoFrequency = *parameters.getRawParameterValue("filterLFOfrequency");
            voice->setFilterLFOParams(percentToLinear(filterLfoDepth.load()), filterLfoFrequency.load());


            // C H O R U S   C O N T R O L
            auto& chorusRate = *parameters.getRawParameterValue("chorusRate");
            auto& chorusDepth = *parameters.getRawParameterValue("chorusDepth");
            auto& chorusMix = *parameters.getRawParameterValue("chorusMix");
            auto& chorusCentreDelay = *parameters.getRawParameterValue("chorusCentreDelay");
            auto& chorusFeedback = *parameters.getRawParameterValue("chorusFeedback");

            chorusEffect.setChorusParams(chorusRate.load(), chorusDepth.load(), chorusCentreDelay.load(), chorusFeedback.load(), chorusMix.load());

            masterGain.setGainDecibels(juce::Decibels::gainToDecibels(percentToLinear(masterVolume.load()))); // decibels control

            oscZoomVal = oscZoom.load();
            hpf.setCutoffFrequency(hpfCutoffFreq.load());
            lowShelfFilter.setGainDB(lowShelfGain.load());
        }
    }
}
