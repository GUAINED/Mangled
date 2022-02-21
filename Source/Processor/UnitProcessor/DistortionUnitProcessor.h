/*
  ==============================================================================

    WaveShaperProcessor.h
    Created: 1 Apr 2021 5:06:19pm
    Author:  CookiMonstor

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "DistortionCircuit.h"
#include "PiecewiseFunction.h"
#include "DCFilter.h"
#include "GainProcessor.h"
//==============================================================================
template <typename SampleType>
class DistortionUnitProcessor
{
public:
    //==============================================================================
    DistortionUnitProcessor(int initNbOfChannels = 2)
        : nbOfChannels(initNbOfChannels)
        , dcFilter(initNbOfChannels)
        //, oversamplerProcessor(initNbOfChannels)
    {
        preGainProcessor.setGainDecibels(DistoUnitConstants::Processor<SampleType>::preGaindBStartValue);
        //preGainProcessor.setRampDurationSeconds();
        //distortionEquation = equationIDToEquation(0);
        postGainProcessor.setGainDecibels(DistoUnitConstants::Processor<SampleType>::postGaindBStartValue);
        dryWet.setMixingRule(juce::dsp::DryWetMixingRule::linear);
        //oversampler.clearOversamplingStages();

        //SampleType osUp = static_cast<SampleType>(0.1);
        //SampleType osDown = static_cast<SampleType>(0.1);
        //SampleType sbUp = static_cast<SampleType>(-50);
        //SampleType sbDown = static_cast<SampleType>(-50);

        //oversampler.addOversamplingStage(juce::dsp::Oversampling<SampleType>::FilterType::filterHalfBandFIREquiripple
        //    , osUp
        //    , sbUp
        //    , osDown
        //    , sbDown);
        for (int binID = 0; binID < 3; ++binID)
        {
            computeDistortionEQAPathData(binID);
        }
    }

    struct Parameters
    {
        bool routing = DistoUnitConstants::Processor<SampleType>::routingStartValue;
        typename DistortionCircuit<SampleType>::Parameters circuit;
        SampleType preGain = DistoUnitConstants::Processor<SampleType>::preGaindBStartValue;
        SampleType postGain = DistoUnitConstants::Processor<SampleType>::postGaindBStartValue;
        SampleType mix = DistoUnitConstants::Processor<SampleType>::mixStartValue;
        bool overSampling = DistoUnitConstants::Processor<SampleType>::overSamplingStartValue;
        bool isBipolar = DistoUnitConstants::Processor<SampleType>::isBipolarStartValue;
        typename DCFilter<SampleType>::Parameters dcFilter;
        bool isBypassed = DistoUnitConstants::Processor<SampleType>::isBypassedStartValue;
        int id = 0;

        inline Parameters& operator=(const Parameters& other)
        {
            // Guard self assignment
            if (this == &other)
                return *this;

            this->routing = other.routing;
            this->circuit = other.circuit;
            this->preGain = other.preGain;
            this->postGain = other.postGain;
            this->overSampling = other.overSampling;
            this->isBipolar = other.isBipolar;
            this->dcFilter = other.dcFilter;
            this->isBypassed = other.isBypassed;
            this->id = other.id;

            return *this;
        };

        inline bool operator==(const Parameters& rhs)
        {
            return ((routing == rhs.routing) &&
                (circuit == rhs.circuit) &&
                (preGain == rhs.preGain) &&
                (postGain == rhs.postGain) &&
                (mix == rhs.mix) &&
                (isBipolar == rhs.isBipolar) &&
                (dcFilter == rhs.dcFilter) &&
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

        sampleRate = spec.sampleRate;

        dryWet.prepare(spec);
        
        //oversampler.initProcessing(2 * spec.maximumBlockSize);

        preGainProcessor.prepare(spec);
        preGainProcessor.setRampDurationSeconds(spec.maximumBlockSize / spec.sampleRate);
        //preGainProcessor.setRampDurationSeconds(0.02);

        sampleRemapper.prepare(spec);
        
        distortionCircuit.prepare(spec);
        
        dcFilter.prepare(spec);
        
        postGainProcessor.prepare(spec);
        postGainProcessor.setRampDurationSeconds(spec.maximumBlockSize / spec.sampleRate);

        reset();
    }

    //==============================================================================
    template <typename ProcessContext>
    void process(const ProcessContext& context) noexcept
    {
        //juce::ignoreUnused(context);
        //processorChain.process(context);

        const auto& inputBlock = context.getInputBlock();
        auto& outputBlock = context.getOutputBlock();
        const auto numChannels = outputBlock.getNumChannels();
        const auto numSamples = outputBlock.getNumSamples();

        jassert(inputBlock.getNumChannels() == numChannels);
        jassert(inputBlock.getNumSamples() == numSamples);

        auto inputNumSamples = inputBlock.getNumSamples();

        //Bypass WaveShaper
        if (params.isBypassed || context.isBypassed)
        {
            skipDUSmoothedValues(static_cast<int>(inputNumSamples));

            if (context.usesSeparateInputAndOutputBlocks())
                outputBlock.copyFrom(inputBlock);

            return;
        }

        //Push the dry sample, where no processing is applied.
        dryWet.pushDrySamples(inputBlock);
        
        //Pre Gain Processing
        preGainProcessor.process(context);

        //OverSampling
        //if (oversamplerIsBypassed)
        //{
        //    auto& inputOSBlock = oversampler.processSamplesUp(inputBlock);
        //    auto numSamp = inputOSBlock.getNumSamples();

        //    //for (int distoUID = 0; distoUID < DistortionConstants::nbOfDUMax; ++distoUID)
        //    //{
        //    //    for (int channelID = 0; channelID < numChannels; ++channelID)
        //    //    {
        //    //        for (int sampleID = 0; sampleID < numSamp; ++sampleID)
        //    //        {
        //    //            SampleType sample = inputOSBlock.getSample(channelID, sampleID);
        //    //            processorChain[distoUID].process();
        //    //        }
        //    //    }
        //    //    processorChain[]
        //    //}

        //}

        if (params.routing)
        {
            //Distortion Circuit
            distortionCircuit.process(context);

            //Sample Remapper
            sampleRemapper.process(context);
        }
        else
        {
            //Sample Remapper
            sampleRemapper.process(context);

            //Distortion Circuit
            distortionCircuit.process(context);
        }

        //if (oversamplerIsBypassed)
        //{
        //    oversampler.processSamplesDown(outputBlock);
        //}

        //DC Filter
        dcFilter.process(context);

        //Post Gain Processing
        postGainProcessor.process(context);

        //Mix
        dryWet.mixWetSamples(outputBlock);
    }

    void processBuffer(juce::AudioBuffer<SampleType>& buffer) noexcept
    {
        juce::dsp::AudioBlock<SampleType> block(buffer);
        juce::dsp::ProcessContextReplacing<SampleType> context(block);

        //const auto& inputBlock = context.getInputBlock();
        //auto& outputBlock = context.getOutputBlock();
        //const auto numChannels = outputBlock.getNumChannels();
        //const auto numSamples = outputBlock.getNumSamples();

        //jassert(inputBlock.getNumChannels() == numChannels);
        //jassert(inputBlock.getNumSamples() == numSamples);

        //Bypass WaveShaper
        if (isBypassed)//|| context.isBypassed)
        {
            //outputBlock.copyFrom(inputBlock);
            return;
        }

        //Push the dry sample, where no processing is applied.
        //dryWet.pushDrySamples(inputBlock);

        //Pre Gain Processing
        preGainProcessor.process(context);

        if (routing)
        {
            //Distortion Circuit
            distortionCircuit.processBuffer(buffer);

            //Sample Remapper
            sampleRemapper.processBuffer(buffer);
        }
        else
        {
            //Sample Remapper
            sampleRemapper.processBuffer(buffer);

            //Distortion Circuit
            distortionCircuit.processBuffer(buffer);
        }


        //DC Filter
        dcFilter.processBuffer(buffer);

        //Post Gain Processing
        postGainProcessor.process(context);

        //Mix
        //dryWet.mixWetSamples(outputBlock);
    }
    //==============================================================================
    void reset() noexcept
    {
        //processorChain.reset();
        //oversampler.reset();

        dryWet.reset();
        preGainProcessor.reset();
        sampleRemapper.reset();
        distortionCircuit.reset();
        dcFilter.reset();
        postGainProcessor.reset();
    }

    /** Sets the Pre Gain of the waveShaper.
    */
    void setPreGainParam(SampleType newPreGain)
    {
        //auto& preGainFx = processorChain.template get<CustomWaveShaper::preGainIndex>();
        //preGainFx.setGainDecibels(newPreGain);

        //preGain = newPreGain;
        preGainProcessor.setGainDecibels(newPreGain);
    }

    /** Sets the Post Gain of the waveShaper.
    */
    void setPostGainParam(SampleType newPostGain)
    {
        //auto& postGainFx = processorChain.template get<DistortionUnitProcessor::postGainIndex>();
        //postGainFx.setGainDecibels(newPostGain);

        //postGain = newPostGain;
        postGainProcessor.setGainDecibels(newPostGain);
    }

    /** Sets the amount of dry and wet signal in the output of the WaveShaper (between 0
    for full dry and 1 for full wet).
    */
    void setMixParam(SampleType newMix)
    {
        jassert(juce::isPositiveAndNotGreaterThan(newMix, static_cast<SampleType> (1.0)));

        //mix = newMix;
        dryWet.setWetMixProportion(newMix);
    }

    /** Sets the Bypass of the waveShaper.
    */
    void setIsBypassed(const bool newIsBypassed)
    {
        params.isBypassed = newIsBypassed;
    }

    /** Sets the Bypass of the waveShaper.
*/
    void setRouting(const bool newRouting)
    {
        params.routing = newRouting;
    }

    /** Sets the OverSampling
    */
    //void setOverSampling(bool newOverSampling)
    //{
    //    oversamplerIsBypassed = newOverSampling;
    //}

    /** Sets the DC Filter IsBypassed of the SampleRempper.
    */
    //void setDCFilterOffOn(const bool newDCFilterOn) 
    //{
    //    dcFilter.setIsBypassed( !newDCFilterOn);
    //}

    void setDistortionUnitParams(const Parameters& newParams)
    {
        if (params != newParams)
        {
            params = newParams;
            distortionCircuit.setParams(newParams.circuit);

            setPreGainParam(newParams.preGain);
            setPostGainParam(newParams.postGain);
            setMixParam(newParams.mix / static_cast<SampleType>(100.0));
            setIsBypassed(newParams.isBypassed);

            setRouting(newParams.routing);

            dcFilter.setParams(newParams.dcFilter);
        }
    };

    void skipDUSmoothedValues(int numSamplesToSkip)
    {
        preGainProcessor.smoothedValuesSkip(numSamplesToSkip);
        sampleRemapper.smoothedValuesSkip(numSamplesToSkip);
        distortionCircuit.smoothedValuesSkip(numSamplesToSkip);
        postGainProcessor.smoothedValuesSkip(numSamplesToSkip);
    }

    void computeDistortionEQAPathData(int binID)
    {
        //int nbOfBins = sampleRemapper.getNbOfActiveBins();
        SampleType samplePostEQA = (SampleType)0.0;
        if (params.routing)
        {
            const SampleType* binYData = sampleRemapper.getBin(binID)->getBinPathData().getReadPointer(1);

            for (int index = 0; index < DistortionConstants::UI::nbOfPointsPerBin; ++index)
            {
                samplePostEQA = distortionCircuit.processSample(binYData[index]);
                sampleRemapper.getBin(binID)->setEQBinData(index, samplePostEQA);
            }

            sampleRemapper.getBin(binID)->pushSampleIntoFifo();
        }
        else
        {
            SampleType x = static_cast<SampleType>(0.0);
            SampleType xInc = static_cast<SampleType>(0.0);
            SampleType leftX = static_cast<SampleType>(0.0);
            SampleType rightX = static_cast<SampleType>(0.0);
            int nbOfPoints = DistortionConstants::UI::nbOfPointsPerBin;

            leftX = sampleRemapper.getBin(binID)->getTargetLeftX();
            rightX = sampleRemapper.getBin(binID)->getTargetRightX();

            x = rightX;

            xInc = (rightX - leftX) / (SampleType)(nbOfPoints - 1);

            for (int index = 0; index < nbOfPoints; ++index)
            {
                samplePostEQA = distortionCircuit.processSample(x);

                samplePostEQA = sampleRemapper.processSample(samplePostEQA);

                sampleRemapper.getBin(binID)->setEQBinData(index, samplePostEQA);

                x -= xInc;

            }

            sampleRemapper.getBin(binID)->pushSampleIntoFifo();
        }
    };

    static void createParametersLayout(std::vector<std::unique_ptr<juce::RangedAudioParameter>>* plugInParameters, int stageID, int distortionUnitID)
    {
        juce::NormalisableRange<SampleType> normalisablePreGainRange{ DistoUnitConstants::Processor<SampleType>::preGaindBMin,
                                                      DistoUnitConstants::Processor<SampleType>::preGaindBMax,
                                                       DistoUnitConstants::Processor<SampleType>::preGaindBIncrement };

        juce::NormalisableRange<SampleType> normalisableDriveRange{ DistoUnitConstants::Processor<SampleType>::driveMin,
                                                                DistoUnitConstants::Processor<SampleType>::driveMax,
                                                                 DistoUnitConstants::Processor<SampleType>::driveIncrement };

        juce::NormalisableRange<SampleType> normalisableWarpRange{ DistoUnitConstants::Processor<SampleType>::warpMin,
                                                            DistoUnitConstants::Processor<SampleType>::warpMax,
                                                             DistoUnitConstants::Processor<SampleType>::warpIncrement };

        juce::NormalisableRange<SampleType> normalisablePostGainRange{ DistoUnitConstants::Processor<SampleType>::postGaindBMin,
                                                                   DistoUnitConstants::Processor<SampleType>::postGaindBMax,
                                                                    DistoUnitConstants::Processor<SampleType>::postGaindBIncrement };

        juce::NormalisableRange<SampleType> normalisableMixRange{ DistoUnitConstants::Processor<SampleType>::mixMin,
                                                    DistoUnitConstants::Processor<SampleType>::mixMax,
                                                    DistoUnitConstants::Processor<SampleType>::mixIncrement };

        bool duOnOffStartValue = distortionUnitID == 0 ? DistoUnitConstants::Processor<SampleType>::isBypassedStartValue
            : !DistoUnitConstants::Processor<SampleType>::isBypassedStartValue;

        juce::String paramString = DistoUnitConstants::ParamStringID::GetParamStringID::isBypassed(stageID, distortionUnitID);
        juce::String automationParamString = DistoUnitConstants::AutomationString::GetString::isBypassed(stageID, distortionUnitID);
        plugInParameters->push_back(std::make_unique<juce::AudioParameterBool>(paramString,
            automationParamString,
            duOnOffStartValue));

        //Distortion Equation
        paramString = DistoUnitConstants::ParamStringID::GetParamStringID::routing(stageID, distortionUnitID);
        automationParamString = DistoUnitConstants::AutomationString::GetString::routing(stageID, distortionUnitID);
        plugInParameters->push_back(std::make_unique<juce::AudioParameterBool>(paramString,
            automationParamString,
            DistoUnitConstants::Processor<SampleType>::routingStartValue));

        paramString = DistoUnitConstants::ParamStringID::GetParamStringID::equationType(stageID, distortionUnitID);
        automationParamString = DistoUnitConstants::AutomationString::GetString::equationType(stageID, distortionUnitID);
        plugInParameters->push_back(std::make_unique<juce::AudioParameterChoice>(paramString,
            automationParamString,
            DistortionConstants::UI::equationTypeStringArray,
            0));

        paramString = DistoUnitConstants::ParamStringID::GetParamStringID::sigmoidEQA(stageID, distortionUnitID);
        automationParamString = DistoUnitConstants::AutomationString::GetString::sigmoidEQA(stageID, distortionUnitID);
        plugInParameters->push_back(std::make_unique<juce::AudioParameterChoice>(paramString,
            automationParamString,
            DistortionConstants::UI::sigmoidEQAStringArray,
            1));

        paramString = DistoUnitConstants::ParamStringID::GetParamStringID::symetricEQA(stageID, distortionUnitID);
        automationParamString = DistoUnitConstants::AutomationString::GetString::symetricEQA(stageID, distortionUnitID);
        plugInParameters->push_back(std::make_unique<juce::AudioParameterChoice>(paramString,
            automationParamString,
            DistortionConstants::UI::symetricEQAStringArray,
            1));

        paramString = DistoUnitConstants::ParamStringID::GetParamStringID::asymetricEQA(stageID, distortionUnitID);
        automationParamString = DistoUnitConstants::AutomationString::GetString::asymetricEQA(stageID, distortionUnitID);
        plugInParameters->push_back(std::make_unique<juce::AudioParameterChoice>(paramString,
            automationParamString,
            DistortionConstants::UI::asymetricEQAStringArray,
            1));

        paramString = DistoUnitConstants::ParamStringID::GetParamStringID::specialEQA(stageID, distortionUnitID);
        automationParamString = DistoUnitConstants::AutomationString::GetString::specialEQA(stageID, distortionUnitID);
        plugInParameters->push_back(std::make_unique<juce::AudioParameterChoice>(paramString,
            automationParamString,
            DistortionConstants::UI::specialEQAStringArray,
            1));

        //DC Filter OnOff
        paramString = DistoUnitConstants::ParamStringID::GetParamStringID::dcFilterIsBypassed(stageID, distortionUnitID);
        automationParamString = DistoUnitConstants::AutomationString::GetString::dcFilterIsBypassed(stageID, distortionUnitID);
        plugInParameters->push_back(std::make_unique<juce::AudioParameterBool>(paramString,
            automationParamString,
            DistoUnitConstants::Processor<SampleType>::dcFilterIsBypassedStartValue));

        //PreGain WaveShaper
        paramString = DistoUnitConstants::ParamStringID::GetParamStringID::preGain(stageID, distortionUnitID);
        automationParamString = DistoUnitConstants::AutomationString::GetString::preGain(stageID, distortionUnitID);
        plugInParameters->push_back(std::make_unique<juce::AudioParameterFloat>(paramString,
            automationParamString,
            normalisablePreGainRange,
            DistoUnitConstants::Processor<SampleType>::preGaindBStartValue));

        //Drive WaveShaper
        paramString = DistoUnitConstants::ParamStringID::GetParamStringID::drive(stageID, distortionUnitID);
        automationParamString = DistoUnitConstants::AutomationString::GetString::drive(stageID, distortionUnitID);
        plugInParameters->push_back(std::make_unique<juce::AudioParameterFloat>(paramString,
            automationParamString,
            normalisableDriveRange,
            DistoUnitConstants::Processor<SampleType>::driveStartValue));

        //Warp WaveShaper
        paramString = DistoUnitConstants::ParamStringID::GetParamStringID::warp(stageID, distortionUnitID);
        automationParamString = DistoUnitConstants::AutomationString::GetString::warp(stageID, distortionUnitID);
        plugInParameters->push_back(std::make_unique<juce::AudioParameterFloat>(paramString,
            automationParamString,
            normalisableWarpRange,
            DistoUnitConstants::Processor<SampleType>::warpStartValue));

        //PostGain WaveShaper
        paramString = DistoUnitConstants::ParamStringID::GetParamStringID::postGain(stageID, distortionUnitID);
        automationParamString = DistoUnitConstants::AutomationString::GetString::postGain(stageID, distortionUnitID);
        plugInParameters->push_back(std::make_unique<juce::AudioParameterFloat>(paramString,
            automationParamString,
            normalisablePostGainRange,
            DistoUnitConstants::Processor<SampleType>::postGaindBStartValue));

        //Mix WaveShaper
        paramString = DistoUnitConstants::ParamStringID::GetParamStringID::mix(stageID, distortionUnitID);
        automationParamString = DistoUnitConstants::AutomationString::GetString::mix(stageID, distortionUnitID);
        plugInParameters->push_back(std::make_unique<juce::AudioParameterFloat>(paramString,
            automationParamString,
            normalisableMixRange,
            DistoUnitConstants::Processor<SampleType>::mixStartValue));
    };

    static void createValueTree(juce::ValueTree& vt, int distortionUnitID, juce::UndoManager* undoManager)
    {
        juce::Identifier id(DistoUnitConstants::ParamStringID::GetParamStringID::id(distortionUnitID));
        juce::ValueTree vtDistortionUnit(id);
        vt.addChild(vtDistortionUnit, -1, nullptr);

        PiecewiseFunction<SampleType>::createValueTree(vtDistortionUnit, undoManager);

        //DistortionCircuit<SampleType>::createValueTree(vtDistortionUnit);
    };

    DCFilter<SampleType>* getDCFilter() { return &dcFilter; };
    PiecewiseFunction<SampleType>* getPiecewiseFunction() { return &sampleRemapper; };
    DistortionCircuit<SampleType>* getDistortionCircuit() { return &distortionCircuit; };

