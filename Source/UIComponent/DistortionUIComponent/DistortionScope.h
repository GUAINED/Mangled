/*
  ==============================================================================

    DistortionScope.h
    Created: 9 Apr 2021 2:09:04pm
    Author:  CookiMonstor

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "../../Processor/UnitProcessor/Distortion/DistortionProcessor.h"
#include "../../Processor/UnitProcessor/Distortion/DistortionCircuit.h"
#include "PiecewiseFunctionPointDragger.h"
#include "PiecewiseFunctionTensionDragger.h"
#include "../../Processor/UnitProcessor/Distortion/PiecewiseFunctionBin.h"
#include "../../DataStructure/MangledIdentifiers.h"
#include "../../DataStructure/MainLayerDataStruct.h"
#include "../../DataStructure/MangledState.h"
#include "../GeneralUIComponent/SelectablePointDragger.h"
//#include "../../DataStructure/ValueTreeWs.h"

//==============================================================================
/*
*/
class DistortionScope  : public juce::Component
{
public:
    DistortionScope(AudioEngineState<float>& mlDataStruct);
    ~DistortionScope() override;

    void paint (juce::Graphics&) override;
    void resized() override;

    void updateUI(int distortionUnitID, PiecewiseFunction<float>* pSM);

    //Set function================================================================================================================================================================
    void setPiecewiseFunctionPointIsSelected(int selectedPointID);
    void setPiecewiseFunctionTensionIsSelected(int selectedCurveID);
    void drawPiecewiseFunctionPath(juce::Path* path, const float* binXData, const float* binYData);// , float* eqBinYData);
    void drawUnipolarPath(const float* binXData, const float* binYData);
    void drawBipolarPath(const float* binXData, const float* binYData);
    void drawSelectedCurvePath(const float* binXData, const float* binYData);
    void drawInnerGrid();
    void drawOuterGrid();
    void drawIsBipolarRectangle();
    void setPointAndTension(int binID, bool pointVisible, float pointX, float pointY, bool tensionVisible, float tensionX, float tensionY);

    //Find Point Or Tension Dragger
    int findPoint(juce::MouseEvent& e);
    int findTension(juce::MouseEvent& e);
    bool isMouseOnWSScope(const juce::MouseEvent& e);

private:
    juce::OwnedArray<SelectablePointDragger> pointDraggerVector;
    juce::OwnedArray<PiecewiseFunctionTensionDragger> tensionDraggerVector;

    juce::OwnedArray<juce::AudioBuffer<float>> binPathData;
    
    //Path
    juce::Path innerGridPath;
    juce::Path outerGridPath;
    juce::Path isBipolarPath;
    juce::Path unipolarSampleRemapPath;
    juce::Path bipolarSampleRemapPath;
    juce::Path selectedCurveSampleRemapPath;
    juce::Path equationDistortionPath;

    juce::Colour bipolarColour = juce::Colours::darkgrey;

    AudioEngineState<float>& dataStruct;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DistortionScope)
};
