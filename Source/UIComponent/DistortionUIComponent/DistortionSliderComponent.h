/*
  ==============================================================================

    DistortionSliderComponent.h
    Created: 3 Jun 2021 12:16:52pm
    Author:  CookiMonstor

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

#include "DistortionScope.h"
#include "../Controls/OnOffButton.h"
#include "../Controls/MangledLabel.h"
#include "../Controls/MangledButton.h"
#include "../Controls/MangledSlider.h"
#include "../Controls/MangledComboBox.h"
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
    , public MangledComboBox::Listener
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
        DistortionScope* pDistortionScope = getScope();
        juce::Rectangle<int> wsScopeBounds = pDistortionScope->getBounds();

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
                int pointID = pDistortionScope->findPoint(c);
                int tensionID = pDistortionScope->findTension(c);

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
                int pointID = pDistortionScope->findPoint(c);

                if (pointID == -1)
                {
                    float scopeHeight = (float)pDistortionScope->getHeight();
                    float scopeWidth = (float)pDistortionScope->getWidth();

                    dataStruct.setMouseAddPointWS(c, scopeWidth, scopeHeight);
                    shouldDrag = true;
                }
            }

        }
    }

    void mouseDrag(const juce::MouseEvent& e)
    {
        juce::Point<int> mousePos = e.getPosition();

        DistortionScope* pDistortionScope = getScope();
        juce::Rectangle<int> wsScopeBounds = pDistortionScope->getBounds();

        bool isMouseOnWs = wsScopeBounds.contains(mousePos);

        if (isMouseOnWs)
        {
            if (shouldDrag)
            {
                juce::MouseEvent c = e.getEventRelativeTo(pDistortionScope);

                float scopeHeight = (float)pDistortionScope->getHeight();
                float scopeWidth = (float)pDistortionScope->getWidth();

                dataStruct.setMouseDragEventWS(c, scopeWidth, scopeHeight);
            }
        }
    }

    void mouseUp(const juce::MouseEvent& e)
    {
        juce::Point<int> mousePos = e.getPosition();

        DistortionScope* pDistortionScope = getScope();
        juce::Rectangle<int> wsScopeBounds = pDistortionScope->getBounds();

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

    void setUI(bool isBipolar);

    //Get Function
    MangledComboBox* getEquationTypeComboBox()      { return &eqaTypeComboBox; };
    MangledComboBox* getSigmoidEquationComboBox()   { return &sigmoidEquationComboBox; };
    MangledComboBox* getSymetricEquationComboBox()  { return &symEquationComboBox; };
    MangledComboBox* getAsymetricEquationComboBox() { return &asymEquationComboBox; };
    MangledComboBox* getSpecialEquationComboBox()   { return &specialEquationComboBox; };
    OnOffButton*     getBipolarOnOffButton()        { return &bipolarOnOffButton; };
    //juce::TextButton* getHybridModeOnOffButton()   { return &hybridModeOnOffButton; };
    OnOffButton*     getDistoProcFirstButton()      { return &distoProcFirstButton; };
    MangledButton*   getDeleteWSPointButton()       { return &deleteWSPointButton; };
    OnOffButton*     getDCFilterOnOffButton()       { return &dcFilterOnOffButton; };
    MangledButton*   getResetWSButton()             { return &resetWSButton; };
    MangledComboBox* getCurveTypeComboBox()         { return &piecewiseFunctionCurveComboBox; };
    MangledSlider*   getPreGainWaveShaperSlider()   { return &preGainWaveShaperSlider; };
    MangledSlider*   getDriveWaveShaperSlider()     { return &driveWaveShaperSlider; };
    MangledSlider*   getWarpWaveShaperSlider()      { return &warpWaveShaperSlider; };
    MangledSlider*   getPostGainWaveShaperSlider()  { return &postGainWaveShaperSlider; };
    MangledSlider*   getMixWaveShaperSlider()       { return &mixWaveShaperSlider; };
    MangledButton*   getResetAllButton()            { return &resetAllButton; };
    DistortionScope* getScope()                     { return &distortionScope; };

private:
    juce::Label eqaTypeLabel;
    MangledComboBox eqaTypeComboBox;

    juce::Label sigmoidEquationLabel;
    MangledComboBox sigmoidEquationComboBox;

    juce::Label symEquationLabel;
    MangledComboBox symEquationComboBox;

    juce::Label asymEquationLabel;
    MangledComboBox asymEquationComboBox;

    juce::Label specialEquationLabel;
    MangledComboBox specialEquationComboBox;
    
    //juce::ListBox listBox;
    //DistoList distoList;
    //juce::Label distortionEquationForNegativeValueWaveShaperLabel;
    //MangledComboBox distortionEquationForNegativeValueWaveShaperComboBox;

    OnOffButton bipolarOnOffButton;
    //juce::TextButton bipolarOnOffButton;

    //juce::TextButton hybridModeOnOffButton;

    OnOffButton distoProcFirstButton;

    MangledButton deleteWSPointButton;

    OnOffButton dcFilterOnOffButton;

    MangledButton resetWSButton;

    MangledComboBox piecewiseFunctionCurveComboBox;

    DistortionScope distortionScope;

    MangledSlider preGainWaveShaperSlider;
    MangledLabel preGainWaveShaperLabel;

    MangledSlider driveWaveShaperSlider;
    MangledLabel driveWaveShaperLabel;

    MangledSlider warpWaveShaperSlider;
    MangledLabel warpWaveShaperLabel;

    MangledSlider postGainWaveShaperSlider;
    MangledLabel postGainWaveShaperLabel;

    MangledSlider mixWaveShaperSlider;
    MangledLabel mixWaveShaperLabel;

    MangledButton resetAllButton;

    AudioEngineState<float>& dataStruct;

    bool shouldDrag = false;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DistortionSliderComponent)
};

