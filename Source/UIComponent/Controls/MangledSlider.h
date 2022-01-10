/*
  ==============================================================================

    MangledSlider.h
    Created: 7 Jan 2022 10:37:34pm
    Author:  CookiMonstor

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "../LookAndFeel/LookAndFeel_V4_NeonGreen.h"

class MangledSlider : public juce::Slider
{
public:
    MangledSlider()
    {
        setLookAndFeel(&laf);
        setColour(juce::Slider::ColourIds::textBoxOutlineColourId, AudioEngineConstants::UI::darkNeonGreen);
        setColour(juce::Slider::ColourIds::textBoxTextColourId, AudioEngineConstants::UI::neonGreen);
        //setColour(juce::Slider::ColourIds::textBoxHighlightColourId, AudioEngineConstants::UI::darkerNeonGreen);
        setColour(juce::Slider::ColourIds::textBoxBackgroundColourId, AudioEngineConstants::UI::darkestNeonGreen);
        //setSliderStyle(juce::Slider::SliderStyle::LinearBar);
        setColour(juce::Label::ColourIds::textColourId, AudioEngineConstants::UI::neonGreen);
    };

    ~MangledSlider()
    {
        setLookAndFeel(nullptr);
    }

private:
    LookAndFeel_V4_NeonGreen laf;
};