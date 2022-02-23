/*
  ==============================================================================

    MainMenuComponent.cpp
    Created: 16 May 2021 2:22:37pm
    Author:  CookiMonstor

  ==============================================================================
*/

#include <JuceHeader.h>
#include "MainMenuComponent.h"

//==============================================================================
MainMenuComponent::MainMenuComponent(AudioEngineState<float>& mlDataStruct)
    : mainLayerMenu(mlDataStruct)
    , dataStruct(mlDataStruct)
{
    setInterceptsMouseClicks(false, true);

    //addChildComponent(stageMenuTab);
    addAndMakeVisible(mainLayerMenu);
    //addChildComponent(punchMenu);
    //addChildComponent(tailMenu);
    //addChildComponent(mainClipperMenu);
}

MainMenuComponent::~MainMenuComponent()
{
}

void MainMenuComponent::paint (juce::Graphics& g)
{
    /* This demo code just fills the component's background and
       draws some placeholder text to get you started.

       You should replace everything in this method with your own
       drawing code..
    */

    //g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));   // clear the background
    g.fillAll(juce::Colours::transparentBlack);
    //g.setColour (juce::Colours::grey);
    //g.drawRect (getLocalBounds(), 1);   // draw an outline around the component

    //g.setColour (juce::Colours::white);
    //g.setFont (14.0f);
    //g.drawText ("mainMenuComponent", getLocalBounds(),
    //            juce::Justification::centred, true);   // draw some placeholder text
}

void MainMenuComponent::resized()
{
    //int rotaryPixelSize = 75;
    //int labelWidth = rotaryPixelSize;
    //int labelHeight = 20;
    //int pixelSpace = 10;

    mainLayerMenu.setBounds(0, 0, getLocalBounds().getWidth(), getLocalBounds().getHeight());
    //punchMenu.setBounds(0, 0, getLocalBounds().getWidth(), getLocalBounds().getHeight());
    //tailMenu.setBounds(0, 0, getLocalBounds().getWidth(), getLocalBounds().getHeight());
    //mainClipperMenu.setBounds(0, 0, getLocalBounds().getWidth(), getLocalBounds().getHeight());
}

void MainMenuComponent::switchToMainLayerMenu()
{
    mainLayerMenu.setVisible(true);
    //punchMenu.setVisible(false);
    //tailMenu.setVisible(false);
    //mainClipperMenu.setVisible(false);
}

void MainMenuComponent::switchToPunchMenu()
{
    mainLayerMenu.setVisible(false);
    //punchMenu.setVisible(true);
    //tailMenu.setVisible(false);
    //mainClipperMenu.setVisible(false);
}

void MainMenuComponent::switchToTailMenu()
{
    mainLayerMenu.setVisible(false);
    //punchMenu.setVisible(false);
    //tailMenu.setVisible(true);
    //mainClipperMenu.setVisible(false);
}
void MainMenuComponent::switchToMainClipperMenu()
{
    mainLayerMenu.setVisible(false);
    //punchMenu.setVisible(false);
    //tailMenu.setVisible(false);
    //mainClipperMenu.setVisible(true);
}