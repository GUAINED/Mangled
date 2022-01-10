/*
  ==============================================================================

    DistortionSliderComponent.cpp
    Created: 3 Jun 2021 12:16:52pm
    Author:  CookiMonstor

  ==============================================================================
*/

#include <JuceHeader.h>
#include "DistortionSliderComponent.h"

//==============================================================================
DistortionSliderComponent::DistortionSliderComponent(MainLayerDataStruct& mlDataStruct)
    : distoProcFirstButton("Routing", "On", true)
    , bipolarOnOffButton("Bipolar", "Off", false)
    , dcFilterOnOffButton("DC Filter", "On", true)
    , resetAllButton("Reset Unit")
    , waveShaperScope(mlDataStruct)
    , dataStruct(mlDataStruct)
    //: listBox("Test", &distoList)
{
    //setInterceptsMouseClicks(false, true);

    eqaTypeLabel.setText("Equation Type", juce::dontSendNotification);
    eqaTypeLabel.setJustificationType(juce::Justification::centred);
    eqaTypeComboBox.addItemList(DistortionConstants::UI::equationTypeStringArray, 1);
    eqaTypeComboBox.setSelectedId(1, juce::dontSendNotification);
    eqaTypeComboBox.addListener(this);
    addAndMakeVisible(eqaTypeLabel);
    addAndMakeVisible(eqaTypeComboBox);

    //Distortion Equation
    sigmoidEquationLabel.setText("Sigmoid Equation", juce::dontSendNotification);
    sigmoidEquationLabel.setJustificationType(juce::Justification::centred);
    sigmoidEquationComboBox.addItemList(DistortionConstants::UI::sigmoidEQAStringArray, 1);
    sigmoidEquationComboBox.setSelectedId(1, juce::dontSendNotification);
    //sigmoidEquationComboBox.addListener(this);
    addAndMakeVisible(sigmoidEquationLabel);
    addAndMakeVisible(sigmoidEquationComboBox);

    symEquationLabel.setText("Symetric Equation", juce::dontSendNotification);
    symEquationLabel.setJustificationType(juce::Justification::centred);
    symEquationComboBox.addItemList(DistortionConstants::UI::symetricEQAStringArray, 1);
    symEquationComboBox.setSelectedId(1, juce::dontSendNotification);
    //symEquationComboBox.addListener(this);
    addChildComponent(symEquationLabel);
    addChildComponent(symEquationComboBox);

    asymEquationLabel.setText("Asymetric Equation", juce::dontSendNotification);
    asymEquationLabel.setJustificationType(juce::Justification::centred);
    asymEquationComboBox.addItemList(DistortionConstants::UI::asymetricEQAStringArray, 1);
    asymEquationComboBox.setSelectedId(1, juce::dontSendNotification);
    //asymEquationComboBox.addListener(this);
    addChildComponent(asymEquationLabel);
    addChildComponent(asymEquationComboBox);

    specialEquationLabel.setText("Special Equation", juce::dontSendNotification);
    specialEquationLabel.setJustificationType(juce::Justification::centred);
    specialEquationComboBox.addItemList(DistortionConstants::UI::specialEQAStringArray, 1);
    specialEquationComboBox.setSelectedId(1, juce::dontSendNotification);
    //specialEquationComboBox.addListener(this);
    addChildComponent(specialEquationLabel);
    addChildComponent(specialEquationComboBox);

    //listBox.setModel(&distoList);
    //addAndMakeVisible(listBox);

    //Bipolar Button
    //bipolarOnOffButton.setClickingTogglesState(true);
    //bipolarOnOffButton.setButtonText("Bipolar Off");
    //bipolarOnOffButton.setColour(juce::TextButton::ColourIds::buttonColourId, juce::Colours::red);
    //bipolarOnOffButton.setColour(juce::TextButton::ColourIds::buttonOnColourId, juce::Colours::green);
    //bipolarOnOffButton.onClick = [this] {updateOnOffButton(&bipolarOnOffButton, "Bipolar "); };
    //bipolarOnOffButton.setToggleState(false, juce::dontSendNotification);

    bipolarOnOffButton.addListener(this);
    addAndMakeVisible(bipolarOnOffButton);

    //Hybrid Mode Button
    //hybridModeOnOffButton.setClickingTogglesState(true);
    //hybridModeOnOffButton.setButtonText("Hybrid Mode Off");
    //hybridModeOnOffButton.setColour(juce::TextButton::ColourIds::buttonColourId, juce::Colours::red);
    //hybridModeOnOffButton.setColour(juce::TextButton::ColourIds::buttonOnColourId, juce::Colours::green);
    //hybridModeOnOffButton.onClick = [this] {updateOnOffButton(&hybridModeOnOffButton, "Hybrid Mode "); };
    //hybridModeOnOffButton.setToggleState(false, juce::dontSendNotification);

    //addAndMakeVisible(hybridModeOnOffButton);
    //hybridModeOnOffButton.addListener(this);

    //distoProcFirstButton.setClickingTogglesState(true);
    //distoProcFirstButton.setButtonText("Disto 1st");
    //distoProcFirstButton.setColour(juce::TextButton::ColourIds::buttonColourId, juce::Colours::red);
    //distoProcFirstButton.setColour(juce::TextButton::ColourIds::buttonOnColourId, juce::Colours::green);
    //distoProcFirstButton.onClick = [this] {updateOnOffButton(&distoProcFirstButton, "Disto 1st "); };
    //distoProcFirstButton.setToggleState(true, juce::dontSendNotification);
    addAndMakeVisible(distoProcFirstButton);

    //Delete WaveShaper Point Button
    deleteWSPointButton.setButtonText("Delete Point");
    deleteWSPointButton.addListener(this);
    addAndMakeVisible(deleteWSPointButton);

    //DC Filter On Off Button
    //dcFilterOnOffButton.setClickingTogglesState(true);
    //dcFilterOnOffButton.setButtonText("DC Filter On");
    //dcFilterOnOffButton.setColour(juce::TextButton::ColourIds::buttonColourId, juce::Colours::red);
    //dcFilterOnOffButton.setColour(juce::TextButton::ColourIds::buttonOnColourId, juce::Colours::green);
    //dcFilterOnOffButton.onClick = [this] {updateOnOffButton(&dcFilterOnOffButton, "DC Filter "); };
    //dcFilterOnOffButton.setToggleState(true, juce::dontSendNotification);
    addAndMakeVisible(dcFilterOnOffButton);

    //Reset WaveShaper Point Button
    resetWSButton.setButtonText("Reset Map");
    resetWSButton.addListener(this);
    addAndMakeVisible(resetWSButton);

    //SampleRemapper Curve Type
    sampleRemapperCurveComboBox.addItemList(DistortionConstants::UI::curveStringArray, 1);
    sampleRemapperCurveComboBox.setSelectedId(1, juce::dontSendNotification);
    sampleRemapperCurveComboBox.addListener(this);
    addAndMakeVisible(sampleRemapperCurveComboBox);

    //WaveShaper Scope
    addAndMakeVisible(waveShaperScope);

    //PreGain
    //preGainWaveShaperSlider.setSliderStyle(juce::Slider::SliderStyle::LinearBarVertical);
    preGainWaveShaperSlider.setSliderStyle(juce::Slider::SliderStyle::LinearVertical);
    preGainWaveShaperSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 50, 20);
    //preGainWaveShaperSlider.setTextBoxStyle(juce::Slider::NoTextBox, true, 50, 20);
    preGainWaveShaperSlider.setTextBoxIsEditable(true);
    preGainWaveShaperLabel.setText("Pre", juce::dontSendNotification);
    preGainWaveShaperLabel.setJustificationType(juce::Justification::centred);

    addAndMakeVisible(preGainWaveShaperSlider);
    addAndMakeVisible(preGainWaveShaperLabel);

    //Drive
    driveWaveShaperSlider.setSliderStyle(juce::Slider::SliderStyle::LinearVertical);
    driveWaveShaperSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 50, 20);
    driveWaveShaperSlider.setTextBoxIsEditable(true);
    driveWaveShaperLabel.setText("Drive", juce::dontSendNotification);
    driveWaveShaperLabel.setJustificationType(juce::Justification::centred);

    addAndMakeVisible(driveWaveShaperSlider);
    addAndMakeVisible(driveWaveShaperLabel);
    //driveWaveShaperSlider.addListener(this);

    warpWaveShaperSlider.setSliderStyle(juce::Slider::SliderStyle::LinearVertical);
    warpWaveShaperSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 50, 20);
    warpWaveShaperSlider.setTextBoxIsEditable(true);
    warpWaveShaperLabel.setText("Warp", juce::dontSendNotification);
    warpWaveShaperLabel.setJustificationType(juce::Justification::centred);

    addAndMakeVisible(warpWaveShaperSlider);
    addAndMakeVisible(warpWaveShaperLabel);

    //PostGain
    postGainWaveShaperSlider.setSliderStyle(juce::Slider::SliderStyle::LinearVertical);
    postGainWaveShaperSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 50, 20);
    postGainWaveShaperSlider.setTextBoxIsEditable(true);
    postGainWaveShaperLabel.setText("Post", juce::dontSendNotification);
    postGainWaveShaperLabel.setJustificationType(juce::Justification::centred);

    addAndMakeVisible(postGainWaveShaperSlider);
    addAndMakeVisible(postGainWaveShaperLabel);

    //Mix WaveShaper
    mixWaveShaperSlider.setSliderStyle(juce::Slider::SliderStyle::LinearVertical);
    mixWaveShaperSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 50, 20);
    //mixWaveShaperSlider.setTextBoxIsEditable(true);
    mixWaveShaperLabel.setText("Mix WS", juce::dontSendNotification);
    mixWaveShaperLabel.setJustificationType(juce::Justification::centred);

    addAndMakeVisible(mixWaveShaperSlider);
    addAndMakeVisible(mixWaveShaperLabel);

    resetAllButton.addListener(this);
    addAndMakeVisible(resetAllButton);
}

