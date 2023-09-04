/*
  ==============================================================================

    Params.h
    Created: 17 Jul 2023 2:28:08pm
    Author:  Alexander Shabelnikov

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>


class Params
{
public:

    enum paramsList
    {
        osc1Octave,
        osc1Cent,
        osc1Gain,
        osc1PulseWidth,
        osc1WaveformType
    };

    Params(juce::AudioProcessor& processor);

    juce::AudioProcessorValueTreeState& getState() { return state; }

    auto getStateByName(const juce::String& name) { return state.getParameter(name); }
    const std::map<juce::String, juce::RangedAudioParameter*>& getParamsMap() const
    {
        return paramsMap;
    }

    
    

private:
    juce::String enumToString(paramsList param) const
    {
        switch (param)
        {
        case osc1Octave: return "osc1Octave";
        case osc1Cent: return "osc1Cent";
        case osc1Gain: return "osc1Gain";
        case osc1PulseWidth: return "osc1PulseWidth";
        case osc1WaveformType: return "osc1WaveformType";
        default: jassertfalse; return {};
        }
    }
    
    
static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    // osc 1 params
    params.push_back(std::make_unique<juce::AudioParameterInt>(juce::ParameterID("osc1Octave", 6), "Osc 1 Octave", -3, 3, 0));
    params.push_back(std::make_unique<juce::AudioParameterInt>(juce::ParameterID("osc1Cent", 7), "Osc 1 Cent", -50, 50, 0));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("osc1Gain", 8), "Osc 1 Gain", 0.0f, 1.0f, 0.5f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("osc1PulseWidth", 9), "Osc 1 Pulse Width", 0.0f, 1.0f, 0.5f));
    params.push_back(std::make_unique<juce::AudioParameterChoice>(juce::ParameterID("osc1WaveformType", 10), "Osc 1 Waveform Type", juce::StringArray {"Sine", "Triangle", "Sawtooth", "Square" }, 0));


//    for (auto& param : params)
//    {
//        DBG("param->paramID: " << param->paramID << " param->name: " << param->name);
//        paramsMap[param->paramID] = param->name;
//    }

//    // add parameter names to map
//    for (auto& param : params)
//    {
//        DBG("Adding param to map");
//      DBG("param->paramID: " << param->paramID << " param->name: " << param->name");
//      paramsMap[param->paramID] = param->name;
//    }

    return { params.begin(), params.end() };
}


    juce::AudioProcessorValueTreeState state;
    std::map<juce::String, juce::RangedAudioParameter*> paramsMap;

//    static std::map<juce::String, juce::String> paramsMap;

    
};
/*
 
 class Params
 {
 public:
     enum paramsList
     {
         osc1Octave,
         osc1Cent,
         osc1Gain,
         osc1PulseWidth,
         osc1WaveformType
     };

     Params(juce::AudioProcessor& processor);
     juce::AudioProcessorValueTreeState& getState() { return state; }

     float getParam(paramsList param) const
     {
         return *state.getRawParameterValue(getParamID(param));
     }

     void setParam(paramsList param, float value)
     {
         state.getParameter(getParamID(param))->setValueNotifyingHost(value);
     }

 private:
     juce::String getParamID(paramsList param) const
     {
         switch (param)
         {
         case osc1Octave: return "osc1Octave";
         case osc1Cent: return "osc1Cent";
         case osc1Gain: return "osc1Gain";
         case osc1PulseWidth: return "osc1PulseWidth";
         case osc1WaveformType: return "osc1WaveformType";
         default: jassertfalse; return {};
         }
     }

     static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
     juce::AudioProcessorValueTreeState state;
 };

 
 
 */
