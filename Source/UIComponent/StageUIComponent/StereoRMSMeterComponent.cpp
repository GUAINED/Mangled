/*
  ==============================================================================

    StereoRMSMeterComponent.cpp
    Created: 18 Nov 2021 10:57:19pm
    Author:  CookiMonstor

  ==============================================================================
*/

#include <JuceHeader.h>
#include "StereoRMSMeterComponent.h"

//==============================================================================
StereoRMSMeterComponent::StereoRMSMeterComponent(juce::String initString)
    : leftRMSMeter(gradient)
    , rightRMSMeter(gradient)
{
    addAndMakeVisible(leftRMSMeter);
    addAndMakeVisible(rightRMSMeter);

    gainSlider.setSliderStyle(juce::Slider::SliderStyle::LinearVertical);
    gainSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 50, 20);
    gainSlider.setTextBoxIsEditable(true);
    addAndMakeVisible(gainSlider);

    gainLabel.setText(initString, juce::dontSendNotification);
    //gainLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(gainLabel);

    leftPeakLabel.setJustificationType(juce::Justification::centred);
    leftPeakLabel.setText("Peak", juce::dontSendNotification);
    addAndMakeVisible(leftPeakLabel);

    rightPeakLabel.setJustificationType(juce::Justification::centred);
    rightPeakLabel.setText("Peak", juce::dontSendNotification);
    addAndMakeVisible(rightPeakLabel);

    juce::Font font = leftRMSLabel.getFont();
    font.setDefaultMinimumHorizontalScaleFactor(0.3f);
    leftRMSLabel.setFont(font);
    leftRMSLabel.setJustificationType(juce::Justification::centred);
    leftRMSLabel.setText("Left", juce::dontSendNotification);
    addAndMakeVisible(leftRMSLabel);

    rightRMSLabel.setJustificationType(juce::Justification::centred);
    rightRMSLabel.setText("Right", juce::dontSendNotification);
    addAndMakeVisible(rightRMSLabel);

}

StereoRMSMeterComponent::~StereoRMSMeterComponent()
{
}

void StereoRMSMeterComponent::paint (juce::Graphics& g)
{
}

void StereoRMSMeterComponent::resized()
{
    gainLabel.setBounds(0, 0, getWidth(), 20);

    leftPeakLabel.setBounds(0, gainLabel.getBottom(), getWidth() / 2, 20);
    rightPeakLabel.setBounds(leftPeakLabel.getRight(), leftPeakLabel.getY(), getWidth() / 2, 20);

    leftRMSLabel.setBounds(0, leftPeakLabel.getBottom(), getWidth() / 2, 20);
    rightRMSLabel.setBounds(leftRMSLabel.getRight(), leftRMSLabel.getY(), getWidth() / 2, 20);

    auto offsetY = 0;
    auto bounds = juce::Rectangle<int>(5, leftRMSLabel.getBottom() + offsetY, getWidth() / 2 - 10, getHeight() - 80 - 2*offsetY - 5);
    //rmsMeter.setBounds(gainLabel.getX(), leftRMSLabel.getBottom(), getWidth(), getHeight() - 40);
    leftRMSMeter.setBounds(bounds);

    auto offsetX = 6;
    bounds = juce::Rectangle<int>(getWidth() / 2 + offsetX, leftRMSLabel.getBottom() + offsetY, leftRMSMeter.getWidth(),leftRMSMeter.getHeight());

    rightRMSMeter.setBounds(bounds);

    gainSlider.setBounds(gainLabel.getX(), leftRMSLabel.getBottom(), getWidth(), getHeight() - 60);

    auto gradBounds = leftRMSMeter.getLocalBounds().toFloat();
    gradient = juce::ColourGradient{ juce::Colours::black, gradBounds.getBottomLeft(), juce::Colours::darkred.darker(0.3f), gradBounds.getTopLeft(), false };
    double val = juce::jmap(0.0, -100.0, 12.0, 0.0, 1.0);
    gradient.addColour(val, juce::Colours::red);
    val = juce::jmap(-0.01, -100.0, 12.0, 0.0, 1.0);
    gradient.addColour(val, juce::Colours::lawngreen);
}
