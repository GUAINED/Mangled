/*
  ==============================================================================

    ValueTreeMainLayer.cpp
    Created: 23 Sep 2021 11:04:33am
    Author:  CookiMonstor

  ==============================================================================
*/

#include "MainLayerDataStruct.h"

//Value Tree Listener
void MainLayerDataStruct::valueTreePropertyChanged(juce::ValueTree& treeWhosePropertyHasChanged, const juce::Identifier& property)
{
    juce::ValueTree parent = treeWhosePropertyHasChanged.getParent();

    //if(property == juce::Identifier("IsOpening"))
    //    return;

    //if ((treeWhosePropertyHasChanged.getType() == juce::Identifier("PARAM"))
    //    || (treeWhosePropertyHasChanged.getType() == juce::Identifier("Ws"))
    //   )
    //if (parent.getType() == (juce::Identifier("APVTSMangled")))
    //{
    shouldUpdate = true;
    //}
    //else if (treeWhosePropertyHasChanged.getType() == (juce::Identifier("Ws")))
    //{
    //    juce::Identifier id("Point");
    //    float valueVTG;
    //    valueVTG = valueTreeMangled.getProperty(id);
    //}
    //else if (parent.getType() == (juce::Identifier("APVTSMangled2")))
        //{
        //    shouldUpdateStage2 = true;
        //}
}

void MainLayerDataStruct::valueTreeChildAdded(juce::ValueTree& parentTree, juce::ValueTree& childWhichHasBeenAdded)
{
    if (parentTree.getType() == juce::Identifier(SampleRemapperConstants::ParamStringID::srPoints))
    {
        //int stageID = getSelectedStageID();
        //int distortionUnitID = getSelectedDistoUnitID();
        //audioEngine.getMainLayerProcessor()->getStageProcessor(stageID)->getDistortionProcessor()->getDistortionUnitProcessor(distortionUnitID)->getSampleRemapper()->IsResizeNedded(parentTree.getNumChildren());
        if (parentTree.getParent().getProperty(juce::Identifier(SampleRemapperConstants::ParamStringID::isBipolar)))
        {
            shouldUpdate = true;
        }
        else
        {
            ///int stageID = mainLayerDataStruct.getSelectedStageID();
            //int distortionUnitID = mainLayerDataStruct.getSelectedDistoUnitID();
            //audioEngine.getMainLayerProcessor()->getStageProcessor(stageID)->getDistortionProcessor()->getDistortionUnitProcessor(distortionUnitID)->getSampleRemapper()->IsResizeNedded(parentTree.getNumChildren());
            if (parentTree.getNumChildren() % 2 == 0)
            {
                shouldUpdate = true;
                //int stageID = mainLayerDataStruct.getSelectedStageID();
                //int distortionUnitID = mainLayerDataStruct.getSelectedDistoUnitID();
                //audioEngine.getMainLayerProcessor()->getStageProcessor(stageID)->getDistortionProcessor()->getDistortionUnitProcessor(distortionUnitID)->getSampleRemapper()->IsResizeNedded(parentTree.getNumChildren());
            }
        }
    }
}

void MainLayerDataStruct::valueTreeChildRemoved(juce::ValueTree& parentTree, juce::ValueTree& childWhichHasBeenRemoved, int indexFromWhichChildWasRemoved)
{
    if (parentTree.getType() == juce::Identifier(SampleRemapperConstants::ParamStringID::srPoints))
    {
        //int stageID = getSelectedStageID();
        //int distortionUnitID = getSelectedDistoUnitID();
        int nbOfChild = parentTree.getNumChildren();
        if (nbOfChild == 0)
            return;
        //audioEngine.getMainLayerProcessor()->getStageProcessor(stageID)->getDistortionProcessor()->getDistortionUnitProcessor(distortionUnitID)->getSampleRemapper()->IsResizeNedded(parentTree.getNumChildren());
        if (parentTree.getParent().getProperty(juce::Identifier(SampleRemapperConstants::ParamStringID::isBipolar)))
        {
            shouldUpdate = true;
        }
        else
        {
            ///int stageID = mainLayerDataStruct.getSelectedStageID();
            //int distortionUnitID = mainLayerDataStruct.getSelectedDistoUnitID();
            //audioEngine.getMainLayerProcessor()->getStageProcessor(stageID)->getDistortionProcessor()->getDistortionUnitProcessor(distortionUnitID)->getSampleRemapper()->IsResizeNedded(parentTree.getNumChildren());
            if (nbOfChild % 2 == 0)
            {
                shouldUpdate = true;
                //int stageID = mainLayerDataStruct.getSelectedStageID();
                //int distortionUnitID = mainLayerDataStruct.getSelectedDistoUnitID();
                //audioEngine.getMainLayerProcessor()->getStageProcessor(stageID)->getDistortionProcessor()->getDistortionUnitProcessor(distortionUnitID)->getSampleRemapper()->IsResizeNedded(parentTree.getNumChildren());
            }
        }

    }
}

void MainLayerDataStruct::valueTreeChildOrderChanged(juce::ValueTree& parentTreeWhoseChildrenHaveMoved, int oldIndex, int newIndex)
{

}

void MainLayerDataStruct::valueTreeParentChanged(juce::ValueTree& treeWhoseParentHasChanged)
{

}

void MainLayerDataStruct::valueTreeRedirected(juce::ValueTree& treeWhichHasBeenChanged)
{

}

//Create Param
juce::AudioProcessorValueTreeState::ParameterLayout MainLayerDataStruct::createParametersLayout(int nbOfStages, int nbOfFiltersPerEQ, int nbOfDistoUnitPerDisto)
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> parameters;

    //Audio Engine Master Gain=============================================================================================================================================================
    AudioEngine<float>::createParametersLayout(&parameters, nbOfStages, nbOfFiltersPerEQ, nbOfDistoUnitPerDisto);

    return { parameters.begin(), parameters.end() };
}

//Create ValueTree============================================================================================================================
juce::ValueTree MainLayerDataStruct::createPointNoListener(float pointX, float pointY, float tension, int curveID, bool horizontalDragOn)
{
    juce::ValueTree vt(SampleRemapperConstants::ParamStringID::point);

    vt.setPropertyExcludingListener(this, SampleRemapperConstants::ParamStringID::pointX, pointX, &undoManager);
    vt.setPropertyExcludingListener(this, SampleRemapperConstants::ParamStringID::pointY, pointY, &undoManager);

    vt.setPropertyExcludingListener(this, SampleRemapperConstants::ParamStringID::tension, tension, &undoManager);
    vt.setPropertyExcludingListener(this, SampleRemapperConstants::ParamStringID::horizontalDragOn, horizontalDragOn, &undoManager);

    vt.setPropertyExcludingListener(this, SampleRemapperConstants::ParamStringID::curveType, curveID, &undoManager);

    return vt;
};

//AudioEngine=================================================================================================================================
void MainLayerDataStruct::setSelectedState(int stateID)
{
    rootAudioEngine.setPropertyExcludingListener(this, AudioEngineConstants::ParamStringID::selectedState, stateID, &undoManager);
}

void MainLayerDataStruct::resetAudioEngineParam()
{
    //undoManager.beginNewTransaction();

    juce::String paramString = AudioEngineConstants::ParamStringID::GetParamStringID::masterGain();
    juce::Value apvtsValue = apvtsMainLayer.getParameterAsValue(paramString);
    apvtsValue = AudioEngineConstants::Processor<float>::masterGainStartValue;

    resetMainLayerParam();
}

float MainLayerDataStruct::getMasterGainValue()
{
    juce::String paramString = AudioEngineConstants::ParamStringID::GetParamStringID::masterGain();
    float masterGain = *apvtsMainLayer.getRawParameterValue(paramString);

    return masterGain;
}

//MainLayer==================================================================================================================================
void MainLayerDataStruct::resetMainLayerParam()
{
    juce::String paramString = MainLayerConstants::ParamStringID::GetParamStringID::isBypassed();
    juce::Value apvtsValue = apvtsMainLayer.getParameterAsValue(paramString);
    apvtsValue = MainLayerConstants::Processor<float>::isBypassedStartValue;

    for (int stageID = 0; stageID < MainLayerConstants::Processor<float>::nbOfStageMax; ++stageID)
    {
        resetStageParam(stageID);
    }
}

void MainLayerDataStruct::resetStageParam(int stageID)
{
    juce::String paramString = StageConstants::AutomationString::GetString::isBypassed(stageID);
    juce::Value apvtsValue = apvtsMainLayer.getParameterAsValue(paramString);
    apvtsValue = StageConstants::Processor<float>::isBypassedStartValue;

    resetScopeParam(stageID);
    resetEQParam(stageID);
    resetPhaserParam(stageID);
    resetDistortionParam(stageID);
}

//Set Function===============================================================================================================================
void MainLayerDataStruct::setSelectedStageID(int stageID)
{
    getMainLayerVT().setProperty(StageConstants::ParamStringID::selectedStageID, stageID, &undoManager);
}

