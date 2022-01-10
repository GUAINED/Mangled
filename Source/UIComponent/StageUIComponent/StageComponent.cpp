/*
  ==============================================================================

    StageComponent.cpp
    Created: 2 Jun 2021 4:45:10pm
    Author:  CookiMonstor

  ==============================================================================
*/

#include <JuceHeader.h>
#include "StageComponent.h"

//==============================================================================
StageComponent::StageComponent(MainLayerDataStruct& mlDataStruct)
    : dataStruct(mlDataStruct)
    , scope(mlDataStruct)
    , eqComponent(mlDataStruct)
    , phaserComponent(mlDataStruct)
    , distortionComponent(mlDataStruct)
    , inputRMSMeter("Input")
    , outputRMSMeter("Ouput")
    , displayEQButton("EQ")
    , displayDistoButton("Distortion")
    , displayEnvButton("Enveloppe")
{
    //setInterceptsMouseClicks(false, true);

    dataLabel.setText("Data to display: ", juce::dontSendNotification);
    addAndMakeVisible(dataLabel);
    //Mode ComboBox
    dataToDisplayComboBox.addItem("FFT", 1);
    dataToDisplayComboBox.addItem("Temporal", 2);
    dataToDisplayComboBox.addItem("RMS", 3);
    dataToDisplayComboBox.addItem("Sound Field", 4);
    dataToDisplayComboBox.setSelectedId(1, juce::dontSendNotification);
    dataToDisplayComboBox.addListener(this);

    addAndMakeVisible(dataToDisplayComboBox);

    displayEQButton.setRadioGroupId(EQOrDistortionButtons, juce::dontSendNotification);
    displayEQButton.addListener(this);
    addAndMakeVisible(displayEQButton);

    displayDistoButton.setRadioGroupId(EQOrDistortionButtons, juce::dontSendNotification);
    displayDistoButton.setToggleState(true, juce::dontSendNotification);
    displayDistoButton.addListener(this);
    addAndMakeVisible(displayDistoButton);

    displayEnvButton.setRadioGroupId(EQOrDistortionButtons, juce::dontSendNotification);

    displayEnvButton.addListener(this);
    addAndMakeVisible(displayEnvButton);

    addAndMakeVisible(scope);
    addAndMakeVisible(eqComponent);
    addChildComponent(phaserComponent);
    addChildComponent(distortionComponent);


    addAndMakeVisible(inputRMSMeter);
    addAndMakeVisible(outputRMSMeter);
}

StageComponent::~StageComponent()
{
}

void StageComponent::paint (juce::Graphics& g)
{
    //g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));   // clear the background
    g.fillAll(juce::Colours::transparentBlack);
    g.setColour (juce::Colours::grey);
    g.drawRect (getLocalBounds(), 1);   // draw an outline around the component
}

void StageComponent::resized()
{
    int rotaryPixelSize = StageConstants::UI::sliderHeight;
    int labelWidth = StageConstants::UI::labelWidth;
    int labelHeight = StageConstants::UI::labelHeight;
    int pixelSpace = StageConstants::UI::pixelSpace;

    int scopeHeight = getHeight() - rotaryPixelSize - 2 * labelHeight - 6 * pixelSpace;
    //int phaserHeight = 2 * pixelSpace + labelHeight + rotaryPixelSize;
    int offset = 110;
    int width = getWidth() - offset;

    dataLabel.setBounds(0, 0, labelWidth + 20, labelHeight);
    dataToDisplayComboBox.setBounds(dataLabel.getRight(), 0, labelWidth + 40, labelHeight);
    int displayButtonWidth = (getWidth() - dataToDisplayComboBox.getRight()) / 3 - 3;
    displayEQButton.setBounds(dataToDisplayComboBox.getRight() + 1, dataToDisplayComboBox.getY(), displayButtonWidth, labelHeight);
    displayDistoButton.setBounds(displayEQButton.getRight() + 1, displayEQButton.getY(), displayButtonWidth, labelHeight);
    displayEnvButton.setBounds(displayDistoButton.getRight() + 1, displayDistoButton.getY(), displayButtonWidth, labelHeight);

    scope.setBounds(dataToDisplayComboBox.getX(), dataToDisplayComboBox.getBottom(), getWidth(), scopeHeight);

    eqComponent.setBounds(scope.getX(), scope.getBottom(), width, getHeight());

    switchEQToWaveShaper();
}

