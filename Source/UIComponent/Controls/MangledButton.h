/*
  ==============================================================================

    MangledButton.h
    Created: 21 Feb 2022 5:48:20pm
    Author:  CookiMonstor

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "../LookAndFeel/LookAndFeel_V4_NeonGreen.h"
//==============================================================================
/*
*/
class MangledButton  : public juce::TextButton
{
public:
    MangledButton()
    {
        setLookAndFeel(&laf);
        setColour(juce::TextButton::ColourIds::buttonColourId, AudioEngineConstants::UI::darkestNeonGreen);
        //setColour(juce::TextButton::ColourIds::textColourOnId, AudioEngineConstants::UI::neonGreen);
        //setColour(juce::TextButton::ColourIds::textColourOffId, AudioEngineConstants::UI::neonGreen);
    };

    MangledButton(const juce::String& buttonName)
        : TextButton(buttonName)
    {
        setLookAndFeel(&laf);
        setColour(juce::TextButton::ColourIds::buttonColourId, AudioEngineConstants::UI::darkestNeonGreen);
        //setColour(juce::TextButton::ColourIds::textColourOnId, AudioEngineConstants::UI::neonGreen);
        //setColour(juce::TextButton::ColourIds::textColourOffId, AudioEngineConstants::UI::neonGreen);
    };

    ~MangledButton() override
    {
        setLookAndFeel(nullptr);
    };

private:
    LookAndFeel_V4_NeonGreen laf;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MangledButton)
};
