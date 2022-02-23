/*
  ==============================================================================

    PiecewiseFunctionTensionDragger.cpp
    Created: 16 Sep 2021 5:36:47pm
    Author:  CookiMonstor

  ==============================================================================
*/

#include "PiecewiseFunctionTensionDragger.h"

//==============================================================================
PiecewiseFunctionTensionDragger::PiecewiseFunctionTensionDragger(juce::Colour circleColour):
    colour(circleColour)
{
    setInterceptsMouseClicks(false, false);
}

PiecewiseFunctionTensionDragger::~PiecewiseFunctionTensionDragger()
{
}

void PiecewiseFunctionTensionDragger::paint (juce::Graphics& g)
{
    float diameter = 10.0f - 1.0f;
    g.setColour(colour);
    //g.fillEllipse(0.0f, 0.0f, diameter, diameter);
    //g.drawRect(0.0, 0.0, diameter, diameter);
    //g.drawEllipse(juce::Point<float>(5.0f, 5.0f));
    g.drawEllipse(1.0f, 1.0f, diameter, diameter, 1.0f);
}

void PiecewiseFunctionTensionDragger::resized()
{
    // This method is where you should set the bounds of any child
    // components that your component contains..

}
