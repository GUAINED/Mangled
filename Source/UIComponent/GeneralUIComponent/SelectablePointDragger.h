/*
  ==============================================================================

    SelectablePointDragger.h
    Created: 26 Oct 2021 3:41:43pm
    Author:  CookiMonstor

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
/*
*/
class SelectablePointDragger  : public juce::Component
{
public:
    SelectablePointDragger();
    SelectablePointDragger(juce::Colour pointColour, juce::Colour circleColour);
    ~SelectablePointDragger() override;

    void paint (juce::Graphics&) override;
    void resized() override;

    void setIsSelected(bool newIsSelected) { isSelected = newIsSelected; };
    void setPointColour(juce::Colour newPointColour) { pointColour = newPointColour; };

    float getDiameter() { return diameter; };
private:
    juce::Colour pointColour;
    juce::Colour circleColour;
    float diameter = 9.0f;
    bool isSelected = false;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SelectablePointDragger)
};