void MainLayerDataStruct::setIsEQ(int stageID, bool newIsEQ)
{
    juce::ValueTree vtStage = getStageVT(stageID);

    vtStage.setPropertyExcludingListener(this, StageConstants::ParamStringID::isEQ, newIsEQ, &undoManager);
}

//Scope======================================================================================================================================
void MainLayerDataStruct::resetScopeParam(int stageID)
{
    juce::String paramString = ScopeConstants::ParamStringID::GetParamStringID::dataType(stageID);
    juce::Value apvtsValue = apvtsMainLayer.getParameterAsValue(paramString);
    apvtsValue = ScopeConstants::Processor<float>::modeStartValue;
}

void MainLayerDataStruct::setScopeZoom(int stageID, int zoomID)
{
    getScopeVT(stageID).setPropertyExcludingListener(this, ScopeConstants::ParamStringID::zoom, zoomID, &undoManager);
}

void MainLayerDataStruct::setScopeIsBypassed(int stageID, bool isBypassed)
{
    //getScopeVT(stageID).setPropertyExcludingListener(this, ScopeConstants::ParamStringID::scope::preEQIsBypassed, isBypassed, nullptr);
    //getScopeVT(stageID).setPropertyExcludingListener(this, ScopeConstants::ParamStringID::scope::postEQIsBypassed, isBypassed, nullptr);
    //getScopeVT(stageID).setProperty(ScopeConstants::ParamStringID::scope::postDistoIsBypassed, isBypassed, nullptr);

}

void MainLayerDataStruct::setScopePreEQIsBypassed(int stageID, bool isBypassed)
{
    //getScopeVT(stageID).setProperty(ScopeConstants::ParamStringID::scope::preEQIsBypassed, isBypassed, &undoManager);
}

void MainLayerDataStruct::setScopePostEQIsBypassed(int stageID, bool isBypassed)
{
    //getScopeVT(stageID).setProperty(ScopeConstants::ParamStringID::scope::postEQIsBypassed, isBypassed, &undoManager);
}

void MainLayerDataStruct::setScopePostDistoIsBypassed(int stageID, bool isBypassed)
{
    //getScopeVT(stageID).setProperty(ScopeConstants::ParamStringID::scope::postDistoIsBypassed, isBypassed, &undoManager);
}

//Set Function EQ============================================================================================================================
void MainLayerDataStruct::resetEQParam(int stageID)
{
    //Distortion Mix
    juce::String paramString = EQConstants::ParamStringID::GetParamStringID::mix(stageID);
    juce::Value apvtsValue = apvtsMainLayer.getParameterAsValue(paramString);
    apvtsValue = EQConstants::Processor<float>::mixStartValue;

    //Distortion isBypassed
    paramString = EQConstants::ParamStringID::GetParamStringID::isBypassed(stageID);
    apvtsValue = apvtsMainLayer.getParameterAsValue(paramString);
    apvtsValue = EQConstants::Processor<float>::isBypassedStartValue;

    for (int filterID = 0; filterID < EQConstants::Processor<float>::nbOfFilterMax; ++filterID)
    {
        paramString = EQConstants::ParamStringID::GetParamStringID::filterCutoff(stageID, filterID);
        apvtsValue = apvtsMainLayer.getParameterAsValue(paramString);
        apvtsValue = EQConstants::BiquadConstants<float>::cutoffStartValue;

        paramString = EQConstants::ParamStringID::GetParamStringID::filterQ(stageID, filterID);
        apvtsValue = apvtsMainLayer.getParameterAsValue(paramString);
        apvtsValue = EQConstants::BiquadConstants<float>::qStartValue;

        paramString = EQConstants::ParamStringID::GetParamStringID::filterGain(stageID, filterID);
        apvtsValue = apvtsMainLayer.getParameterAsValue(paramString);
        apvtsValue = EQConstants::BiquadConstants<float>::gainLinStartValue;

        paramString = EQConstants::ParamStringID::GetParamStringID::filterType(stageID, filterID);
        apvtsValue = apvtsMainLayer.getParameterAsValue(paramString);
        apvtsValue = EQConstants::BiquadConstants<float>::typeStartValue;

        paramString = EQConstants::ParamStringID::GetParamStringID::filterOrder(stageID, filterID);
        apvtsValue = apvtsMainLayer.getParameterAsValue(paramString);
        apvtsValue = EQConstants::BiquadConstants<float>::orderStartValue;

        paramString = EQConstants::ParamStringID::GetParamStringID::filterIsBypassed(stageID, filterID);
        apvtsValue = apvtsMainLayer.getParameterAsValue(paramString);
        apvtsValue = EQConstants::BiquadConstants<float>::isBypassedStartValue;

        paramString = EQConstants::ParamStringID::GetParamStringID::filterIsActive(stageID, filterID);
        apvtsValue = apvtsMainLayer.getParameterAsValue(paramString);
        apvtsValue = EQConstants::BiquadConstants<float>::isActiveStartValue;
    }
}

void MainLayerDataStruct::setEQFilterCutOff(int stageID, int filterID, float newCutoff)
{
    juce::String paramString = EQConstants::ParamStringID::GetParamStringID::filterCutoff(stageID, filterID);
    juce::Value cutoffValue = apvtsMainLayer.getParameterAsValue(paramString);
    
    cutoffValue = newCutoff;
}

void MainLayerDataStruct::setEQFilterQ(int stageID, int filterID, float newQ)
{
    juce::String paramString = EQConstants::ParamStringID::GetParamStringID::filterQ(stageID, filterID);
    juce::Value qValue = apvtsMainLayer.getParameterAsValue(paramString);

    qValue = newQ;
}

void MainLayerDataStruct::setEQFilterGain(int stageID, int filterID, float newGain)
{
    juce::String paramString = EQConstants::ParamStringID::GetParamStringID::filterGain(stageID, filterID);
    juce::Value gainValue = apvtsMainLayer.getParameterAsValue(paramString);

    gainValue = newGain;
}

void MainLayerDataStruct::setEQFilterType(int stageID, int filterID, int filterType)
{
    juce::String paramString = EQConstants::ParamStringID::GetParamStringID::filterType(stageID, filterID);
    juce::Value filterTypeValue = apvtsMainLayer.getParameterAsValue(paramString);

    filterTypeValue = filterType;
}

void MainLayerDataStruct::setEQFilterIsBypassed(int stageID, int filterID, bool newIsBypassed)
{
    juce::String paramString = EQConstants::ParamStringID::GetParamStringID::filterIsBypassed(stageID, filterID);
    juce::Value isBypassedValue = apvtsMainLayer.getParameterAsValue(paramString);

    isBypassedValue = newIsBypassed;
}

void MainLayerDataStruct::setEQFilterIsActive(int stageID, int filterID, bool newIsActive)
{
    juce::String paramString = EQConstants::ParamStringID::GetParamStringID::filterIsActive(stageID, filterID);
    juce::Value isActiveValue = apvtsMainLayer.getParameterAsValue(paramString);

    isActiveValue = newIsActive;
}

void MainLayerDataStruct::setSelectedFilterID(int stageID, int filterID)
{
    undoManager.beginNewTransaction();
    getEQVT(stageID).setPropertyExcludingListener(this, EQConstants::ParamStringID::selectedFilterID, filterID, &undoManager);
}

void MainLayerDataStruct::setMouseAddFilterEQ(const juce::MouseEvent& e, int filterID, float scopeWidth, float scopeHeight)
{
    if (filterID == -1)
        return;

    int stageID = getSelectedStageID();

    int xPos = e.getPosition().getX();
    int yPos = e.getPosition().getY();

    // FQG VALUE
    float xPosRemap = juce::jmap((float)xPos, 0.0f, scopeWidth - 100.0f, 0.0f, 1.0f);
    float filterCutoff = juce::mapToLog10(xPosRemap, 10.0f, 22050.0f);

    float gainMin = 0.0f;
    float gainMax = 0.0f;

    switch (getScopeZoom(stageID))
    {
        case ScopeConstants::UI::ZoomID::thirtydB:
            gainMin = ScopeConstants::UI::thirtydB::Constants<float>::minGaindB;
            gainMax = ScopeConstants::UI::thirtydB::Constants<float>::maxGaindB;
            break;

        case ScopeConstants::UI::ZoomID::fifteendB:
            gainMin = ScopeConstants::UI::fifteendB::Constants<float>::minGaindB;
            gainMax = ScopeConstants::UI::fifteendB::Constants<float>::maxGaindB;
            break;

        case ScopeConstants::UI::ZoomID::ninedB:
            gainMin = ScopeConstants::UI::ninedB::Constants<float>::minGaindB;
            gainMax = ScopeConstants::UI::ninedB::Constants<float>::maxGaindB;
            break;

        case ScopeConstants::UI::ZoomID::sixdB:
            gainMin = ScopeConstants::UI::sixdB::Constants<float>::minGaindB;
            gainMax = ScopeConstants::UI::sixdB::Constants<float>::maxGaindB;
            break;

        case ScopeConstants::UI::ZoomID::threedB:
            gainMin = ScopeConstants::UI::threedB::Constants<float>::minGaindB;
            gainMax = ScopeConstants::UI::threedB::Constants<float>::maxGaindB;
            break;

        default:
            gainMin = ScopeConstants::UI::thirtydB::Constants<float>::minGaindB;
            gainMax = ScopeConstants::UI::thirtydB::Constants<float>::maxGaindB;
            break;
    }

    float filterGain = juce::jmap((float)yPos, 0.0f, scopeHeight, gainMax, gainMin);

    //undoManager.beginNewTransaction();
    setSelectedFilterID(stageID, filterID);
    setEQFilterCutOff(stageID, filterID, filterCutoff);
    setEQFilterGain(stageID, filterID, filterGain);
    setEQFilterIsBypassed(stageID, filterID, false);
    setEQFilterIsActive(stageID, filterID, false);
    addActiveFilterCount(stageID);
    //setEQFilterQ();
}

