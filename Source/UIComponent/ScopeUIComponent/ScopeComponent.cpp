/*
  ==============================================================================

    ScopeComponent.cpp
    Created: 9 May 2021 10:51:57pm
    Author:  CookiMonstor

  ==============================================================================
*/

#include <JuceHeader.h>
#include "ScopeComponent.h"

//==============================================================================
ScopeComponent::ScopeComponent(AudioEngineState<float>& mlDataStruct)
    : dataStruct(mlDataStruct)
    , scopeDisplay(mlDataStruct)
    , preEQOnOffButton("PreEQ", "On", ScopeConstants::Processor<float>::preEQIsBypassedStartValue)
    , postEQOnOffButton("PostEQ", "On", ScopeConstants::Processor<float>::postEQIsBypassedStartValue)
    , postDistoOnOffButton("PostDisto", "On", ScopeConstants::Processor<float>::postDistoIsBypassedStartValue)
    , normalizeButton("Normalize", "On", RMSConstants::Processor<float>::isNormalizedStartValue)
    , subViewButton("Sub View", "Off", RMSConstants::Processor<float>::subViewIsBypassedStartValue)
    , monoViewButton("Mono View", "Off", RMSConstants::Processor<float>::monoViewIsBypassedStartValue)
{
    setInterceptsMouseClicks(false, true);

    //preEQOnOffButton.setOnColour();

    //Scope
    addAndMakeVisible(temporalScope);
    addAndMakeVisible(scopeDisplay);

    preEQOnOffButton.setTrueColour(juce::Colours::black);
    preEQOnOffButton.setFalseColour(juce::Colours::cyan);
    addAndMakeVisible(preEQOnOffButton);

    postEQOnOffButton.setTrueColour(juce::Colours::black);
    postEQOnOffButton.setFalseColour(juce::Colours::orange);
    addAndMakeVisible(postEQOnOffButton);

    postDistoOnOffButton.setTrueColour(juce::Colours::black);
    postDistoOnOffButton.setFalseColour(juce::Colours::purple);
    addAndMakeVisible(postDistoOnOffButton);

    subdivisionComboBox.addItem("1/1", 1);
    subdivisionComboBox.addItem("1/2", 2);
    subdivisionComboBox.addItem("1/3", 3);
    subdivisionComboBox.addItem("1/4", 4);
    subdivisionComboBox.addItem("1/5", 5);
    subdivisionComboBox.addItem("1/6", 6);
    subdivisionComboBox.addItem("1/7", 7);
    subdivisionComboBox.addItem("1/8", 8);
    subdivisionComboBox.setSelectedId(4);
    subdivisionComboBox.addListener(this);

    addAndMakeVisible(subdivisionComboBox);
    addAndMakeVisible(normalizeButton);
    addAndMakeVisible(monoViewButton);
    addAndMakeVisible(subViewButton);
    monoViewButton.addListener(this);
    subViewButton.addListener(this);

    subViewSlider.setSliderStyle(juce::Slider::SliderStyle::LinearHorizontal);
    subViewSlider.setTextBoxStyle(juce::Slider::TextBoxRight, true, 50, 20);
    subViewSlider.setTextBoxIsEditable(true);
    addAndMakeVisible(subViewSlider);
}

ScopeComponent::~ScopeComponent()
{
    preEQOnOffButton.setLookAndFeel(nullptr);
    postEQOnOffButton.setLookAndFeel(nullptr);
    postDistoOnOffButton.setLookAndFeel(nullptr);
}

void ScopeComponent::paint (juce::Graphics& g)
{
    /* This demo code just fills the component's background and
       draws some placeholder text to get you started.

       You should replace everything in this method with your own
       drawing code..
    */

    //g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));   // clear the background
    //g.fillAll(juce::Colours::black); 
    
    g.setColour (juce::Colours::grey);
    g.drawRect (getLocalBounds(), 1);   // draw an outline around the component

    //g.setColour (juce::Colours::white);
    //g.setFont (14.0f);
    //g.drawText ("ScopeComponent", getLocalBounds(),
    //            juce::Justification::centred, true);   // draw some placeholder text
}

