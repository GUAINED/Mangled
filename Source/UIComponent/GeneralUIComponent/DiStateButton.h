/*
  ==============================================================================

    DiStateButton.h
    Created: 2 Dec 2021 3:19:42pm
    Author:  CookiMonstor

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
//==============================================================================
/*
*/

class DiStateButton : public juce::TextButton
{
public:
    DiStateButton(juce::String initButtonString, bool startingToogleState, juce::Colour trueColor, juce::Colour falseColor);

    ~DiStateButton() override;

    void setTrueColour(juce::Colour trueColour);

    void setFalseColour(juce::Colour falseColour);

private:
    juce::String buttonString;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DiStateButton)
};