int MainLayerDataStruct::findNextAvailableFilterIDEQ(int stageID)
{
    int filterID = 0;
    juce::String paramString = EQConstants::ParamStringID::GetParamStringID::filterIsActive(stageID, filterID);
    bool isActive = !(* apvtsMainLayer.getRawParameterValue(paramString));

    while (isActive && (filterID < EQConstants::Processor<float>::nbOfFilterMax - 1))
    {
        filterID++;
        paramString = EQConstants::ParamStringID::GetParamStringID::filterIsActive(stageID, filterID);
        isActive = ! (*apvtsMainLayer.getRawParameterValue(paramString));
    }

    //if (filterID == EQConstants::Processor<float>::nbOfFilterMax)
    if (filterID == EQConstants::Processor<float>::nbOfFilterMax - 1 && isActive)
    {
        filterID = -1;
    }

    return filterID;
}

int MainLayerDataStruct::findNextActiveFilterDown(int stageID)
{
    if (getNbOfActiveFilters(stageID) == 0)
        return -1;

    int filterID = getSelectedFilterID(stageID) - 1;
    juce::String paramString;
    bool isActive = true;
    //int count = 0;
    if(filterID < 0)
        filterID = EQConstants::Processor<float>::nbOfFilterMax - 1;

    paramString = EQConstants::ParamStringID::GetParamStringID::filterIsActive(stageID, filterID);
    isActive = (*apvtsMainLayer.getRawParameterValue(paramString));

    while (isActive && (filterID > -1))// EQConstants::Processor<float>::nbOfFilterMax)
    {
        //count++;
        filterID--;
        filterID = (filterID + EQConstants::Processor<float>::nbOfFilterMax - 1) % (EQConstants::Processor<float>::nbOfFilterMax - 1);
        paramString = EQConstants::ParamStringID::GetParamStringID::filterIsActive(stageID, filterID);
        isActive = (*apvtsMainLayer.getRawParameterValue(paramString));
    }

    return filterID;
}

int MainLayerDataStruct::findNextActiveFilterUp(int stageID)
{
    if (getNbOfActiveFilters(stageID) == 0)
        return -1;

    int filterID = getSelectedFilterID(stageID) + 1;
    juce::String paramString;
    bool isActive = true;
    if (filterID > EQConstants::Processor<float>::nbOfFilterMax - 1)
        filterID = 0;

    paramString = EQConstants::ParamStringID::GetParamStringID::filterIsActive(stageID, filterID);
    isActive = (*apvtsMainLayer.getRawParameterValue(paramString));

    while (isActive && (filterID < EQConstants::Processor<float>::nbOfFilterMax ))
    {
        filterID++;
        filterID = filterID % (EQConstants::Processor<float>::nbOfFilterMax - 1);
        paramString = EQConstants::ParamStringID::GetParamStringID::filterIsActive(stageID, filterID);
        isActive = (*apvtsMainLayer.getRawParameterValue(paramString));
    }

    return filterID;
}

void MainLayerDataStruct::addActiveFilterCount(int stageID)
{
    int yolo = getNbOfActiveFilters(stageID);
    int nbOfActiveFilters = juce::jlimit(0, EQConstants::Processor<float>::nbOfFilterMax - 1, getNbOfActiveFilters(stageID) + 1);
    getEQVT(stageID).setPropertyExcludingListener(this, EQConstants::ParamStringID::nbOfActiveFilters, nbOfActiveFilters, &undoManager);
}

void MainLayerDataStruct::remodeActiveFilterCount(int stageID)
{
    int nbOfActiveFilters = juce::jlimit(0, EQConstants::Processor<float>::nbOfFilterMax - 1, getNbOfActiveFilters(stageID) - 1);
    getEQVT(stageID).setPropertyExcludingListener(this, EQConstants::ParamStringID::nbOfActiveFilters, nbOfActiveFilters, &undoManager);
}

void MainLayerDataStruct::setMouseDragEventEQ(const juce::MouseEvent& e, float scopeWidth, float scopeHeight)
{
    int stageID = getSelectedStageID();
    int filterID = getSelectedFilterID(stageID);

    if (filterID == -1)
        return;

    float xPos = static_cast<float>(e.getPosition().getX());
    float yPos = static_cast<float>(e.getPosition().getY());
    yPos = juce::jlimit(0.0f + scopeHeight / 14.0f, scopeHeight - scopeHeight / 14.0f, yPos);
    float filterCutoff = Conversion<float>::fromMouseToCutoff(xPos, 0.0f, scopeWidth - 100.0f, 10.0f, 22050.0f);
    setEQFilterCutOff(stageID, filterID, filterCutoff);

    int filterType = getEQFilterType(stageID, filterID);

    if (filterType == EQConstants::BiquadConstants<float>::Types::peak || filterType == EQConstants::BiquadConstants<float>::Types::lowshelf || filterType == EQConstants::BiquadConstants<float>::Types::highshelf)
    {
        
        float gainMin = 0.0f;
        float gainMax = 0.0f;

        switch (getScopeZoom(stageID))
        {
        case ScopeConstants::UI::ZoomID::thirtydB:
            gainMin = ScopeConstants::UI::thirtydB::Constants<float>::minGaindB;
            gainMax = ScopeConstants::UI::thirtydB::Constants<float>::maxGaindB;
            break;

        case ScopeConstants::UI::ZoomID::fifteendB:
            gainMin = ScopeConstants::UI::fifteendB::Constants<float>::minGaindB;
            gainMax = ScopeConstants::UI::fifteendB::Constants<float>::maxGaindB;
            break;

        case ScopeConstants::UI::ZoomID::ninedB:
            gainMin = ScopeConstants::UI::ninedB::Constants<float>::minGaindB;
            gainMax = ScopeConstants::UI::ninedB::Constants<float>::maxGaindB;
            break;

        case ScopeConstants::UI::ZoomID::sixdB:
            gainMin = ScopeConstants::UI::sixdB::Constants<float>::minGaindB;
            gainMax = ScopeConstants::UI::sixdB::Constants<float>::maxGaindB;
            break;

        case ScopeConstants::UI::ZoomID::threedB:
            gainMin = ScopeConstants::UI::threedB::Constants<float>::minGaindB;
            gainMax = ScopeConstants::UI::threedB::Constants<float>::maxGaindB;
            break;

        default:
            gainMin = ScopeConstants::UI::thirtydB::Constants<float>::minGaindB;
            gainMax = ScopeConstants::UI::thirtydB::Constants<float>::maxGaindB;
            break;
        }

        float filterGain = Conversion<float>::fromMouseToGain(yPos, scopeHeight, 0.0f, gainMin, gainMax);
        setEQFilterGain(stageID, filterID, filterGain);
    }
    else
    {
        float qMin = 0.0f;
        float qMax = 0.0f;

        switch (getScopeZoom(stageID))
        {
        case ScopeConstants::UI::ZoomID::thirtydB:
            qMin = ScopeConstants::UI::thirtydB::Constants<float>::qMin;
            qMax = ScopeConstants::UI::thirtydB::Constants<float>::qMax;
            break;

        case ScopeConstants::UI::ZoomID::fifteendB:
            qMin = ScopeConstants::UI::fifteendB::Constants<float>::qMin;
            qMax = ScopeConstants::UI::fifteendB::Constants<float>::qMax;
            break;

        case ScopeConstants::UI::ZoomID::ninedB:
            qMin = ScopeConstants::UI::ninedB::Constants<float>::qMin;
            qMax = ScopeConstants::UI::ninedB::Constants<float>::qMax;
            break;

        case ScopeConstants::UI::ZoomID::sixdB:
            qMin = ScopeConstants::UI::sixdB::Constants<float>::qMin;
            qMax = ScopeConstants::UI::sixdB::Constants<float>::qMax;
            break;

        case ScopeConstants::UI::ZoomID::threedB:
            qMin = ScopeConstants::UI::threedB::Constants<float>::qMin;
            qMax = ScopeConstants::UI::threedB::Constants<float>::qMax;
            break;

        default:
            qMin = ScopeConstants::UI::thirtydB::Constants<float>::qMin;
            qMax = ScopeConstants::UI::thirtydB::Constants<float>::qMax;
            break;
        }

        float filterQ = Conversion<float>::fromMouseToQ(yPos, scopeHeight - scopeHeight / 14.0f, 0.0f + scopeHeight / 14.0f, qMin, qMax);
        setEQFilterQ(stageID, filterID, filterQ);
    }
}

