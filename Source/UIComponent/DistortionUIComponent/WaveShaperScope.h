/*
  ==============================================================================

    WaveShaperScope.h
    Created: 9 Apr 2021 2:09:04pm
    Author:  CookiMonstor

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "../../Processor/UnitProcessor/DistortionProcessor.h"
#include "../../Processor/UnitProcessor/DistortionCircuit.h"
#include "WaveShaperPointDragger.h"
#include "WaveShaperTensionDragger.h"
#include "../../Processor/UnitProcessor/SampleRemapperBin.h"
#include "../../DataStructure/MangledIdentifiers.h"
#include "../../DataStructure/MainLayerDataStruct.h"
#include "../GeneralUIComponent/SelectablePointDragger.h"
//#include "../../DataStructure/ValueTreeWs.h"

//==============================================================================
/*
*/
class WaveShaperScope  : public juce::Component
{
public:
    WaveShaperScope(MainLayerDataStruct& mlDataStruct);
    ~WaveShaperScope() override;

    void paint (juce::Graphics&) override;
    void resized() override;

    void updateUI(MainLayerDataStruct& mainLayerDataStruct, SampleRemapper<float>* pSM);

    //Set function================================================================================================================================================================
    void setWaveShaperPointIsSelected(int selectedPointID);
    void setWaveShaperTensionIsSelected(int selectedCurveID);
    void drawSampleRemapperPath(juce::Path* path, float* binXData, float* binYData);// , float* eqBinYData);
    void drawUnipolarPath(float* binXData, float* binYData);
    void drawBipolarPath(float* binXData, float* binYData);
    void drawSelectedCurvePath(float* binXData, float* binYData);
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
    juce::OwnedArray<WaveShaperTensionDragger> tensionDraggerVector;

    //Path
    juce::Path innerGridPath;
    juce::Path outerGridPath;
    juce::Path isBipolarPath;
    juce::Path unipolarSampleRemapPath;
    juce::Path bipolarSampleRemapPath;
    juce::Path selectedCurveSampleRemapPath;
    juce::Path equationDistortionPath;

    juce::Colour bipolarColour = juce::Colours::darkgrey;

    MainLayerDataStruct& dataStruct;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (WaveShaperScope)
};
