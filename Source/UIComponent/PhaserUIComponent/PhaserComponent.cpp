/*
  ==============================================================================

    PhaserComponent.cpp
    Created: 8 Apr 2021 7:24:29pm
    Author:  CookiMonstor

  ==============================================================================
*/

#include <JuceHeader.h>
#include "PhaserComponent.h"

//==============================================================================
PhaserComponent::PhaserComponent(MainLayerDataStruct& mlDataStruct)
    : dataStruct(mlDataStruct)
    , phaserOnOffButton("", "On", PhaserConstants::Processor<float>::isBypassedStartValue)
    , resetButton("Reset")
{
    //Phaser===========================================================================
    //Centre Freq Phaser
    centreFrequencyPhaserSlider.setSliderStyle(juce::Slider::SliderStyle::Rotary);
    centreFrequencyPhaserSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 100, 20);
    centreFrequencyPhaserSlider.setTextBoxIsEditable(true);
    centreFrequencyPhaserLabel.setText(PhaserConstants::AutomationString::centerFrequency, juce::dontSendNotification);
    centreFrequencyPhaserLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(centreFrequencyPhaserSlider);
    addAndMakeVisible(centreFrequencyPhaserLabel);

    //Feedback Phaser
    feedbackPhaserSlider.setSliderStyle(juce::Slider::SliderStyle::Rotary);
    feedbackPhaserSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 100, 20);
    feedbackPhaserSlider.setTextBoxIsEditable(true);
    feedbackPhaserLabel.setText(PhaserConstants::AutomationString::feedback, juce::dontSendNotification);
    feedbackPhaserLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(feedbackPhaserSlider);
    addAndMakeVisible(feedbackPhaserLabel);

    //Rate Phaser
    ratePhaserSlider.setSliderStyle(juce::Slider::SliderStyle::Rotary);
    ratePhaserSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 100, 20);
    ratePhaserLabel.setText(PhaserConstants::AutomationString::rate, juce::dontSendNotification);
    ratePhaserLabel.setJustificationType(juce::Justification::centred);
    ratePhaserSlider.setEnabled(false);

    addAndMakeVisible(ratePhaserSlider);
    addAndMakeVisible(ratePhaserLabel);

    //Depth Phaser
    depthPhaserSlider.setSliderStyle(juce::Slider::SliderStyle::Rotary);
    depthPhaserSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 100, 20);
    depthPhaserLabel.setText(PhaserConstants::AutomationString::depth, juce::dontSendNotification);
    depthPhaserLabel.setJustificationType(juce::Justification::centred);
    depthPhaserSlider.setEnabled(false);

    addAndMakeVisible(depthPhaserSlider);
    addAndMakeVisible(depthPhaserLabel);

    //Number Of Stages Phaser
    numStagesPhaserLabel.setText(PhaserConstants::AutomationString::nbOfStages, juce::dontSendNotification);

    for (int index = 0; index < PhaserConstants::UI::nbOfStgStringArray.size(); ++index)
    {
        numStagesPhaserComboBox.addItem(PhaserConstants::UI::nbOfStgStringArray[index], index + 1);
    }
    numStagesPhaserComboBox.setSelectedId(2, juce::dontSendNotification);

    addAndMakeVisible(numStagesPhaserLabel);
    addAndMakeVisible(numStagesPhaserComboBox);

    //Mix Phaser
    mixPhaserSlider.setSliderStyle(juce::Slider::SliderStyle::LinearHorizontal);
    mixPhaserSlider.setTextBoxStyle(juce::Slider::TextBoxRight, true, 50, 20);
    mixPhaserLabel.setText(PhaserConstants::AutomationString::mix, juce::dontSendNotification);
    mixPhaserLabel.setJustificationType(juce::Justification::centred);

    addAndMakeVisible(mixPhaserSlider);
    //addAndMakeVisible(mixPhaserLabel);

    addAndMakeVisible(phaserOnOffButton);

    resetButton.addListener(this);
    addAndMakeVisible(resetButton);
}

PhaserComponent::~PhaserComponent()
{
}

void PhaserComponent::paint (juce::Graphics& g)
{
    /* This demo code just fills the component's background and
       draws some placeholder text to get you started.

       You should replace everything in this method with your own
       drawing code..
    */

    //g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));   // clear the background
    g.fillAll(juce::Colours::transparentBlack);
    g.setColour (juce::Colours::grey);
    g.drawRect (getLocalBounds(), 1);   // draw an outline around the component

    //g.setColour (juce::Colours::white);
    //g.setFont (14.0f);
    //g.drawText ("PhaserComponent", getLocalBounds(),
    //            juce::Justification::centred, true);   // draw some placeholder text
}

void PhaserComponent::resized()
{
    int rotaryPixelSize = 75;
    int labelWidth = rotaryPixelSize;
    int labelHeight = 20;
    int pixelSpace = 10;
    auto buttonOnOffWidth = labelHeight + 10;
    numStagesPhaserLabel.setBounds(pixelSpace, pixelSpace , 2 * labelWidth, labelHeight);
    numStagesPhaserComboBox.setBounds(numStagesPhaserLabel.getX(), numStagesPhaserLabel.getBottom(), 2 * labelWidth, labelHeight);

    centreFrequencyPhaserLabel.setBounds(numStagesPhaserLabel.getRight() + pixelSpace, numStagesPhaserLabel.getY(), labelWidth, labelHeight);
    centreFrequencyPhaserSlider.setBounds(centreFrequencyPhaserLabel.getX(), centreFrequencyPhaserLabel.getBottom(), rotaryPixelSize, rotaryPixelSize);

    feedbackPhaserLabel.setBounds(centreFrequencyPhaserLabel.getRight() + pixelSpace, centreFrequencyPhaserLabel.getY(), labelWidth, labelHeight);
    feedbackPhaserSlider.setBounds(centreFrequencyPhaserSlider.getRight() + pixelSpace, feedbackPhaserLabel.getBottom(), rotaryPixelSize, rotaryPixelSize);

    ratePhaserLabel.setBounds(feedbackPhaserLabel.getRight() + pixelSpace, feedbackPhaserLabel.getY(), labelWidth, labelHeight);
    ratePhaserSlider.setBounds(feedbackPhaserSlider.getRight() + pixelSpace, ratePhaserLabel.getBottom(), rotaryPixelSize, rotaryPixelSize);

    depthPhaserLabel.setBounds(ratePhaserLabel.getRight() + pixelSpace, ratePhaserLabel.getY(), labelWidth, labelHeight);
    depthPhaserSlider.setBounds(ratePhaserSlider.getRight() + pixelSpace, depthPhaserLabel.getBottom(), rotaryPixelSize, rotaryPixelSize);

    mixPhaserSlider.setBounds(getRight() - 2 * labelWidth - pixelSpace, depthPhaserLabel.getY(), 2 * labelWidth, labelHeight);

    phaserOnOffButton.setBounds(mixPhaserSlider.getX() - 1 - buttonOnOffWidth, depthPhaserLabel.getY(), buttonOnOffWidth, labelHeight);

    resetButton.setBounds(phaserOnOffButton.getX(), phaserOnOffButton.getBottom(), labelWidth,labelHeight);
}

void PhaserComponent::buttonClicked(juce::Button* button)
{
    int stageID = dataStruct.getSelectedStageID();

    if (button == getResetButton())
    {
        dataStruct.resetPhaserParam(stageID);
        numStagesPhaserComboBox.setSelectedId(PhaserConstants::Processor<float>::nbOfStgStartValue + 1, juce::dontSendNotification);
    }
}
