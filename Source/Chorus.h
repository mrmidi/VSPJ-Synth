/*
  ==============================================================================

    Chorus.h
    Created: 6 Jun 2024 8:01:29pm
    Author:  Alexander Shabelnikov

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

// Chorus effect class inheriting from juce::dsp::Chorus<float>
class Chorus : public juce::dsp::Chorus<float>
{
public:
    Chorus();
    
    ~Chorus(); 

    // Prepare the chorus effect with the given processing specification
    void prepare(const juce::dsp::ProcessSpec& spec);
    // Process the audio buffer through the chorus effect
    void process(juce::AudioBuffer<float>& buffer);
    // Reset the chorus effect state
    void reset();

    // Set the rate of the chorus effect
    void setRate(float rate);
    // Set the depth of the chorus effect
    void setDepth(float depth);
    // Set the centre delay of the chorus effect
    void setCentreDelay(float centreDelay);
    // Set the feedback of the chorus effect
    void setFeedback(float feedback);
    // Set the mix of the chorus effect
    void setMix(float mix);

    // Set multiple parameters of the chorus effect at once
    void setChorusParams(float rate, float depth, float centreDelay, float feedback, float mix);

private:

      // helper function to convert percentage to linear
    float percentToLinear(int percentage) {
      return static_cast<float>(percentage) / 100.0f;
    }

    float rate = 1.0f;          // Modulation rate of the chorus effect
    float depth = 0.5f;         // Modulation depth of the chorus effect
    float centreDelay = 0.0f;   // Centre delay time of the chorus effect
    float feedback = 0.0f;      // Feedback amount of the chorus effect
    float mix = 0.5f;           // Mix between dry and wet signal

    double currentSampleRate = 44100.0; // Default sample rate
};
