/*
  ==============================================================================

    DistortionProcessor.h
    Created: 19 Apr 2021 4:09:59pm
    Author:  CookiMonstor

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
//#include "ProcessorBase.h"
#include "DistortionUnitProcessor.h"
//#include "DistortionEquation.h"

template <typename SampleType>
class DistortionProcessor
{
public:
    //==============================================================================
    DistortionProcessor(int initNbOfChannels = 2)
        :nbOfChannels(initNbOfChannels)
    {
        processorChain.resize(DistortionConstants::Processor<SampleType>::nbOfDUMax);
        dryWet.setMixingRule(juce::dsp::DryWetMixingRule::linear);

    }

    //==============================================================================
    struct Parameters
    {
        SampleType mix;
        bool isBypassed;
        int id = 0;

        inline Parameters& operator=(const Parameters& other)
        {
            // Guard self assignment
            if (this == &other)
                return *this;

            this->mix = other.mix;
            this->isBypassed = other.isBypassed;
            this->id = other.id;

            return *this;
        };

        inline bool operator==(const Parameters& rhs)
        {
            return ((mix == rhs.mix) &&
                (isBypassed == rhs.isBypassed) &&
                (id == rhs.id)
                );
        };

        inline bool operator!=(const Parameters& rhs)
        {
            return !(*this == rhs);
        };
    };

    //==============================================================================
    void prepare(const juce::dsp::ProcessSpec& spec)
    {
        //juce::ignoreUnused (spec);
        //processorChain.prepare(spec);

        for (int i = 0; i < processorChain.size(); ++i)
            processorChain[i].prepare(spec);

        dryWet.prepare(spec);

        reset();
    }

    //==============================================================================
    template <typename ProcessContext>
    void process(const ProcessContext& context) noexcept
    {
        const auto& inputBlock = context.getInputBlock();
        auto& outputBlock = context.getOutputBlock();
        const auto numChannels = outputBlock.getNumChannels();
        const auto numSamples = outputBlock.getNumSamples();
        
        jassert(nbOfChannels == inputBlock.getNumChannels());
        jassert(numChannels == inputBlock.getNumChannels());
        jassert(inputBlock.getNumSamples() == numSamples);

        //Bypass WaveShaper
        if (params.isBypassed || context.isBypassed)
        {
            outputBlock.copyFrom(inputBlock);
            return;
        }

        //Push the dry sample, where no processing is applied.
        dryWet.pushDrySamples(inputBlock);

        //WaveShaper Processing (function)
        for (int distortionUnitID = 0; distortionUnitID < processorChain.size(); ++distortionUnitID)
        {
            processorChain[distortionUnitID].process(context);
        }

        //Mix
        dryWet.mixWetSamples(outputBlock);
    }

    void processBuffer(juce::AudioBuffer<SampleType>& buffer)
    {
        //const auto& inputBlock = context.getInputBlock();
        //auto& outputBlock = context.getOutputBlock();
        //const auto numChannels = outputBlock.getNumChannels();
        //const auto numSamples = outputBlock.getNumSamples();

        //jassert(inputBlock.getNumChannels() == numChannels);
        //jassert(inputBlock.getNumSamples() == numSamples);

        //Bypass WaveShaper
        if (isBypassed)// || context.isBypassed)
        {
            //outputBlock.copyFrom(inputBlock);
            return;
        }

        //Push the dry sample, where no processing is applied.
        //dryWet.pushDrySamples(inputBlock);

        //WaveShaper Processing (function)
        for (int i = 0; i < processorChain.size(); ++i)
            processorChain[i].processBuffer(buffer);

        //Mix
        //dryWet.mixWetSamples(outputBlock);
    }

    //==============================================================================
    void reset() noexcept
    {
        for (int distortionUnitID = 0; distortionUnitID < processorChain.size(); ++distortionUnitID)
        {
            processorChain[distortionUnitID].reset();
        }

        dryWet.reset();
    }

    /** Sets the amount of dry and wet signal in the output of the WaveShaper (between 0
    for full dry and 1 for full wet).
    */
    void setMixParam(SampleType newMix)
    {
        dryWet.setWetMixProportion(newMix / static_cast<SampleType>(100.0));
    }

    /** Sets the Bypass of the waveShaper.
    */
    void setIsBypassed(const bool newIsBypassed)
    {
        params.isBypassed = newIsBypassed;
    }

    void setParams(Parameters& newParams)
    {
        if (params != newParams)
        {
            params = newParams;
            setMixParam(newParams.mix);
            setIsBypassed(newParams.isBypassed);
        }
    }

    DistortionUnitProcessor<SampleType>* getDistortionUnitProcessor(int distortionUnitProcessorID) { return &processorChain[distortionUnitProcessorID]; };

    static void createParametersLayout(std::vector<std::unique_ptr<juce::RangedAudioParameter>>* plugInParameters, int stageID, int nbOfDistoUnitPerDisto)
    {
        juce::NormalisableRange<SampleType> normalisableMixRange{ DistortionConstants::Processor<SampleType>::mixMin,
                                                            DistortionConstants::Processor<SampleType>::mixMax,
                                                            DistortionConstants::Processor<SampleType>::mixIncrement };

        //OverSampling Distortion
        juce::String paramString = DistortionConstants::ParamStringID::GetParamStringID::overSampling(stageID);
        juce::String automationParamString = DistortionConstants::AutomationString::GetString::overSampling(stageID);
        plugInParameters->push_back(std::make_unique<juce::AudioParameterBool>(paramString,
            automationParamString,
            DistortionConstants::Processor<SampleType>::oversamplingStartValue));

        //Mix Distortion
        paramString = DistortionConstants::ParamStringID::GetParamStringID::mix(stageID);
        automationParamString = DistortionConstants::AutomationString::GetString::mix(stageID);
        plugInParameters->push_back(std::make_unique<juce::AudioParameterFloat>(paramString,
            automationParamString,
            normalisableMixRange,
            DistortionConstants::Processor<SampleType>::mixStartValue));

        //OnOff Distortion
        paramString = DistortionConstants::ParamStringID::GetParamStringID::isBypassed(stageID);
        automationParamString = DistortionConstants::AutomationString::GetString::isBypassed(stageID);
        plugInParameters->push_back(std::make_unique<juce::AudioParameterBool>(paramString,
            automationParamString,
            DistortionConstants::Processor<SampleType>::isBypassedStartValue));

        for (int distortionUnitID = 0; distortionUnitID < nbOfDistoUnitPerDisto; ++distortionUnitID)
        {
            DistortionUnitProcessor<SampleType>::createParametersLayout(plugInParameters, stageID, distortionUnitID);
        }
    };

    static void createValueTree(juce::ValueTree& vt, juce::UndoManager* undoManager)
    {
        //juce::Identifer id();
        juce::ValueTree vtDistortion(DistortionConstants::ParamStringID::distortion);
        vt.addChild(vtDistortion, -1, nullptr);

        vt.setProperty(DistoUnitConstants::ParamStringID::selectedDistoUnitID, 0, nullptr);

        for (int distortionUnit = 0; distortionUnit < 2; ++distortionUnit)
            DistortionUnitProcessor<SampleType>::createValueTree(vtDistortion, distortionUnit, undoManager);
    };

    void setParams(juce::AudioProcessorValueTreeState& apvts, int stageID)
    {
        //juce::AudioProcessorValueTreeState& apvts = mainLayerDataStruct.getAPVTSMainLayer();

        DistortionProcessor<float>::Parameters newDistortionProcessorParams;
        //DistortionUnitProcessor<float>::Parameters newDistortionUnitProcessorParams;

        DistortionProcessor<float>* pDistortionProcessor = audioEngine.getMainLayerProcessor()->getStageProcessor(stageID)->getDistortionProcessor();

        //Over Sampling
        juce::String paramString = DistortionConstants::ParamStringID::GetParamStringID::overSampling(stageID);
        //auto& newOverSampling = *apvts.getRawParameterValue(paramString);
        
        //Mix Distortion
        paramString = DistortionConstants::ParamStringID::GetParamStringID::mix(stageID);
        //auto& newMixDistortionProcessor = *apvts.getRawParameterValue(paramString);
        setMixParam(*apvts.getRawParameterValue(paramString));
        //Bypass Distortion
        paramString = DistortionConstants::ParamStringID::GetParamStringID::isBypassed(stageID);
        //auto& newIsBypassedDistortionProcessor = *apvts.getRawParameterValue(paramString);
        setIsBypassed(*apvts.getRawParameterValue(paramString));
        //newDistortionProcessorParams.mix = newMixDistortionProcessor;
        //newDistortionProcessorParams.isBypassed = newIsBypassedDistortionProcessor;
        //pDistortionProcessor->setParams(newDistortionProcessorParams);

        //for (int distortionUnitID = 0; distortionUnitID < DistortionConstants::Processor<float>::nbOfDUMax; ++distortionUnitID)
        //{
        //    DistortionUnitProcessor<float>* pDistortionDU = pDistortionProcessor->getDistortionUnitProcessor(distortionUnitID);

        //    //Distortion First Unit OnOff
        //    paramString = DistoUnitConstants::ParamStringID::GetParamStringID::isBypassed(stageID, distortionUnitID);
        //    newDistortionUnitProcessorParams.isBypassed = *apvts.getRawParameterValue(paramString);

        //    //paramString = (juce::String)("Sigmoid") + (juce::String)(stageID)+juce::String(distortionUnitID);
        //    //auto& sigmoidEQA = *apvts.getRawParameterValue(paramString);
        //    //
        //    //if (sigmoidEQA == 0)
        //    //{
        //    //    int yolo = 1;
        //    //}
        //    //
        //    //paramString = (juce::String)("Symetric") + (juce::String)(stageID)+juce::String(distortionUnitID);
        //    //auto& symetricEQA = *apvts.getRawParameterValue(paramString);

        //    //if (symetricEQA == 1)
        //    //{
        //    //    int yolo = 2;
        //    //}
        //    //Distortion Routing
        //    paramString = DistoUnitConstants::ParamStringID::GetParamStringID::routing(stageID, distortionUnitID);
        //    newDistortionUnitProcessorParams.routing = *apvts.getRawParameterValue(paramString);

        //    //paramString = getDistortionDUEquationParamString(stageID, distortionUnitID);
        //    newDistortionUnitProcessorParams.circuit.equationType = mainLayerDataStruct.getDistortionCircuitEquationType(stageID, distortionUnitID);

        //    //paramString = getDistortionDUEquationParamString(stageID, distortionUnitID);
        //    newDistortionUnitProcessorParams.circuit.equationID = mainLayerDataStruct.getDistortionCircuitEquationID(stageID, distortionUnitID);

        //    //DC Filter On
        //    paramString = DistoUnitConstants::ParamStringID::GetParamStringID::dcFilterIsBypassed(stageID, distortionUnitID);
        //    newDistortionUnitProcessorParams.dcFilterIsBypassed = *apvts.getRawParameterValue(paramString);

        //    //PreGain WaveShaper
        //    paramString = DistoUnitConstants::ParamStringID::GetParamStringID::preGain(stageID, distortionUnitID);
        //    newDistortionUnitProcessorParams.preGain = *apvts.getRawParameterValue(paramString);

        //    //Drive WaveShaper
        //    paramString = DistoUnitConstants::ParamStringID::GetParamStringID::drive(stageID, distortionUnitID);
        //    newDistortionUnitProcessorParams.circuit.drive = *apvts.getRawParameterValue(paramString);

        //    //Warp WaveShaper
        //    paramString = DistoUnitConstants::ParamStringID::GetParamStringID::warp(stageID, distortionUnitID);
        //    newDistortionUnitProcessorParams.circuit.warp = *apvts.getRawParameterValue(paramString);

        //    //PostGain WaveShaper
        //    paramString = DistoUnitConstants::ParamStringID::GetParamStringID::postGain(stageID, distortionUnitID);
        //    newDistortionUnitProcessorParams.postGain = *apvts.getRawParameterValue(paramString);

        //    //Mix WaveShaper
        //    paramString = DistoUnitConstants::ParamStringID::GetParamStringID::mix(stageID, distortionUnitID);
        //    newDistortionUnitProcessorParams.mix = *apvts.getRawParameterValue(paramString);

        //    //OverSampling
        //    newDistortionUnitProcessorParams.oversampling = newOverSampling;

        //    //DistortionUnit ID
        //    newDistortionUnitProcessorParams.distortionUnitID = distortionUnitID;

        //    //if (&newDistortionUnitProcessorParams != &(stageProcessor->getDistortionProcessor()->getDistortionUnitParams(distortionUnit)))
        //    {
        //        pDistortionDU->setDistortionUnitParams(newDistortionUnitProcessorParams);
        //    }

        //    SampleRemapper<float>* pSampleRemapper = pDistortionDU->getSampleRemapper();

        //    int nbOfPoints = mainLayerDataStruct.getNbOfPoints(stageID, distortionUnitID);
        //    pSampleRemapper->setNbOfActiveBins(nbOfPoints - 1 + 2);

        //    juce::Point<float> leftPoint = mainLayerDataStruct.getPoint(stageID, distortionUnitID, 0);
        //    juce::Point<float> rightPoint(0.0f, 0.0f);
        //    float tension = mainLayerDataStruct.getTension(stageID, distortionUnitID, 0);
        //    int curveID = mainLayerDataStruct.getPointCurveType(stageID, distortionUnitID, 0);
        //    SampleRemapperBin<float>* pSRBin = nullptr;

        //    for (int binID = 0; binID < nbOfPoints - 1; ++binID)
        //    {
        //        pSRBin = pSampleRemapper->getBin(binID);
        //        SampleRemapperBin<float>::BinParams srBinParams;
        //        srBinParams.leftPoint = mainLayerDataStruct.getPoint(stageID, distortionUnitID, binID + 1);
        //        srBinParams.rightPoint = mainLayerDataStruct.getPoint(stageID, distortionUnitID, binID);
        //        srBinParams.tension = mainLayerDataStruct.getTension(stageID, distortionUnitID, binID);
        //        srBinParams.curveID = mainLayerDataStruct.getPointCurveType(stageID, distortionUnitID, binID);

        //        if (srBinParams != pSRBin->getBinParams())
        //        {
        //            pSRBin->setBinParam(srBinParams);
        //        }
        //        pDistortionDU->computeDistortionEQAPathData(binID);
        //    }//SR Bin Loop
        //}//Disto Unit loop
    
}

private:
    //==============================================================================
    //juce::dsp::ProcessorChain<CustomWaveShaper<float>, CustomWaveShaper<float>> processorChain; // [1]
    std::vector<DistortionUnitProcessor<SampleType>> processorChain;
    //juce::dsp::Oversampling<SampleType> oversamplerProcessor;
    //juce::dsp::Oversampling<SampleType> oversampler;
    juce::dsp::DryWetMixer<SampleType> dryWet;

    Parameters params;

    int nbOfChannels;
    //bool isBypassed = false;
};