private:
    //DistortionUnit Module
    GainProcessor<SampleType> preGainProcessor;

    //juce::dsp::Oversampling<SampleType> testout;

    PiecewiseFunction<SampleType> sampleRemapper;

    DistortionCircuit<SampleType> distortionCircuit;

    DCFilter<SampleType> dcFilter;

    GainProcessor<SampleType> postGainProcessor;

    juce::dsp::DryWetMixer<SampleType> dryWet;

    //SampleType preGain = 0.0f, postGain = 0.0f; //Gain are in dB.

    Parameters params;

    //SampleType mix = 1.0f;

    //bool routing = true;

    //bool isBypassed = false;
    //bool oversamplerIsBypassed = true;

    int nbOfChannels = 2;
    double sampleRate = 44100.0;
    //GainProcessor<SampleType> yolo;
    //Unsed--------------------------------------------------------------------------------------------------------------------
    //enum
    //{
    //    preGainIndex,
    //    waveshaperIndex,
    //    postGainIndex
    //};

    //juce::dsp::ProcessorChain<juce::dsp::Gain<float>, juce::dsp::WaveShaper<float>, juce::dsp::Gain<float>> processorChain; // [1]
 

    //==============================================================================
    // Although clang supports C++17, their standard library still has no invoke_result
    // support. Remove the "|| JUCE_CLANG" once clang supports this properly!
    //#if (! JUCE_CXX17_IS_AVAILABLE) || (JUCE_CLANG && ! JUCE_WINDOWS)
    //    template <typename Functor>
    //    static CustomWaveShaper<typename std::result_of<Functor>, Functor> CreateWaveShaper(Functor functionToUse) { return { functionToUse }; }
    //#else
    //    template <typename Functor>
    //    static CustomWaveShaper<typename std::invoke_result<Functor>, Functor> CreateWaveShaper(Functor functionToUse) { return { functionToUse }; }
    //#endif
};

