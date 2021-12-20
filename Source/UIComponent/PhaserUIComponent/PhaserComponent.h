/*
  ==============================================================================

    PhaserComponent.h
    Created: 8 Apr 2021 7:24:29pm
    Author:  CookiMonstor

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "../../DataStructure/MainLayerDataStruct.h"
#include "../../DataStructure/MangledIdentifiers.h"
#include "../GeneralUIComponent/OnOffButton.h"

//==============================================================================
/*
*/
class PhaserComponent  : public juce::Component
                       , public juce::Button::Listener
{
public:
    PhaserComponent(MainLayerDataStruct& mlDataStruct);
    ~PhaserComponent() override;

    void paint (juce::Graphics&) override;
    void resized() override;

    //Button::Listener
    void buttonClicked(juce::Button* button) override;

    //Get function
    juce::Slider*   getCentreFrequencyPhaserSlider() { return &centreFrequencyPhaserSlider; };
    juce::Slider*   getFeedbackPhaserSlider()        { return &feedbackPhaserSlider; };
    juce::Slider*   getRatePhaserSlider()            { return &ratePhaserSlider; };
    juce::Slider*   getDepthPhaserSlider()           { return &depthPhaserSlider; };
    juce::Slider*   getMixPhaserSlider()             { return &mixPhaserSlider; };
    juce::ComboBox* getNumStagesPhaserComboBox()     { return &numStagesPhaserComboBox; };
    OnOffButton*    getPhaserOnOffButton()           { return &phaserOnOffButton; };
    juce::TextButton* getResetButton() { return &resetButton; };

private:

    juce::Slider centreFrequencyPhaserSlider;
    juce::Label centreFrequencyPhaserLabel;

    juce::Slider feedbackPhaserSlider;
    juce::Label feedbackPhaserLabel;

    juce::Slider ratePhaserSlider;
    juce::Label ratePhaserLabel;

    juce::Slider depthPhaserSlider;
    juce::Label depthPhaserLabel;

    juce::Slider mixPhaserSlider;
    juce::Label mixPhaserLabel;

    juce::ComboBox numStagesPhaserComboBox;
    juce::Label numStagesPhaserLabel;

    OnOffButton phaserOnOffButton;

    juce::TextButton resetButton;

    MainLayerDataStruct& dataStruct;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PhaserComponent)
};
