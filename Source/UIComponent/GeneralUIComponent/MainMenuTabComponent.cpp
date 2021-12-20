/*
  ==============================================================================

    mainMenuTabComponent.cpp
    Created: 15 May 2021 9:25:46pm
    Author:  CookiMonstor

  ==============================================================================
*/

#include <JuceHeader.h>
#include "MainMenuTabComponent.h"
#include "../../DataStructure/MangledIdentifiers.h"

//==============================================================================
MainMenuTabComponent::MainMenuTabComponent()
    : mainLayerOnOffButton("", "On", MainLayerConstants::Processor<float>::isBypassedStartValue)
    , mainLayerTextButton("Main Layer")
//,
//onOffPunchTextButton("On"),
//punchTextButton("Punch"),
//onOffTailTextButton("On"),
//tailTextButton("Tail"),
//onOffMainClipperTextButton("On"),
//mainClipperTextButton("Main Clipper"),
//optionsTextButton("Options")
//mainMenuBar(juce::TabbedButtonBar::TabsAtTop),
//onOffMainLayerButton("On", mainMenuBar),
//mainLayerButton("Main Layer", mainMenuBar),
//onOffPunchButton("On", mainMenuBar),
//punchButton("Punch", mainMenuBar),
//onOffTailButton("On", mainMenuBar),
//tailButton("Tail", mainMenuBar),
//onOffMainClipperButton("On", mainMenuBar),
//mainClipperButton("Main Clipper", mainMenuBar),
//optionsButton("Options", mainMenuBar)
{
    //Text Button===============================================================================================
    //Main Layer Button
    addAndMakeVisible(mainLayerOnOffButton);

    mainLayerTextButton.setButtonText("Main Layer");
    mainLayerTextButton.setClickingTogglesState(true);
    mainLayerTextButton.setColour(juce::TextButton::ColourIds::buttonColourId, juce::Colours::grey);
    mainLayerTextButton.setColour(juce::TextButton::ColourIds::buttonOnColourId, juce::Colours::black);
    mainLayerTextButton.setRadioGroupId(LayerButtons);
    mainLayerTextButton.setToggleState(true, juce::dontSendNotification);
    addAndMakeVisible(mainLayerTextButton);

    ////Punch Button
    //punchTextButton.setButtonText("Punch");
    //punchTextButton.setRadioGroupId(LayerButtons);
    //punchTextButton.setClickingTogglesState(true);
    //punchTextButton.setColour(juce::TextButton::ColourIds::buttonColourId, juce::Colours::grey);
    //punchTextButton.setColour(juce::TextButton::ColourIds::buttonOnColourId, juce::Colours::black);

    //onOffPunchTextButton.setClickingTogglesState(true);
    //onOffPunchTextButton.setButtonText("OFF");
    //onOffPunchTextButton.setColour(juce::TextButton::ColourIds::buttonColourId, juce::Colours::red);
    //onOffPunchTextButton.setColour(juce::TextButton::ColourIds::buttonOnColourId, juce::Colours::green);
    //onOffPunchTextButton.onClick = [this] {updateOnOffButton(&onOffPunchTextButton); };
    //onOffPunchTextButton.setToggleState(false, juce::dontSendNotification);

    ////Tail Button
    //tailTextButton.setButtonText("Tail");
    //tailTextButton.setRadioGroupId(LayerButtons);
    //tailTextButton.setClickingTogglesState(true);
    //tailTextButton.setColour(juce::TextButton::ColourIds::buttonColourId, juce::Colours::grey);
    //tailTextButton.setColour(juce::TextButton::ColourIds::buttonOnColourId, juce::Colours::black);

    //onOffTailTextButton.setClickingTogglesState(true);
    //onOffTailTextButton.setButtonText("OFF");
    //onOffTailTextButton.setColour(juce::TextButton::ColourIds::buttonColourId, juce::Colours::red);
    //onOffTailTextButton.setColour(juce::TextButton::ColourIds::buttonOnColourId, juce::Colours::green);
    //onOffTailTextButton.onClick = [this] {updateOnOffButton(&onOffTailTextButton); };
    //onOffTailTextButton.setToggleState(false, juce::dontSendNotification);

    ////Main Clipper Button
    //mainClipperTextButton.setButtonText("Main Clipper");
    //mainClipperTextButton.setRadioGroupId(LayerButtons);
    //mainClipperTextButton.setClickingTogglesState(true);
    //mainClipperTextButton.setColour(juce::TextButton::ColourIds::buttonColourId, juce::Colours::grey);
    //mainClipperTextButton.setColour(juce::TextButton::ColourIds::buttonOnColourId, juce::Colours::black);

    //onOffMainClipperTextButton.setClickingTogglesState(true);
    //onOffMainClipperTextButton.setButtonText("OFF");
    //onOffMainClipperTextButton.setColour(juce::TextButton::ColourIds::buttonColourId, juce::Colours::red);
    //onOffMainClipperTextButton.setColour(juce::TextButton::ColourIds::buttonOnColourId, juce::Colours::green);
    //onOffMainClipperTextButton.onClick = [this] {updateOnOffButton(&onOffMainClipperTextButton); };
    //onOffMainClipperTextButton.setToggleState(false, juce::dontSendNotification);
    //
    //optionsTextButton.setButtonText("Options");
    //optionsTextButton.setRadioGroupId(LayerButtons);
    //optionsTextButton.setClickingTogglesState(true);
    //optionsTextButton.setColour(juce::TextButton::ColourIds::buttonColourId, juce::Colours::grey);
    //optionsTextButton.setColour(juce::TextButton::ColourIds::buttonOnColourId, juce::Colours::black);

    //addAndMakeVisible(onOffPunchTextButton);
    //addAndMakeVisible(punchTextButton);

    //addAndMakeVisible(onOffTailTextButton);
    //addAndMakeVisible(tailTextButton);

    //addAndMakeVisible(onOffMainClipperTextButton);
    //addAndMakeVisible(mainClipperTextButton);

    //addAndMakeVisible(optionsTextButton);

    //Tab Bar==================================================================================================
    //onOffMainLayerButton.setClickingTogglesState(true);
    //onOffPunchButton.setClickingTogglesState(false);
    //onOffTailButton.setClickingTogglesState(false);
    //onOffMainClipperButton.setClickingTogglesState(false);
    //optionsButton.setClickingTogglesState(false);

    //addChildComponent(onOffMainLayerButton);
    //addChildComponent(mainLayerButton);

    //addChildComponent(onOffPunchButton);
    //addChildComponent(punchButton);

    //addChildComponent(onOffTailButton);
    //addChildComponent(tailButton);

    //addChildComponent(onOffMainClipperButton);
    //addChildComponent(mainClipperButton);

    //addChildComponent(optionsButton);

    //addChildComponent(mainMenuBar);
}

