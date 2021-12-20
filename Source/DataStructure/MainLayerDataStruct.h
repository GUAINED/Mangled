/*
  ==============================================================================

    ValueTreeMainLayer.h
    Created: 23 Sep 2021 11:04:33am
    Author:  CookiMonstor

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "MangledIdentifiers.h"
#include "Conversion.h"
//#include "../Processor/UnitProcessor/SampleRemapperBin.h"

class MainLayerDataStruct: juce::ValueTree::Listener
{
    //=================================================================================================================
    //The data structure is composed of 2 structures:
    //  -the APVTS which handle almost all button/slider/cb
    //  -the ValueTree which handle EVERY others importants parameters
    //  such has: Point for Waveshaper, Points for enveloppes...
    //The Value Tree State is design as such:
    //=================================================================================================================

public:
    MainLayerDataStruct(juce::AudioProcessor& processorToConnectTo,
        const juce::Identifier& valueTreeType,
        int nbOfStages,
        int nbOfFiltersPerEQ,
        int nbOfDistoUnitPerDisto)
        : apvtsMainLayer(processorToConnectTo, &undoManager, valueTreeType, createParametersLayout(nbOfStages, nbOfFiltersPerEQ, nbOfDistoUnitPerDisto))
    {
        //apvtsMainLayer.state.addListener(processorToConnectTo);
        //root = new ValueTreeWsBloc(createRootTree());
        juce::Identifier id(MainLayerConstants::ParamStringID::mainLayer);
        juce::ValueTree vt(id);
        rootMainLayer = vt;
        //apvtsMainLayer.state.addChild(rootMainLayer, 1, nullptr);
        createValueTreeMainLayer();

        apvtsMainLayer.state.addListener(this);
        rootMainLayer.addListener(this);
        undoManager.clearUndoHistory();
        undoManager.beginNewTransaction();
    };

    //~DataStructMainLayer();
    
    void valueTreePropertyChanged(juce::ValueTree& treeWhosePropertyHasChanged, const juce::Identifier& property) override;
    void valueTreeChildAdded(juce::ValueTree& parentTree, juce::ValueTree& childWhichHasBeenAdded) override;
    void valueTreeChildRemoved(juce::ValueTree& parentTree, juce::ValueTree& childWhichHasBeenRemoved, int indexFromWhichChildWasRemoved) override;
    void valueTreeChildOrderChanged(juce::ValueTree& parentTreeWhoseChildrenHaveMoved, int oldIndex, int newIndex) override;
    void valueTreeParentChanged(juce::ValueTree& treeWhoseParentHasChanged) override;
    void valueTreeRedirected(juce::ValueTree& treeWhichHasBeenChanged) override;

    //Create ValueTree=====================================================================================================
    void createValueTreeMainLayer();
    void createValueTreeStage(juce::ValueTree& vt, int stageID);
    void createValueTreeScope(juce::ValueTree& vt);
    void createValueTreeEQ(juce::ValueTree& vt);
    void createValueTreeDistortion(juce::ValueTree& vt);
    void createValueTreeDistortionUnit(juce::ValueTree& vt, int distortionUnitID);
    void createValueTreeSampleRemapper(juce::ValueTree& vt);
    //void createValueTreeWaveShaperDragPointSave(juce::ValueTree& vt);
    void createValueTreeSampleRemapperPointUnipolar(juce::ValueTree& vt);
    void createValueTreeSampleRemapperPointBipolar(juce::ValueTree& vt);
    juce::ValueTree createPoint(float pointX, float pointY, float tension, int curveID, bool horizontalDragOn);
    juce::ValueTree createPointNoListener(float pointX, float pointY, float tension, int curveID, bool horizontalDragOn);

    void createValueTreeDistortionCircuit(juce::ValueTree& vt);
    //Audio Engine function=================================================================================================
    void resetAudioEngineParam();
    float getMasterGainValue();

    //Main Layer function===================================================================================================
    void resetMainLayerParam();

    //Stage function=========================================================================================================
    void resetStageParam(int stageID);
    void setSelectedStageID(int stageID);
    void setIsEQ(int stageID, bool newIsEQ);

    //Scope function=========================================================================================================
    void resetScopeParam(int stageID);
    void setScopeZoom(int stageID, int zoomID);
    void setScopePreEQIsBypassed(int stageID, bool newIsBypassed);
    void setScopePostEQIsBypassed(int stageID, bool newIsBypassed);
    void setScopePostDistoIsBypassed(int stageID, bool newIsBypassed);

    void setScopeIsBypassed(int stageID, bool newIsBypassed);

    //EQ function============================================================================================================
    void resetEQParam(int stageID);
    
    //Set function EQ
    void setEQFilterCutOff(int stageID, int filterID, float newCutoff);
    void setEQFilterQ(int stageID, int filterID, float newQ);
    void setEQFilterGain(int stageID, int filterID, float newGain);
    void setEQFilterType(int stageID, int filterID, int filterType);
    void setEQFilterIsBypassed(int stageID, int filterID, bool newIsBypassed);
    void setEQFilterIsActive(int stageID, int filterID, bool newIsActive);
    void setSelectedFilterID(int stageID, int filterID);
    //Mouse for EQ
    void setMouseDragEventEQ(const juce::MouseEvent& e, float scopeWidth, float scopeHeight);
    void setMouseAddFilterEQ(const juce::MouseEvent& e, int filterID,float scopeWidth, float scopeHeight);
    void setMouseWheelEventEQ(const juce::MouseEvent& event, const juce::MouseWheelDetails& wheel, int filterID);
    int findNextAvailableFilterIDEQ(int stageID);
    int findNextActiveFilterDown(int stageID);
    int findNextActiveFilterUp(int stageID);

    //Phaser==================================================================================================================
    void resetPhaserParam(int stageID);

    //Distortion==============================================================================================================
    void resetDistortionParam(int stageID);
    void resetDistortionDUParam(int stageID, int distortionUnitID);

    //SampleRemapper Function
    void switchFromUnipolarToBipolar(int stageID, int distortionUnitID);
    void switchFromBipolarToUnipolar(int stageID, int distortionUnitID);

    //Value Tree Add/Remove point from SampleRemapper
    void setMouseAddPointWS(juce::MouseEvent& e, float scopeWidth, float scopeHeight);
    void setMouseDragEventWS(juce::MouseEvent& e, float scopeWidth, float scopeHeight);
    void setMouseUpWS();
    void addPoint(int stageID, int distortionUnitID, float pointX, float pointY);
    void deleteSelectedPoint(int stageID, int distortionUnitID);
    void resetSampleRemapper(int stageID, int distortionUnitID);

    //Set function SampleRemapper
    void setDistoUnitID(int stageID, int distortionUnitID);
    void setSelectedDistoUnitID(int distortionUnitID);
    void setPoint(int stageID, int distortionUnitID, int pointID, juce::Point<float> newPoint);
    void setPointX(int stageID, int distortionUnitID, int pointID, float newPointX);
    void setPointY(int stageID, int distortionUnitID, int pointID, float newPointY);
    void setPointXNoListener(int stageID, int distortionUnitID, int pointID, float newPointX);
    void setPointYNoListener(int stageID, int distortionUnitID, int pointID, float newPointY);
    void setSelectedPoint(int stageID, int distortionUnitID, juce::Point<float> newPoint);
    void setTension(int stageID, int distortionUnitID, int pointID, float newTension);
    void setTensionNoListener(int stageID, int distortionUnitID, int pointID, float newTension);
    void setSelectedTension(int stageID, int distortionUnitID, float newTension);
    void setIsBipolar(int stageID, int distortionUnitID, bool isBipolar);
    void setHorizontalDragOn(int stageID, int distortionUnitID, int pointID, bool horizontalDragOn);
    void setPointCurveType(int stageID, int distortionUnitID, int pointID, int curveType);
    void setPointCurveTypeNoListener(int stageID, int distortionUnitID, int pointID, int curveType);
    void setSelectedIDPoint(int stageID, int distortionUnitID, int pointID);
    void setSelectedIDTension(int stageID, int distortionUnitID, int tensionID);
    void setSelectedIDCurve(int stageID, int distortionUnitID, int curveID);
    void setPointAndTensionID(int stageID, int distortionUnitID, int pointID, int tensionID);
    void setPointAndTensionIDNoTransaction(int stageID, int distortionUnitID, int pointID, int tensionID);
    
    //Distortion Circuit
    void setDistortionCircuitEquation(int stageID, int distortionUnitID, int equationType, int equationID);
    
    //Get Function=========================================================================================================
    juce::AudioProcessorValueTreeState& getAPVTSMainLayer() { return apvtsMainLayer; };
    juce::UndoManager& getUndoManager() { return undoManager; };

    juce::ValueTree getRoot() { return rootMainLayer; };
    juce::ValueTree getStageVT(int stageID);
    juce::ValueTree getSelectedStageVT();
    juce::ValueTree getScopeVT(int stageID);
    juce::ValueTree getEQVT(int stageID);
    juce::ValueTree getDistortionVT(int stageID);
    juce::ValueTree getDistoUnitVT(int stageID, int distoUnitID);
    juce::ValueTree getSelectedDistoUnitVT();
    juce::ValueTree getSampleRemapperVT(int stageID, int distortionUnitID);
    juce::ValueTree getDistortionCircuitVT(int stageID, int distortionUnitID);
    juce::ValueTree getSampleRemapperPointsVT(int stageID, int distortionUnitID);
    
    //MainLayer=============================================================================================================
    int getSelectedStageID();

    //Scope
    int getScopeZoom(int stageID);

    //EQ
    int getSelectedFilterID(int stageID);
    int getEQFilterType(int stageID, int filterID);
    float getEQFilterCutoff(int stageID, int filterID);
    float getEQFilterQ(int stageID, int filterID);
    float getEQFilterGain(int stageID, int filterID);
    bool getEQFilterIsBypassed(int stageID, int filterID);
    bool getEQFilterIsActive(int stageID, int filterID);

    //Distortion
    bool getDistoUnitRouting(int stageID, int distortionUnitID);
    float getDistoUnitDrive(int stageID, int distortionUnitID);
    //Sample Remapper
    int getSelectedDistoUnitID();
    int getDistortionUnitID(int stageID);
    bool getIsEQ(int stageID);
    bool getSelectedIsEQ();
    int getNbOfPoints(int stageID, int distortionUnitID);
    float getPointX(int stageID, int distortionUnitID, int pointID);
    float getPointY(int stageID, int distortionUnitID, int pointID);
    juce::Point<float> getPoint(int stageID, int distortionUnitID, int pointID);
    juce::Point<float> getSelectedPoint(int stageID, int distortionUnitID);
    int getSelectedPointID(int stageID, int distortionUnitID);
    int getSelectedCurveID(int stageID, int distortionUnitID);
    float getTension(int stageID, int distortionUnitID, int pointID);
    int getSelectedTensionID(int stageID, int distortionUnitID);
    bool getHorizontalDragOn(int stageID, int distortionUnitID, int pointID);
    int getPointCurveType(int stageID, int distortionUnitID, int pointID);
    bool getIsBipolar(int stageID, int distortionUnitID);
    
    int getDistortionCircuitEquationType(int stageID, int distortionUnitID);
    int getDistortionCircuitEquationID(int stageID, int distortionUnitID);
    //Should update
    void setShouldUpdate(bool newValue) { shouldUpdate = newValue; };
    bool getShouldUpdate() { return shouldUpdate; };

    void trigListener() { getRoot().sendPropertyChangeMessage(StageConstants::ParamStringID::selectedStageID); };
private:
    juce::AudioProcessorValueTreeState::ParameterLayout createParametersLayout(int nbOfStages, 
                                                                                int nbOfFiltersPerEQ, 
                                                                                int nbOfDistoUnitPerDisto);

    void createMainParametersLayout(std::vector<std::unique_ptr<juce::RangedAudioParameter>>* plugInParameters);

    void createMainLayerParametersLayout(std::vector<std::unique_ptr<juce::RangedAudioParameter>>* plugInParameters, int nbOfStages, int nbOfFiltersPerEQ, int nbOfDistoUnitPerDisto);

    void createStageParametersLayout(std::vector<std::unique_ptr<juce::RangedAudioParameter>>* plugInParameters,
                                        int stageID, 
                                        int nbOfFiltersPerEQ, 
                                        int nbOfDistoUnitPerDisto);

    void createScopeParametersLayout(std::vector<std::unique_ptr<juce::RangedAudioParameter>>* plugInParameters, int stageID);
    void createEQParametersLayout(std::vector<std::unique_ptr<juce::RangedAudioParameter>> *plugInParameters, int stageID, int nbOfFiltersPerEQ);
    void createPhaserParametersLayout(std::vector<std::unique_ptr<juce::RangedAudioParameter>>* plugInParameters, int stageID);
    void createDistortionParametersLayout(std::vector<std::unique_ptr<juce::RangedAudioParameter>>* plugInParameters, int stageID, int nbOfDistoUnitPerDisto);
    juce::AudioProcessorValueTreeState apvtsMainLayer;
    juce::ValueTree rootMainLayer;
    juce::UndoManager undoManager;

    std::atomic<bool> shouldUpdate;
};