DistortionSliderComponent::~DistortionSliderComponent()
{
}

void DistortionSliderComponent::paint (juce::Graphics& g)
{
    /* This demo code just fills the component's background and
       draws some placeholder text to get you started.

       You should replace everything in this method with your own
       drawing code..
    */

    //g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));   // clear the background
    g.fillAll(juce::Colours::transparentBlack);
    g.setColour (juce::Colours::grey);
    //g.drawRect (getLocalBounds(), 1);   // draw an outline around the component

    //g.setColour (juce::Colours::white);
    //g.setFont (14.0f);
    //g.drawText ("DistortionSliderComponent", getLocalBounds(),
    //            juce::Justification::centred, true);   // draw some placeholder text
}

void DistortionSliderComponent::resized()
{
    int labelWidth = DistortionConstants::UI::labelWidth;
    int labelHeight = DistortionConstants::UI::labelHeight;
    int pixelSpace = DistortionConstants::UI::pixelSpace;
    int cbWidth = 2 * labelWidth - 15;

    eqaTypeLabel.setBounds(0, pixelSpace, cbWidth, labelHeight);
    eqaTypeComboBox.setBounds(eqaTypeLabel.getX() + pixelSpace, eqaTypeLabel.getBottom(), cbWidth - 5, labelHeight);

    sigmoidEquationLabel.setBounds(0, eqaTypeComboBox.getBottom() + 5, cbWidth, labelHeight);
    sigmoidEquationComboBox.setBounds(sigmoidEquationLabel.getX() + pixelSpace, sigmoidEquationLabel.getBottom(), cbWidth - 5, labelHeight);

    symEquationLabel.setBounds(sigmoidEquationLabel.getBounds());
    symEquationComboBox.setBounds(sigmoidEquationComboBox.getBounds());

    asymEquationLabel.setBounds(sigmoidEquationLabel.getBounds());
    asymEquationComboBox.setBounds(sigmoidEquationComboBox.getBounds());

    specialEquationLabel.setBounds(sigmoidEquationLabel.getBounds());
    specialEquationComboBox.setBounds(sigmoidEquationComboBox.getBounds());

    distoProcFirstButton.setBounds(specialEquationComboBox.getX(), specialEquationComboBox.getBottom() + pixelSpace, labelWidth, labelHeight);
    dcFilterOnOffButton.setBounds(distoProcFirstButton.getX(), distoProcFirstButton.getBottom() + 1, labelWidth, labelHeight);
    resetAllButton.setBounds(dcFilterOnOffButton.getX(), dcFilterOnOffButton.getBottom() + 1, labelWidth, labelHeight);

    //sigmoidEquationLabel.setBounds(0, pixelSpace, cbWidth, labelHeight);
    //sigmoidEquationComboBox.setBounds(sigmoidEquationLabel.getX() + pixelSpace, sigmoidEquationLabel.getBottom(), cbWidth - 5, labelHeight);

    //symEquationLabel.setBounds(0, sigmoidEquationComboBox.getBottom() + 5, cbWidth, labelHeight);
    //symEquationComboBox.setBounds(symEquationLabel.getX() + pixelSpace, symEquationLabel.getBottom(), cbWidth - 5, labelHeight);

    //asymEquationLabel.setBounds(0, symEquationComboBox.getBottom() + 5, cbWidth, labelHeight);
    //asymEquationComboBox.setBounds(asymEquationLabel.getX() + pixelSpace, asymEquationLabel.getBottom(), cbWidth - 5, labelHeight);

    //specialEquationLabel.setBounds(0, asymEquationComboBox.getBottom() + 5, cbWidth, labelHeight);
    //specialEquationComboBox.setBounds(specialEquationLabel.getX() + pixelSpace, specialEquationLabel.getBottom(), cbWidth - 5, labelHeight);

    waveShaperScope.setBounds(sigmoidEquationComboBox.getRight() + pixelSpace, pixelSpace, getHeight() - 2 * pixelSpace, getHeight() - 2 * pixelSpace);



    bipolarOnOffButton.setBounds(waveShaperScope.getRight() + pixelSpace, waveShaperScope.getY(), labelWidth, labelHeight);

    //hybridModeOnOffButton.setBounds(bipolarOnOffButton.getX(), bipolarOnOffButton.getBottom(), labelWidth, labelHeight);

    deleteWSPointButton.setBounds(bipolarOnOffButton.getX(), bipolarOnOffButton.getBottom() + 1, labelWidth, labelHeight);

    resetWSButton.setBounds(deleteWSPointButton.getX(), deleteWSPointButton.getBottom() + 1, labelWidth, labelHeight);

    sampleRemapperCurveComboBox.setBounds(resetWSButton.getX(), resetWSButton.getBottom() + 1, labelWidth, labelHeight);



    //waveShaperScope.setCentrePosition(waveShaperScope.getRight() / 2, waveShaperScope.getBottom() / 2);

    labelWidth -= 20;
    auto sliderHeight = getLocalBounds().getHeight() - 2 * pixelSpace - labelHeight;
    preGainWaveShaperLabel.setBounds(bipolarOnOffButton.getRight(), waveShaperScope.getY(), labelWidth, labelHeight);
    preGainWaveShaperSlider.setBounds(preGainWaveShaperLabel.getX(), preGainWaveShaperLabel.getBottom(), labelWidth, sliderHeight);

    driveWaveShaperLabel.setBounds(preGainWaveShaperLabel.getRight(), preGainWaveShaperLabel.getY(), labelWidth, labelHeight);
    driveWaveShaperSlider.setBounds(driveWaveShaperLabel.getX(), driveWaveShaperLabel.getBottom(), labelWidth, sliderHeight);

    warpWaveShaperLabel.setBounds(driveWaveShaperLabel.getRight(), driveWaveShaperLabel.getY(), labelWidth, labelHeight);
    warpWaveShaperSlider.setBounds(warpWaveShaperLabel.getX(), warpWaveShaperLabel.getBottom(), labelWidth, sliderHeight);

    postGainWaveShaperLabel.setBounds(warpWaveShaperLabel.getRight(), warpWaveShaperLabel.getY(), labelWidth, labelHeight);
    postGainWaveShaperSlider.setBounds(postGainWaveShaperLabel.getX(), postGainWaveShaperLabel.getBottom(), labelWidth, sliderHeight);

    mixWaveShaperLabel.setBounds(postGainWaveShaperLabel.getRight(), postGainWaveShaperLabel.getY(), labelWidth, labelHeight);
    mixWaveShaperSlider.setBounds(mixWaveShaperLabel.getX(), mixWaveShaperLabel.getBottom(), labelWidth, sliderHeight);

    
}
void DistortionSliderComponent::buttonClicked(juce::Button* button)
{
    int stageID = dataStruct.getSelectedStageID();
    int distortionUnitID = dataStruct.getSelectedDistoUnitID();

    if (button == getResetAllButton())
    {
        dataStruct.resetDistortionDUParam(stageID, distortionUnitID);
        getSigmoidEquationComboBox()->setSelectedId(1, juce::dontSendNotification);
        getSymetricEquationComboBox()->setSelectedId(0, juce::dontSendNotification);
        getAsymetricEquationComboBox()->setSelectedId(0, juce::dontSendNotification);
        getSpecialEquationComboBox()->setSelectedId(0, juce::dontSendNotification);
    }
    //WaveShaper Button
    else if (button == getBipolarOnOffButton()) //Display Main Clipper Menu
    {
        bool isBipolar = getBipolarOnOffButton()->getToggleState();
        dataStruct.setIsBipolar(stageID, distortionUnitID, isBipolar);

    }
    else if (button == getDeleteWSPointButton()) //Display Main Clipper Menu
    {
        dataStruct.deleteSelectedPoint(stageID, distortionUnitID);
    }
    else if (button == getResetWSButton()) //Display Main Clipper Menu
    {
        dataStruct.resetSampleRemapper(stageID, distortionUnitID);
    }
}

