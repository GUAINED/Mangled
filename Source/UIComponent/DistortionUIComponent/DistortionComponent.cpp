/*
  ==============================================================================

    WaveShaperComponent.cpp
    Created: 8 Apr 2021 7:34:55pm
    Author:  CookiMonstor

  ==============================================================================
*/

#include <JuceHeader.h>
#include "DistortionComponent.h"

//==============================================================================
DistortionComponent::DistortionComponent(MainLayerDataStruct& mlDataStruct)
    : dataStruct(mlDataStruct)
{

    setInterceptsMouseClicks(false, true);

    for (int distortionUnit = 0; distortionUnit < DistortionConstants::Processor<float>::nbOfDUMax; ++distortionUnit)
    {
        distortionSliderComponent.add(new DistortionSliderComponent(mlDataStruct));
        addChildComponent(distortionSliderComponent[distortionUnit]);
        //distortionSliderComponent[distortionUnit]->getDistortionEquationWaveShaperComboBox(true)->addListener(this);
        //distortionSliderComponent[distortionUnit]->getDriveWaveShaperSlider()->addListener(this);
    }
    
    //distortionSliderComponent[0]->getBipolarOnOffToggleButton()->addListener(this);

    addAndMakeVisible(distortionUnitMenuTab);

    for (int distortionUnitID = 0; distortionUnitID < DistortionConstants::Processor<float>::nbOfDUMax; ++distortionUnitID)
    {
        distortionUnitMenuTab.getDistoUnitIDButton(distortionUnitID)->addListener(this);
    }
        

    distortionSliderComponent[0]->setVisible(true);
}

DistortionComponent::~DistortionComponent()
{
}

void DistortionComponent::paint (juce::Graphics& g)
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

void DistortionComponent::resized()
{
    int labelHeight = DistortionConstants::UI::labelHeight;

    distortionUnitMenuTab.setBounds(0,0,getLocalBounds().getWidth(), labelHeight);

    for (int distortionUnit = 0; distortionUnit < DistortionConstants::Processor<float>::nbOfDUMax; ++distortionUnit)
    {
        distortionSliderComponent[distortionUnit]->setBounds(distortionUnitMenuTab.getX(), distortionUnitMenuTab.getBottom(), getLocalBounds().getWidth(), getLocalBounds().getHeight() - labelHeight);
    }
}

void DistortionComponent::buttonClicked(juce::Button* button)
{
    int stageID = dataStruct.getSelectedStageID();

    if (button == distortionUnitMenuTab.getDistoUnitIDButton(0))
    {
        if (button->getToggleState())
        {
            switchDistortionUnit(0);
            dataStruct.setDistoUnitID(stageID, 0);
        }
    }
    else if (button == distortionUnitMenuTab.getDistoUnitIDButton(1))
    {
        if (button->getToggleState())
        {
            dataStruct.setDistoUnitID(stageID, 1);
            switchDistortionUnit(1);
        }

    }

}

void DistortionComponent::buttonStateChanged(juce::Button* button)
{
    if (button == nullptr)
    {

    }
}

void DistortionComponent::switchDistortionUnit(int distortionUnitIDToDisplay)
{
    for (int distortionUnitID = 0; distortionUnitID < distortionSliderComponent.size(); ++distortionUnitID)
    {
        distortionSliderComponent[distortionUnitID]->setVisible(false);
    }

    distortionSliderComponent[distortionUnitIDToDisplay]->setVisible(true);
}