MainMenuTabComponent::~MainMenuTabComponent()
{
}

void MainMenuTabComponent::paint (juce::Graphics& g)
{
    /* This demo code just fills the component's background and
       draws some placeholder text to get you started.

       You should replace everything in this method with your own
       drawing code..
    */

    //g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));   // clear the background
    g.fillAll(juce::Colours::transparentBlack);
    g.setColour (juce::Colours::grey);
    g.drawRect (getLocalBounds(), 1);   // draw an outline around the component

    //g.setColour (juce::Colours::white);
    //g.setFont (14.0f);
    //g.drawText ("mainMenuTabComponent", getLocalBounds(),
    //            juce::Justification::centred, true);   // draw some placeholder text
}

void MainMenuTabComponent::resized()
{
    //auto width = getLocalBounds().getWidth();
    auto height = getLocalBounds().getHeight();
    int rotaryPixelSize = 75;
    int labelWidth = rotaryPixelSize;
    //int labelHeight = 20;

    //Text Button
    mainLayerOnOffButton.setBounds(0, 0, height + 10, height);
    mainLayerTextButton.setBounds(mainLayerOnOffButton.getRight(), 0, labelWidth, height);

    //onOffPunchTextButton.setBounds(mainLayerTextButton.getRight() + 20, 0, height + 10, height);
    //punchTextButton.setBounds(onOffPunchTextButton.getRight(), 0, buttonWidth, height);

    //onOffTailTextButton.setBounds(punchTextButton.getRight() + 20, 0, height + 10, height);
    //tailTextButton.setBounds(onOffTailTextButton.getRight(), 0, buttonWidth, height);

    //onOffMainClipperTextButton.setBounds(tailTextButton.getRight() + 20, 0, height + 10, height);
    //mainClipperTextButton.setBounds(onOffMainClipperTextButton.getRight(), 0, buttonWidth, height);

    //optionsTextButton.setBounds(mainClipperTextButton.getRight() + 20, 0, buttonWidth, height);

    //onOffMainLayerButton.setBounds(0, 0, height, height);
    //mainLayerButton.setBounds(onOffMainLayerButton.getRight(), 0, buttonWidth, height);

    //onOffPunchButton.setBounds(mainLayerButton.getRight(), 0, height, height);
    //punchButton.setBounds(onOffPunchButton.getRight(), 0, buttonWidth, height);

    //onOffTailButton.setBounds(punchButton.getRight(), 0, height, height);
    //tailButton.setBounds(onOffTailButton.getRight(), 0, buttonWidth, height);

    //onOffMainClipperButton.setBounds(tailButton.getRight(), 0, height, height);
    //mainClipperButton.setBounds(onOffMainClipperButton.getRight(), 0, buttonWidth, height);
    //
    //optionsButton.setBounds(mainClipperButton.getRight(), 0, buttonWidth, height);
}
