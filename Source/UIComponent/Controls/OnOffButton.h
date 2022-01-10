/*
  ==============================================================================

    OnOffButton.h
    Created: 24 Oct 2021 2:24:30pm
    Author:  CookiMonstor

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "../LookAndFeel/LookAndFeel_V4_Gradient.h"
#include "../../DataStructure/MangledIdentifiers.h"
//==============================================================================
/*
*/

class OnOffButton  : public juce::TextButton
{
public:
    OnOffButton(juce::String initButtonString, juce::String onOffStartString, bool startingToogleState);

    ~OnOffButton() override;

    void updateOnOffButton();

    void setTrueColour(juce::Colour colourOn);

    void setFalseColour(juce::Colour colourOff);

private:
    juce::String buttonString;
    juce::String onOffString;

    LookAndFeel_V4_Gradient lafGradient;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (OnOffButton)
};