void MainLayerDataStruct::setMouseWheelEventEQ(const juce::MouseEvent& event, const juce::MouseWheelDetails& wheel, int filterID)
{
    int stageID = getSelectedStageID();
    int filterType = getEQFilterType(stageID, filterID);

    if (filterType == EQConstants::BiquadConstants<float>::Types::peak || filterType == EQConstants::BiquadConstants<float>::Types::lowshelf || filterType == EQConstants::BiquadConstants<float>::Types::highshelf)
    {
        float rot = wheel.deltaY;

        juce::String paramString = EQConstants::ParamStringID::GetParamStringID::filterQ(stageID, filterID);
        float sliderValue = *apvtsMainLayer.getRawParameterValue(paramString);
        sliderValue += sliderValue * rot;
        setEQFilterQ(stageID, filterID, sliderValue);
    }
}

//Phaser=====================================================================================================================================
void MainLayerDataStruct::resetPhaserParam(int stageID)
{
    //Phaser Centre Frequency
    juce::String paramString = PhaserConstants::ParamStringID::GetParamStringID::centerFrequency(stageID);
    juce::Value apvtsValue = apvtsMainLayer.getParameterAsValue(paramString);
    apvtsValue = PhaserConstants::Processor<float>::cfreqStartValue;

    //Phaser Feedback
    paramString = PhaserConstants::ParamStringID::GetParamStringID::feedback(stageID);
    apvtsValue = apvtsMainLayer.getParameterAsValue(paramString);
    apvtsValue = PhaserConstants::Processor<float>::feedbackStartValue;

    //Phaser Rate
    paramString = PhaserConstants::ParamStringID::GetParamStringID::rate(stageID);
    apvtsValue = apvtsMainLayer.getParameterAsValue(paramString);
    apvtsValue = PhaserConstants::Processor<float>::rateStartValue;

    //Phaser Depth
    paramString = PhaserConstants::ParamStringID::GetParamStringID::depth(stageID);
    apvtsValue = apvtsMainLayer.getParameterAsValue(paramString);
    apvtsValue = PhaserConstants::Processor<float>::depthStartValue;

    //Phaser Nb Of Stages
    paramString = PhaserConstants::ParamStringID::GetParamStringID::nbOfStages(stageID);
    apvtsValue = apvtsMainLayer.getParameterAsValue(paramString);
    apvtsValue = PhaserConstants::Processor<float>::nbOfStgStartValue;

    //Phaser Mix
    paramString = PhaserConstants::ParamStringID::GetParamStringID::mix(stageID);
    apvtsValue = apvtsMainLayer.getParameterAsValue(paramString);
    apvtsValue = PhaserConstants::Processor<float>::mixStartValue;

    //Phaser isBypassed
    paramString = PhaserConstants::ParamStringID::GetParamStringID::isBypassed(stageID);
    apvtsValue = apvtsMainLayer.getParameterAsValue(paramString);
    apvtsValue = PhaserConstants::Processor<float>::isBypassedStartValue;
}

//Set function SampleRemapper====================================================================================================================
void MainLayerDataStruct::resetDistortionParam(int stageID)
{
    //Distortion Mix
    juce::String paramString = DistortionConstants::ParamStringID::GetParamStringID::mix(stageID);
    juce::Value apvtsValue = apvtsMainLayer.getParameterAsValue(paramString);
    apvtsValue = DistortionConstants::Processor<float>::mixStartValue;

    //Distortion isBypassed
    paramString = DistortionConstants::ParamStringID::GetParamStringID::isBypassed(stageID);
    apvtsValue = apvtsMainLayer.getParameterAsValue(paramString);
    apvtsValue = DistortionConstants::Processor<float>::isBypassedStartValue;

    for (int distortionUnitID = 0; distortionUnitID < 2; ++distortionUnitID)
    {
        resetDistortionDUParam(stageID, distortionUnitID);
    }
}

void MainLayerDataStruct::resetDistortionDUParam(int stageID, int distortionUnitID)
{
    bool duOnOffStartValue = true;

    //DU OnOff
    juce::String paramString = DistoUnitConstants::ParamStringID::GetParamStringID::isBypassed(stageID, distortionUnitID);
    juce::Value apvtsValue = apvtsMainLayer.getParameterAsValue(paramString);
    apvtsValue = duOnOffStartValue;

    getDistortionCircuitVT(stageID, distortionUnitID).setPropertyExcludingListener(this, DistortionCircuitConstants::ParamsID::equationType, 0, &undoManager);
    
    getDistortionCircuitVT(stageID, distortionUnitID).setPropertyExcludingListener(this, DistortionCircuitConstants::ParamsID::equationID, 0, &undoManager);

    //DU Routing
    paramString = DistoUnitConstants::ParamStringID::GetParamStringID::routing(stageID, distortionUnitID);
    apvtsValue = apvtsMainLayer.getParameterAsValue(paramString);
    apvtsValue = DistoUnitConstants::Processor<float>::routingStartValue;

    //DU PreGain
    paramString = DistoUnitConstants::ParamStringID::GetParamStringID::preGain(stageID, distortionUnitID);
    apvtsValue = apvtsMainLayer.getParameterAsValue(paramString);
    apvtsValue = DistoUnitConstants::Processor<float>::preGaindBStartValue;

    //DU Drive
    paramString = DistoUnitConstants::ParamStringID::GetParamStringID::drive(stageID, distortionUnitID);
    apvtsValue = apvtsMainLayer.getParameterAsValue(paramString);
    apvtsValue = DistoUnitConstants::Processor<float>::driveStartValue;

    //DU PostGain
    paramString = DistoUnitConstants::ParamStringID::GetParamStringID::postGain(stageID, distortionUnitID);
    apvtsValue = apvtsMainLayer.getParameterAsValue(paramString);
    apvtsValue = DistoUnitConstants::Processor<float>::postGaindBStartValue;

    //DU Warp
    paramString = DistoUnitConstants::ParamStringID::GetParamStringID::warp(stageID, distortionUnitID);
    apvtsValue = apvtsMainLayer.getParameterAsValue(paramString);
    apvtsValue = DistoUnitConstants::Processor<float>::warpStartValue;

    //Is Bipolar. Has to be done that way. OtherWise the undoManager Will begin a new transaction.
    getSampleRemapperVT(stageID, distortionUnitID).setProperty(SampleRemapperConstants::ParamStringID::isBipolar, false, &undoManager);

    //DU DCFilterOn
    paramString = DistoUnitConstants::ParamStringID::GetParamStringID::dcFilterIsBypassed(stageID, distortionUnitID);
    apvtsValue = apvtsMainLayer.getParameterAsValue(paramString);
    apvtsValue = DistoUnitConstants::Processor<float>::dcFilterIsBypassedStartValue;

    //DU Mix
    paramString = DistoUnitConstants::ParamStringID::GetParamStringID::mix(stageID, distortionUnitID);
    apvtsValue = apvtsMainLayer.getParameterAsValue(paramString);
    apvtsValue = DistoUnitConstants::Processor<float>::mixStartValue;

    resetSampleRemapper(stageID, distortionUnitID);
}

//SampleRemapper Function
void MainLayerDataStruct::switchFromUnipolarToBipolar(int stageID, int distortionUnitID)
{
    juce::ValueTree vtWS = getSampleRemapperPointsVT(stageID, distortionUnitID);
    int nbOfPoints = getNbOfPoints(stageID, distortionUnitID);

    int pointID = nbOfPoints - 1;
    float pointX = getPointX(stageID, distortionUnitID, pointID);
    float pointY = getPointY(stageID, distortionUnitID, pointID);

    //undoManager.beginNewTransaction();

    while (!(pointX == 0.0f) && !(pointY == 0.0f) && (pointID > 0))//(pointID < nbOfPoints)
    {
        pointID--;
        pointX = getPointX(stageID, distortionUnitID, pointID);
        pointY = getPointY(stageID, distortionUnitID, pointID);
    }

    pointID--;
    pointX = getPointX(stageID, distortionUnitID, pointID);
    pointY = getPointY(stageID, distortionUnitID, pointID);

    while ((pointX == 0.0f) && (pointY == 0.0f) && (pointID > 0))
    {
        vtWS.removeChild(pointID, &undoManager);
        pointID--;
        pointX = getPointX(stageID, distortionUnitID, pointID);
        pointY = getPointX(stageID, distortionUnitID, pointID);
    }
    //zeroCrossingPointID;

    nbOfPoints = getNbOfPoints(stageID, distortionUnitID);

    if (nbOfPoints > 2)
    {
        for (int point = 1; point < nbOfPoints - 1; ++point)
        {
            setHorizontalDragOn(stageID, distortionUnitID, point, true);
        }
    }

    pointID = getSelectedPointID(stageID, distortionUnitID);

    //if (curveID == nbOfPoints / 2 - 1)
    {
        setSelectedIDCurve(stageID, distortionUnitID, pointID);
    }

    vtWS.sendPropertyChangeMessage(SampleRemapperConstants::ParamStringID::tension);
}

