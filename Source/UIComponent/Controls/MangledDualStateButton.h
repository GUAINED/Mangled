/*
  ==============================================================================

    MangledDualStateButton.h
    Created: 8 Jan 2022 8:06:48pm
    Author:  CookiMonstor

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "../../DataStructure/MangledIdentifiers.h"
#include "../LookAndFeel/LookAndFeel_V4_NeonGreen.h"

class MangledDualStateButton : public juce::TextButton
{
public:
    MangledDualStateButton(const juce::String& buttonName)
        : TextButton(buttonName)
    {
        setClickingTogglesState(true);
        setColour(juce::TextButton::ColourIds::buttonOnColourId, AudioEngineConstants::UI::darkNeonGreen);// AudioEngineConstants::UI::darkestNeonGreen);
        setColour(juce::TextButton::ColourIds::buttonColourId, juce::Colours::black);//.brighter(0.1f)
        //setColour(juce::TextButton::ColourIds::textColourOnId, AudioEngineConstants::UI::neonGreen);
        //setColour(juce::TextButton::ColourIds::textColourOffId, AudioEngineConstants::UI::neonGreen);
        setLookAndFeel(&laf);
        //setColour(juce::TextButton::ColourIds::textColourOnId, juce::Colours::black);
        //setColour(juce::TextButton::ColourIds::textColourOffId, AudioEngineConstants::UI::neonGreen);
    };

    ~MangledDualStateButton()
    {
        setLookAndFeel(nullptr);
    };
private:
    LookAndFeel_V4_NeonGreen laf;
};