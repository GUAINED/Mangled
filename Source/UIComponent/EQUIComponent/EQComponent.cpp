/*
  ==============================================================================

    EQComponent.cpp
    Created: 8 Apr 2021 6:40:44pm
    Author:  CookiMonstor

  ==============================================================================
*/

#include <JuceHeader.h>
#include "EQComponent.h"

//==============================================================================
EQComponent::EQComponent(AudioEngineState<float>& mlDataStruct)
    : dataStruct(mlDataStruct)
    , onOffButton("", "On", true)
{
    //Reset Button
    resetButton.setButtonText("Reset");
    resetButton.addListener(this);
    addAndMakeVisible(resetButton);

    //Bypass
    addAndMakeVisible(onOffButton);

    //Filter Id Button
    filterIdEQLabel.setColour(juce::Label::ColourIds::textColourId, juce::Colours::black);
    filterIdEQLabel.setColour(juce::Label::ColourIds::backgroundColourId, juce::Colours::white);
    filterIdEQLabel.setJustificationType(juce::Justification::centred);
    filterIdEQLabel.setText("Double-Click To Add Filter", juce::dontSendNotification);

    addAndMakeVisible(filterIdEQLabel);

    filterIdDownEQButton.setButtonText("Down");
    filterIdDownEQButton.addListener(this);
    addChildComponent(filterIdDownEQButton);
    //filterIdDownEQButton.setEnabled(false);

    //filterOnOffEQButton.setButtonText("Off");
    //filterOnOffEQButton.setColour(juce::TextButton::ColourIds::buttonColourId, juce::Colours::red);
    //filterOnOffEQButton.setClickingTogglesState(true);
    //filterOnOffEQButton.setColour(juce::TextButton::ColourIds::buttonOnColourId, juce::Colours::green);

    //addAndMakeVisible(filterOnOffEQButton);

    filterIdUpEQButton.setButtonText("Up");
    filterIdUpEQButton.addListener(this);
    addChildComponent(filterIdUpEQButton);

    //eqSlider.add(new EQSliderComponent);
    //addAndMakeVisible(eqSlider[0]);

    for (int i = 0; i < EQConstants::Processor<float>::nbOfFilterMax; ++i)
    {
        eqSlider.add(new EQSliderComponent(mlDataStruct, EQConstants::UI::FilterColour::filterColour[i]));
        addChildComponent(eqSlider[i]);
        eqSlider[i]->getFilterIsActiveEQTextButton()->addListener(this);
    }

    //Mix EQ
    mixSlider.setSliderStyle(juce::Slider::SliderStyle::LinearHorizontal);
    mixSlider.setTextBoxStyle(juce::Slider::TextBoxRight, true, 50, 20);
    mixLabel.setText("Mix EQ: ", juce::dontSendNotification);
    mixLabel.setJustificationType(juce::Justification::centred);

    addAndMakeVisible(mixSlider);
    addAndMakeVisible(mixLabel);

}

EQComponent::~EQComponent()
{
}

void EQComponent::paint (juce::Graphics& g)
{
    //g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));   // clear the background
    g.fillAll(juce::Colours::transparentBlack); 
    //const auto bounds = getBounds().toFloat();
    //juce::ColourGradient grad = juce::ColourGradient{ juce::Colours::transparentBlack,
    //                                                    bounds.getBottomLeft(),
    //                                                    juce::Colours::mediumpurple.withMultipliedAlpha(0.6f),
    //                                                    bounds.getTopLeft(),
    //                                                    false };
    //g.setGradientFill(grad);
    //g.fillRect(getBounds());
    g.setColour (juce::Colours::grey);
    g.drawRect (getLocalBounds(), 1);   // draw an outline around the component
}

void EQComponent::resized()
{
    int labelWidth = EQConstants::UI::labelWidth;
    int labelHeight = EQConstants::UI::labelHeight;
    int pixelSpace = EQConstants::UI::pixelSpace;

    //EQ========================================================================================================================================
    resetButton.setBounds(5, 0, labelWidth, labelHeight);

    filterIdEQLabel.setBounds((getWidth() - (labelWidth + 50))/2, 0, labelWidth + 50, labelHeight);
    filterIdDownEQButton.setBounds(filterIdEQLabel.getX() - (labelWidth - 20), 0, labelWidth - 20, labelHeight);
    filterIdUpEQButton.setBounds(filterIdEQLabel.getRight(), filterIdDownEQButton.getY(), labelWidth - 20, labelHeight);

    mixSlider.setBounds(getWidth() - 2 * labelWidth, 0, 2 * labelWidth, labelHeight);

    onOffButton.setBounds(mixSlider.getX() - pixelSpace - labelWidth, filterIdUpEQButton.getY(), labelWidth, labelHeight);

    for (int i = 0; i < EQConstants::Processor<float>::nbOfFilterMax; ++i)
    {
        eqSlider[i]->setBounds(0, onOffButton.getBottom(), getWidth(), getHeight() - labelHeight);
    }

}

void EQComponent::switchToEQSliderMenu(int newEQSliderID)
{
    //currentEQSliderVisible = newEQSliderID;
    
    for (int i = 0; i < EQConstants::Processor<float>::nbOfFilterMax; ++i)
    {
        eqSlider[i]->setVisible(false);
        //eqSlider[i]->setEnabled(false);
    }

    if (newEQSliderID != -1)
    {
        eqSlider[newEQSliderID]->setVisible(true);
    }
}

