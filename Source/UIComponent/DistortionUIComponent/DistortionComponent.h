/*
  ==============================================================================

    WaveShaperComponent.h
    Created: 8 Apr 2021 7:34:55pm
    Author:  CookiMonstor

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "../../Processor/UnitProcessor/DistortionProcessor.h"
#include "WaveShaperScope.h"
#include "DistortionUnitMenuTabComponent.h"
#include "DistortionSliderComponent.h"

//==============================================================================
/*
*/
class DistortionComponent : public juce::Component
                          , public juce::Button::Listener
{
public:
    DistortionComponent(MainLayerDataStruct& mlDataStruct);
    ~DistortionComponent() override;

    void paint (juce::Graphics&) override;
    void resized() override;

    //Update
    //void updateOnOffButton(juce::Button* button, juce::String addedString = "");

    //Button Listener
    void buttonClicked(juce::Button* button) override;
    void buttonStateChanged(juce::Button* button) override;

    //Get function
    DistortionUnitMenuTabComponent* getDistortionUnitMenuTab()                      { return &distortionUnitMenuTab; };
    DistortionSliderComponent*      getDistortionSlider(int distortionUnitSliderID) { return distortionSliderComponent[distortionUnitSliderID]; };

    void switchDistortionUnit(int distortionUnitIDToDisplay);

private:
    DistortionUnitMenuTabComponent distortionUnitMenuTab;

    juce::OwnedArray<DistortionSliderComponent> distortionSliderComponent;

    MainLayerDataStruct& dataStruct;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DistortionComponent)
};