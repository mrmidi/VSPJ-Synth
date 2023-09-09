/*
  ==============================================================================

    AdsrControlGroup.h
    Created: 9 Sep 2023 12:12:36pm
    Author:  Alexander Shabelnikov

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
/*
*/
class AdsrControlGroup  : public juce::Component
{
public:
    AdsrControlGroup();
    ~AdsrControlGroup() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AdsrControlGroup)
};