void ScopeComponent::resized()
{
    int rotaryPixelSize = 75;
    int labelWidth = rotaryPixelSize;
    int labelHeight = 20;
    int pixelSpace = 10;
    int offset = 5;

    preEQOnOffButton.setBounds(offset, 0, labelWidth, labelHeight);
    postEQOnOffButton.setBounds(preEQOnOffButton.getRight() + pixelSpace, preEQOnOffButton.getY(), labelWidth, labelHeight);
    postDistoOnOffButton.setBounds(postEQOnOffButton.getRight() + pixelSpace, preEQOnOffButton.getY(), labelWidth, labelHeight);

    subdivisionComboBox.setBounds(preEQOnOffButton.getX(), preEQOnOffButton.getY(), labelWidth, labelHeight);
    normalizeButton.setBounds(subdivisionComboBox.getRight() + pixelSpace, subdivisionComboBox.getY(), labelWidth, labelHeight);
    monoViewButton.setBounds(normalizeButton.getRight() + pixelSpace, normalizeButton.getY(), labelWidth, labelHeight);
    subViewButton.setBounds(monoViewButton.getRight() + pixelSpace, monoViewButton.getY(), labelWidth, labelHeight);
    subViewSlider.setBounds(subViewButton.getRight() + pixelSpace, subViewButton.getY(), 2 * labelWidth, labelHeight);

    temporalScope.setBounds(preEQOnOffButton.getX(), preEQOnOffButton.getBottom() + 3, getLocalBounds().getWidth() - pixelSpace, getLocalBounds().getHeight() - labelHeight - 9);

    scopeDisplay.setBounds(temporalScope.getBounds());
}

void ScopeComponent::buttonClicked(juce::Button* button)
{
    if (button == &monoViewButton)
    {
        temporalScope.switchScope(monoViewButton.getToggleState());
    }
    else if (button == &subViewButton)
    {
        subViewSlider.setVisible(! subViewButton.getToggleState());
    }
}
//
//void ScopeComponent::buttonStateChanged(juce::Button* button)
//{
//}
//
void ScopeComponent::comboBoxChanged(juce::ComboBox* comboBoxThatHasChanged)
{
    if (comboBoxThatHasChanged == &subdivisionComboBox)
    {
        int subdivision = subdivisionComboBox.getSelectedId();
        temporalScope.computeGrid(subdivision);
    }
}

void ScopeComponent::switchScope(int dataID)
{
    switch (dataID)
    {
    case 0: //FFT
        scopeDisplay.setVisible(true);
        preEQOnOffButton.setVisible(true);
        postEQOnOffButton.setVisible(true);
        postDistoOnOffButton.setVisible(true);

        temporalScope.setVisible(false);
        subdivisionComboBox.setVisible(false);
        subViewButton.setVisible(false);
        subViewSlider.setVisible(false);
        normalizeButton.setVisible(false);
        monoViewButton.setVisible(false);
        break;
    case 1: //Temporal
        temporalScope.setVisible(true);
        subdivisionComboBox.setVisible(true);
        subViewButton.setVisible(true);
        subViewSlider.setVisible(! subViewButton.getToggleState());
        normalizeButton.setVisible(true);
        monoViewButton.setVisible(true);

        scopeDisplay.setVisible(false);
        preEQOnOffButton.setVisible(false);
        postEQOnOffButton.setVisible(false);
        postDistoOnOffButton.setVisible(false);
        break;
    default:
        temporalScope.setVisible(false);
        subdivisionComboBox.setVisible(false);
        subViewButton.setVisible(false);
        subViewSlider.setVisible(false);
        normalizeButton.setVisible(false);
        monoViewButton.setVisible(false);

        scopeDisplay.setVisible(true);
        preEQOnOffButton.setVisible(true);
        postEQOnOffButton.setVisible(true);
        postDistoOnOffButton.setVisible(true);
        break;
    }
}