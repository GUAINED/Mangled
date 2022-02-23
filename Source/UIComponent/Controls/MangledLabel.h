/*
  ==============================================================================

    MangledLabel.h
    Created: 22 Feb 2022 2:30:23pm
    Author:  CookiMonstor

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "../../DataStructure/MangledIdentifiers.h"
//==============================================================================
/*
*/
class MangledLabel  : public juce::Label
{
public:
    MangledLabel()
        : juce::Label()
    {
        setColour(juce::Label::ColourIds::textColourId, AudioEngineConstants::UI::neonGreen);
        setJustificationType(juce::Justification::centred);
    };
    MangledLabel(const juce::String& labelName)
        : juce::Label(labelName)
    {
        setColour(juce::Label::ColourIds::textColourId, AudioEngineConstants::UI::neonGreen);
        setJustificationType(juce::Justification::centred);
    };
    ~MangledLabel() override
    {

    };


private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MangledLabel)
};
