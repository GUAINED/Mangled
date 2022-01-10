/*
  ==============================================================================

    ScopeComponent.h
    Created: 9 May 2021 10:51:57pm
    Author:  CookiMonstor

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "ScopeDisplay.h"
#include "TemporalScope.h"
#include "../Controls/OnOffButton.h"

//==============================================================================
/*
*/
class ScopeComponent : public juce::Component
                     , public juce::Button::Listener
                     , public juce::ComboBox::Listener
{
public:
    ScopeComponent(MainLayerDataStruct& mlDataStruct);
    ~ScopeComponent() override;

    void paint (juce::Graphics&) override;
    void resized() override;

    //Button Listener============================================================================================================
    void buttonClicked(juce::Button* button) override;
    //void buttonStateChanged(juce::Button* button) override;

    //Combo Box============================================================================================================
    void comboBoxChanged(juce::ComboBox* comboBoxThatHasChanged) override;

    //Draw function============================================================================================================
    //void setMode(ScopeDisplay::ScopeMode newMode) { scopeDisplay.setMode(newMode); };

    void switchScope(int dataID);

    ScopeDisplay* getScopeDisplay() { return &scopeDisplay; };
    TemporalScope* getTemporalScope() { return &temporalScope; };
    OnOffButton* getPreEQOnOffButton() { return &preEQOnOffButton; };
    OnOffButton* getPostEQOnOffButton() { return &postEQOnOffButton; };
    OnOffButton* getPostDistoOnOffButton() { return &postDistoOnOffButton; };
    OnOffButton* getIsNormalizedOnOffButton() { return &normalizeButton; };
    OnOffButton* getMonoViewOnOffButton() { return &monoViewButton; };

    OnOffButton* getSubViewOnOffButton() { return &subViewButton; };
    juce::Slider* getSubViewSlider() { return &subViewSlider; };
private:
    ScopeDisplay scopeDisplay;
    TemporalScope temporalScope;

    OnOffButton preEQOnOffButton;
    OnOffButton postEQOnOffButton;
    OnOffButton postDistoOnOffButton;

    juce::ComboBox subdivisionComboBox;
    OnOffButton normalizeButton;
    OnOffButton monoViewButton;
    OnOffButton subViewButton;

    juce::Slider subViewSlider;

    MainLayerDataStruct& dataStruct;

    LookAndFeel_V4_Gradient lafGradient;

    //float bufferDataScope[2*ScopeDisplay::ScopeParams::fftSize];

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ScopeComponent)
};
