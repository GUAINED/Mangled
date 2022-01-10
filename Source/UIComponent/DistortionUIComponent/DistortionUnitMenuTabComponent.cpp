/*
  ==============================================================================

    DistortionUnitMenuTabComponent.cpp
    Created: 2 Jun 2021 11:25:48pm
    Author:  CookiMonstor

  ==============================================================================
*/

#include <JuceHeader.h>
#include "DistortionUnitMenuTabComponent.h"

//==============================================================================
DistortionUnitMenuTabComponent::DistortionUnitMenuTabComponent()
    : onOffButton("Distortion", "On", DistoUnitConstants::Processor<float>::isBypassedStartValue)
{
    juce::String buttonIDString;

    distoUnitOnOffButtonVector.add(new OnOffButton("", "On", false));
    distoUnitOnOffButtonVector.add(new OnOffButton("", "Off", true));

    for (int distortionUnitID = 0; distortionUnitID < DistortionConstants::Processor<float>::nbOfDUMax; ++distortionUnitID)
    {
        buttonIDString = DistortionConstants::UI::distoUnitString + (juce::String) (distortionUnitID + 1);
        distoUnitIDButtonVector.add(new MangledDualStateButton(buttonIDString));
        //distoUnitIDButtonVector[distortionUnitID]->setClickingTogglesState(true);
        //distoUnitIDButtonVector[distortionUnitID]->setColour(juce::TextButton::ColourIds::buttonColourId, juce::Colours::grey);
        //distoUnitIDButtonVector[distortionUnitID]->setColour(juce::TextButton::ColourIds::buttonOnColourId, juce::Colours::black);
        distoUnitIDButtonVector[distortionUnitID]->setRadioGroupId(DistortionUnitButtons);

        addAndMakeVisible(distoUnitIDButtonVector[distortionUnitID]);

        addAndMakeVisible(distoUnitOnOffButtonVector[distortionUnitID]);
    }

    distoUnitIDButtonVector[0]->setToggleState(true, juce::dontSendNotification);

    //Bypass
    //isBypassedDistortionProcessorTextButton.setClickingTogglesState(true);
    //isBypassedDistortionProcessorTextButton.setButtonText("BYPASS ALL OFF");
    //isBypassedDistortionProcessorTextButton.setColour(juce::TextButton::ColourIds::buttonColourId, juce::Colours::red);
    //isBypassedDistortionProcessorTextButton.setColour(juce::TextButton::ColourIds::buttonOnColourId, juce::Colours::green);
    //isBypassedDistortionProcessorTextButton.onClick = [this] {updateOnOffButton(&isBypassedDistortionProcessorTextButton, "BYPASS ALL "); };
    //isBypassedDistortionProcessorTextButton.setToggleState(false, juce::dontSendNotification);

    //addAndMakeVisible(isBypassedDistortionProcessorTextButton);

    addAndMakeVisible(onOffButton);

    oversamplerOnOffButton.setClickingTogglesState(true);
    oversamplerOnOffButton.setButtonText("OS OFF");
    oversamplerOnOffButton.setColour(juce::TextButton::ColourIds::buttonColourId, juce::Colours::red);
    oversamplerOnOffButton.setColour(juce::TextButton::ColourIds::buttonOnColourId, juce::Colours::green);
    oversamplerOnOffButton.onClick = [this] {updateOnOffButton(&oversamplerOnOffButton, "OS "); };
    oversamplerOnOffButton.setToggleState(false, juce::dontSendNotification);
    oversamplerOnOffButton.setEnabled(false);
    addAndMakeVisible(oversamplerOnOffButton);

    //Mix Distortion
    mixDistortionSlider.setSliderStyle(juce::Slider::SliderStyle::LinearHorizontal);
    mixDistortionSlider.setTextBoxStyle(juce::Slider::TextBoxRight, true, 50, 20);
    mixDistortionLabel.setText("Mix Distortion: ", juce::dontSendNotification);
    mixDistortionLabel.setJustificationType(juce::Justification::centred);

    addAndMakeVisible(mixDistortionSlider);
    addAndMakeVisible(mixDistortionLabel);

}

DistortionUnitMenuTabComponent::~DistortionUnitMenuTabComponent()
{
}

void DistortionUnitMenuTabComponent::paint (juce::Graphics& g)
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

}

void DistortionUnitMenuTabComponent::resized()
{
    auto height = getHeight();
    int labelWidth = DistortionConstants::UI::labelWidth;
    int labelHeight = DistortionConstants::UI::labelHeight;
    auto buttonOnOffWidth = height + 10;
    int pixelSpace = DistortionConstants::UI::pixelSpace;

    distoUnitOnOffButtonVector[0]->setBounds(0, 0, buttonOnOffWidth, labelHeight);
    distoUnitIDButtonVector[0]->setBounds(distoUnitOnOffButtonVector[0]->getRight() + 1, distoUnitOnOffButtonVector[0]->getY(), labelWidth - 2 * pixelSpace, labelHeight);

    distoUnitOnOffButtonVector[1]->setBounds(distoUnitIDButtonVector[0]->getRight() + pixelSpace, distoUnitIDButtonVector[0]->getY(), buttonOnOffWidth, labelHeight);
    distoUnitIDButtonVector[1]->setBounds(distoUnitOnOffButtonVector[1]->getRight() + 1, distoUnitOnOffButtonVector[1]->getY(), labelWidth - 2 * pixelSpace, labelHeight);


    mixDistortionSlider.setBounds(getRight() - pixelSpace - 2 * labelWidth, distoUnitOnOffButtonVector[0]->getY(), 2 * labelWidth, labelHeight);
    onOffButton.setBounds(mixDistortionSlider.getX() - 1 - (labelWidth + labelHeight), distoUnitIDButtonVector[1]->getY(), labelWidth + labelHeight, labelHeight);
    oversamplerOnOffButton.setBounds(onOffButton.getX() - 1 - labelWidth, distoUnitIDButtonVector[1]->getY(), labelWidth, labelHeight);
    
}

void DistortionUnitMenuTabComponent::updateOnOffButton(juce::Button* button, juce::String addedString)
{
    auto state = button->getToggleState();
    juce::String stateString = state ? "ON" : "OFF";

    button->setButtonText(addedString + stateString);
}
