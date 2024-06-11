/*
  ==============================================================================

    SynthVoice.cpp
    Created: 14 Jul 2023 1:18:09pm
    Author:  Aleksandr Shabelnikov

  ==============================================================================
*/

#include "SynthVoice.h"

SynthVoice::SynthVoice()
{
    // empty constructor
}

bool SynthVoice::canPlaySound(juce::SynthesiserSound *sound)
{
    // Return true if this voice can play the given sound
    return dynamic_cast<SynthSound *>(sound) != nullptr;
}

void SynthVoice::startNote(int midiNoteNumber, float velocity,
                           juce::SynthesiserSound *sound, int currentPitchWheelPosition)
{
    // Convert the MIDI note number to a frequency.
    originalFrequency = juce::MidiMessage::getMidiNoteInHertz(midiNoteNumber);
    DBG("Starting note with original frequency: " << originalFrequency);
    DBG("Velocity: " << velocity);
    osc1.setMusicalFrequency(originalFrequency);
    osc2.setMusicalFrequency(originalFrequency);

    // Apply pitch bend if the pitch wheel is not centered
    if (currentPitchWheelPosition != 8192)
    {
        applyPitchBend(currentPitchWheelPosition);
    }

    // Set the level of the oscillator based on the velocity.
    osc1.setVelocity(velocity);
    osc2.setVelocity(velocity);

    adsr.updateParams(0.1f, 0.2f, 0.8f, 0.5f); // default values will be overridden by APVTS
    adsr.noteOn();
    filterAdsr.updateParams(0.1f, 0.2f, 0.8f, 0.5f); // default values will be overridden by APVTS
    filterAdsr.noteOn();

    // osc1.getFrequency();
    // osc2.getFrequency();
}

void SynthVoice::stopNote(float velocity, bool allowTailOff)
{
    // check if sustain pedal is pressed
    if (isSustainPedalDown())
    {
        DBG("Sustain pedal is down. Note will not stop");
    }

    // This is called when a note stops
    adsr.noteOff();
    filterAdsr.noteOff();
    filterAdsr.reset();

    if (!allowTailOff || !adsr.isActive())
        clearCurrentNote();
}

void SynthVoice::pitchWheelMoved(int newPitchWheelValue)
{
    // Handle pitch wheel changes
    applyPitchBend(newPitchWheelValue);
}

void SynthVoice::applyPitchBend(int pitchWheelValue)
{
    // Assuming the pitch bend range is ±2 semitones
    float bendRangeInSemitones = 2.0f;
    // Calculate how far the pitch wheel is from the center
    float bendAmount = (pitchWheelValue - 8192.0f) / 8192.0f;
    CUSTOMDBG("Bend amount: " << bendAmount);
    // Calculate the frequency multiplier
    float frequencyMultiplier = std::pow(2.0f, bendRangeInSemitones * bendAmount / 12.0f);

    // Apply the frequency multiplier to your oscillators
    osc1.setPitchBendMultiplier(frequencyMultiplier);
    osc2.setPitchBendMultiplier(frequencyMultiplier);
}

void SynthVoice::controllerMoved(int controllerNumber, int newControllerValue)
{
    // Handle controller movements
    if (controllerNumber == 1) // 1 is the mod wheel's CC number
    {
        modControllerCutOffFreq = mapModWheelToFrequency(newControllerValue);
        // DBG("Mod wheel value: " << newControllerValue << ", mapped frequency: " << modControllerCutOffFreq);
        filter.setModCutoff(modControllerCutOffFreq);
    }
    // if sustain pedal is pressed - print dbg message
    if (controllerNumber == 64)
    {
        if (newControllerValue > 63)
        {
            CUSTOMDBG("Sustain pedal pressed");
        }
        if (newControllerValue < 64)
        {
            CUSTOMDBG("Sustain pedal released");
        }
    }
}