void MainLayerDataStruct::switchFromBipolarToUnipolar(int stageID, int distortionUnitID)
{
    juce::ValueTree vtWSPoints = getSampleRemapperPointsVT(stageID, distortionUnitID);
    int nbOfPoints = getNbOfPoints(stageID, distortionUnitID);
    int zeroCrossingPointID = 0;
    float zeroCPointX = getPointX(stageID, distortionUnitID, zeroCrossingPointID);

    //undoManager.beginNewTransaction();

    while ((zeroCPointX > 0.0f) && (zeroCrossingPointID < nbOfPoints))
    {
        zeroCrossingPointID++;
        zeroCPointX = getPointX(stageID, distortionUnitID, zeroCrossingPointID);
    }

    //zeroCrossingPointID;

    //zeroCPointX = getPointX(stageID, distortionUnitID, zeroCrossingPointID);
    float leftX = getPointX(stageID, distortionUnitID, zeroCrossingPointID);
    float leftY = getPointY(stageID, distortionUnitID, zeroCrossingPointID);
    float rightX = getPointX(stageID, distortionUnitID, zeroCrossingPointID - 1);
    float rightY = getPointY(stageID, distortionUnitID, zeroCrossingPointID - 1);

    for (int pointID = zeroCrossingPointID; pointID < nbOfPoints; ++pointID)
    {
        int nbOfChild = vtWSPoints.getNumChildren();
        vtWSPoints.removeChild(nbOfChild - 1, &undoManager);
    }

    //float pointY = 0.0f;
    int lastID = nbOfPoints - 1;
    float pointX = 0.0f;
    float pointY = juce::jmap(pointX, leftX, rightX, 0.0f, 1.0f);
    float tension = getTension(stageID, distortionUnitID, zeroCrossingPointID - 1);
    //pointY = std::pow(pointY, tension);
    pointY = juce::jmap(pointY, 0.0f, 1.0f, leftY, rightY);

    //tension = getTension(stageID, distortionUnitID, lastID);
    int curveID = getPointCurveType(stageID, distortionUnitID, zeroCrossingPointID - 1);
    curveID == 0 ? tension = -tension : tension = tension;
    vtWSPoints.addChild(createPointNoListener(pointX, pointY, 0.0f, 0, false), -1, &undoManager);
    vtWSPoints.addChild(createPointNoListener(pointX, -pointY, tension, curveID, false), -1, &undoManager);

    nbOfPoints = getNbOfPoints(stageID, distortionUnitID);
    lastID = nbOfPoints - 1 - 2; //We skip the 2 new zerocrossing points

    for (int pointID = lastID; pointID > 0; --pointID)
    {
        pointX = -getPointX(stageID, distortionUnitID, pointID);
        pointY = -getPointY(stageID, distortionUnitID, pointID);
        tension = getTension(stageID, distortionUnitID, pointID - 1);
        curveID = getPointCurveType(stageID, distortionUnitID, pointID - 1);

        curveID == 0 ? tension = -tension : tension = tension;

        vtWSPoints.addChild(createPointNoListener(pointX, pointY, tension, curveID,true), -1, &undoManager);
    }

    pointX = -getPointX(stageID, distortionUnitID, 0);
    pointY = -getPointY(stageID, distortionUnitID, 0);
    tension = 0.0f;
    vtWSPoints.addChild(createPointNoListener(pointX, pointY, tension, 0, true), -1, &undoManager);

    vtWSPoints.sendPropertyChangeMessage(SampleRemapperConstants::ParamStringID::tension);
}

//Value Tree Add/Remove point from SampleRemapper================================================================================================
void MainLayerDataStruct::setMouseAddPointWS(juce::MouseEvent& e, float scopeWidth, float scopeHeight)
{
    int stageID = getSelectedStageID();
    int distortionUnitID = getSelectedDistoUnitID();
    int nbOfPoints = getNbOfPoints(stageID, distortionUnitID);
    //int halfNbOfPoints = nbOfPoints / 2;

    if (nbOfPoints == DistortionConstants::WaveShaper<float>::nbOfPointMax)
        return;

    float scopeMinLin = DistortionConstants::UI::scopeMinLin_f;
    float scopeMaxLin = DistortionConstants::UI::scopeMaxLin_f;
    juce::Point<float> point = e.getPosition().toFloat();
    float pointX = juce::jmap(point.getX(), 0.0f, scopeWidth, scopeMinLin, scopeMaxLin);
    float pointY = juce::jmap(point.getY(), scopeHeight, 0.0f, scopeMinLin, scopeMaxLin);
    //float tension = 1.0f;

    addPoint(stageID, distortionUnitID, pointX, pointY);
}

void MainLayerDataStruct::setMouseDragEventWS(juce::MouseEvent& e, float scopeWidth, float scopeHeight)
{
    int stageID = getSelectedStageID();
    int distortionUnitID = getSelectedDistoUnitID();
    int pointID = getSelectedPointID(stageID, distortionUnitID);
    int tensionID = getSelectedTensionID(stageID, distortionUnitID);
    float pointX = 0.0f;
    float pointY = 0.0f;

    float scopeMin =  DistortionConstants::UI::scopeMinLin_f;
    float scopeMax = DistortionConstants::UI::scopeMaxLin_f;

    //undoManager.beginNewTransaction();

    if (pointID > -1)
    {
        pointX = juce::jmap((float)e.getPosition().getX(), 0.0f, scopeWidth, scopeMin, scopeMax);
        pointY = juce::jmap((float)e.getPosition().getY(), scopeHeight, 0.0f, scopeMin, scopeMax);
        juce::Point<float> newPoint(pointX, pointY);

        setSelectedPoint(stageID, distortionUnitID, newPoint);
    }
    else if (tensionID > -1)
    {
        float rightY = getPointY(stageID, distortionUnitID, tensionID);
        float leftY = getPointY(stageID, distortionUnitID, tensionID + 1);
        rightY = juce::jmap(rightY, scopeMin, scopeMax, scopeHeight, 0.0f);
        leftY = juce::jmap(leftY, scopeMin, scopeMax, scopeHeight, 0.0f);

        if (leftY != rightY)
        {
            float maxY = juce::jmax(rightY, leftY);
            float minY = juce::jmin(rightY, leftY);
            float mousePos = (float)e.getPosition().getY();
            mousePos = juce::jlimit(minY, maxY, mousePos);
            float tension = juce::jmap(mousePos, minY, maxY, -0.5f, 0.5f);

            setSelectedTension(stageID, distortionUnitID, tension);
            
        }
    }

    //getWaveShaperPointsVT(stageID, distortionUnitID).getChild(tensionID).sendPropertyChangeMessage(SampleRemapperConstants::ParamStringID::tension);


}

void MainLayerDataStruct::setMouseUpWS()
{
}

void MainLayerDataStruct::addPoint(int stageID, int distortionUnitID, float pointX, float pointY)
{
    juce::ValueTree vtWS = getSampleRemapperPointsVT(stageID, distortionUnitID);

    int nbOfChild = vtWS.getNumChildren();
    if (nbOfChild == DistortionConstants::WaveShaper<float>::nbOfPointMax)
        return;

    int childID = 0;
    int curveType = 0;
    float wsPointX = getPoint(stageID, distortionUnitID, childID).getX();

    while (!(wsPointX < pointX) && (childID < nbOfChild))
    {
        ++childID;
        wsPointX = getPoint(stageID, distortionUnitID, childID).getX();

    }

    undoManager.beginNewTransaction();

    float tensionChild = getTension(stageID, distortionUnitID, childID - 1);

    vtWS.addChild(createPointNoListener(pointX, pointY, tensionChild, 0,true), childID, &undoManager);

    setPointAndTensionIDNoTransaction(stageID, distortionUnitID, childID, -1);

    if (!getIsBipolar(stageID, distortionUnitID))
    {
        nbOfChild = getNbOfPoints(stageID, distortionUnitID);
        int mirrorID = nbOfChild - childID;
        curveType = getPointCurveType(stageID, distortionUnitID, childID - 1);
        tensionChild = getTension(stageID, distortionUnitID, childID - 1);

        curveType == 0 ? tensionChild = -tensionChild : tensionChild = tensionChild;

        vtWS.addChild(createPointNoListener(-pointX, -pointY, tensionChild, curveType,true), mirrorID, &undoManager);

        curveType = getPointCurveType(stageID, distortionUnitID, childID);
        //tensionChild = 0.0f;
        setPointCurveTypeNoListener(stageID, distortionUnitID, mirrorID - 1, 0);
        setTensionNoListener(stageID, distortionUnitID, mirrorID - 1, 0.0f);
    }

    setSelectedIDCurve(stageID, distortionUnitID, childID);
    getSampleRemapperPointsVT(stageID, distortionUnitID).getChild(childID).sendPropertyChangeMessage(SampleRemapperConstants::ParamStringID::selectedCurveID);
}

