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
#include "SampleRemapper.h"
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
        preGainProcessor.setGainDecibels(DistortionConstants::DistortionUnit<SampleType>::preGaindBStartValue);
        //preGainProcessor.setRampDurationSeconds();
        //distortionEquation = equationIDToEquation(0);
        postGainProcessor.setGainDecibels(DistortionConstants::DistortionUnit<SampleType>::postGaindBStartValue);
        dryWet.setMixingRule(juce::dsp::DryWetMixingRule::linear);
        //oversampler.clearOversamplingStages();

        SampleType osUp = static_cast<SampleType>(0.1);
        SampleType osDown = static_cast<SampleType>(0.1);
        SampleType sbUp = static_cast<SampleType>(-50);
        SampleType sbDown = static_cast<SampleType>(-50);

        //oversampler.addOversamplingStage(juce::dsp::Oversampling<SampleType>::FilterType::filterHalfBandFIREquiripple
        //    , osUp
        //    , sbUp
        //    , osDown
        //    , sbDown);
        computeDistortionEQAPathData();
    }

    struct DistortionUnitParams
    {
        bool routing;
        int distortionEquationType;
        int distortionEquationID;
        //std::function<float(float, float)> distortionEquation;
        SampleType preGain;
        SampleType drive;
        SampleType warp;
        SampleType postGain;
        SampleType mix;
        bool oversampling;
        bool isBipolar;
        bool dcFilterIsBypassed;
        bool isBypassed;
        int distortionUnitID;
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
        if (isBypassed || context.isBypassed)
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

        if (routing)
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

        mix = newMix;
        dryWet.setWetMixProportion(mix);
    }

    /** Sets the Bypass of the waveShaper.
    */
    void setIsBypassed(const bool newIsBypassed)
    {
        isBypassed = newIsBypassed;
    }

    /** Sets the Bypass of the waveShaper.
*/
    void setRouting(const bool newRouting)
    {
        routing = newRouting;
    }

    /** Sets the OverSampling
    */
    //void setOverSampling(bool newOverSampling)
    //{
    //    oversamplerIsBypassed = newOverSampling;
    //}

    /** Sets the DC Filter IsBypassed of the SampleRempper.
    */
    void setDCFilterOffOn(const bool newDCFilterOn) 
    {
        dcFilter.setIsBypassed( !newDCFilterOn);
    }

    void setDistortionUnitParams(DistortionUnitParams& newDistortionUnitParams)
    {
        getDistortionCircuit()->setDistortionEquationParam(newDistortionUnitParams.distortionEquationType,
            newDistortionUnitParams.distortionEquationID);
        getDistortionCircuit()->setDriveParam(newDistortionUnitParams.drive);
        getDistortionCircuit()->setWarpParam(newDistortionUnitParams.warp);

        setPreGainParam(newDistortionUnitParams.preGain);
        setPostGainParam(newDistortionUnitParams.postGain);
        setMixParam(newDistortionUnitParams.mix / static_cast<SampleType>(100.0));
        //setIsBipolar(newDistortionUnitParams.isBipolar);
        setIsBypassed(newDistortionUnitParams.isBypassed);

        setRouting(newDistortionUnitParams.routing);

        getDCFilter()->setIsBypassed(newDistortionUnitParams.dcFilterIsBypassed);

        //setOverSampling(newDistortionUnitParams.oversampling);
    }

    void skipDUSmoothedValues(int numSamplesToSkip)
    {
        preGainProcessor.smoothedValuesSkip(numSamplesToSkip);
        sampleRemapper.smoothedValuesSkip(numSamplesToSkip);
        distortionCircuit.smoothedValuesSkip(numSamplesToSkip);
        postGainProcessor.smoothedValuesSkip(numSamplesToSkip);
    }

    void computeDistortionEQAPathData()
    {
        int nbOfBins = sampleRemapper.getNbOfActiveBins();
        SampleType samplePostEQA = (SampleType) 0.0;
        if (routing)
        {
            for (int binID = 0; binID < nbOfBins; ++binID)
            {
                SampleType* binYData = sampleRemapper.getBin(binID)->getBinPathYData();

                for (int index = 0; index < 101; ++index)
                {
                    samplePostEQA = distortionCircuit.processSample(binYData[index]);
                    sampleRemapper.getBin(binID)->setEQBinData(index, samplePostEQA);
                }
            }
        }
        else
        {
            SampleType x = static_cast<SampleType>(0.0);
            SampleType xInc = static_cast<SampleType>(0.0);
            SampleType leftX = static_cast<SampleType>(0.0);
            SampleType rightX = static_cast<SampleType>(0.0);
            int nbOfPoints = DistortionConstants::UI::nbOfPointsPerBin;

            for (int binID = 0; binID < sampleRemapper.getNbOfActiveBins(); ++binID)
            {
                leftX = sampleRemapper.getBin(binID)->getTargetLeftX();
                rightX = sampleRemapper.getBin(binID)->getTargetRightX();

                x = rightX;

                xInc = (rightX - leftX) / (SampleType)(nbOfPoints - 1);

                for(int index = 0; index < nbOfPoints; ++index)
                {
                    samplePostEQA = distortionCircuit.processSample(x);

                    samplePostEQA = sampleRemapper.processSample(samplePostEQA);

                    sampleRemapper.getBin(binID)->setEQBinData(index, samplePostEQA);
                        
                    x -= xInc;
                        
                }
            }
        }
    }

    DCFilter<SampleType>* getDCFilter() { return &dcFilter; };
    SampleRemapper<SampleType>* getSampleRemapper() { return &sampleRemapper; };
    DistortionCircuit<SampleType>* getDistortionCircuit() { return &distortionCircuit; };

private:
    //DistortionUnit Module
    GainProcessor<SampleType> preGainProcessor;

    //juce::dsp::Oversampling<SampleType> testout;

    SampleRemapper<SampleType> sampleRemapper;

    DistortionCircuit<SampleType> distortionCircuit;

    DCFilter<SampleType> dcFilter;

    GainProcessor<SampleType> postGainProcessor;

    juce::dsp::DryWetMixer<SampleType> dryWet;

    //SampleType preGain = 0.0f, postGain = 0.0f; //Gain are in dB.

    SampleType mix = 1.0f;

    bool routing = true;

    bool isBypassed = false;
    bool oversamplerIsBypassed = true;

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

