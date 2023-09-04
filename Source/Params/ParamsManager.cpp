/*
  ==============================================================================

    ParamsManager.cpp
    Created: 27 Aug 2023 10:41:57am
    Author:  Alexander Shabelnikov

  ==============================================================================
*/

#include "ParamsManager.h"
void ParamsManager::updateParams()
{
    const auto& paramsMap = params.getParamsMap();

    for (const auto& [paramID, param] : paramsMap)
    {
        float value = param->getValue();

        // Now, based on the paramID, apply the value to the corresponding component.
        // This can be done using a switch or if-else statements to map paramID to specific actions.

        if (paramID == "osc1Gain")
        {
            // synthVoice.setOsc1Gain(value);
        }
        else if (paramID == "osc1WaveformType")
        {
            *synthVoice.setOsc1WaveformType(static_cast<int>(value));
        }
        // ... handle other parameters ...

        // Alternatively, if you have a method in SynthVoice or another component that takes a parameter ID and value:
        // synthVoice.setParameter(paramID, value);
    }
}
