/*
  ==============================================================================

    mainMenuComponent.h
    Created: 16 May 2021 2:22:37pm
    Author:  CookiMonstor

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>


#include "../../UIComponent/MainLayer/MainLayerMenuComponent.h"
#include "../../UIComponent/ScopeUIComponent/scopeComponent.h"

//==============================================================================
class MainMenuComponent  : public juce::Component
{
public:
    MainMenuComponent(MainLayerDataStruct& mlDataStruct);
    ~MainMenuComponent() override;

    void paint (juce::Graphics&) override;
    void resized() override;

    void switchToMainLayerMenu();
    void switchToPunchMenu();
    void switchToTailMenu();
    void switchToMainClipperMenu();

    MainLayerMenuComponent*   getMainLayerMenu()   { return &mainLayerMenu; };
    //PunchMenuComponent*       getPunchMenu()       { return &punchMenu; };
    //TailMenuComponent*        getTailMenu()        { return &tailMenu; };
    //MainClipperMenuComponent* getMainClipperMenu() { return &mainClipperMenu; };

private:
    
    MainLayerMenuComponent mainLayerMenu;
    //PunchMenuComponent punchMenu;
    //TailMenuComponent tailMenu;
    //MainClipperMenuComponent mainClipperMenu;
    MainLayerDataStruct& dataStruct;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainMenuComponent)
};
