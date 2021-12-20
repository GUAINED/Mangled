/*
  ==============================================================================

    FilterDragger.h
    Created: 24 Jun 2021 8:25:55pm
    Author:  CookiMonstor

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
/*
*/
class FilterDragger  : public juce::Component
{
public:
    FilterDragger();
    FilterDragger(juce::Colour colour);
    ~FilterDragger() override;
    
    void paint (juce::Graphics&) override;
    void resized() override;

    void setCircleColour(juce::Colour colour) { circleColour = colour; };
    void setIsSelected(bool newIsSelected)
    {
        isSelected = newIsSelected;
        //repaint();
    }

private:
    bool isSelected = false;

    juce::Colour circleColour = juce::Colours::cyan;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FilterDragger)
};
