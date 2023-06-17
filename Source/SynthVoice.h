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
    #include "Filter.h"
    #include "Specs.h"
    #include "Adsr.h"

    class SynthVoice : public juce::SynthesiserVoice
    {
    public:
        SynthVoice()
            : phase (0), phaseDelta (0), amplitude (0)
        {
            specs.setAll(44100.0f, 512, 2);

            // adsrParams.attack = 0.5f;
            // adsrParams.decay = 0.5f;
            // adsrParams.sustain = 1.0f;
            // adsrParams.release = 1.0f;
            adsr.updateAttack(0.5f);
            adsr.updateDecay(0.5f);
            adsr.updateSustain(1.0f);
            adsr.updateRelease(1.0f);

//            adsr.setParameters(adsrParams);
            osc1.setWaveform(Oscillator::Sawtooth);
            osc2.setWaveform(Oscillator::Sine);
            filter.prepare (specs.getSpecs());
//            adsr.prepare (specs.getSpecs());
//            filterAdsr.prepare(specs.getSpecs());
            //filterAdsr.setParameters(adsrParams);
        }

        void prepareToPlay(double sampleRate, int samplesPerBlock, int outputChannels)
        {
            specs.setAll(sampleRate, samplesPerBlock, outputChannels);
            filter.prepare(specs.getSpecs());
            filterAdsr.setSampleRate(sampleRate);
            
        }

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
            filterAdsr.noteOn();
        }
        
        void stopNote (float /*velocity*/, bool allowTailOff) override
        {
            adsr.noteOff();
            filterAdsr.noteOff();
            
            if (! allowTailOff || ! adsr.isActive())
                clearCurrentNote();
        }


        
        void renderNextBlock (juce::AudioBuffer<float>& outputBuffer, int startSample, int numSamples) override
        {
            if (!adsr.isActive()) return; // envelope has finished, quit

            while (--numSamples >= 0)
            {
                float filterMod = filterAdsr.getNextSample(); // get next sample from envelope

                 //Generate samples from each oscillator
                 auto currentSample1 = osc1.getNextSample() * amplitude * adsr.getNextSample() * osc1.getGain();  // osc1
                 auto currentSample2 = osc2.getNextSample() * amplitude * adsr.getNextSample() * osc2.getGain();  // osc2

                // apply filter to each oscillator
                float cutoff = filter.getBaseCutOffFreq() + filterMod * filter.getDepth() * filter.getBaseCutOffFreq();
                cutoff = std::clamp<float> (cutoff, 20.0f, 20000.0f);
                filter.setCutoffFrequency(cutoff);
                currentSample1 = filter.processNextSample(0, currentSample1);
                currentSample2 = filter.processNextSample(0, currentSample2);

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
            // get new sample rate from synthvoice
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

        void setDepth(int depth) {
            filter.setDepth(depth);
        }

        
        void setOscType(int osc, int wave) 
        {
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
        
        void setDetune(int osc, float detune) 
        {
            if (osc == 1) {
                osc1.setDetune(detune);
            }
            else if (osc == 2) {
                osc2.setDetune(detune);
            }
        }

        void setPulseWidth(int osc, float pw) 
        {
            if (osc == 1) {
                osc1.setPulseWidth(pw);
            }
            else if (osc == 2) {
                osc2.setPulseWidth(pw);
            }
        }

        void setAttack(float attack)
        {
            adsr.updateAttack(attack);
        }

        void setDecay(float decay)
        {
            adsr.updateDecay(decay);
        }

        void setSustain(float sustain)
        {
            adsr.updateSustain(sustain);
        }

        void setRelease(float release)
        {
            adsr.updateRelease(release);
        }

        

        void setFilterParams(int type, float cutoff, float resonance)
        {
            filter.setParams(type, cutoff, resonance);
        }

        void setFilterType(int type)
        {
            filter.selectFilterType(type);
        }

        void setFilterCutoff(float cutoff)
        {
            filter.setCutoffFrequency(cutoff);
            filter.setBaseCutOffFreq(cutoff);
        }

        void setFilterResonance(float resonance)
        {
            filter.setResonance(resonance);
        }
        
        void setFilterAttack(float attack)
        {
            filterAdsr.updateAttack(attack);
        }

        void setFilterDecay(float decay)
        {
            filterAdsr.updateDecay(decay);
        }

        void setFilterSustain(float sustain)
        {
            filterAdsr.updateSustain(sustain);
        }

        void setFilterRelease(float release)
        {
            filterAdsr.updateRelease(release);
        }

        

        int getFilterType() const
        {
            return static_cast<int>(filter.getType());
        }

        float getFilterCutoff() const
        {
            return filter.getCutoffFrequency();
        }

        float getFilterResonance() const
        {
            return filter.getResonance();
        }




    private:
        double frequency, phase, phaseDelta, amplitude;
        double currentSampleRate;
        Specs specs;

        void updatePhaseDelta()
        {
            auto cyclesPerSample = frequency / getSampleRate(); // [2]
            phaseDelta = cyclesPerSample * 2.0 * juce::MathConstants<double>::pi;
        }
        
    public:
//        juce::ADSR adsr;
//        juce::ADSR::Parameters adsrParams;
        float volume = 1.0f;
        Oscillator osc1, osc2;
        Mixer mixer;
        Filter filter;
        Adsr filterAdsr;
        Adsr adsr;
    };
