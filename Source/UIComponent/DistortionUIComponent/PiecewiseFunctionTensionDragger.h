/*
  ==============================================================================

    PiecewiseFunctionTensionDragger.h
    Created: 16 Sep 2021 5:36:47pm
    Author:  CookiMonstor

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
/*
*/
class PiecewiseFunctionTensionDragger  : public juce::Component
{
public:
    PiecewiseFunctionTensionDragger(juce::Colour colour);
    ~PiecewiseFunctionTensionDragger() override;

    void paint (juce::Graphics&) override;
    void resized() override;
    void setColour(juce::Colour newColour) { colour = newColour; };
private:
    juce::Colour colour;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PiecewiseFunctionTensionDragger)
};
