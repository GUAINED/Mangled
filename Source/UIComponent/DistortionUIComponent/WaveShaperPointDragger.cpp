/*
  ==============================================================================

    WaveShaperPointDragger.cpp
    Created: 16 Sep 2021 5:17:37pm
    Author:  CookiMonstor

  ==============================================================================
*/

#include <JuceHeader.h>
#include "WaveShaperPointDragger.h"

//==============================================================================
WaveShaperPointDragger::WaveShaperPointDragger()
{
    isSelected = false;
    colour = juce::Colours::blue;
    setInterceptsMouseClicks(false, false);
}

WaveShaperPointDragger::WaveShaperPointDragger(juce::Colour circleColour)
    : colour(circleColour)
{
    setInterceptsMouseClicks(false, false);
}

//WaveShaperPointDragger::WaveShaperPointDragger(const WaveShaperPointDragger& obj)
//{
//    colour = obj.colour;
//    isSelected = obj.isSelected;
//}

WaveShaperPointDragger::~WaveShaperPointDragger()
{
}

void WaveShaperPointDragger::paint (juce::Graphics& g)
{
    /* This demo code just fills the component's background and
       draws some placeholder text to get you started.

       You should replace everything in this method with your own
       drawing code..
    */
    float diameter = 10.0f;
    g.setColour(colour);
    g.fillEllipse(0.0f, 0.0f, diameter, diameter);

    if (isSelected)
    {
        g.setColour(juce::Colours::orange);
        g.fillEllipse(1.5f, 1.5f, diameter - 3.0f, diameter - 3.0f);
    }
}

void WaveShaperPointDragger::resized()
{
    // This method is where you should set the bounds of any child
    // components that your component contains..

}
