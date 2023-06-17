/*
  ==============================================================================

    SynthAudioSource.h
    Created: 10 Jun 2023 2:54:11pm
    Author:  Aleksandr Shabelnikov

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "Filter.h"

class SynthAudioSource : public juce::AudioSource
{
public:
    SynthAudioSource(juce::MidiKeyboardState& keyState, juce::MidiKeyboardComponent::Orientation orientation);

    void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;
    void releaseResources() override;
    void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill) override;
    
    void setCurrentSampleRate(float sampleRate) { currentSampleRate = sampleRate; }


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

    void setOscType(int oscType, int oscIndex) {
        for (int i = 0; i < synth.getNumVoices(); i++) {
            if (auto voice = dynamic_cast<SynthVoice*>(synth.getVoice(i))) {
                voice->setOscType(oscType, oscIndex);
            }
        }
    }

    void setOscGain(int oscIndex, float gain) {
        for (int i = 0; i < synth.getNumVoices(); i++) {
            if (auto voice = dynamic_cast<SynthVoice*>(synth.getVoice(i))) {
                voice->setOscGain(oscIndex, gain);
            }
        }
    }

    void setDetune(int oscIndex, float detune) {
        for (int i = 0; i < synth.getNumVoices(); i++) {
            if (auto voice = dynamic_cast<SynthVoice*>(synth.getVoice(i))) {
                voice->setDetune(oscIndex, detune);
            }
        }
    }

    void setPulseWidth(int oscIndex, float pw) {
        for (int i = 0; i < synth.getNumVoices(); i++) {
            if (auto voice = dynamic_cast<SynthVoice*>(synth.getVoice(i))) {
                voice->setPulseWidth(oscIndex, pw);
            }
        }
    }

    void setAttack(float attack) {
        for (int i = 0; i < synth.getNumVoices(); i++) {
            if (auto voice = dynamic_cast<SynthVoice*>(synth.getVoice(i))) {
                voice->setAttack(attack);
            }
        }
    }

    void setDecay(float decay) {
        for (int i = 0; i < synth.getNumVoices(); i++) {
            if (auto voice = dynamic_cast<SynthVoice*>(synth.getVoice(i))) {
                voice->setDecay(decay);
            }
        }
    }

    void setSustain(float sustain) {
        for (int i = 0; i < synth.getNumVoices(); i++) {
            if (auto voice = dynamic_cast<SynthVoice*>(synth.getVoice(i))) {
                voice->setSustain(sustain);
            }
        }
    }

    void setRelease(float release) {
        for (int i = 0; i < synth.getNumVoices(); i++) {
            if (auto voice = dynamic_cast<SynthVoice*>(synth.getVoice(i))) {
                voice->setRelease(release);
            }
        }
    }



    int getOscType(int oscIndex) {
      return dynamic_cast<SynthVoice*>(synth.getVoice(0))->getWaveform(oscIndex);
    }

    juce::MidiMessageCollector* getMidiCollector()
    {
        return &midiCollector;
    }

    void setFilterType(int filterType) {
        for (int i = 0; i < synth.getNumVoices(); i++) {
            if (auto voice = dynamic_cast<SynthVoice*>(synth.getVoice(i))) {
                voice->setFilterType(filterType);
            }
        }
    }

    void setFilterCutoff(float cutoff) {
        for (int i = 0; i < synth.getNumVoices(); i++) {
            if (auto voice = dynamic_cast<SynthVoice*>(synth.getVoice(i))) {
                voice->setFilterCutoff(cutoff);
            }
        }
    }

    void setFilterResonance(float resonance) {
        for (int i = 0; i < synth.getNumVoices(); i++) {
            if (auto voice = dynamic_cast<SynthVoice*>(synth.getVoice(i))) {
                voice->setFilterResonance(resonance);
            }
        }
    }

   void setFilterAttack(float attack) {
       for (int i = 0; i < synth.getNumVoices(); i++) {
           if (auto voice = dynamic_cast<SynthVoice*>(synth.getVoice(i))) {
               voice->setFilterAttack(attack);
           }
       }
   }

   void setFilterDecay(float decay) {
       for (int i = 0; i < synth.getNumVoices(); i++) {
           if (auto voice = dynamic_cast<SynthVoice*>(synth.getVoice(i))) {
               voice->setFilterDecay(decay);
           }
       }
   }

    void setFilterSustain(float sustain) {
         for (int i = 0; i < synth.getNumVoices(); i++) {
              if (auto voice = dynamic_cast<SynthVoice*>(synth.getVoice(i))) {
                voice->setFilterSustain(sustain);
              }
         }
    }

    void setFilterRelease(float release) {
         for (int i = 0; i < synth.getNumVoices(); i++) {
              if (auto voice = dynamic_cast<SynthVoice*>(synth.getVoice(i))) {
                voice->setFilterRelease(release);
              }
         }
    }

    void setDepth(int depth) {
        for (int i = 0; i < synth.getNumVoices(); i++) {
            if (auto voice = dynamic_cast<SynthVoice*>(synth.getVoice(i))) {
                voice->setDepth(depth);
            }
        }
    }


private:
    float currentSampleRate;
    juce::MidiKeyboardState& keyboardState;
    juce::Synthesiser synth;
    juce::MidiKeyboardComponent::Orientation keyboardOrientation;
    juce::MidiMessageCollector midiCollector;
    juce::ADSR adsr;
    juce::ADSR::Parameters adsrParams;
    float cutoffFreq;
    float resonance;
    
};