void DistortionSliderComponent::comboBoxChanged(juce::ComboBox* comboBox)
{
    int stageID = dataStruct.getSelectedStageID();
    int distortionUnitID = dataStruct.getSelectedDistoUnitID();

    if (comboBox == getCurveTypeComboBox())
    {
        int curveType = comboBox->getSelectedId() - 1;
        int curveID = dataStruct.getSelectedCurveID(stageID, distortionUnitID);
        dataStruct.setPointCurveType(stageID, distortionUnitID, curveID, curveType);
    }
    else if (comboBox == &eqaTypeComboBox)
    {
        switchComboBox();
    }
    //else if (comboBox == getSigmoidEquationComboBox())
    //{
    //    int equationType = DistortionCircuitConstants::Processor<float>::EquationType::sigmoid;
    //    int equationID = comboBox->getSelectedId() - 1;
    //    getSymetricEquationComboBox()->setSelectedId(0, juce::dontSendNotification);
    //    getAsymetricEquationComboBox()->setSelectedId(0, juce::dontSendNotification);
    //    getSpecialEquationComboBox()->setSelectedId(0, juce::dontSendNotification);
    //    dataStruct.setDistortionCircuitEquation(stageID, distortionUnitID, equationType, equationID);
    //}
    //else if (comboBox == getSymetricEquationComboBox())
    //{
    //    int equationType = DistortionCircuitConstants::Processor<float>::EquationType::symetric;
    //    int equationID = comboBox->getSelectedId() - 1;
    //    getSigmoidEquationComboBox()->setSelectedId(0, juce::dontSendNotification);
    //    getAsymetricEquationComboBox()->setSelectedId(0, juce::dontSendNotification);
    //    getSpecialEquationComboBox()->setSelectedId(0, juce::dontSendNotification);

    //    dataStruct.setDistortionCircuitEquation(stageID, distortionUnitID, equationType, equationID);
    //}
    //else if (comboBox == getAsymetricEquationComboBox())
    //{
    //    int equationType = DistortionCircuitConstants::Processor<float>::EquationType::asymetric;
    //    int equationID = comboBox->getSelectedId() - 1;
    //    getSigmoidEquationComboBox()->setSelectedId(0, juce::dontSendNotification);
    //    getSymetricEquationComboBox()->setSelectedId(0, juce::dontSendNotification);
    //    getSpecialEquationComboBox()->setSelectedId(0, juce::dontSendNotification);

    //    dataStruct.setDistortionCircuitEquation(stageID, distortionUnitID, equationType, equationID);
    //}
    //else if (comboBox == getSpecialEquationComboBox())
    //{
    //    int equationType = DistortionCircuitConstants::Processor<float>::EquationType::special;
    //    int equationID = comboBox->getSelectedId() - 1;
    //    getSigmoidEquationComboBox()->setSelectedId(0, juce::dontSendNotification);
    //    getSymetricEquationComboBox()->setSelectedId(0, juce::dontSendNotification);
    //    getAsymetricEquationComboBox()->setSelectedId(0, juce::dontSendNotification);

    //    dataStruct.setDistortionCircuitEquation(stageID, distortionUnitID, equationType, equationID);
    //}
}

