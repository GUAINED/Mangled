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
#include "../../Processor/UnitProcessor/EQProcessor.h"
#include "../GeneralUIComponent/OnOffButton.h"
//==============================================================================
/*
*/
class EQComponent  : public juce::Component,
    public juce::Button::Listener
{
public:
    EQComponent(MainLayerDataStruct& mlDataStruct);
    ~EQComponent() override;

    void paint (juce::Graphics&) override;
    void resized() override;

    void switchToEQSliderMenu(int newEQSliderID);
    //void switchUpOrDownEQSliderMenu(int newCurrentEQSliderID);
    void switchToWaveshaper();
    void switchFromWaveshaper();
    void updateFilterLabelForNoFilter();
    void addFilter();
    void removeFilter();

    void buttonClicked(juce::Button* button);
    void updateOnOffButton(juce::Button* button, juce::String addedString);

    //Set Function
    void setNbOfActiveFilter(int newNbOfActiveFilter) { nbOfActiveFilter = newNbOfActiveFilter; };
    
    //Get Function
    juce::Button*      getResetButton()                     { return &resetButton; };
    juce::Label*       getFilterIdEQLabel()                 { return &filterIdEQLabel; };
    OnOffButton*       getOnOffButton()                     { return &onOffButton; };
    juce::TextButton*  getFilterIdDownEQTextButton()        { return &filterIdDownEQButton; };
    juce::TextButton*  getFilterIdUpEQTextButton()          { return &filterIdUpEQButton; };
    juce::Slider*      getMixEQSlider()                     { return &mixSlider; };
    EQSliderComponent* getEQSliderComponent(int eqSliderID) { return eqSlider[eqSliderID]; };
    int                getNbOfActiveFilter()                { return nbOfActiveFilter; };

private:
    juce::TextButton resetButton;
    juce::TextButton filterIdDownEQButton;
    juce::Label filterIdEQLabel;
    juce::TextButton filterIdUpEQButton;

    //juce::Label isBypassedEQLabel;
    OnOffButton onOffButton;

    juce::Slider mixSlider;
    juce::Label mixLabel;

    juce::OwnedArray<EQSliderComponent> eqSlider;

    int nbOfActiveFilter = 0;

    MainLayerDataStruct& dataStruct;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (EQComponent)
};
