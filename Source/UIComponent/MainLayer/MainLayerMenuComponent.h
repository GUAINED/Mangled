/*
  ==============================================================================

    MainLayerMenuComponent.h
    Created: 15 May 2021 10:47:17pm
    Author:  CookiMonstor

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

#include "../../UIComponent/StageUIComponent/StageComponent.h"
#include "../../UIComponent/ScopeUIComponent/ScopeComponent.h"

#include "../../UIComponent/EQUIComponent/EQComponent.h"
#include "../../UIComponent/PhaserUIComponent/PhaserComponent.h"
#include "../../UIComponent/DistortionUIComponent/DistortionComponent.h"

#include "../../UIComponent/Controls/MangledDualStateButton.h"

#include "../../Processor/LayerProcessor/MainLayerProcessor.h"
#include "../../Processor/UnitProcessor/EQProcessor.h"

//==============================================================================
/*
*/
class MainLayerMenuComponent  : public juce::Component,
    public juce::Button::Listener
{
public:
    enum RadioButtonIds
    {
        StageButtons = 1001
    };
    MainLayerMenuComponent(MainLayerDataStruct& mlDataStruct);
    ~MainLayerMenuComponent() override;

    void paint (juce::Graphics&) override;
    void resized() override;

    void buttonClicked(juce::Button* button);

    void switchToStageMenu(int stageID);
    void setButtonOn(int buttonID, juce::NotificationType notifType = juce::NotificationType::sendNotification);

    OnOffButton*      getStageOnOffButton(int stageID) { return stageOnOffButtonVector[stageID]; };
    juce::TextButton* getStageIDButton(int stageID)    { return stageIDButtonVector[stageID]; };
    StageComponent*   getStageComponent(int stageID)   { return stageComponentVector[stageID]; };
private:
    juce::OwnedArray<OnOffButton> stageOnOffButtonVector;
    juce::OwnedArray<MangledDualStateButton> stageIDButtonVector;
    juce::OwnedArray<StageComponent> stageComponentVector;

    MainLayerDataStruct& dataStruct;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainLayerMenuComponent)
};