//void DistortionSliderComponent::updateOnOffButton(juce::Button* button, juce::String addedString = "")
//{
//    auto state = button->getToggleState();
//    juce::String stateString = state ? "ON" : "OFF";
//
//    button->setButtonText(addedString + stateString);
//}

void DistortionSliderComponent::switchComboBox()
{
    switch (eqaTypeComboBox.getSelectedId() - 1)
    {
    case DistortionCircuitConstants::Processor<float>::EquationType::sigmoid:
        getSigmoidEquationComboBox()->setVisible(true);
        getSymetricEquationComboBox()->setVisible(false);
        getAsymetricEquationComboBox()->setVisible(false);
        getSpecialEquationComboBox()->setVisible(false);

        sigmoidEquationLabel.setVisible(true);
        symEquationLabel.setVisible(false);
        asymEquationLabel.setVisible(false);
        specialEquationLabel.setVisible(false);
        break;
    case DistortionCircuitConstants::Processor<float>::EquationType::symetric:
        getSigmoidEquationComboBox()->setVisible(false);
        getSymetricEquationComboBox()->setVisible(true);
        getAsymetricEquationComboBox()->setVisible(false);
        getSpecialEquationComboBox()->setVisible(false);

        sigmoidEquationLabel.setVisible(false);
        symEquationLabel.setVisible(true);
        asymEquationLabel.setVisible(false);
        specialEquationLabel.setVisible(false);
        break;
    case DistortionCircuitConstants::Processor<float>::EquationType::asymetric:
        getSigmoidEquationComboBox()->setVisible(false);
        getSymetricEquationComboBox()->setVisible(false);
        getAsymetricEquationComboBox()->setVisible(true);
        getSpecialEquationComboBox()->setVisible(false);

        sigmoidEquationLabel.setVisible(false);
        symEquationLabel.setVisible(false);
        asymEquationLabel.setVisible(true);
        specialEquationLabel.setVisible(false);
        break;
    case DistortionCircuitConstants::Processor<float>::EquationType::special:
        getSigmoidEquationComboBox()->setVisible(false);
        getSymetricEquationComboBox()->setVisible(false);
        getAsymetricEquationComboBox()->setVisible(false);
        getSpecialEquationComboBox()->setVisible(true);

        sigmoidEquationLabel.setVisible(false);
        symEquationLabel.setVisible(false);
        asymEquationLabel.setVisible(false);
        specialEquationLabel.setVisible(true);
        break;
    default:
        getSigmoidEquationComboBox()->setVisible(true);
        getSymetricEquationComboBox()->setVisible(false);
        getAsymetricEquationComboBox()->setVisible(false);
        getSpecialEquationComboBox()->setVisible(false);

        sigmoidEquationLabel.setVisible(true);
        symEquationLabel.setVisible(false);
        asymEquationLabel.setVisible(false);
        specialEquationLabel.setVisible(false);
        break;
    }
}

