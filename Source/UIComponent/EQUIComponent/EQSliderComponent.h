/*
  ==============================================================================

    EQSliderComponent.h
    Created: 1 Jun 2021 4:13:23pm
    Author:  CookiMonstor

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "../../DataStructure/MangledIdentifiers.h"
#include "../../DataStructure/MainLayerDataStruct.h"
#include "../GeneralUIComponent/OnOffButton.h"
//==============================================================================
/*
*/
class EQSliderComponent : public juce::Component
                        , public juce::Button::Listener
                        , public juce::ComboBox::Listener
{
public:
    EQSliderComponent(MainLayerDataStruct& mlDataStruct, juce::Colour initBackgroundColour);
    ~EQSliderComponent() override;

    void paint (juce::Graphics&) override;
    void resized() override;

    void buttonClicked(juce::Button* button);
    void comboBoxChanged(juce::ComboBox* comboBoxThatHasChanged);

    void setLargeGradient();
    void setSmallGradient();
    //Get Function
    juce::Slider*     getFilterCutoffEQSlider()       { return &filterCutoffEQSlider; };
    juce::Slider*     getFilterQEQSlider()            { return &filterQEQSlider; };
    juce::Slider*     getFilterGainEQSlider()         { return &filterGainEQSlider; };
    juce::ComboBox*   getFilterTypeEQComboBox()       { return &filterTypeEQComboBox; };
    juce::ComboBox*   getFilterOrderEQComboBox()      { return &filterOrderEQComboBox; };
    OnOffButton*      getFilterOnOffEQTextButton()    { return &filterOnOffEQButton; };
    juce::TextButton* getFilterIsActiveEQTextButton() { return &filterIsActiveEQButton; };
private:
    juce::Colour backgroundColour;
    juce::ColourGradient colourGradient;

    juce::Label filterCutoffEQLabel;
    juce::Slider filterCutoffEQSlider;

    juce::Label filterQEQLabel;
    juce::Slider filterQEQSlider;

    juce::Label filterGainEQLabel;
    juce::Slider filterGainEQSlider;

    juce::Label filterTypeEQLabel;
    juce::ComboBox filterTypeEQComboBox;

    juce::Label filterOrderEQLabel;
    juce::ComboBox filterOrderEQComboBox;

    OnOffButton filterOnOffEQButton;

    juce::TextButton filterIsActiveEQButton;

    MainLayerDataStruct& dataStruct;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (EQSliderComponent)
};
