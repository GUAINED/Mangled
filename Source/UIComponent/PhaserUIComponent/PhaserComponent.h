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
#include "../../DataStructure/MangledState.h"
#include "../../DataStructure/MangledIdentifiers.h"
#include "../Controls/OnOffButton.h"
#include "../Controls/MangledSlider.h"
#include "../Controls/MangledComboBox.h"

//==============================================================================
/*
*/
class PhaserComponent  : public juce::Component
                       , public juce::Button::Listener
{
public:
    PhaserComponent(AudioEngineState<float>& mlDataStruct);
    ~PhaserComponent() override;

    void paint (juce::Graphics&) override;
    void resized() override;

    //Button::Listener
    void buttonClicked(juce::Button* button) override;

    //Get function
    MangledSlider*  getCentreFrequencyPhaserSlider() { return &centreFrequencyPhaserSlider; };
    MangledSlider*  getFeedbackPhaserSlider()        { return &feedbackPhaserSlider; };
    MangledSlider*  getRatePhaserSlider()            { return &ratePhaserSlider; };
    MangledSlider*  getDepthPhaserSlider()           { return &depthPhaserSlider; };
    MangledSlider*  getMixPhaserSlider()             { return &mixPhaserSlider; };
    MangledComboBox* getNumStagesPhaserComboBox()     { return &numStagesPhaserComboBox; };
    OnOffButton*    getPhaserOnOffButton()           { return &phaserOnOffButton; };
    juce::TextButton* getResetButton() { return &resetButton; };

private:

    MangledSlider centreFrequencyPhaserSlider;
    juce::Label centreFrequencyPhaserLabel;

    MangledSlider feedbackPhaserSlider;
    juce::Label feedbackPhaserLabel;

    MangledSlider ratePhaserSlider;
    juce::Label ratePhaserLabel;

    MangledSlider depthPhaserSlider;
    juce::Label depthPhaserLabel;

    MangledSlider mixPhaserSlider;
    juce::Label mixPhaserLabel;

    MangledComboBox numStagesPhaserComboBox;
    juce::Label numStagesPhaserLabel;

    OnOffButton phaserOnOffButton;

    juce::TextButton resetButton;

    AudioEngineState<float>& dataStruct;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PhaserComponent)
};
