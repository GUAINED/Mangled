/*
  ==============================================================================

    FilterDragger.cpp
    Created: 24 Jun 2021 8:25:55pm
    Author:  CookiMonstor

  ==============================================================================
*/

#include <JuceHeader.h>
#include "FilterDragger.h"

//==============================================================================
FilterDragger::FilterDragger()
{
    setInterceptsMouseClicks(false, false);
}
FilterDragger::FilterDragger(juce::Colour colour)
{
    // In your constructor, you should add any child components, and
    // initialise any special settings that your component needs.
    circleColour = colour;
    setInterceptsMouseClicks(false, false);
}

FilterDragger::~FilterDragger()
{
}

void FilterDragger::paint (juce::Graphics& g)
{
    /* This demo code just fills the component's background and
       draws some placeholder text to get you started.

       You should replace everything in this method with your own
       drawing code..
    */

    //g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));   // clear the background
    float circleReducedBy = 3.0f;
    float diameter = 15.0f;
    g.setColour(circleColour);
    g.fillEllipse(0.0f, 0.0f, diameter, diameter);
    if (isSelected)
    {
        g.setColour(juce::Colours::ghostwhite);
        g.fillEllipse(0.0f, 0.0f, diameter, diameter);
        
        g.setColour(circleColour);
        g.fillEllipse(0.0f + circleReducedBy, 0.0f + circleReducedBy, diameter - 2.0f * circleReducedBy, diameter - 2.0f * circleReducedBy);
        //g.setColour(circleColour);
        //g.drawEllipse(0.0f, 0.0f, diameter, diameter, 0.5f);
        //g.fillEllipse(0.0f, 0.0f, diameter, diameter);
        //g.drawEllipse(0.0f - circleReducedBy, 0.0f - circleReducedBy, diameter - 2.0f*circleReducedBy, diameter - 2.0f*circleReducedBy, 0.5f);
    }
    else
    {
        g.setColour(circleColour);
        g.fillEllipse(0.0f, 0.0f, diameter, diameter);
        //g.setColour(circleColour);
        //g.fillEllipse(0.0f, 0.0f, diameter, diameter);
        //g.drawEllipse(0.0f, 0.0f, 20.0f, 20.0f, 0.5f);
    }



    //g.setColour (juce::Colours::white);
    //g.setFont (14.0f);
    //g.drawText ("FilterDragger", getLocalBounds(),
                //juce::Justification::centred, true);   // draw some placeholder text
}

void FilterDragger::resized()
{
    // This method is where you should set the bounds of any child
    // components that your component contains..

}
