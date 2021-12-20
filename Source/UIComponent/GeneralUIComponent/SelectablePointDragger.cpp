/*
  ==============================================================================

    SelectablePointDragger.cpp
    Created: 26 Oct 2021 3:41:43pm
    Author:  CookiMonstor

  ==============================================================================
*/

#include <JuceHeader.h>
#include "SelectablePointDragger.h"

//==============================================================================
SelectablePointDragger::SelectablePointDragger()
{
    isSelected = false;
    pointColour = juce::Colours::blue;
    circleColour = juce::Colours::orange;
    setInterceptsMouseClicks(false, false);
}

SelectablePointDragger::SelectablePointDragger(juce::Colour pointColour, juce::Colour circleColour)
    : pointColour(pointColour)
    , circleColour(circleColour)
{
    setInterceptsMouseClicks(false, false);
}

SelectablePointDragger::~SelectablePointDragger()
{
}

void SelectablePointDragger::paint (juce::Graphics& g)
{
    g.setColour(pointColour);
    g.fillEllipse(1.0f, 1.0f, diameter, diameter);

    if (isSelected)
    {
        g.setColour(circleColour);
        g.fillEllipse(2.5f, 2.5f, diameter - 3.0f, diameter - 3.0f);
    }
}

void SelectablePointDragger::resized()
{
    // This method is where you should set the bounds of any child
    // components that your component contains..

}
