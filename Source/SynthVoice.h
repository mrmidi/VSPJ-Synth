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
#include "Osc.h"
#include "Mixer.h"




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
        osc1.setWaveform(Oscillator::Sawtooth);
        osc2.setWaveform(Oscillator::Sine);
    }
//    SynthVoice(int osc1, int osc2) {
//        osc1.setWaveform(osc1);
//        osc2.setWaveform(osc2);
//        adsrParams.attack = 0.5f;
//        adsrParams.decay = 0.5f;
//        adsrParams.sustain = 1.0f;
//        adsrParams.release = 1.0f;
//        adsr.setParameters(adsrParams);
//    }

    bool canPlaySound (juce::SynthesiserSound* sound) override
    {
        return dynamic_cast<SynthSound*> (sound) != nullptr;
    }

    void startNote (int midiNoteNumber, float velocity,
                    juce::SynthesiserSound* /*sound*/, int /*currentPitchWheelPosition*/) override
    {
        frequency = juce::MidiMessage::getMidiNoteInHertz (midiNoteNumber);

        osc1.setFrequency(frequency);
        osc2.setFrequency(frequency);
        amplitude = velocity;
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
            // Generate samples from each oscillator
            auto currentSample1 = osc1.getNextSample() * amplitude * adsr.getNextSample() * osc1.getGain();
            auto currentSample2 = osc2.getNextSample() * amplitude * adsr.getNextSample() * osc2.getGain();

            // Add the samples together (mix them)
            auto mixedSample = (currentSample1 + currentSample2) / 2.0f * volume;

            for (auto i = outputBuffer.getNumChannels(); --i >= 0;)
                outputBuffer.addSample (i, startSample, mixedSample);

            phase += phaseDelta;
            ++startSample;
        }
    }




    void pitchWheelMoved (int /*newPitchWheelValue*/) override {}
    void controllerMoved (int /*controllerNumber*/, int /*newControllerValue*/) override {}

    void setCurrentPlaybackSampleRate (double newRate) override
    {
        currentSampleRate = newRate;
        updatePhaseDelta();
    }


public:
    void setVolume(float newVolume) {
        volume = newVolume;
    }

    int getWaveform(int osc) {
        if (osc == 1) {
            return osc1.getWaveform();
        }
        else if (osc == 2) {
            return osc2.getWaveform();
        }
        return -1;
    }

    
    void setOscType(int osc, int wave) {
        // get osc type
        std::cout << "Setting Oscillator " << osc << " to waveform " << wave << std::endl;
            
        Oscillator::Waveform waveform = static_cast<Oscillator::Waveform> (wave);
        if (osc == 1) {
            osc1.setWaveform(waveform);
        }
        else if (osc == 2) {
            osc2.setWaveform(waveform);
        }
    }

    void setOscGain(int osc, float gain) {
        if (osc == 1) {
            osc1.setGain(gain);
        }
        else if (osc == 2) {
            osc2.setGain(gain);
        }
    }
    
    void setDetune(int osc, float detune) {
        if (osc == 1) {
            osc1.setDetune(detune);
        }
        else if (osc == 2) {
            osc2.setDetune(detune);
        }
    }

    void setPulseWidth(int osc, float pw) {
        if (osc == 1) {
            osc1.setPulseWidth(pw);
        }
        else if (osc == 2) {
            osc2.setPulseWidth(pw);
        }
    }

private:
    double frequency, phase, phaseDelta, amplitude;
    double currentSampleRate;

    void updatePhaseDelta()
    {
        auto cyclesPerSample = frequency / getSampleRate(); // [2]
        phaseDelta = cyclesPerSample * 2.0 * juce::MathConstants<double>::pi;
    }
    
public:
    juce::ADSR adsr;
    juce::ADSR::Parameters adsrParams;
    float volume = 1.0f;
    Oscillator osc1, osc2;
    Mixer mixer;
};