void EQComponent::switchToWaveshaper()
{
    int labelHeight = EQConstants::UI::labelHeight;
    int pixelSpace = EQConstants::UI::pixelSpace;
    onOffButton.setVisible(false);
    resetButton.setVisible(false);
    filterIdDownEQButton.setVisible(false);
    filterIdEQLabel.setVisible(false);
    filterIdUpEQButton.setVisible(false);

    mixSlider.setVisible(false);
    mixLabel.setVisible(false);

    for (int i = 0; i < EQConstants::Processor<float>::nbOfFilterMax; ++i)
    {
        eqSlider[i]->setBounds(0, -pixelSpace, getLocalBounds().getWidth(), 2*labelHeight - pixelSpace);
        eqSlider[i]->setSmallGradient();
    }

}

void EQComponent::switchFromWaveshaper()
{
    int labelHeight = 20;

    onOffButton.setVisible(true);
    resetButton.setVisible(true);
    filterIdDownEQButton.setVisible(true);
    filterIdEQLabel.setVisible(true);
    filterIdUpEQButton.setVisible(true);

    mixSlider.setVisible(true);
    mixLabel.setVisible(true);

    //for (int i = 0; i < EQConstants::nbOfFilterMax; ++i)
    //{
    //    eqSlider[i]->setBounds(0, 0, getLocalBounds().getWidth(), labelHeight);
    //}

    for (int i = 0; i < EQConstants::Processor<float>::nbOfFilterMax; ++i)
    {
        eqSlider[i]->setBounds(0, onOffButton.getBottom(), getLocalBounds().getWidth(), getLocalBounds().getHeight() - labelHeight);
        eqSlider[i]->setLargeGradient();
    }
}

void EQComponent::buttonClicked(juce::Button* button)
{
    int stageID = dataStruct.getSelectedStageID();
    int filterID = dataStruct.getSelectedFilterID(stageID);
    if (button == getResetButton())
    {
        dataStruct.setSelectedFilterID(stageID, -1);
        dataStruct.setNbOfActiveFilter(stageID, 0);
        dataStruct.resetEQParam(stageID);

    }
    else if (button == eqSlider[filterID]->getFilterIsActiveEQTextButton())
    {
        eqSlider[filterID]->setVisible(!eqSlider[filterID]->getFilterIsActiveEQTextButton()->getToggleState());
        if (eqSlider[filterID]->getFilterIsActiveEQTextButton()->getToggleState())
        {
            dataStruct.remodeActiveFilterCount(stageID);
            dataStruct.setEQFilterType(stageID, filterID, 0);
        }

        //dataStruct.setSelectedFilterID(stageID, -1);
        //int newFilterID = dataStruct.findNextActiveFilterUp(stageID);
        //dataStruct.setSelectedFilterID(stageID, newFilterID);
        //switchToEQSliderMenu(newFilterID);
    }
    else if (button == &filterIdUpEQButton)
    {
        int selectedFilterID = dataStruct.findNextActiveFilterUp(stageID);
        if(selectedFilterID == -1)
        {
            return;
        }

        dataStruct.setSelectedFilterID(stageID, selectedFilterID);
        switchToEQSliderMenu(selectedFilterID);
    }
    //EQ Filter ID Down
    else if (button == &filterIdDownEQButton)
    {
        int selectedFilterID = dataStruct.findNextActiveFilterDown(stageID);
        if (selectedFilterID == -1)
        {
            return;
        }
        dataStruct.setSelectedFilterID(stageID, selectedFilterID);
        switchToEQSliderMenu(selectedFilterID);
    }
}

void EQComponent::updateOnOffButton(juce::Button* button, juce::String addedString)
{
    auto state = button->getToggleState();
    juce::String stateString = state ? "ON" : "OFF";

    button->setButtonText(addedString + stateString);
}

//void EQComponent::addFilter()
//{
//    //isAnyActiveFilter
//    nbOfActiveFilter += 1;
//}

//void EQComponent::removeFilter()
//{
//    nbOfActiveFilter -= 1;
//
//    if(nbOfActiveFilter < 0)
//        nbOfActiveFilter = 0;
//
//    if (nbOfActiveFilter == 0)
//    {
//        updateFilterLabelForNoFilter();
//
//    }
//    else if (nbOfActiveFilter > EQConstants::Processor<float>::nbOfFilterMax)
//    {
//        nbOfActiveFilter = EQConstants::Processor<float>::nbOfFilterMax;
//    }
//    else
//    {
//        //filterIdEQLabel.setColour(juce::Label::ColourIds::textColourId, juce::Colours::black);
//        //filterIdEQLabel.setColour(juce::Label::ColourIds::backgroundColourId, juce::Colours::white);
//        //filterIdEQLabel.setText("Filter number: " + (juce::String)(currentEQSliderVisible + 1) + "/" + (juce::String)nbOfActiveFilter, juce::dontSendNotification);
//    }
//}

void EQComponent::updateFilterLabelForNoFilter()
{
    filterIdEQLabel.setColour(juce::Label::ColourIds::textColourId, juce::Colours::black);
    filterIdEQLabel.setColour(juce::Label::ColourIds::backgroundColourId, juce::Colours::white);
    filterIdEQLabel.setText("Double-Click To Add Filter", juce::dontSendNotification);
}