void SynthVoice::prepareToPlay(int samplesPerBlockExpected, double sampleRate, int numChannels)
{
    juce::dsp::ProcessSpec spec;
    spec.maximumBlockSize = samplesPerBlockExpected;
    spec.sampleRate = sampleRate;
    spec.numChannels = numChannels;

    noiseGen.prepare(spec); // prepare the noise generator

    adsr.reset();
    adsr.setSampleRate(sampleRate);

    for (int i = 0; i < numChannels; ++i)
    {
        DBG("Initializing oscillator " << i);
        osc1.setSampleRate(sampleRate);
        osc2.setSampleRate(sampleRate);

        DBG("Initializing filter " << i);
        filter.prepareToPlay(sampleRate);

        DBG("Initializing LFO " << i);
        tremoloLFO.prepareToPlay(sampleRate, samplesPerBlockExpected, numChannels);
    }

    // prepare the noise generator
    noiseGen.prepare(spec);

    tremoloLFO.setDepth(0.5f);  // Depth of 50%
    tremoloLFO.setFrequency(5); // Rate of 5 Hz

    // prepare the filter adsr
    filterAdsr.reset();
    filterAdsr.setSampleRate(sampleRate);

    DBG("Initialization finished");
    isPrepared = true;
}

float SynthVoice::mapModWheelToFrequency(int modWheelValue)
{
    // Ensure the modWheelValue is in the range [0, 127]
    modWheelValue = juce::jlimit(0, 127, modWheelValue);

    float normalizedValue = modWheelValue / 127.0f; // Converts to [0, 1]
    // return 20.0f + normalizedValue * (3000.0f - 20.0f); // Maps to [20, 3000]
    return normalizedValue; // now frequency is calculated by filter based on [0, 1] range
}

void SynthVoice::renderNextBlock(juce::AudioBuffer<float> &outputBuffer, int startSample, int numSamples)
{
    jassert(isPrepared);
    if (!isVoiceActive())
        return;

    static long dbgCounter = 0;
    for (int sample = 0; sample < numSamples; ++sample)
    {
        // Get the next sample from the oscillator
        auto oscSample = osc1.getNextSample();
        auto osc2Sample = osc2.getNextSample();

        // noise
        float noiseSample = noiseGen.getNextSample() * noiseLevel;

        // Apply the ADSR envelope to the oscillators samples
        auto envSample = adsr.getNextSample() * oscSample;
        auto envSample2 = adsr.getNextSample() * osc2Sample;
        auto noiseEnvSample = adsr.getNextSample() * noiseSample;


        // Get the next sample from the LFO
        float lfoSample = 1.0f;
        if (isLFOEnabled)
        {
            lfoSample = (tremoloLFO.getNextSample() + 1.0f) * 0.5f; // Convert LFO range from [0, 2] to [0, 1] with midpoint at 0.5
        }

        // get the next sample from the filter LFO
        // we should convert [0, 2] to [0, 1]
        float filterLfoSample = (filterLFO.getNextSample() * 0.5f);
        // DBG("Filter LFO sample: " << filterLfoSample);


        // Calculate filter modulation using filter ADSR
        auto filterMod = filterAdsr.getNextSample();
        // DBG("Filter mod: " << filterMod);

        // Combine oscillator samples and apply LFO
        LFOsc::lfoType type = tremoloLFO.getType();
        float drySample;
        if (type == LFOsc::lfoType::TREMOLO)
        {
            drySample = (envSample + envSample2) * 0.5f * lfoSample;
        }
        else if (type == LFOsc::lfoType::PWM)
        {
            CUSTOMDBG("PWM LFO DETECTED");
            lfoSample = (lfoSample + 1.0f) / 2.0f;
            osc2.setPulseWidth(lfoSample);
            drySample = (envSample + envSample2 + noiseEnvSample) * 0.5f;
        }
        else
        {
            drySample = (envSample + envSample + noiseEnvSample) * 0.5f;
        }

//        DBG("filterMod: " << filterMod);
        if (isFilterADSR) {
            filter.setADSRCutOff(filterMod);
        }

        if (filterLFO.getDepth() > 0) {
            filter.setLFOMod(filterLfoSample);
        }


        // Filter the dry sample
        float wetSample = filter.processSample(drySample);

        // Calculate the dry/wet mix
        float mixedSample = drySample * (1.0f - filterAmount) + wetSample * filterAmount;

        for (int channel = 0; channel < outputBuffer.getNumChannels(); ++channel)
        {
            // Add the mixed sample to the output buffer
            outputBuffer.addSample(channel, startSample + sample, mixedSample);
        }
    }
}

