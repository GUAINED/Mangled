/*
  ==============================================================================

    RMSMeter.cpp
    Created: 18 Nov 2021 8:30:15pm
    Author:  CookiMonstor

  ==============================================================================
*/

#include <JuceHeader.h>
#include "RMSMeter.h"

//==============================================================================
RMSMeter::RMSMeter(juce::ColourGradient& grad)
    : gradient(grad)
{
    rmsValue = 0.0f;
    peakValue = 0.0f;
}

RMSMeter::~RMSMeter()
{
}

void RMSMeter::paint (juce::Graphics& g)
{
    auto scaledY = juce::jmap(rmsValue, -100.f, 12.0f, static_cast<float>(getHeight()), 0.0f);
    auto bounds = juce::Rectangle<int>(0, 0, getWidth(), getHeight());
    auto rmsBounds = juce::Rectangle<int>(0, (int) scaledY, getWidth(), getHeight());
    g.setColour(juce::Colours::black);
    g.fillRect(bounds);
    g.setGradientFill(gradient);
    g.fillRect(rmsBounds);

    auto scaledPeak = juce::jmap(peakValue, -100.f, 12.0f, static_cast<float>(getHeight()), 0.0f);
    rmsBounds = juce::Rectangle<int>(0, (int)scaledPeak, getWidth(), 1);
    //g.setColour(juce::Colours::cyan);
    g.fillRect(rmsBounds);

    //g.setGradientFill(gradient);
    rmsBounds = juce::Rectangle<int>(0, rmsBounds.getBottom(), getWidth(), (int)scaledY);
    g.setOpacity(0.2f);
    g.fillRect(rmsBounds);
}

void RMSMeter::resized()
{
    //const auto bounds = getLocalBounds().toFloat();
    //gradient = juce::ColourGradient{ juce::Colours::purple, bounds.getBottomLeft(), juce::Colours::red, bounds.getTopLeft(), false };
    //double val = juce::jmap(0.0, -100.0, 12.0, 0.0, 1.0);
    //gradient.addColour(val, juce::Colours::cyan);
    
    ////auto lim = std::pow(10.0, -0.01);
    //const auto bounds = getLocalBounds().toFloat();
    //gradient = juce::ColourGradient{ juce::Colours::black, bounds.getBottomLeft(), juce::Colours::darkred, bounds.getTopLeft(), false };
    //double val = juce::jmap(0.0, -100.0, 12.0, 0.0, 1.0);
    //gradient.addColour(val, juce::Colours::red);
    //val = juce::jmap(-0.01, -100.0, 12.0, 0.0, 1.0);
    //gradient.addColour(val, juce::Colours::lawngreen);
}
