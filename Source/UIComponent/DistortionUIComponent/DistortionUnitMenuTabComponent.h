/*
  ==============================================================================

    DistortionUnitMenuTabComponent.h
    Created: 2 Jun 2021 11:25:48pm
    Author:  CookiMonstor

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "../GeneralUIComponent/OnOffButton.h"
#include "../../DataStructure/MangledIdentifiers.h"

//==============================================================================
/*
*/
class DistortionUnitMenuTabComponent  : public juce::Component
{
public:
    enum RadioButtonIds
    {
        DistortionUnitButtons = 1001
    };

    DistortionUnitMenuTabComponent();
    ~DistortionUnitMenuTabComponent() override;

    void paint (juce::Graphics&) override;
    void resized() override;

    void updateOnOffButton(juce::Button* button, juce::String addedString = "");

    //Get Function
    OnOffButton* getDistoUnitOnOffButton(int distortionUnitID)   { return distoUnitOnOffButtonVector[distortionUnitID]; };
    juce::TextButton* getDistoUnitIDButton(int distortionUnitID) { return distoUnitIDButtonVector[distortionUnitID]; };

    juce::TextButton* getOverSamplerOnOffButton() { return &oversamplerOnOffButton; };
    OnOffButton* getOnOffButton()            { return &onOffButton; };
    juce::Slider*     getMixDistortionSlider()    { return &mixDistortionSlider; };

private:
    juce::OwnedArray<OnOffButton> distoUnitOnOffButtonVector;
    juce::OwnedArray<juce::TextButton> distoUnitIDButtonVector;

    //OnOffButton isBypassedSecondDistortionUnitTextButton;
    //juce::TextButton secondDistortionUnitTextButton;

    OnOffButton onOffButton;

    juce::TextButton oversamplerOnOffButton;

    juce::Slider mixDistortionSlider;
    juce::Label mixDistortionLabel;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DistortionUnitMenuTabComponent)
};