void MainLayerDataStruct::deleteSelectedPoint(int stageID, int distortionUnitID)
{
    int selectedID = getSelectedPointID(stageID, distortionUnitID);
    //int nbOfPoints = getNbOfPoints(stageID, distortionUnitID);
    juce::ValueTree vtWSPoints = getSampleRemapperPointsVT(stageID, distortionUnitID);
    int nbOfChildren = getSampleRemapperPointsVT(stageID, distortionUnitID).getNumChildren() - 1;
    bool horzDragOn = getHorizontalDragOn(stageID, distortionUnitID, selectedID);

    //if (selectedID == -1 || selectedID == 0 || selectedID == (nbOfPoints - 1))
    //    return;

    //HorizontalDragOn = true mean the point is either at -1 or +1 or at 0 on unipolar mode.
    if (selectedID == -1 || !horzDragOn)
        return;

    undoManager.beginNewTransaction();

    vtWSPoints.removeChild(selectedID, &undoManager);

    setSelectedIDPoint(stageID, distortionUnitID, selectedID -1);
    setSelectedIDTension(stageID, distortionUnitID, -1);
    setSelectedIDCurve(stageID, distortionUnitID, selectedID-1);

    bool isBipolar = getIsBipolar(stageID, distortionUnitID);

    if (!isBipolar)
    {
        int mirrorID = nbOfChildren - selectedID - 1;

        vtWSPoints.removeChild(mirrorID, &undoManager);

        int curveType = getPointCurveType(stageID, distortionUnitID, selectedID - 1);
        float tension = getTension(stageID, distortionUnitID, selectedID - 1);

        curveType == 0 ? tension = -tension : tension = tension;

        setPointCurveTypeNoListener(stageID, distortionUnitID, mirrorID - 1, curveType);
        setTensionNoListener(stageID, distortionUnitID, mirrorID - 1, tension);
    }
    //undoManager.beginNewTransaction

    getSampleRemapperPointsVT(stageID, distortionUnitID).getChild(selectedID - 1).sendPropertyChangeMessage(SampleRemapperConstants::ParamStringID::tension);
}

void MainLayerDataStruct::resetSampleRemapper(int stageID, int distortionUnitID)
{
    juce::ValueTree vtWS = getSampleRemapperVT(stageID, distortionUnitID);

    undoManager.beginNewTransaction();

    vtWS.removeAllChildren(&undoManager);

    setSelectedIDPoint(stageID, distortionUnitID, -1);
    setSelectedIDTension(stageID, distortionUnitID, -1);

    bool isBipolar = getIsBipolar(stageID, distortionUnitID);

    if (isBipolar)
    {
        SampleRemapper<float>::createValueTreePointBipolar(vtWS, &undoManager);
    }
    else
    {
        SampleRemapper<float>::createValueTreePointUnipolar(vtWS, &undoManager);
    }

}

void MainLayerDataStruct::setDistoUnitID(int stageID, int distortionUnitID)
{
    getDistortionVT(stageID).setProperty(DistoUnitConstants::ParamStringID::selectedDistoUnitID, distortionUnitID, nullptr);
}

void MainLayerDataStruct::setSelectedDistoUnitID(int distortionUnitID)
{
    int stageID = getSelectedStageID();
    getDistortionVT(stageID).setProperty(DistoUnitConstants::ParamStringID::selectedDistoUnitID, distortionUnitID, nullptr);
}

void MainLayerDataStruct::setPoint(int stageID, int distortionUnitID, int pointID, juce::Point<float> newPoint)
{
    //int everyPointButFirst = 0;
    juce::ValueTree vtWSPoint = getSampleRemapperPointsVT(stageID, distortionUnitID);
    int nbOfChildren = vtWSPoint.getNumChildren() - 1;

    float pointX = newPoint.getX();
    float pointY = newPoint.getY();

    //undoManager.beginNewTransaction();

    //setPointYNoListener(stageID, distortionUnitID, pointID, pointY);
    
    bool canDragHorizontal = getHorizontalDragOn(stageID, distortionUnitID, pointID);
    //We do not set X Value for the first and last Point. They are fixed at -1.0 and +1.0
    if (canDragHorizontal) 
    {
        setPointX(stageID, distortionUnitID, pointID, pointX);
    }

    bool isBipolar = getIsBipolar(stageID, distortionUnitID);

    if (!isBipolar)
    {
        int mirrorID = nbOfChildren - pointID;

        setPointY(stageID, distortionUnitID, mirrorID, -pointY);

        if(canDragHorizontal)
            setPointX(stageID, distortionUnitID, mirrorID, -pointX);
    }

    setPointY(stageID, distortionUnitID, pointID, pointY);
    //vtWSPoint.sendPropertyChangeMessage(SampleRemapperConstants::ParamStringID::pointY); NoListener
}

void MainLayerDataStruct::setPointX(int stageID, int distortionUnitID, int pointID, float newPointX)
{
    newPointX = juce::jlimit(getPoint(stageID, distortionUnitID, pointID + 1).getX(),
                             getPoint(stageID, distortionUnitID, pointID - 1).getX(),
                             newPointX
                            );

    getSampleRemapperPointsVT(stageID, distortionUnitID).getChild(pointID).setProperty(SampleRemapperConstants::ParamStringID::pointX, newPointX, &undoManager);
}

void MainLayerDataStruct::setPointY(int stageID, int distortionUnitID, int pointID, float newPointY)
{
    newPointY = juce::jlimit(DistortionConstants::WaveShaper<float>::pointYMinLin,
                             DistortionConstants::WaveShaper<float>::pointYMaxLin,
                             newPointY
                            );

    getSampleRemapperPointsVT(stageID, distortionUnitID).getChild(pointID).setProperty(SampleRemapperConstants::ParamStringID::pointY, newPointY, &undoManager);
}

void MainLayerDataStruct::setPointXNoListener(int stageID, int distortionUnitID, int pointID, float newPointX)
{
    newPointX = juce::jlimit(getPoint(stageID, distortionUnitID, pointID + 1).getX(),
        getPoint(stageID, distortionUnitID, pointID - 1).getX(),
        newPointX
    );

    getSampleRemapperPointsVT(stageID, distortionUnitID).getChild(pointID).setPropertyExcludingListener(this, SampleRemapperConstants::ParamStringID::pointX, newPointX, &undoManager);
}

void MainLayerDataStruct::setPointYNoListener(int stageID, int distortionUnitID, int pointID, float newPointY)
{
    newPointY = juce::jlimit(DistortionConstants::WaveShaper<float>::pointYMinLin,
        DistortionConstants::WaveShaper<float>::pointYMaxLin,
        newPointY
    );

    getSampleRemapperPointsVT(stageID, distortionUnitID).getChild(pointID).setPropertyExcludingListener(this, SampleRemapperConstants::ParamStringID::pointY, newPointY, &undoManager);
}

void MainLayerDataStruct::setSelectedPoint(int stageID, int distortionUnitID, juce::Point<float> newPoint)
{
    int selectedID = getSelectedPointID(stageID, distortionUnitID);
    setPoint(stageID, distortionUnitID, selectedID, newPoint);
}

void MainLayerDataStruct::setTension(int stageID, int distortionUnitID, int pointID, float newTension)
{
    getSampleRemapperPointsVT(stageID, distortionUnitID).getChild(pointID).setPropertyExcludingListener(this, SampleRemapperConstants::ParamStringID::tension, newTension, &undoManager);

    bool isBipolar = getIsBipolar(stageID, distortionUnitID);

    if (!isBipolar)
    {
        int nbOfChildren = getSampleRemapperPointsVT(stageID, distortionUnitID).getNumChildren() - 2;
        int mirrorID = nbOfChildren - pointID;
        int curveType = getPointCurveType(stageID, distortionUnitID, pointID);
        float tension = 1.0f;

        if (curveType == DistortionConstants::WaveShaper<float>::CurveTypes::polynomial
            || curveType == DistortionConstants::WaveShaper<float>::CurveTypes::asinh)
        {
            tension = -newTension;
        }
        else
        {
            tension = newTension;
        }

        getSampleRemapperPointsVT(stageID, distortionUnitID).getChild(mirrorID).setPropertyExcludingListener(this, SampleRemapperConstants::ParamStringID::tension, tension, &undoManager);
    }

    getSampleRemapperPointsVT(stageID, distortionUnitID).getChild(pointID).sendPropertyChangeMessage(SampleRemapperConstants::ParamStringID::tension);
}

void MainLayerDataStruct::setTensionNoListener(int stageID, int distortionUnitID, int pointID, float newTension)
{
    getSampleRemapperPointsVT(stageID, distortionUnitID).getChild(pointID).setPropertyExcludingListener(this, SampleRemapperConstants::ParamStringID::tension, newTension, &undoManager);

    bool isBipolar = getIsBipolar(stageID, distortionUnitID);

    if (!isBipolar)
    {
        int nbOfChildren = getSampleRemapperPointsVT(stageID, distortionUnitID).getNumChildren() - 2;
        int mirrorID = nbOfChildren - pointID;
        int curveType = getPointCurveType(stageID, distortionUnitID, pointID);
        float tension = 1.0f;

        if (curveType == 0)
        {
            tension = -newTension;
        }
        else
        {
            tension = newTension;
        }

        getSampleRemapperPointsVT(stageID, distortionUnitID).getChild(mirrorID).setPropertyExcludingListener(this, SampleRemapperConstants::ParamStringID::tension, tension, &undoManager);
    }
}