void SynthVoice::setFilterLFOParams(float depth, float frequency)
{
    // DBG("Setting filter LFO params: depth " << depth << ", frequency " << frequency);
    filterLFO.setDepth(depth);
    filterLFO.setFrequency(frequency);
}

void SynthVoice::enableFilterADSR(int state)
{
    bool isEnabled = (state == 1); // 1 is "Enabled" and 0 is "Disabled"
    
    if (isEnabled == isFilterADSR)
    {
        return;
    }
    DBG("Filter ADSR is toggled: " << (int) isEnabled);
    isFilterADSR = isEnabled;
}


void SynthVoice::setOsc1Params(int octave, int cent, float gain, float pulseWidth, int waveformType)
{
    osc1.setWaveform(static_cast<Oscillator::Waveform>(waveformType));
    osc1.setGain(gain);
    osc1.setOctave(octave);
    osc1.setDetune(cent);
    if (tremoloLFO.getType() != LFOsc::lfoType::PWM)
        osc1.setPulseWidth(pulseWidth);
    osc1.setMusicalFrequency(originalFrequency);
}

void SynthVoice::setLfoType(int type)
{
    if (type == tremoloLFO.getType())
    {
        return;
    }
    DBG("Switching LFO type to " << type);
    tremoloLFO.setType(static_cast<LFOsc::lfoType>(type));
}

void SynthVoice::enableLFO(bool isEnabled)
{
    if (isEnabled == isLFOEnabled)
        return;

    isLFOEnabled = isEnabled;
}

void SynthVoice::setOsc2Params(int octave, int cent, float gain, float pulseWidth, int waveformType)
{
    osc2.setWaveform(static_cast<Oscillator::Waveform>(waveformType));
    osc2.setGain(gain); 
    osc2.setOctave(octave);
    osc2.setDetune(cent);
    if (tremoloLFO.getType() != LFOsc::lfoType::PWM)
        osc2.setPulseWidth(pulseWidth);
    osc2.setMusicalFrequency(originalFrequency);
}

void SynthVoice::setLFOParams(float depth, float frequency, int source, int type)
{
    tremoloLFO.setDepth(depth);
    tremoloLFO.setFrequency(frequency);
}

void SynthVoice::setADSRParams(float attack, float decay, float sustain, float release)
{
    adsr.updateParams(attack, decay, sustain, release);
}

void SynthVoice::setFilterParams(int type, float cutoff, float resonance, float amount)
{
    filterAmount = amount;
    filter.setParams(cutoff, resonance);
}

void SynthVoice::setFilterAdsrParams(float attack, float decay, float sustain, float release, float baseCutoffFreq)
{
    float currentAttack, currentDecay, currentSustain, currentRelease;
    filterAdsr.getParams(currentAttack, currentDecay, currentSustain, currentRelease);
    if (currentAttack == attack && currentDecay == decay && currentSustain == sustain && currentRelease == release)
    {
        return;
    }

    filterAdsr.updateParams(attack, decay, sustain, release);
}

void SynthVoice::setNoiseLevel(float level)
{
    if (level == noiseLevel)
    {
        return;
    }
    CUSTOMDBG("Setting noise level to " << level);
    noiseLevel = level;
}

void SynthVoice::setNoiseType(int type)
{
    noiseGen.setType(type);
}
