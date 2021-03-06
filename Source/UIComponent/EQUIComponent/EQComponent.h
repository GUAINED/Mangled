/*
  ==============================================================================

    EQComponent.h
    Created: 8 Apr 2021 6:40:44pm
    Author:  CookiMonstor

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "EQSliderComponent.h"
#include "../../DataStructure/MainLayerDataStruct.h"
#include "../../DataStructure/MangledState.h"
#include "../../Processor/UnitProcessor/EQ/EQProcessor.h"
#include "../Controls/OnOffButton.h"
#include "../Controls/MangledSlider.h"
#include "../Controls/MangledButton.h"
//==============================================================================
/*
*/
class EQComponent  : public juce::Component,
    public juce::Button::Listener
{
public:
    EQComponent(AudioEngineState<float>& mlDataStruct);
    ~EQComponent() override;

    void paint (juce::Graphics&) override;
    void resized() override;

    void switchToEQSliderMenu(int newEQSliderID);
    //void switchUpOrDownEQSliderMenu(int newCurrentEQSliderID);
    void switchToWaveshaper();
    void switchFromWaveshaper();
    void updateFilterLabelForNoFilter();
    //void addFilter();
    //void removeFilter();

    void buttonClicked(juce::Button* button);
    void updateOnOffButton(juce::Button* button, juce::String addedString);

    //Set Function
    //void setNbOfActiveFilter(int newNbOfActiveFilter) { nbOfActiveFilter = newNbOfActiveFilter; };
    
    //Get Function
    MangledButton*      getResetButton()                     { return &resetButton; };
    juce::Label*       getFilterIdEQLabel()                 { return &filterIdEQLabel; };
    OnOffButton*       getOnOffButton()                     { return &onOffButton; };
    MangledButton*  getFilterIdDownEQTextButton()        { return &filterIdDownEQButton; };
    MangledButton*  getFilterIdUpEQTextButton()          { return &filterIdUpEQButton; };
    MangledSlider*      getMixEQSlider()                     { return &mixSlider; };
    EQSliderComponent* getEQSliderComponent(int eqSliderID) { return eqSlider[eqSliderID]; };

private:
    MangledButton resetButton;
    MangledButton filterIdDownEQButton;
    juce::Label filterIdEQLabel;
    MangledButton filterIdUpEQButton;

    //juce::Label isBypassedEQLabel;
    OnOffButton onOffButton;

    MangledSlider mixSlider;
    juce::Label mixLabel;

    juce::OwnedArray<EQSliderComponent> eqSlider;

    AudioEngineState<float>& dataStruct;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (EQComponent)
};