void MainLayerDataStruct::setSelectedTension(int stageID, int distortionUnitID, float newTension)
{
    int selectedID = getSelectedTensionID(stageID, distortionUnitID);
    setTension(stageID, distortionUnitID, selectedID, newTension);
}

void MainLayerDataStruct::setHorizontalDragOn(int stageID, int distortionUnitID, int pointID, bool horizontalDragOn)
{
    juce::ValueTree vtWSPoints = getSampleRemapperPointsVT(stageID, distortionUnitID).getChild(pointID);

    vtWSPoints.setPropertyExcludingListener(this, SampleRemapperConstants::ParamStringID::horizontalDragOn, horizontalDragOn, nullptr);
}

void MainLayerDataStruct::setPointCurveType(int stageID, int distortionUnitID, int pointID, int curveType)
{
    if (pointID >= 0)
    {
        juce::ValueTree vtWSPoints = getSampleRemapperPointsVT(stageID, distortionUnitID).getChild(pointID);

        vtWSPoints.setPropertyExcludingListener(this, SampleRemapperConstants::ParamStringID::curveType, curveType, &undoManager);

        if (!getIsBipolar(stageID, distortionUnitID))
        {
            int nbOfChildren = getSampleRemapperPointsVT(stageID, distortionUnitID).getNumChildren() - 2;
            int mirrorID = nbOfChildren - pointID;
            vtWSPoints = getSampleRemapperPointsVT(stageID, distortionUnitID).getChild(mirrorID);
            vtWSPoints.setPropertyExcludingListener(this, SampleRemapperConstants::ParamStringID::curveType, curveType, &undoManager);

            float tension = getTension(stageID, distortionUnitID, pointID);
            setTensionNoListener(stageID, distortionUnitID, mirrorID, tension);
        }

        if (curveType == DistortionConstants::WaveShaper<float>::CurveTypes::polynomial)
        {
            float tension = getTension(stageID, distortionUnitID, pointID);
            setTensionNoListener(stageID, distortionUnitID, pointID, -tension);
        }
        getSampleRemapperPointsVT(stageID, distortionUnitID).getChild(pointID).sendPropertyChangeMessage(SampleRemapperConstants::ParamStringID::curveType);
        undoManager.beginNewTransaction();
    }
}

void MainLayerDataStruct::setPointCurveTypeNoListener(int stageID, int distortionUnitID, int pointID, int curveType)
{
    if (pointID >= 0)
    {
        juce::ValueTree vtWSPoints = getSampleRemapperPointsVT(stageID, distortionUnitID).getChild(pointID);

        vtWSPoints.setPropertyExcludingListener(this, SampleRemapperConstants::ParamStringID::curveType, curveType, &undoManager);

        //float tension = getTension(stageID, distortionUnitID, pointID);

        if (!getIsBipolar(stageID, distortionUnitID))
        {
            int nbOfChildren = getSampleRemapperPointsVT(stageID, distortionUnitID).getNumChildren() - 2;
            int mirrorID = nbOfChildren - pointID;
            vtWSPoints = getSampleRemapperPointsVT(stageID, distortionUnitID).getChild(mirrorID);
            vtWSPoints.setPropertyExcludingListener(this, SampleRemapperConstants::ParamStringID::curveType, curveType, &undoManager);
        }
    }
}

void MainLayerDataStruct::setIsBipolar(int stageID, int distortionUnitID, bool isBipolar)
{
    undoManager.beginNewTransaction();

    getSampleRemapperVT(stageID, distortionUnitID).setPropertyExcludingListener(this, SampleRemapperConstants::ParamStringID::isBipolar, isBipolar, &undoManager);

    isBipolar ? switchFromUnipolarToBipolar(stageID, distortionUnitID) : switchFromBipolarToUnipolar(stageID, distortionUnitID);

}

void MainLayerDataStruct::setSelectedIDPoint(int stageID, int distortionUnitID, int pointID)
{
    bool isBipolar = getIsBipolar(stageID, distortionUnitID);
    int nbOfPoint = getNbOfPoints(stageID, distortionUnitID);

    if (isBipolar)
    {
        getSampleRemapperVT(stageID, distortionUnitID).setPropertyExcludingListener(this, SampleRemapperConstants::ParamStringID::selectedPointID, pointID, &undoManager);
        //saveSelectedPoint();
    }
    else
    {
        if ((pointID < nbOfPoint / 2))
        {
            getSampleRemapperVT(stageID, distortionUnitID).setPropertyExcludingListener(this, SampleRemapperConstants::ParamStringID::selectedPointID, pointID, &undoManager);
            //saveSelectedPoint();
        }
    }
}

void MainLayerDataStruct::setSelectedIDTension(int stageID, int distortionUnitID, int tensionID)
{
    getSampleRemapperVT(stageID, distortionUnitID).setPropertyExcludingListener(this, SampleRemapperConstants::ParamStringID::selectedTensionID, tensionID, &undoManager);
}

void MainLayerDataStruct::setSelectedIDCurve(int stageID, int distortionUnitID, int curveID)
{
    int nbOfPoints = getNbOfPoints(stageID, distortionUnitID);

    getIsBipolar(stageID, distortionUnitID) ? nbOfPoints = nbOfPoints - 1 : nbOfPoints = nbOfPoints / 2 - 1;

    if (curveID == nbOfPoints)
    {
        curveID--;
    }

    if (curveID < 0)
        curveID = 0;

    getSampleRemapperVT(stageID, distortionUnitID).setPropertyExcludingListener(this, SampleRemapperConstants::ParamStringID::selectedCurveID, curveID, &undoManager);
}

void MainLayerDataStruct::setPointAndTensionID(int stageID, int distortionUnitID, int pointID, int tensionID)
{
    undoManager.beginNewTransaction();

    setSelectedIDPoint(stageID, distortionUnitID, pointID);
    setSelectedIDTension(stageID, distortionUnitID, tensionID);

    if (pointID != -1)
    {
        setSelectedIDCurve(stageID, distortionUnitID, pointID);
    }
    else if (tensionID != -1)
    {
        setSelectedIDCurve(stageID, distortionUnitID, tensionID);
    }

    //getWaveShaperVT(stageID, distortionUnitID).sendPropertyChangeMessage(SampleRemapperConstants::ParamStringID::selectedCurveID);
}

void MainLayerDataStruct::setPointAndTensionIDNoTransaction(int stageID, int distortionUnitID, int pointID, int tensionID)
{
    setSelectedIDPoint(stageID, distortionUnitID, pointID);
    setSelectedIDTension(stageID, distortionUnitID, tensionID);

    if (pointID != -1)
    {
        setSelectedIDCurve(stageID, distortionUnitID, pointID);
    }
    else if (tensionID != -1)
    {
        setSelectedIDCurve(stageID, distortionUnitID, tensionID);
    }
}

//void MainLayerDataStruct::setDistortionCircuitEquation(int stageID, int distortionUnitID, int equationType, int equationID)
//{
//    //undoManager.beginNewTransaction();
//    juce::ValueTree vt = getDistortionCircuitVT(stageID, distortionUnitID);
//    //vt.setPropertyExcludingListener(this, DistortionCircuitConstants::ParamsID::equationType, equationType, &undoManager);
//    vt.setProperty(DistortionCircuitConstants::ParamsID::equationType, equationType, &undoManager);
//
//    vt.setProperty(DistortionCircuitConstants::ParamsID::equationID, equationID, &undoManager);
//    
//}

//Get function==============================================================================================================================
juce::ValueTree MainLayerDataStruct::getMainLayerVT()
{
    return rootAudioEngine.getChildWithName(juce::Identifier(MainLayerConstants::ParamStringID::mainLayer));
}

juce::ValueTree MainLayerDataStruct::getStageVT(int stageID)
{
    juce::Identifier name(MainLayerConstants::ParamStringID::stage + (juce::String)stageID);

    return getMainLayerVT().getChildWithName(name);
}

juce::ValueTree MainLayerDataStruct::getSelectedStageVT()
{
    int selectedStageID = getSelectedStageID();
    
    return getStageVT(selectedStageID);
}

juce::ValueTree MainLayerDataStruct::getScopeVT(int stageID)
{
    return getStageVT(stageID).getChildWithName(ScopeConstants::ParamStringID::scope);
}

juce::ValueTree MainLayerDataStruct::getEQVT(int stageID)
{
    return getStageVT(stageID).getChildWithName(EQConstants::ParamStringID::eq);
}

juce::ValueTree MainLayerDataStruct::getDistortionVT(int stageID)
{
    return getStageVT(stageID).getChildWithName(DistortionConstants::ParamStringID::distortion);
}

juce::ValueTree MainLayerDataStruct::getDistoUnitVT(int stageID, int distortionUnitID)
{
    juce::Identifier name(DistoUnitConstants::ParamStringID::distoUnit + (juce::String)distortionUnitID);

    return getDistortionVT(stageID).getChildWithName(name);
}

