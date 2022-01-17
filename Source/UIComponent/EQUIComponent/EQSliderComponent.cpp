/*
  ==============================================================================

    EQSliderComponent.cpp
    Created: 1 Jun 2021 4:13:23pm
    Author:  CookiMonstor

  ==============================================================================
*/

#include <JuceHeader.h>
#include "EQSliderComponent.h"

//==============================================================================
EQSliderComponent::EQSliderComponent(AudioEngineState<float>& mlDataStruct, juce::Colour initBackgroundColour)
    : dataStruct(mlDataStruct)
    , filterOnOffEQButton("", "Off", EQConstants::BiquadConstants<float>::isBypassedStartValue)
    , backgroundColour(initBackgroundColour)
{
    //Filter CutOff==================================================================================================
    filterCutoffEQSlider.setSliderStyle(juce::Slider::SliderStyle::LinearHorizontal);
    filterCutoffEQSlider.setTextBoxStyle(juce::Slider::TextBoxRight, true, 100, 20);
    filterCutoffEQSlider.setTextBoxIsEditable(true);
    addAndMakeVisible(filterCutoffEQSlider);

    juce::String labelString = EQConstants::AutomationString::filter + EQConstants::AutomationString::cutoff + ":";
    filterCutoffEQLabel.setText(labelString, juce::dontSendNotification);
    addAndMakeVisible(filterCutoffEQLabel);

    //Filter Q=======================================================================================================
    filterQEQSlider.setSliderStyle(juce::Slider::SliderStyle::LinearHorizontal);
    filterQEQSlider.setTextBoxStyle(juce::Slider::TextBoxRight, true, 100, 20);
    filterQEQSlider.setTextBoxIsEditable(true);
    addAndMakeVisible(filterQEQSlider);

    labelString = EQConstants::AutomationString::filter + EQConstants::AutomationString::q + ":";
    filterQEQLabel.setText(labelString, juce::dontSendNotification);
    addAndMakeVisible(filterQEQLabel);

    //Filter Gain===================================================================================================
    filterGainEQSlider.setSliderStyle(juce::Slider::SliderStyle::LinearHorizontal);
    filterGainEQSlider.setTextBoxStyle(juce::Slider::TextBoxRight, true, 100, 20);
    filterGainEQSlider.setTextBoxIsEditable(true);
    addAndMakeVisible(filterGainEQSlider);

    labelString = EQConstants::AutomationString::filter + EQConstants::AutomationString::gain + ":";
    filterGainEQLabel.setText(labelString, juce::dontSendNotification);
    addAndMakeVisible(filterGainEQLabel);

    //Filter Type==================================================================================================
    for (int index = 0; index < EQConstants::BiquadConstants<float>::Types::maxTypes; ++index)
    {
        filterTypeEQComboBox.addItem(EQConstants::UI::typeStringArray[index], index + 1);
    }
    filterTypeEQComboBox.setSelectedId(1);
    filterTypeEQComboBox.addListener(this);
    addAndMakeVisible(filterTypeEQComboBox);

    labelString = EQConstants::AutomationString::filter + EQConstants::AutomationString::type + ":";
    filterTypeEQLabel.setText(labelString, juce::dontSendNotification);
    addAndMakeVisible(filterTypeEQLabel);

    //Filter Order================================================================================================
    for (int index = 0; index < EQConstants::BiquadConstants<float>::Orders::maxOrders; ++index)
    {
        filterOrderEQComboBox.addItem(EQConstants::UI::orderStringArray[index], index + 1);
    }
    filterOrderEQComboBox.setSelectedId(1);

    addAndMakeVisible(filterOrderEQComboBox);

    labelString = EQConstants::AutomationString::filter + EQConstants::AutomationString::order + ":";
    filterOrderEQLabel.setText(labelString, juce::dontSendNotification);
    addAndMakeVisible(filterOrderEQLabel);


    //Filter On Off==============================================================================================
    addAndMakeVisible(filterOnOffEQButton);

    //Filter is Active===========================================================================================
    filterIsActiveEQButton.setButtonText(EQConstants::AutomationString::deleteFilter);
    filterIsActiveEQButton.setColour(juce::TextButton::ColourIds::buttonColourId, juce::Colours::black);
    filterIsActiveEQButton.setColour(juce::TextButton::ColourIds::textColourOnId, juce::Colours::yellow);
    filterIsActiveEQButton.setColour(juce::TextButton::ColourIds::textColourOffId, juce::Colours::yellow);
    filterIsActiveEQButton.setClickingTogglesState(true);
    filterIsActiveEQButton.setToggleState(! EQConstants::BiquadConstants<float>::isActiveStartValue, juce::sendNotification);
    //filterIsActiveEQButton.addListener(this);
    addAndMakeVisible(filterIsActiveEQButton);
}

EQSliderComponent::~EQSliderComponent()
{
}

void EQSliderComponent::paint (juce::Graphics& g)
{

    //g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));   // clear the background
    //g.fillAll(juce::Colours::black);

    //g.setColour(backgroundColour);
    //g.setOpacity(0.3f);
    g.setGradientFill(colourGradient);
    g.fillRect(getLocalBounds());
    g.setColour (juce::Colours::grey);
    g.drawRect (getLocalBounds(), 1);   // draw an outline around the component

    //g.setColour (juce::Colours::white);
    //g.setFont (14.0f);
    //g.drawText ("EQSliderComponent", getLocalBounds(),
    //            juce::Justification::centred, true);   // draw some placeholder text
}

