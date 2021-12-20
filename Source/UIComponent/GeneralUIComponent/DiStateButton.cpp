/*
  ==============================================================================

    DiStateButton.cpp
    Created: 2 Dec 2021 3:19:42pm
    Author:  CookiMonstor

  ==============================================================================
*/

#include "DiStateButton.h"
//==============================================================================
DiStateButton::DiStateButton(juce::String initButtonString = "", bool startingToogleState = false, juce::Colour trueColor = juce::Colours::black, juce::Colour falseColor = juce::Colours::darkgrey)
    : buttonString(initButtonString)
{
    setClickingTogglesState(true);
    setTrueColour(trueColor);
    setFalseColour(falseColor);
    setToggleState(startingToogleState, juce::dontSendNotification);

    setButtonText(buttonString);
};

DiStateButton::~DiStateButton()
{
    setLookAndFeel(nullptr);
}

void DiStateButton::setTrueColour(juce::Colour trueColour)
{
    setColour(juce::TextButton::ColourIds::buttonOnColourId, trueColour);
}

void DiStateButton::setFalseColour(juce::Colour falseColour)
{
    setColour(juce::TextButton::ColourIds::buttonColourId, falseColour);
}
