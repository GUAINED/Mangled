/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "DataStructure/hostInformation.h"
#include "DataStructure/UnitProcessorAttachmentStructure.h"
#include "DataStructure/MainLayerDataStruct.h"
#include "DataStructure/MangledState.h"
#include "Processor/AudioEngine.h"
#include "UIComponent/ScopeUIComponent/ScopeDisplay.h"

//==============================================================================
/**
*/

class MangledAudioProcessor  : public juce::AudioProcessor
                             //, public juce::ValueTree::Listener
{
public:
    //==============================================================================
    MangledAudioProcessor();
    ~MangledAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    hostInformation& getHostInfo() { return hostInformation; };
    AudioEngine<float>* getAudioEngine() { return &audioEngine; };
    //MainLayerDataStruct& getMainLayerDataStruct() { return mainLayerDataStruct; };
    AudioEngineState<float>& getMainLayerDataStruct() { return mainLayerDataStruct; };
    std::atomic<double> bpm{ 120.0 };
    std::atomic<double> ppq{ 0.0 };
    std::atomic<juce::int64> ppqSample{ 0 };
    std::atomic<int> bufferSize{ 0 };

    //double getBPM() { return bpm.load(); };
    //double getPPQ() { return ppq.load(); };
    //juce::int64 getPPQSample() { return ppqSample.load(); };
    //int getBufferSize() { return bufferSize.load(); };
    void saveState(int stateID)
    {
        juce::ValueTree apvtsState = mainLayerDataStruct.getAPVTSMainLayer().copyState();
        state[stateID]->removeAllChildren(nullptr);
        state[stateID]->addChild(apvtsState, 0, nullptr);

        juce::ValueTree rootVT = juce::ValueTree(mainLayerDataStruct.getRoot().getType());
        rootVT.copyPropertiesAndChildrenFrom(mainLayerDataStruct.getRoot(), nullptr);
        state[stateID]->addChild(rootVT, 1, nullptr);
    }

    void loadState(int stateID)
    {
        if (state[stateID]->getNumChildren() != 0)
        {
            juce::ValueTree apvtsVT = state[stateID]->getChildWithName(mainLayerDataStruct.getAPVTSMainLayer().state.getType());
            mainLayerDataStruct.getAPVTSMainLayer().replaceState(apvtsVT);
            
            //mainLayerDataStruct.trigListener();
            
            juce::ValueTree rootVT = state[stateID]->getChildWithName(mainLayerDataStruct.getRoot().getType());

            mainLayerDataStruct.getRoot().copyPropertiesAndChildrenFrom(rootVT, nullptr);

            //setParams();
        }
    }

    void resetFifo()
    {
        for (int stageID = 0; stageID < MainLayerConstants::Processor<float>::nbOfStageMax; ++stageID)
        {
            audioEngine.getMainLayerProcessor()->getStageProcessor(stageID)->getEQProcessor()->resetFifo();
        }
    }
private:
    //int count = 0;
    //==============================================================================
    //MainLayerDataStruct mainLayerDataStruct;
    AudioEngineState<float> mainLayerDataStruct;
    hostInformation hostInformation;
    AudioEngine<float> audioEngine;

    juce::OwnedArray<juce::ValueTree, juce::CriticalSection> state;
    //juce::ValueTree stateB;

    //std::atomic<double> bpm{ 120.0 };
    //std::atomic<double> ppq{ 0.0 };
    //std::atomic<juce::int64> ppqSample{ 0 };
    //std::atomic<int> bufferSize{ 0 };

    void setParams();
    void setStageParams(MainLayerProcessor<float>* pMainLayerProcessor, int stageID);
    void setScopeParams(juce::AudioProcessorValueTreeState& apvts, int stageID);
    void setEQParams(juce::AudioProcessorValueTreeState& apvts, int stageID);
    void setPhaserParams(juce::AudioProcessorValueTreeState& apvts, int stageID);
    void setDistortionParams(juce::AudioProcessorValueTreeState& apvts, int stageID);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MangledAudioProcessor)
};
