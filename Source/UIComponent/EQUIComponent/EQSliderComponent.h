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
#include "../../DataStructure/MangledState.h"
#include "../Controls/OnOffButton.h"
#include "../Controls/MangledLabel.h"
#include "../Controls/MangledButton.h"
#include "../Controls/MangledSlider.h"
#include "../Controls/MangledComboBox.h"
//==============================================================================
/*
*/
class EQSliderComponent : public juce::Component
                        , public juce::Button::Listener
                        , public MangledComboBox::Listener
{
public:
    EQSliderComponent(AudioEngineState<float>& mlDataStruct, juce::Colour initBackgroundColour);
    ~EQSliderComponent() override;

    void paint (juce::Graphics&) override;
    void resized() override;

    void buttonClicked(juce::Button* button);
    void comboBoxChanged(juce::ComboBox* comboBoxThatHasChanged);

    void setLargeGradient();
    void setSmallGradient();
    //Get Function
    MangledSlider*     getFilterCutoffEQSlider()       { return &filterCutoffEQSlider; };
    MangledSlider*     getFilterQEQSlider()            { return &filterQEQSlider; };
    MangledSlider*     getFilterGainEQSlider()         { return &filterGainEQSlider; };
    MangledComboBox*   getFilterTypeEQComboBox()       { return &filterTypeEQComboBox; };
    MangledComboBox*   getFilterOrderEQComboBox()      { return &filterOrderEQComboBox; };
    OnOffButton*      getFilterOnOffEQTextButton()    { return &filterOnOffEQButton; };
    juce::TextButton* getFilterIsActiveEQTextButton() { return &filterIsActiveEQButton; };
private:
    juce::Colour backgroundColour;
    juce::ColourGradient colourGradient;

    MangledLabel filterCutoffEQLabel;
    MangledSlider filterCutoffEQSlider;

    MangledLabel filterQEQLabel;
    MangledSlider filterQEQSlider;

    MangledLabel filterGainEQLabel;
    MangledSlider filterGainEQSlider;

    juce::Label filterTypeEQLabel;
    MangledComboBox filterTypeEQComboBox;

    juce::Label filterOrderEQLabel;
    MangledComboBox filterOrderEQComboBox;

    OnOffButton filterOnOffEQButton;

    MangledButton filterIsActiveEQButton;

    AudioEngineState<float>& dataStruct;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (EQSliderComponent)
};
