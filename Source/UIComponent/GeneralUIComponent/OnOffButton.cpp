/*
  ==============================================================================

    OnOffButton.cpp
    Created: 24 Oct 2021 2:24:30pm
    Author:  CookiMonstor

  ==============================================================================
*/

#include "OnOffButton.h"
//==============================================================================
OnOffButton::OnOffButton(juce::String initButtonString = "", juce::String onOffStartString = "Off", bool startingToogleState = false)
    : buttonString(initButtonString)
    , onOffString(onOffStartString)
{
    setClickingTogglesState(true);
    setLookAndFeel(&lafGradient);
    setColour(juce::TextButton::ColourIds::buttonColourId, juce::Colours::lawngreen.brighter(0.2f)); 
    setColour(juce::TextButton::ColourIds::buttonOnColourId, juce::Colours::red);
    setToggleState(startingToogleState, juce::dontSendNotification);
    onClick = [this] {updateOnOffButton(); };

    juce::String stg = buttonString == "" ? "" 
                                          : buttonString + " ";
    setButtonText(stg + onOffString);
};

OnOffButton::~OnOffButton()
{
    setLookAndFeel(nullptr);
}

void OnOffButton::updateOnOffButton()
{
    auto state = getToggleState();
    juce::String stg = buttonString == "" ? "" : buttonString + " ";
    juce::String stateString = state ? "Off" : "On";

    setButtonText(stg + stateString);
}

void OnOffButton::setTrueColour(juce::Colour colourOn)
{
    setColour(juce::TextButton::ColourIds::buttonOnColourId, colourOn);
}

void OnOffButton::setFalseColour(juce::Colour colourOff)
{
    setColour(juce::TextButton::ColourIds::buttonColourId, colourOff);
}