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
#include "../Processor/AudioEngine.h"
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
        , shouldUpdate(true)
        , rootAudioEngine(AudioEngineConstants::ParamStringID::audioEngine)
        //, phaserState(apvtsMainLayer)
    {
        //apvtsMainLayer.state.addListener(processorToConnectTo);
        //root = new ValueTreeWsBloc(createRootTree());
        //juce::Identifier id(MainLayerConstants::ParamStringID::mainLayer);
        //juce::ValueTree vt(id);
        //rootMainLayer = vt;
        //apvtsMainLayer.state.addChild(rootMainLayer, 1, nullptr);
        rootAudioEngine.setProperty(AudioEngineConstants::ParamStringID::selectedState,
                                    AudioEngineConstants::Processor<float>::StateID::A,
                                    nullptr);
        MainLayerProcessor<float>::createValueTree(rootAudioEngine, &undoManager);

        //apvtsMainLayer.state.addChild(rootMainLayer, 0, nullptr);
        apvtsMainLayer.state.addListener(this);
        rootAudioEngine.addListener(this);
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
    juce::ValueTree createPointNoListener(float pointX, float pointY, float tension, int curveID, bool horizontalDragOn);

    //Audio Engine function=================================================================================================
    void setSelectedState(int stateID);
    void resetAudioEngineParam();
    float getMasterGainValue();

    //Main Layer function===================================================================================================
    void resetMainLayerParam();

    //Stage function=========================================================================================================
    void resetStageParam(int stageID);
    void setSelectedStageID(int stageID);
    void setStageProcessorID(int stageID, int processorID);

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
    void addActiveFilterCount(int stageID);
    void remodeActiveFilterCount(int stageID);

    //Phaser==================================================================================================================
    void resetPhaserParam(int stageID);

    //Distortion==============================================================================================================
    void resetDistortionParam(int stageID);
    void resetDistortionDUParam(int stageID, int distortionUnitID);

    //PiecewiseFunction Function
    void switchFromUnipolarToBipolar(int stageID, int distortionUnitID);
    void switchFromBipolarToUnipolar(int stageID, int distortionUnitID);

    //Value Tree Add/Remove point from PiecewiseFunction
    void setMouseAddPointWS(juce::MouseEvent& e, float scopeWidth, float scopeHeight);
    void setMouseDragEventWS(juce::MouseEvent& e, float scopeWidth, float scopeHeight);
    void setMouseUpWS();
    void addPoint(int stageID, int distortionUnitID, float pointX, float pointY);
    void deleteSelectedPoint(int stageID, int distortionUnitID);
    void resetPiecewiseFunction(int stageID, int distortionUnitID);

    //Set function PiecewiseFunction
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
    //void setDistortionCircuitEquation(int stageID, int distortionUnitID, int equationType, int equationID);
    
    //Get Function=========================================================================================================
    juce::AudioProcessorValueTreeState& getAPVTSMainLayer() { return apvtsMainLayer; };
    juce::UndoManager& getUndoManager() { return undoManager; };

    juce::ValueTree getRoot() { return rootAudioEngine; };
    juce::ValueTree getMainLayerVT();
    juce::ValueTree getStageVT(int stageID);
    juce::ValueTree getSelectedStageVT();
    juce::ValueTree getScopeVT(int stageID);
    juce::ValueTree getEQVT(int stageID);
    juce::ValueTree getDistortionVT(int stageID);
    juce::ValueTree getDistoUnitVT(int stageID, int distoUnitID);
    juce::ValueTree getSelectedDistoUnitVT();
    juce::ValueTree getPiecewiseFunctionVT(int stageID, int distortionUnitID);
    juce::ValueTree getDistortionCircuitVT(int stageID, int distortionUnitID);
    juce::ValueTree getPiecewiseFunctionPointsVT(int stageID, int distortionUnitID);
    
    //Audio Engine
    int getSelectedStateID();

    //MainLayer
    
    //Stage
    int getSelectedStageID();
    int getStageProcessorID(int stageID);
    int getSelectedStageProcessorID();

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
    int getNbOfActiveFilters(int stageID);

    //Distortion
    bool getDistoUnitRouting(int stageID, int distortionUnitID);
    float getDistoUnitDrive(int stageID, int distortionUnitID);
    //Sample Remapper
    int getSelectedDistoUnitID();
    int getDistortionUnitID(int stageID);
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
    
    //int getDistortionCircuitEquationType(int stageID, int distortionUnitID);
    //int getDistortionCircuitEquationID(int stageID, int distortionUnitID);
    //Should update
    void setShouldUpdate(bool newValue) { shouldUpdate = newValue; };
    bool getShouldUpdate() { return shouldUpdate; };

    void trigListener() { getRoot().sendPropertyChangeMessage(StageConstants::ParamStringID::selectedStageID); };
private:
    juce::AudioProcessorValueTreeState::ParameterLayout createParametersLayout(int nbOfStages, 
                                                                                int nbOfFiltersPerEQ, 
                                                                                int nbOfDistoUnitPerDisto);

    juce::AudioProcessorValueTreeState apvtsMainLayer;
    juce::ValueTree rootAudioEngine;
    juce::UndoManager undoManager;
    
    std::atomic<bool> shouldUpdate;

    //CustomPhaser<float>::State phaserState;
};