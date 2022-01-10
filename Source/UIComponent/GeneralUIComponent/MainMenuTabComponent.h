/*
  ==============================================================================

    mainMenuTabComponent.h
    Created: 15 May 2021 9:25:46pm
    Author:  CookiMonstor

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "../Controls/OnOffButton.h"
#include "../Controls/MangledDualStateButton.h"
//==============================================================================
/*
*/
class MainMenuTabComponent  : public juce::Component
{
public:
    enum RadioButtonIds
    {
        LayerButtons = 1001
    };

    MainMenuTabComponent();
    ~MainMenuTabComponent() override;

    void paint (juce::Graphics&) override;
    void resized() override;

    //Get Function
    OnOffButton* getMainLayerOnOffButton() { return &mainLayerOnOffButton; };
    juce::TextButton* getMainLayerTextButton() { return &mainLayerTextButton; };

    //juce::TextButton* getOnOffPunchTextButton() { return &onOffPunchTextButton; };
    //juce::TextButton* getPunchTextButton() { return &punchTextButton; };

    //juce::TextButton* getOnOffTailTextButton() { return &onOffTailTextButton; };
    //juce::TextButton* getTailTextButton() { return &tailTextButton; };

    //juce::TextButton* getOnOffMainCLipperTextButton() { return &onOffMainClipperTextButton; };
    //juce::TextButton* getMainClipperTextButton() { return &mainClipperTextButton; };

    //juce::TextButton* getOptionsTextButton() { return &optionsTextButton; };

    //TabBar Button
    //juce::TabBarButton* getOnOffMainLayerButton() { return &onOffMainLayerButton; };
    //juce::TabBarButton* getMainLayerButton() { return &mainLayerButton; };

    //juce::TabBarButton* getOnOffPunchButton() { return &onOffPunchButton; };
    //juce::TabBarButton* getPunchButton() { return &punchButton; };

    //juce::TabBarButton* getOnOffTailButton() { return &onOffTailButton; };
    //juce::TabBarButton* getTailButton() { return &tailButton; };

    //juce::TabBarButton* getOnOffMainCLipperButton() { return &onOffMainClipperButton; };
    //juce::TabBarButton* getMainClipperButton() { return &mainClipperButton; };

    //juce::TabBarButton* getOptionsButton() { return &optionsButton; };

private:
    //Text Button===================================
    OnOffButton mainLayerOnOffButton;
    MangledDualStateButton mainLayerTextButton;

    //juce::TextButton onOffPunchTextButton;
    //juce::TextButton punchTextButton;

    //juce::TextButton onOffTailTextButton;
    //juce::TextButton tailTextButton;

    //juce::TextButton onOffMainClipperTextButton;
    //juce::TextButton mainClipperTextButton;

    //juce::TextButton optionsTextButton;

    //Tab Bar========================================
    //juce::TabbedButtonBar mainMenuBar;

    //juce::TabBarButton onOffMainLayerButton;
    //juce::TabBarButton mainLayerButton;

    //juce::TabBarButton onOffPunchButton;
    //juce::TabBarButton punchButton;

    //juce::TabBarButton onOffTailButton;
    //juce::TabBarButton tailButton;

    //juce::TabBarButton onOffMainClipperButton;
    //juce::TabBarButton mainClipperButton;

    //juce::TabBarButton optionsButton;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainMenuTabComponent)
};
