/*
  ==============================================================================

    SampleRemapping.h
    Created: 6 Sep 2021 12:11:08pm
    Author:  CookiMonstor

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PiecewiseFunctionBin.h"

template <typename SampleType>
class PiecewiseFunction
{
public:
    PiecewiseFunction()
    {
        SampleType zero = static_cast<SampleType>(0.0);
        SampleType one = static_cast<SampleType>(1.0);
        SampleType minusOne = -one;

        //pwfBins.resize(DistortionConstants::WaveShaper<SampleType>::nbOfPointMax);
        
        for (int binID = 0; binID < DistortionConstants::WaveShaper<SampleType>::nbOfPointMax; ++binID)
        {
            pwfBins.add(new PiecewiseFunctionBin<SampleType>());
        }

        pwfBins[0]->setLeftPoint(juce::Point<SampleType>(zero, zero));
        pwfBins[0]->setRightPoint(juce::Point<SampleType>(one, one));
        pwfBins[0]->setTension(zero);
        pwfBins[0]->computePathData();
        //pwfBins[0]->pushSampleIntoFifo();

        pwfBins[1]->setLeftPoint(juce::Point<SampleType>(zero, zero));
        pwfBins[1]->setRightPoint(juce::Point<SampleType>(zero, zero));
        pwfBins[1]->setTension(zero);
        pwfBins[1]->computePathData();
        //pwfBins[1]->pushSampleIntoFifo();

        pwfBins[2]->setLeftPoint(juce::Point<SampleType>(minusOne, minusOne));
        pwfBins[2]->setRightPoint(juce::Point<SampleType>(zero, zero));
        pwfBins[2]->setTension(zero);
        pwfBins[2]->computePathData();
        //pwfBins[2]->pushSampleIntoFifo();

        nbOfActiveBins = 3;

        //pwfBins[0].setLeftPoint(juce::Point<SampleType>(one, one));
        //pwfBins[0].setRightPoint(juce::Point<SampleType>(1000000.0f, 1000000.0f));
        //pwfBins[0].setTension(one);
        //pwfBins[0].computePathData();

        //pwfBins[1].setLeftPoint(juce::Point<SampleType>(zero, zero));
        //pwfBins[1].setRightPoint(juce::Point<SampleType>(one, one));
        //pwfBins[1].setTension(one);
        //pwfBins[1].computePathData();

        //pwfBins[2].setLeftPoint(juce::Point<SampleType>(zero, zero));
        //pwfBins[2].setRightPoint(juce::Point<SampleType>(zero, zero));
        //pwfBins[2].setTension(one);
        //pwfBins[2].computePathData();

        //pwfBins[3].setLeftPoint(juce::Point<SampleType>(minusOne, minusOne));
        //pwfBins[3].setRightPoint(juce::Point<SampleType>(zero, zero));
        //pwfBins[3].setTension(one);
        //pwfBins[3].computePathData();

        //pwfBins[4].setLeftPoint(juce::Point<SampleType>(-1000000.0f, -1000000.0f));
        //pwfBins[4].setRightPoint(juce::Point<SampleType>(minusOne, minusOne));
        //pwfBins[4].setTension(one);
        //pwfBins[4].computePathData();

        //nbOfActiveBins = 3;
    };

    //===========================================================
    void prepare(const juce::dsp::ProcessSpec& spec) noexcept
    {
        sampleRate = spec.sampleRate;

        for (int bin = 0; bin < pwfBins.size(); ++bin)
        {
            pwfBins[bin]->prepare(spec);
            for (int fifo = 0; fifo < 5; ++fifo)
            {
                pwfBins[bin]->pushSampleIntoFifo();
                pwfBins[bin]->pullSampleFromFifo(pwfBins[bin]->getBinPathData());
            }
        }

        reset();
    };

    //===========================================================
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
            smoothedValuesSkip(static_cast<int>(inputNumSamples));

            if (context.usesSeparateInputAndOutputBlocks())
                outputBlock.copyFrom(inputBlock);

            return;
        }

        //Push the dry sample, where no processing is applied.
        //dryWet.pushDrySamples(inputBlock);

        int binID = 0;
        //SampleType one = static_cast<SampleType>(1.0);
        //SampleType minusOne = -one;

        //WaveShaper Processing (function)
        for (int sampleIndex = 0; sampleIndex < numSamples; ++sampleIndex)
        {
            //auto* inputSamples = inputBlock.getChannelPointer(channel);
            //auto* outputSamples = outputBlock.getChannelPointer(channel);

            getNextTensionAcrossVector();

            for (int channel = 0; channel < numChannels; ++channel)
            {
                auto input = inputBlock.getSample(channel, sampleIndex);
                //auto output = input;

                //We need to hardclip at one because if a sample > 1.0 then pow yield a nan.
                //input = juce::jlimit(-juce::Decibels::decibelsToGain(120.0f), juce::Decibels::decibelsToGain(120.0f), input);

                auto output = processSample(input);
                //jassert(binID < pwfBins.size());

                //Hard clip
                //output = juce::jlimit(-xLimMax, xLimMax, output);
                //output = juce::jlimit(minusOne, one, output);

                outputBlock.getChannelPointer(channel)[sampleIndex] = output;

                binID = 0;
            }
        }

        //Mix
        //dryWet.mixWetSamples(outputBlock);
        
    };

    void processBuffer(juce::AudioBuffer<SampleType>& buffer) noexcept
    {
        //juce::ignoreUnused(context);
        //processorChain.process(context);

        //const auto& inputBlock = context.getInputBlock();
        //auto& outputBlock = context.getOutputBlock();
        //const auto numChannels = outputBlock.getNumChannels();
        //const auto numSamples = outputBlock.getNumSamples();

        //jassert(inputBlock.getNumChannels() == numChannels);
        //jassert(inputBlock.getNumSamples() == numSamples);

        //Bypass WaveShaper
        //if ((!isBypassed))//|| context.isBypassed)
        //{
        //    outputBlock.copyFrom(inputBlock);
        //    return;
        //}

        //Push the dry sample, where no processing is applied.
        //dryWet.pushDrySamples(inputBlock);

        int index = 0;
        SampleType one = static_cast<SampleType>(1.0);
        SampleType minusOne = -one;

        SampleType* pInputSample = buffer.getWritePointer(0, 0);
        SampleType inputSample = *pInputSample;

        //WaveShaper Processing (function)
        for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
        {
            //auto* inputSamples = inputBlock.getChannelPointer(channel);
            //auto* outputSamples = outputBlock.getChannelPointer(channel);

            for (int i = 0; i < buffer.getNumSamples(); ++i)
            {
                pInputSample = (SampleType*)buffer.getWritePointer(channel, i);
                inputSample = (SampleType)*pInputSample;
                SampleType output = static_cast<SampleType>(0.0);

                if (inputSample < minusOne)
                {
                    index = nbOfActiveCurves - 1;
                    output = -pwfBins[index].remapSample(-inputSample);

                }
                else if (inputSample > one)
                {
                    index = 0;
                    output = pwfBins[index].remapSample(inputSample);
                }
                else
                {
                    while ((!(pwfBins[index].isSampleInRange(inputSample))) && index < nbOfActiveBins) //Check if it is NOT in range.
                    {
                        jassert(index < pwfBins.size());
                        index++;
                    }

                    output = pwfBins[index].remapSample(inputSample);
                }

                jassert(index < pwfBins.size());

                //Hard clip
                //output = juce::jlimit(-xLimMax, xLimMax, output);
                output = juce::jlimit(minusOne, one, output);

                *pInputSample = output;

                index = 0;
            }
        }

        //Post Gain Processing
        //postGainProcessor.process(context);

        //Mix
        //dryWet.mixWetSamples(outputBlock);

    };

    SampleType processSample(SampleType sample)
    {
        SampleType input = juce::jlimit(static_cast<SampleType>(-1.0), static_cast<SampleType>(1.0), sample);
        SampleType output = static_cast<SampleType>(0.0);
        int binID = 0;

        //if (binLimPlusOne.isSampleInRange(input))
        //{
        //    output = binLimPlusOne.remapSample(input);

        //}
        //else if (binLimMinusOne.isSampleInRange(input))
        //{
        //    output = binLimMinusOne.remapSample(input);
        //}
        //else
        {
            while ((!(pwfBins[binID]->isSampleInRange(sample))) && binID < nbOfActiveBins) //Check if it is NOT in range.
            {
                jassert(binID < pwfBins.size());
                binID++;
            }

            output = pwfBins[binID]->remapSample(input);
        }

        return output;
    }

    //===========================================================
    void reset() noexcept
    {
        SampleType zero = static_cast<SampleType>(0.0);
        SampleType one = static_cast<SampleType>(1.0);
        SampleType minusOne = -one;

        //pwfBins.resize(DistortionConstants::WaveShaper<SampleType>::nbOfPointMax);
        
        pwfBins[0]->setLeftPoint(juce::Point<SampleType>(zero, zero));
        pwfBins[0]->setRightPoint(juce::Point<SampleType>(one, one));
        pwfBins[0]->setTension(zero);

        pwfBins[1]->setLeftPoint(juce::Point<SampleType>(zero, zero));
        pwfBins[1]->setRightPoint(juce::Point<SampleType>(zero, zero));
        pwfBins[1]->setTension(zero);

        pwfBins[2]->setLeftPoint(juce::Point<SampleType>(minusOne, minusOne));
        pwfBins[2]->setRightPoint(juce::Point<SampleType>(zero, zero));
        pwfBins[2]->setTension(zero);

        nbOfActiveBins = 3;

        for (int bin = 0; bin < pwfBins.size(); ++bin)
        {
            pwfBins[bin]->reset();
            
            //for (int fifo = 0; fifo < 10; ++fifo)
            //{
            //    pwfBins[bin]->pushSampleIntoFifo();
            //}


        }
    }

    void smoothedValuesSkip(int numSamplesToSkip)
    {
        for (int binIndex = 0; binIndex < nbOfActiveBins; ++binIndex)
        {
            pwfBins[binIndex]->smoothedValuesSkip(numSamplesToSkip);
        }
    }

    void getNextTensionAcrossVector()
    {
        //binLimPlusOne.getNextPointValue();
        //binLimPlusOne.getNextTensionValue();

        for (int binIndex = 0; binIndex < nbOfActiveBins; ++binIndex)
        {
            pwfBins[binIndex]->getNextPointValue();
            pwfBins[binIndex]->getNextTensionValue();
        }

        //binLimMinusOne.getNextPointValue();
        //binLimMinusOne.getNextTensionValue();
    };

    static void createValueTree(juce::ValueTree& vt, juce::UndoManager* undoManager)//, int distortionUnitID)
    {
        juce::Identifier id(PiecewiseFunctionConstants::ParamStringID::piecewiseFunction);
        juce::ValueTree vtPiecewiseFunction(id);
        vt.addChild(vtPiecewiseFunction, -1, nullptr);

        vtPiecewiseFunction.setProperty(juce::Identifier(PiecewiseFunctionConstants::ParamStringID::isBipolar), false, nullptr);
        vtPiecewiseFunction.setProperty(juce::Identifier(PiecewiseFunctionConstants::ParamStringID::selectedCurveID), 0, nullptr);
        vtPiecewiseFunction.setProperty(juce::Identifier(PiecewiseFunctionConstants::ParamStringID::selectedPointID), 0, nullptr);
        vtPiecewiseFunction.setProperty(juce::Identifier(PiecewiseFunctionConstants::ParamStringID::selectedTensionID), -1, nullptr);

        createValueTreePointUnipolar(vtPiecewiseFunction, undoManager);
    };

    static void createValueTreePointUnipolar(juce::ValueTree& vt, juce::UndoManager* undoManager)//, int distortionUnitID)
    {
        juce::Identifier id(PiecewiseFunctionConstants::ParamStringID::srPoints);
        juce::ValueTree vtPiecewiseFunctionPoint(id);
        vt.addChild(vtPiecewiseFunctionPoint, -1, undoManager);

        SampleType zero = static_cast<SampleType>(0.0);
        SampleType one = static_cast<SampleType>(1.0);

        vtPiecewiseFunctionPoint.addChild(createPoint(one, one, zero, 0, false, undoManager), -1, undoManager);
        vtPiecewiseFunctionPoint.addChild(createPoint(zero, zero, zero, 0, false, undoManager), -1, undoManager);
        vtPiecewiseFunctionPoint.addChild(createPoint(zero, zero, zero, 0, false, undoManager), -1, undoManager);
        vtPiecewiseFunctionPoint.addChild(createPoint(-one, -one, zero, 0, false, undoManager), -1, undoManager);
    };

    static void createValueTreePointBipolar(juce::ValueTree& vt, juce::UndoManager* undoManager)//, int distortionUnitID)
    {
        juce::Identifier id(PiecewiseFunctionConstants::ParamStringID::srPoints);
        juce::ValueTree vtPiecewiseFunctionPoint(id);
        vt.addChild(vtPiecewiseFunctionPoint, -1, undoManager);

        SampleType zero = static_cast<SampleType>(0.0);
        SampleType one = static_cast<SampleType>(1.0);

        vtPiecewiseFunctionPoint.addChild(createPoint(one, one, zero, 0, false, undoManager), -1, undoManager);
        vtPiecewiseFunctionPoint.addChild(createPoint(zero, zero, zero, 0, true, undoManager), -1, undoManager);
        vtPiecewiseFunctionPoint.addChild(createPoint(-one, -one, zero, 0, false, undoManager), -1, undoManager);
    };

    static juce::ValueTree createPoint(float pointX, float pointY, float tension, int curveID, bool horizontalDragOn, juce::UndoManager* undoManager)
    {
        juce::Identifier id(PiecewiseFunctionConstants::ParamStringID::point);
        juce::ValueTree vt(id);
        //juce::ValueTree vt(DistortionConstants::WaveShaperParamStringID::point);

        vt.setProperty(PiecewiseFunctionConstants::ParamStringID::pointX, pointX, undoManager);
        vt.setProperty(PiecewiseFunctionConstants::ParamStringID::pointY, pointY, undoManager);

        vt.setProperty(PiecewiseFunctionConstants::ParamStringID::tension, tension, undoManager);
        vt.setProperty(PiecewiseFunctionConstants::ParamStringID::horizontalDragOn, horizontalDragOn, undoManager);

        vt.setProperty(PiecewiseFunctionConstants::ParamStringID::curveType, curveID, undoManager);

        return vt;
    };


    //Set Function
    void setNbOfActiveBins(const int newNbOfActiveBins) { nbOfActiveBins = newNbOfActiveBins; };

    //Get function
    PiecewiseFunctionBin<SampleType>* getBin(int binID) { return pwfBins[binID]; };
    int getNbOfActiveBins() { return nbOfActiveBins; };

private:
    //PiecewiseFunctionBin<SampleType> binLimPlusOne;
    //std::vector< PiecewiseFunctionBin<SampleType> > pwfBins;
    juce::OwnedArray< PiecewiseFunctionBin<SampleType> > pwfBins;
    //PiecewiseFunction<SampleType> binLimMinusOne;

    SampleType xLimMax = static_cast<SampleType>(DistortionConstants::WaveShaper<SampleType>::sampleLimitValue);
    
    int nbOfActiveBins;

    bool isBypassed = false;
    double sampleRate = 44100.0;
};