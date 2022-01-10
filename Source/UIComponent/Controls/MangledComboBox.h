/*
  ==============================================================================

    MangledComboBox.h
    Created: 8 Jan 2022 12:00:19am
    Author:  CookiMonstor

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "../LookAndFeel/LookAndFeel_V4_NeonGreen.h"

class MangledComboBox : public juce::ComboBox
{
public:
    MangledComboBox()
    {
        setLookAndFeel(&laf);
        //setColour(juce::ComboBox::backgroundColourId, juce::Colours::black);
        
    };

    ~MangledComboBox()
    {
        setLookAndFeel(nullptr);
    };

private:
    LookAndFeel_V4_NeonGreen laf;
};
