/*
  ==============================================================================

    SynthVoice.h
    Created: 10 Jun 2023 1:31:31pm
    Author:  Aleksandr Shabelnikov

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "SynthSound.h"

class SynthVoice : public juce::SynthesiserVoice
{
public:
    SynthVoice()
        : phase (0), phaseDelta (0), amplitude (0)
    {
        adsrParams.attack = 0.5f;
        adsrParams.decay = 0.5f;
        adsrParams.sustain = 1.0f;
        adsrParams.release = 1.0f;
        adsr.setParameters(adsrParams);
    }

    bool canPlaySound (juce::SynthesiserSound* sound) override
    {
        return dynamic_cast<SynthSound*> (sound) != nullptr;
    }

    void startNote (int midiNoteNumber, float velocity,
                    juce::SynthesiserSound* /*sound*/, int /*currentPitchWheelPosition*/) override
    {
        frequency = juce::MidiMessage::getMidiNoteInHertz (midiNoteNumber);
        phase = 0.0;
        amplitude = velocity;
        updatePhaseDelta();
        adsr.noteOn();
    }

    void stopNote (float /*velocity*/, bool allowTailOff) override
    {
        adsr.noteOff();
    }

    void renderNextBlock (juce::AudioBuffer<float>& outputBuffer, int startSample, int numSamples) override
    {
        if (!adsr.isActive()) return;

        while (--numSamples >= 0)
        {
            auto currentSample = (float) (std::sin (phase) * amplitude * adsr.getNextSample());

            for (auto i = outputBuffer.getNumChannels(); --i >= 0;)
                outputBuffer.addSample (i, startSample, currentSample);

            phase += phaseDelta;
            ++startSample;
        }
    }

    void pitchWheelMoved (int /*newPitchWheelValue*/) override {}
    void controllerMoved (int /*controllerNumber*/, int /*newControllerValue*/) override {}

    void setCurrentPlaybackSampleRate (double newRate) override
    {
        juce::SynthesiserVoice::setCurrentPlaybackSampleRate (newRate);
        updatePhaseDelta();
    }

private:
    double frequency, phase, phaseDelta, amplitude;

    void updatePhaseDelta()
    {
        auto cyclesPerSample = frequency / getSampleRate(); // [2]
        phaseDelta = cyclesPerSample * 2.0 * juce::MathConstants<double>::pi;
    }
    
public:
    juce::ADSR adsr;
    juce::ADSR::Parameters adsrParams;
};
