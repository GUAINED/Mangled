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
    {
        processorChain.resize(DistortionConstants::Processor<SampleType>::nbOfDUMax);
        dryWet.setMixingRule(juce::dsp::DryWetMixingRule::linear);

    }

    //==============================================================================
    struct DistortionParams
    {
        //bool isBypassedFirstDistortionUnit;
        //bool isBypassedSecondDistortionUnit;
        SampleType mix;
        bool isBypassed;
    };

    enum class DistortionUnitID
    {
        firstDistortionUnitID,
        secondDistortionUnitID,
        maxDistortionUnitID
    };

    //==============================================================================
    void prepare(const juce::dsp::ProcessSpec& spec)
    {
        //juce::ignoreUnused (spec);
        //processorChain.prepare(spec);

        for (int i = 0; i < static_cast<int>(DistortionProcessor::DistortionUnitID::maxDistortionUnitID); ++i)
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
        
        
        jassert(inputBlock.getNumChannels() == numChannels);
        jassert(inputBlock.getNumSamples() == numSamples);

        //Bypass WaveShaper
        if (isBypassed || context.isBypassed)
        {
            outputBlock.copyFrom(inputBlock);
            return;
        }

        //Push the dry sample, where no processing is applied.
        dryWet.pushDrySamples(inputBlock);

        //WaveShaper Processing (function)
        for (int distortionUnitID = 0; distortionUnitID < DistortionConstants::Processor<SampleType>::nbOfDUMax; ++distortionUnitID)
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
        for (int i = 0; i < static_cast<int>(DistortionProcessor::DistortionUnitID::maxDistortionUnitID); ++i)
            processorChain[i].processBuffer(buffer);

        //Mix
        //dryWet.mixWetSamples(outputBlock);
    }

    //==============================================================================
    void reset() noexcept
    {
        for (int distortionUnitID = 0; distortionUnitID < DistortionConstants::Processor<SampleType>::nbOfDUMax; ++distortionUnitID)
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
        isBypassed = newIsBypassed;
    }

    void setDistortionParams(DistortionProcessor<SampleType>::DistortionParams& newDistortionParams)
    {
        setMixParam(newDistortionParams.mix);
        setIsBypassed(newDistortionParams.isBypassed);
    }

    DistortionUnitProcessor<SampleType>* getDistortionUnitProcessor(int distortionUnitProcessorID) { return &processorChain[distortionUnitProcessorID]; };

private:
    //==============================================================================
    //juce::dsp::ProcessorChain<CustomWaveShaper<float>, CustomWaveShaper<float>> processorChain; // [1]
    std::vector<DistortionUnitProcessor<SampleType>> processorChain;
    //juce::dsp::Oversampling<SampleType> oversamplerProcessor;
    //juce::dsp::Oversampling<SampleType> oversampler;
    juce::dsp::DryWetMixer<SampleType> dryWet;

    bool isBypassed = false;
};