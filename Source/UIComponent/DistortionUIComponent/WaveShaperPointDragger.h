/*
  ==============================================================================

    WaveShaperPointDragger.h
    Created: 16 Sep 2021 5:17:37pm
    Author:  CookiMonstor

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
/*
*/
class WaveShaperPointDragger  : public juce::Component
{
public:
    WaveShaperPointDragger();
    WaveShaperPointDragger(juce::Colour circleColour);
    //WaveShaperPointDragger(const WaveShaperPointDragger& obj);
    ~WaveShaperPointDragger() override;

    void paint (juce::Graphics&) override;
    void resized() override;

    void setIsSelected(bool newIsSelected) {
        isSelected = newIsSelected;
        //isSelected ? colour = juce::Colours::orange : colour = juce::Colours::blue;
    };
    void setColour(juce::Colour newColour) { colour = newColour; };

private:
    juce::Colour colour;
    bool isSelected = false;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (WaveShaperPointDragger)
};