juce::ValueTree MainLayerDataStruct::getSelectedDistoUnitVT()
{
    int selectedStageID = getSelectedStageID();
    juce::ValueTree distoVT = getDistortionVT(selectedStageID);
    int selectedDistoUnit = getSelectedDistoUnitID();

    return distoVT.getChild(selectedDistoUnit);
}

juce::ValueTree MainLayerDataStruct::getSampleRemapperVT(int stageID, int distortionUnitID)
{
    return getDistoUnitVT(stageID, distortionUnitID).getChildWithName(SampleRemapperConstants::ParamStringID::sampleRemapper);
}

juce::ValueTree MainLayerDataStruct::getDistortionCircuitVT(int stageID, int distortionUnitID)
{
    return getDistoUnitVT(stageID, distortionUnitID).getChildWithName(DistortionCircuitConstants::ParamsID::distortionCircuit);
}

juce::ValueTree MainLayerDataStruct::getSampleRemapperPointsVT(int stageID, int distortionUnitID)
{
    return getSampleRemapperVT(stageID, distortionUnitID).getChildWithName(SampleRemapperConstants::ParamStringID::srPoints);
}

//Get Function Audio Engine
int MainLayerDataStruct::getSelectedState()
{
    return rootAudioEngine.getProperty(AudioEngineConstants::ParamStringID::selectedState);
}

//Get function Main Layer

//Get Function Stage
int MainLayerDataStruct::getSelectedStageID()
{
    return getMainLayerVT().getProperty(StageConstants::ParamStringID::selectedStageID);
}

//Get Function Scope
int MainLayerDataStruct::getScopeZoom(int stageID)
{
    juce::ValueTree vtScope = getScopeVT(stageID);

    return vtScope.getProperty(ScopeConstants::ParamStringID::zoom);
}

//Get Function EQ
int MainLayerDataStruct::getSelectedFilterID(int stageID)
{
    juce::ValueTree vtEQ = getEQVT(stageID);
    
    return vtEQ.getProperty(EQConstants::ParamStringID::selectedFilterID);
}

int MainLayerDataStruct::getEQFilterType(int stageID, int filterID)
{
    juce::String paramString = EQConstants::ParamStringID::GetParamStringID::filterType(stageID, filterID);
    int filterType = (int) *apvtsMainLayer.getRawParameterValue(paramString);

    return filterType;
}

float MainLayerDataStruct::getEQFilterCutoff(int stageID, int filterID)
{
    juce::String paramString = EQConstants::ParamStringID::GetParamStringID::filterCutoff(stageID, filterID);
    float filterCutoff = *apvtsMainLayer.getRawParameterValue(paramString);

    return filterCutoff;
}

float MainLayerDataStruct::getEQFilterQ(int stageID, int filterID)
{
    juce::String paramString = EQConstants::ParamStringID::GetParamStringID::filterQ(stageID, filterID);
    float filterQ = *apvtsMainLayer.getRawParameterValue(paramString);

    return filterQ;
}

float MainLayerDataStruct::getEQFilterGain(int stageID, int filterID)
{
    juce::String paramString = EQConstants::ParamStringID::GetParamStringID::filterGain(stageID, filterID);
    float filterGain = *apvtsMainLayer.getRawParameterValue(paramString);

    return filterGain;
}

bool MainLayerDataStruct::getEQFilterIsBypassed(int stageID, int filterID)
{
    juce::String paramString = EQConstants::ParamStringID::GetParamStringID::filterIsBypassed(stageID, filterID);
    bool filterIsBypassed = *apvtsMainLayer.getRawParameterValue(paramString);

    return filterIsBypassed;
}

bool MainLayerDataStruct::getEQFilterIsActive(int stageID, int filterID)
{
    juce::String paramString = EQConstants::ParamStringID::GetParamStringID::filterIsActive(stageID, filterID);
    bool filterIsActive = ! (*apvtsMainLayer.getRawParameterValue(paramString));

    return filterIsActive;
}

int MainLayerDataStruct::getNbOfActiveFilters(int stageID)
{
    int nbOfActiveFilter = getEQVT(stageID).getProperty(EQConstants::ParamStringID::nbOfActiveFilters);
    return nbOfActiveFilter;
}

//Get Function Distortion
bool MainLayerDataStruct::getDistoUnitRouting(int stageID, int distortionUnitID)
{
    juce::String paramString = DistoUnitConstants::ParamStringID::GetParamStringID::routing(stageID, distortionUnitID);
    bool distoRouting = *apvtsMainLayer.getRawParameterValue(paramString);

    return distoRouting;
}

float MainLayerDataStruct::getDistoUnitDrive(int stageID, int distortionUnitID)
{
    juce::String paramString = DistoUnitConstants::ParamStringID::GetParamStringID::drive(stageID, distortionUnitID);
    float distoEquationType = *apvtsMainLayer.getRawParameterValue(paramString);

    return distoEquationType;
}

int MainLayerDataStruct::getSelectedDistoUnitID()
{
    int selectedStageID = getSelectedStageID();
    juce::ValueTree distoVT = getDistortionVT(selectedStageID);

    return distoVT.getProperty(DistoUnitConstants::ParamStringID::selectedDistoUnitID);
}

int MainLayerDataStruct::getDistortionUnitID(int stageID)
{
    juce::ValueTree vtDistortion = getDistortionVT(stageID);

    return vtDistortion.getProperty(DistoUnitConstants::ParamStringID::selectedDistoUnitID);
}

bool MainLayerDataStruct::getIsEQ(int stageID)
{
    return getStageVT(stageID).getProperty(StageConstants::ParamStringID::isEQ);
}

bool MainLayerDataStruct::getSelectedIsEQ()
{
    return getSelectedStageVT().getProperty(StageConstants::ParamStringID::isEQ);
}

int MainLayerDataStruct::getNbOfPoints(int stageID, int distortionUnitID)
{
    return getSampleRemapperPointsVT(stageID, distortionUnitID).getNumChildren();
}

float MainLayerDataStruct::getPointX(int stageID, int distortionUnitID, int pointID)
{
    return getSampleRemapperPointsVT(stageID, distortionUnitID).getChild(pointID).getProperty("PointX");
}

float MainLayerDataStruct::getPointY(int stageID, int distortionUnitID, int pointID)
{
    return getSampleRemapperPointsVT(stageID, distortionUnitID).getChild(pointID).getProperty("PointY");
}

juce::Point<float> MainLayerDataStruct::getPoint(int stageID, int distortionUnitID, int pointID)
{
    float pointX = getPointX(stageID, distortionUnitID, pointID);
    float pointY = getPointY(stageID, distortionUnitID, pointID);

    juce::Point<float> point(pointX, pointY);

    return point;
}

juce::Point<float> MainLayerDataStruct::getSelectedPoint(int stageID, int distortionUnitID)
{
    int selectedID = getSelectedPointID(stageID, distortionUnitID);
    return getPoint(stageID, distortionUnitID, selectedID);
}

float MainLayerDataStruct::getTension(int stageID, int distortionUnitID, int pointID)
{
    return getSampleRemapperPointsVT(stageID, distortionUnitID).getChild(pointID).getProperty(SampleRemapperConstants::ParamStringID::tension);
}

int MainLayerDataStruct::getSelectedPointID(int stageID, int distortionUnitID)
{
    return getSampleRemapperVT(stageID, distortionUnitID).getProperty(SampleRemapperConstants::ParamStringID::selectedPointID);
}

int MainLayerDataStruct::getSelectedTensionID(int stageID, int distortionUnitID)
{

    return getSampleRemapperVT(stageID, distortionUnitID).getProperty(SampleRemapperConstants::ParamStringID::selectedTensionID);
}

int MainLayerDataStruct::getSelectedCurveID(int stageID, int distortionUnitID)
{
    return getSampleRemapperVT(stageID, distortionUnitID).getProperty(SampleRemapperConstants::ParamStringID::selectedCurveID);
}

bool MainLayerDataStruct::getHorizontalDragOn(int stageID, int distortionUnitID, int pointID)
{
    return getSampleRemapperPointsVT(stageID, distortionUnitID).getChild(pointID).getProperty(SampleRemapperConstants::ParamStringID::horizontalDragOn);
}

int MainLayerDataStruct::getPointCurveType(int stageID, int distortionUnitID, int pointID)
{
    return getSampleRemapperPointsVT(stageID, distortionUnitID).getChild(pointID).getProperty(SampleRemapperConstants::ParamStringID::curveType);
}

bool MainLayerDataStruct::getIsBipolar(int stageID, int distortionUnitID)
{
    return getSampleRemapperVT(stageID, distortionUnitID).getProperty(SampleRemapperConstants::ParamStringID::isBipolar);
}

int MainLayerDataStruct::getDistortionCircuitEquationType(int stageID, int distortionUnitID)
{
    return getDistortionCircuitVT(stageID, distortionUnitID).getProperty(DistortionCircuitConstants::ParamsID::equationType);
}

int MainLayerDataStruct::getDistortionCircuitEquationID(int stageID, int distortionUnitID)
{
    return getDistortionCircuitVT(stageID, distortionUnitID).getProperty(DistortionCircuitConstants::ParamsID::equationID);
}