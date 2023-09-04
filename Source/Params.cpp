/*
  ==============================================================================

    Params.cpp
    Created: 17 Jul 2023 2:28:08pm
    Author:  Alexander Shabelnikov

  ==============================================================================
*/

#include "Params.h"

    Params::Params(juce::AudioProcessor& processor)
        : state(processor, nullptr, "state", createParameterLayout())
    {
        // Populate the parameters map
        for (auto& paramID : { "osc1Octave", "osc1Cent", "osc1Gain", "osc1PulseWidth", "osc1WaveformType" })
        {
            paramsMap[paramID] = state.getParameter(paramID);
        }
    }
