/*
  ==============================================================================

    MainLayerProcessor.h
    Created: 19 Apr 2021 6:18:29pm
    Author:  CookiMonstor

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
//#include "../../DataStructure/MangledIdentifiers.h"
#include "../StageProcessor/StageProcessorBase.h"
//#include "../UnitProcessor/AllUnitProcessor.h"

//==============================================================================
template <typename SampleType>
class MainLayerProcessor
{
public:
    MainLayerProcessor(int initNbOfChannels)
        : nbOfChannels(initNbOfChannels)
    {
        for (int stageID = 0; stageID < MainLayerConstants::Processor<float>::nbOfStageMax; ++stageID)
        {
            stageProcessorVector.add(new StageProcessorBase<SampleType>(nbOfChannels));
        }

        stageProcessorVector[0]->setIsBypassed(false);
    }

    struct MainLayerProcessorParams
    {
        bool onOff;
    };

    void prepare(const juce::dsp::ProcessSpec& spec) //override
    {
        for (int stageID = 0; stageID < MainLayerConstants::Processor<float>::nbOfStageMax; ++stageID)
        {
            stageProcessorVector[stageID]->prepare(spec);
        }
    }

    template <typename ProcessContext>
    void process(const ProcessContext& context) noexcept
    {
        if (isBypassed || context.isBypassed)
        {
            return;
        }

        for (int stageID = 0; stageID < MainLayerConstants::Processor<float>::nbOfStageMax; ++stageID)
        {
            stageProcessorVector[stageID]->process(context);
        }

    }

    void processBuffer(juce::AudioBuffer<SampleType>& buffer, juce::MidiBuffer& midiBuffer) //override
    {
        if (isBypassed)
            return;

        for (int stageID = 0; stageID < MainLayerConstants::nbOfStageMax; ++stageID)
        {
            stageProcessorVector[stageID]->processBuffer(buffer, midiBuffer);
        }

    }

    void reset() //override
    {

        for (int stageID = 0; stageID < MainLayerConstants::nbOfStageMax; ++stageID)
        {
            stageProcessorVector[stageID]->reset();
        }

    }

    /** Sets the parameters.
    */
    void setIsBypassed(bool newIsBypassed)
    { 
        isBypassed = newIsBypassed; 
    };

    const juce::String getName() const { return "Tail Processor"; };

    StageProcessorBase<SampleType>* getStageProcessor(int stageID) { return (stageID >=  4) ? stageProcessorVector[3] : stageProcessorVector[stageID]; };

    static void createParametersLayout(std::vector<std::unique_ptr<juce::RangedAudioParameter>>* plugInParameters, int nbOfStages, int nbOfFiltersPerEQ, int nbOfDistoUnitPerDisto)
    {
        juce::String paramString = MainLayerConstants::ParamStringID::GetParamStringID::isBypassed();
        juce::String automationParamString = MainLayerConstants::AutomationString::GetString::isBypassed();

        plugInParameters->push_back(std::make_unique<juce::AudioParameterBool>(paramString,
            automationParamString,
            MainLayerConstants::Processor<float>::isBypassedStartValue));

        //Main Layer Stage Attachement========================================================================================================================================================
        //paramString = getPostProcessingOnOffParamString();
        //automationParamString = getPostProcessingOnOffParamAutomationString();
        //plugInParameters->push_back(std::make_unique<juce::AudioParameterBool>(paramString,
        //    automationParamString,
        //    true));

        //Main Layer All Stage All Parameters Layout==========================================================================================================================================
        for (int stageID = 0; stageID < nbOfStages; ++stageID)
        {
            StageProcessorBase<SampleType>::createParametersLayout(plugInParameters, stageID, nbOfFiltersPerEQ, nbOfDistoUnitPerDisto);
        }
    };

    static void createValueTree(juce::ValueTree& vt, juce::UndoManager* undoManager)
    {
        //vt.removeAllChildren(nullptr);
        juce::ValueTree vtMainLayer(MainLayerConstants::ParamStringID::mainLayer);
        //vt.setProperty("IsOpening", true, nullptr);
        vt.addChild(vtMainLayer, -1, nullptr);

        vtMainLayer.setProperty(juce::Identifier("Name"), MainLayerConstants::ParamStringID::mainLayer, nullptr);
        //vt.setProperty(AudioEngineConstants,0, nullptr);
        vtMainLayer.setProperty(StageConstants::ParamStringID::selectedStageID, 0, nullptr);

        for (int stage = 0; stage < 4; ++stage)
        {
            StageProcessorBase<SampleType>::createValueTree(vtMainLayer, stage, undoManager);
        }

    }

private:

    juce::OwnedArray<StageProcessorBase<SampleType>> stageProcessorVector;

    int nbOfChannels = 2;
    bool isBypassed = false;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainLayerProcessor)
};