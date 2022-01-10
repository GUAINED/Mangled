/*
  ==============================================================================

    MainLayerMenuComponent.cpp
    Created: 15 May 2021 10:47:17pm
    Author:  CookiMonstor

  ==============================================================================
*/

#include <JuceHeader.h>
#include "MainLayerMenuComponent.h"

//==============================================================================
MainLayerMenuComponent::MainLayerMenuComponent(MainLayerDataStruct& mlDataStruct)
    : dataStruct(mlDataStruct)
{
    setInterceptsMouseClicks(false, true);

    //addAndMakeVisible(stageMenuTab);

    for (int stageID = 0; stageID < MainLayerConstants::Processor<float>::nbOfStageMax; ++stageID)
    {
        stageComponentVector.add(new StageComponent(dataStruct));
        addChildComponent(stageComponentVector[stageID]);

        //getStageMenuTab()->getStageIDButton(stageID)->addListener(this);
    }
    
    switchToStageMenu(0);

    stageOnOffButtonVector.add(new OnOffButton("", "On", StageConstants::Processor<float>::isBypassedStartValue));
    stageOnOffButtonVector.add(new OnOffButton("", "Off", !StageConstants::Processor<float>::isBypassedStartValue));
    stageOnOffButtonVector.add(new OnOffButton("", "Off", !StageConstants::Processor<float>::isBypassedStartValue));
    stageOnOffButtonVector.add(new OnOffButton("", "Off", !StageConstants::Processor<float>::isBypassedStartValue));

    for (int stageID = 0; stageID < MainLayerConstants::Processor<float>::nbOfStageMax; ++stageID)
    {
        stageIDButtonVector.add(new MangledDualStateButton("Stage " + (juce::String)(stageID + 1)));
        //stageIDButtonVector[stageID]->setClickingTogglesState(true);
        //stageIDButtonVector[stageID]->setColour(juce::TextButton::ColourIds::buttonColourId, juce::Colours::grey);
        //stageIDButtonVector[stageID]->setColour(juce::TextButton::ColourIds::buttonOnColourId, juce::Colours::black);
        stageIDButtonVector[stageID]->setRadioGroupId(StageButtons, juce::dontSendNotification);
        stageIDButtonVector[stageID]->addListener(this);
        addAndMakeVisible(stageIDButtonVector[stageID]);
        addAndMakeVisible(stageOnOffButtonVector[stageID]);
    }

    stageIDButtonVector[0]->setToggleState(true, juce::dontSendNotification);
}

MainLayerMenuComponent::~MainLayerMenuComponent()
{
}

void MainLayerMenuComponent::paint (juce::Graphics& g)
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
    //g.drawText ("MainLayerMenuComponent", getLocalBounds(),
    //            juce::Justification::centred, true);   // draw some placeholder text
}

void MainLayerMenuComponent::resized()
{
    //int rotaryPixelSize = 75;
    //int labelWidth = rotaryPixelSize;
    int labelHeight = 20;
    //int pixelSpace = 10;

    //int phaserHeight = 2 * pixelSpace + labelHeight + rotaryPixelSize;

    //stageMenuTab.setBounds(0, 0, getLocalBounds().getWidth(), labelHeight);

    auto width = getLocalBounds().getWidth();
    auto height = labelHeight;
    auto buttonIDWidth = (width - 5 * (height + 10 + 20)) / 5;
    auto buttonOnOffWidth = height + 10;
    int offset = 20;

    for (int stageID = 0; stageID < stageOnOffButtonVector.size(); ++stageID)
    {
        stageOnOffButtonVector[stageID]->setBounds(stageID * (buttonIDWidth + buttonOnOffWidth + offset), 0, buttonOnOffWidth, height);
        stageIDButtonVector[stageID]->setBounds(stageID * (buttonIDWidth + buttonOnOffWidth + offset) + buttonOnOffWidth, 0, buttonIDWidth, height);
    }

    for (int stageID = 0; stageID < MainLayerConstants::Processor<float>::nbOfStageMax; ++stageID)
    {
        auto yolo = getHeight() - labelHeight;
        stageComponentVector[stageID]->setBounds(stageOnOffButtonVector[0]->getX(), stageOnOffButtonVector[0]->getBottom(), getWidth(), yolo);
    }


}

void MainLayerMenuComponent::switchToStageMenu(int stageID)
{

    for (int i = 0; i < MainLayerConstants::Processor<float>::nbOfStageMax; ++i)
    {
        stageComponentVector[i]->setVisible(false);
        //stageComponentVector[i]->getScope()->getScopeDisplay()->setIsDisplayed(false);
    }

    //stageComponentVector[stageID]->getScope()->getScopeDisplay()->setIsDisplayed(true);
    stageComponentVector[stageID]->setVisible(true);
}

void MainLayerMenuComponent::buttonClicked(juce::Button* button)
{
    if (button == getStageIDButton(0))
    {
        switchToStageMenu(0);
        dataStruct.setSelectedStageID(0);
    }
    else if (button == getStageIDButton(1))
    {
        switchToStageMenu(1);
        dataStruct.setSelectedStageID(1);
    }
    else if (button == getStageIDButton(2))
    {
        switchToStageMenu(2);
        dataStruct.setSelectedStageID(2);
    }
    else if (button == getStageIDButton(3))
    {
        switchToStageMenu(3);
        dataStruct.setSelectedStageID(3);
    }
}

void MainLayerMenuComponent::setButtonOn(int stageID, juce::NotificationType notifType)
{
    if (stageID < MainLayerConstants::Processor<float>::nbOfStageMax)
    {
        stageIDButtonVector[stageID]->setToggleState(true, notifType);
    }
    else
    {
        //postProcessingButton.setToggleState(true, juce::sendNotification);
    }
}
