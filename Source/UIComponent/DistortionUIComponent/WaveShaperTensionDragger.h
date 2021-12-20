/*
  ==============================================================================

    WaveShaperTensionDragger.h
    Created: 16 Sep 2021 5:36:47pm
    Author:  CookiMonstor

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
/*
*/
class WaveShaperTensionDragger  : public juce::Component
{
public:
    WaveShaperTensionDragger(juce::Colour colour);
    ~WaveShaperTensionDragger() override;

    void paint (juce::Graphics&) override;
    void resized() override;
    void setColour(juce::Colour newColour) { colour = newColour; };
private:
    juce::Colour colour;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (WaveShaperTensionDragger)
};
