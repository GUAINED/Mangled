/*
  ==============================================================================

    DistortionSliderComponent.h
    Created: 3 Jun 2021 12:16:52pm
    Author:  CookiMonstor

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

#include "WaveShaperScope.h"
#include "../Controls/OnOffButton.h"
#include "../Controls/MangledSlider.h"
//class DistoList : public juce::ListBoxModel
//{
//public:
//    DistoList() {};
//
//    int getNumRows() override { return 10; };
//
//    void paintListBoxItem(int rowNumber, juce::Graphics& g, int width, int height, bool roxIsSelected) override
//    {
//
//    }
//};

//==============================================================================
/*
*/
class DistortionSliderComponent  
    : public juce::Component
    , public juce::ComboBox::Listener
    , public juce::Button::Listener
{
public:
    DistortionSliderComponent(AudioEngineState<float>& mlDataStruct);
    ~DistortionSliderComponent() override;

    void paint (juce::Graphics&) override;
    void resized() override;

    void mouseDown(const juce::MouseEvent& e)
    {
        juce::Point<int> mousePos = e.getPosition();
        //Stage
        int stageID = dataStruct.getSelectedStageID();

        //WaveShaper Scope
        int distortionUnitID = dataStruct.getSelectedDistoUnitID();
        //juce::MouseEvent mouseEventRelativeWS = e.getEventRelativeTo(ent()->getDistortionSlider(distortionUnitID));

        //mousePos = mouseEventRelativeWS.getPosition();
        WaveShaperScope* pWaveShaperScope = getScope();
        juce::Rectangle<int> wsScopeBounds = pWaveShaperScope->getBounds();

        bool isBipolar = dataStruct.getIsBipolar(stageID, distortionUnitID);

        if (!isBipolar)
        {
            int xPos = wsScopeBounds.getX() + wsScopeBounds.getWidth() / 2 - 5;
            wsScopeBounds.setX(xPos);
        }

        bool isMouseOnWs = wsScopeBounds.contains(mousePos);

        //WaveShaper Event============================================================================================================================================================================
        if (isMouseOnWs)
        {
            juce::MouseEvent c =  e.getEventRelativeTo(getScope());

            if (e.getNumberOfClicks() == 1)
            {
                int pointID = pWaveShaperScope->findPoint(c);
                int tensionID = pWaveShaperScope->findTension(c);

                dataStruct.setPointAndTensionID(stageID, distortionUnitID, pointID, tensionID);

                if (pointID != -1)
                {
                    shouldDrag = true;
                    int curveType = dataStruct.getPointCurveType(stageID, distortionUnitID, pointID);
                    getCurveTypeComboBox()->setSelectedId(curveType + 1, juce::dontSendNotification);
                }
                else if (tensionID != -1)
                {
                    shouldDrag = true;
                    int curveType = dataStruct.getPointCurveType(stageID, distortionUnitID, tensionID);
                    getCurveTypeComboBox()->setSelectedId(curveType + 1, juce::dontSendNotification);
                }
            }
            else if (e.getNumberOfClicks() == 2)
            {
                int pointID = pWaveShaperScope->findPoint(c);

                if (pointID == -1)
                {
                    float scopeHeight = (float)pWaveShaperScope->getHeight();
                    float scopeWidth = (float)pWaveShaperScope->getWidth();

                    dataStruct.setMouseAddPointWS(c, scopeWidth, scopeHeight);
                    shouldDrag = true;
                }
            }

        }
    }

    void mouseDrag(const juce::MouseEvent& e)
    {
        juce::Point<int> mousePos = e.getPosition();

        WaveShaperScope* pWaveShaperScope = getScope();
        juce::Rectangle<int> wsScopeBounds = pWaveShaperScope->getBounds();

        bool isMouseOnWs = wsScopeBounds.contains(mousePos);

        if (isMouseOnWs)
        {
            if (shouldDrag)
            {
                juce::MouseEvent c = e.getEventRelativeTo(pWaveShaperScope);

                float scopeHeight = (float)pWaveShaperScope->getHeight();
                float scopeWidth = (float)pWaveShaperScope->getWidth();

                dataStruct.setMouseDragEventWS(c, scopeWidth, scopeHeight);
            }
        }
    }

    void mouseUp(const juce::MouseEvent& e)
    {
        juce::Point<int> mousePos = e.getPosition();

        WaveShaperScope* pWaveShaperScope = getScope();
        juce::Rectangle<int> wsScopeBounds = pWaveShaperScope->getBounds();

        bool isMouseOnWs = wsScopeBounds.contains(mousePos);

        if (isMouseOnWs && shouldDrag)
        {
            shouldDrag = false;
        }
    }

    //Button Listener
    void buttonClicked(juce::Button* button) override;

    //ComboBox Listener
    void comboBoxChanged(juce::ComboBox* comboBox) override;

    void switchComboBox();

    void setUI(int circuitID, int circuitType, bool isBipolar);

    //Get Function
    juce::ComboBox*   getEquationTypeComboBox()      { return &eqaTypeComboBox; };
    juce::ComboBox*   getSigmoidEquationComboBox()   { return &sigmoidEquationComboBox; };
    juce::ComboBox*   getSymetricEquationComboBox()  { return &symEquationComboBox; };
    juce::ComboBox*   getAsymetricEquationComboBox() { return &asymEquationComboBox; };
    juce::ComboBox*   getSpecialEquationComboBox()   { return &specialEquationComboBox; };
    OnOffButton*      getBipolarOnOffButton()        { return &bipolarOnOffButton; };
    //juce::TextButton* getHybridModeOnOffButton()   { return &hybridModeOnOffButton; };
    OnOffButton*      getDistoProcFirstButton()      { return &distoProcFirstButton; };
    juce::TextButton* getDeleteWSPointButton()       { return &deleteWSPointButton; };
    OnOffButton*      getDCFilterOnOffButton()       { return &dcFilterOnOffButton; };
    juce::TextButton* getResetWSButton()             { return &resetWSButton; };
    juce::ComboBox*   getCurveTypeComboBox()         { return &piecewiseFunctionCurveComboBox; };
    MangledSlider*     getPreGainWaveShaperSlider()   { return &preGainWaveShaperSlider; };
    MangledSlider*     getDriveWaveShaperSlider()     { return &driveWaveShaperSlider; };
    MangledSlider*     getWarpWaveShaperSlider()      { return &warpWaveShaperSlider; };
    MangledSlider*     getPostGainWaveShaperSlider()  { return &postGainWaveShaperSlider; };
    MangledSlider*     getMixWaveShaperSlider()       { return &mixWaveShaperSlider; };
    juce::TextButton* getResetAllButton() { return &resetAllButton; };
    WaveShaperScope* getScope() { return &waveShaperScope; };

private:
    juce::Label eqaTypeLabel;
    juce::ComboBox eqaTypeComboBox;

    juce::Label sigmoidEquationLabel;
    juce::ComboBox sigmoidEquationComboBox;

    juce::Label symEquationLabel;
    juce::ComboBox symEquationComboBox;

    juce::Label asymEquationLabel;
    juce::ComboBox asymEquationComboBox;

    juce::Label specialEquationLabel;
    juce::ComboBox specialEquationComboBox;
    
    //juce::ListBox listBox;
    //DistoList distoList;
    //juce::Label distortionEquationForNegativeValueWaveShaperLabel;
    //juce::ComboBox distortionEquationForNegativeValueWaveShaperComboBox;

    OnOffButton bipolarOnOffButton;
    //juce::TextButton bipolarOnOffButton;

    //juce::TextButton hybridModeOnOffButton;

    OnOffButton distoProcFirstButton;

    juce::TextButton deleteWSPointButton;

    OnOffButton dcFilterOnOffButton;

    juce::TextButton resetWSButton;

    juce::ComboBox piecewiseFunctionCurveComboBox;

    WaveShaperScope waveShaperScope;

    MangledSlider preGainWaveShaperSlider;
    juce::Label preGainWaveShaperLabel;

    MangledSlider driveWaveShaperSlider;
    juce::Label driveWaveShaperLabel;

    MangledSlider warpWaveShaperSlider;
    juce::Label warpWaveShaperLabel;

    MangledSlider postGainWaveShaperSlider;
    juce::Label postGainWaveShaperLabel;

    MangledSlider mixWaveShaperSlider;
    juce::Label mixWaveShaperLabel;

    juce::TextButton resetAllButton;

    AudioEngineState<float>& dataStruct;

    bool shouldDrag = false;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DistortionSliderComponent)
};

