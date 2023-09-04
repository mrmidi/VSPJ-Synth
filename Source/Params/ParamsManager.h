/*
  ==============================================================================

    ParamsManager.h
    Created: 27 Aug 2023 10:41:57am
    Author:  Alexander Shabelnikov

  ==============================================================================
*/

#include <JuceHeader.h>
#include "../Params.h"
#include "../SynthVoice.h"

#pragma once
class ParamsManager
{
public:
    ParamsManager(Params& params, SynthVoice& voice)
        : params(params), synthVoice(voice)
    {
    }

    void updateParams();

private:
    Params& params;
    SynthVoice& synthVoice;
    


};