void EQSliderComponent::resized()
{
    int labelWidth = EQConstants::UI::labelWidth;
    int labelHeight = EQConstants::UI::labelHeight;
    int pixelSpace = EQConstants::UI::pixelSpace;

    filterTypeEQLabel.setBounds(pixelSpace, pixelSpace, labelWidth + 5, labelHeight);
    filterTypeEQComboBox.setBounds(filterTypeEQLabel.getRight(), filterTypeEQLabel.getY(), labelWidth + labelWidth/2, labelHeight);

    filterOnOffEQButton.setBounds( filterTypeEQComboBox.getRight() + pixelSpace,  filterTypeEQComboBox.getY(), labelWidth, labelHeight);

    filterOrderEQLabel.setBounds(filterOnOffEQButton.getRight() + pixelSpace, filterTypeEQComboBox.getY(), labelWidth + 5, labelHeight);
    filterOrderEQComboBox.setBounds(filterOrderEQLabel.getRight(), filterOrderEQLabel.getY(), labelWidth + 30, labelHeight);

    filterIsActiveEQButton.setBounds(getRight() - labelWidth - pixelSpace, filterTypeEQComboBox.getY(), labelWidth, labelHeight);

    filterCutoffEQLabel.setBounds(filterTypeEQLabel.getX(),  filterTypeEQLabel.getBottom() + pixelSpace, labelWidth, labelHeight);
    filterCutoffEQSlider.setBounds(filterCutoffEQLabel.getRight(),  filterCutoffEQLabel.getY(),  getWidth() - labelWidth - 2*pixelSpace, labelHeight);

    filterQEQLabel.setBounds(filterCutoffEQLabel.getX(),  filterCutoffEQLabel.getBottom() + pixelSpace, labelWidth, labelHeight);
    filterQEQSlider.setBounds(filterQEQLabel.getRight(),  filterQEQLabel.getY(),  getWidth() - labelWidth - 2*pixelSpace, labelHeight);

    filterGainEQLabel.setBounds(filterQEQLabel.getX(), filterQEQLabel.getBottom() + pixelSpace, labelWidth, labelHeight);
    filterGainEQSlider.setBounds(filterGainEQLabel.getRight(), filterGainEQLabel.getY(),  getWidth() - labelWidth - 2*pixelSpace, labelHeight);

    setSmallGradient();
}
void EQSliderComponent::setLargeGradient()
{
    const auto bounds = getBounds().toFloat();
    colourGradient = juce::ColourGradient{ backgroundColour.withMultipliedAlpha(0.7f), bounds.getBottomLeft(),juce::Colours::transparentWhite, bounds.getTopLeft(), false };

}
void EQSliderComponent::setSmallGradient()
{
    //const auto bounds = getBounds().toFloat();
    //juce::Point<float> bottom = bounds.getBottomLeft();
    //juce::Point<float> top = bounds.getTopLeft();
    //top.setY(top.getY() + 20.0f);
    //colourGradient = juce::ColourGradient{ backgroundColour.withMultipliedAlpha(0.6f), bottom,juce::Colours::transparentWhite, top, false };

    const auto bounds = getBounds().toFloat();
    juce::Point<float> bottom = bounds.getTopRight();
    juce::Point<float> top = bounds.getTopLeft();
    //top.setY(top.getY() + 20.0f);
    colourGradient = juce::ColourGradient{ backgroundColour.withMultipliedAlpha(0.8f), bottom,juce::Colours::transparentWhite, top, false };

}
void EQSliderComponent::buttonClicked(juce::Button* button)
{
    //int stageID = dataStruct.getSelectedStageID();

    if (button == &filterIsActiveEQButton)
    {
        setVisible(!filterIsActiveEQButton.getToggleState());

        //int filterID = dataStruct.findNextActiveFilterDown(stageID);
        //dataStruct.setSelectedFilterID(stageID, filterID);
    }
}

void EQSliderComponent::comboBoxChanged(juce::ComboBox* comboBoxThatHasChanged)
{
    if (comboBoxThatHasChanged == getFilterTypeEQComboBox())
    {
        int filterType = comboBoxThatHasChanged->getSelectedId() - 1;

        if (filterType == EQConstants::BiquadConstants<float>::Types::peak ||
            filterType == EQConstants::BiquadConstants<float>::Types::lowshelf ||
            filterType == EQConstants::BiquadConstants<float>::Types::highshelf)
        {
            filterGainEQLabel.setVisible(true);
            getFilterGainEQSlider()->setVisible(true);
            getFilterQEQSlider()->setVisible(true);
        }
        else if (filterType == EQConstants::BiquadConstants<float>::Types::lowpass ||
                filterType == EQConstants::BiquadConstants<float>::Types::highpass ||
                filterType == EQConstants::BiquadConstants<float>::Types::notch ||
                filterType == EQConstants::BiquadConstants<float>::Types::bandpass)
        {
            filterGainEQLabel.setVisible(false);
            getFilterGainEQSlider()->setVisible(false);
            getFilterQEQSlider()->setVisible(true);
        }
        else
        {
            filterGainEQLabel.setVisible(true);
            getFilterCutoffEQSlider()->setVisible(true);
            getFilterGainEQSlider()->setVisible(true);
            getFilterQEQSlider()->setVisible(true);
        }
    }
}