void DistortionSliderComponent::setUI(int circuitID, int circuitType)
{
    switchComboBox();
    //switch (circuitType)
    //{
    //    case DistortionCircuitConstants::Processor<float>::EquationType::sigmoid :
    //        sigmoidEquationComboBox.setSelectedId(circuitID + 1, juce::dontSendNotification);
    //        symEquationComboBox.setSelectedId(0, juce::dontSendNotification);
    //        asymEquationComboBox.setSelectedId(0, juce::dontSendNotification);
    //        specialEquationComboBox.setSelectedId(0, juce::dontSendNotification);
    //        break;
    //    case DistortionCircuitConstants::Processor<float>::EquationType::symetric:
    //        sigmoidEquationComboBox.setSelectedId(0, juce::dontSendNotification);
    //        symEquationComboBox.setSelectedId(circuitID + 1, juce::dontSendNotification);
    //        asymEquationComboBox.setSelectedId(0, juce::dontSendNotification);
    //        specialEquationComboBox.setSelectedId(0, juce::dontSendNotification);
    //        break;
    //    case DistortionCircuitConstants::Processor<float>::EquationType::asymetric:
    //        sigmoidEquationComboBox.setSelectedId(0, juce::dontSendNotification);
    //        symEquationComboBox.setSelectedId(0, juce::dontSendNotification);
    //        asymEquationComboBox.setSelectedId(circuitID + 1, juce::dontSendNotification);
    //        specialEquationComboBox.setSelectedId(0, juce::dontSendNotification);
    //        break;
    //    case DistortionCircuitConstants::Processor<float>::EquationType::special:
    //        sigmoidEquationComboBox.setSelectedId(0, juce::dontSendNotification);
    //        symEquationComboBox.setSelectedId(0, juce::dontSendNotification);
    //        asymEquationComboBox.setSelectedId(0, juce::dontSendNotification);
    //        specialEquationComboBox.setSelectedId(circuitID + 1, juce::dontSendNotification);
    //        break;
    //    default:

    //    break;
    //}
}