void StageComponent::switchEQToWaveShaper()//MainLayerProcessor::MainLayerProcessorStageID& currentStageProccessorID)
{
    int rotaryPixelSize = StageConstants::UI::sliderHeight;
    int labelHeight = StageConstants::UI::labelHeight;
    int pixelSpace = StageConstants::UI::pixelSpace;

    int scopeHeight = getHeight() - rotaryPixelSize - 2 * labelHeight - 6 * pixelSpace - rotaryPixelSize - 2 * labelHeight - 8 * pixelSpace;
    int eqHeight = 2 * labelHeight - 2 * pixelSpace;
    int phaserHeight = 2* pixelSpace + labelHeight + rotaryPixelSize;
    int offset = 110;
    int width = getWidth() - offset;
    scope.setBounds(dataLabel.getX(), dataToDisplayComboBox.getBottom(), getWidth(), scopeHeight);

    eqComponent.setBounds(scope.getX(), scope.getBottom(), width, eqHeight);

    eqComponent.switchToWaveshaper();

    phaserComponent.setBounds(eqComponent.getX(), eqComponent.getBottom(), width, phaserHeight); // - 2*labelHeight + 2*pixelSpace
    distortionComponent.setBounds(phaserComponent.getX(), phaserComponent.getBottom(), width, getHeight() - phaserComponent.getBottom());//
    inputRMSMeter.setBounds(eqComponent.getRight(), scope.getBottom(), offset / 2, getHeight() - scope.getBottom() - pixelSpace);
    outputRMSMeter.setBounds(inputRMSMeter.getRight(), inputRMSMeter.getY(), offset / 2, inputRMSMeter.getHeight());


    phaserComponent.setVisible(true);
    distortionComponent.setVisible(true);
    inputRMSMeter.setVisible(true);
    outputRMSMeter.setVisible(true);
}

void StageComponent::switchWaveShaperToEQ()//MainLayerProcessor::MainLayerProcessorStageID& currentStageProccessorID)
{
    int rotaryPixelSize = StageConstants::UI::sliderHeight;
    int labelHeight = StageConstants::UI::labelHeight;
    int pixelSpace = StageConstants::UI::pixelSpace;

    int scopeHeight = getHeight() - rotaryPixelSize - 2 * labelHeight - 6 * pixelSpace;
    //int width = getWidth() - 110;

    phaserComponent.setVisible(false);
    distortionComponent.setVisible(false);
    inputRMSMeter.setVisible(false);
    outputRMSMeter.setVisible(false);

    scope.setBounds(dataLabel.getX(), dataToDisplayComboBox.getBottom(), getWidth(), scopeHeight);

    eqComponent.setBounds(scope.getX(), scope.getBottom(), getWidth(), getHeight() - scope.getBottom());
    eqComponent.switchFromWaveshaper();
}

void StageComponent::buttonClicked(juce::Button* button)
{
    int stageID = dataStruct.getSelectedStageID();

    if (button == getDisplayEQButton())
    {
        dataStruct.setIsEQ(stageID, true);
        switchWaveShaperToEQ();
    }
    else if (button == getDisplayDistoButton())
    {
        dataStruct.setIsEQ(stageID, false);
        switchEQToWaveShaper();
    }
}

void StageComponent::comboBoxChanged(juce::ComboBox* comboBoxThatHasChanged)
{
    if (comboBoxThatHasChanged == &dataToDisplayComboBox)
    {
        int selectedID = dataToDisplayComboBox.getSelectedId() - 1;

        scope.switchScope(selectedID);
    }
}
