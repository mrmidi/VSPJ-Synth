/*
  ==============================================================================

    FilterControlGroup.h
    Created: 9 Sep 2023 7:30:55pm
    Author:  Alexander Shabelnikov

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
/*
*/
class FilterControlGroup  : public juce::Component
{
public:
    FilterControlGroup();
    ~FilterControlGroup() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FilterControlGroup)
};
