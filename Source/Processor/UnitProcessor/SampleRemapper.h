/*
  ==============================================================================

    SampleRemapping.h
    Created: 6 Sep 2021 12:11:08pm
    Author:  CookiMonstor

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "SampleRemapperBin.h"

template <typename SampleType>
class SampleRemapper
{
public:
    SampleRemapper()
    {
        SampleType zero = static_cast<SampleType>(0.0);
        SampleType one = static_cast<SampleType>(1.0);
        SampleType minusOne = -one;

        //sampleRemapperBins.resize(DistortionConstants::WaveShaper<SampleType>::nbOfPointMax);
        
        for (int binID = 0; binID < DistortionConstants::WaveShaper<SampleType>::nbOfPointMax; ++binID)
        {
            sampleRemapperBins.add(new SampleRemapperBin<SampleType>());
        }

        sampleRemapperBins[0]->setLeftPoint(juce::Point<SampleType>(zero, zero));
        sampleRemapperBins[0]->setRightPoint(juce::Point<SampleType>(one, one));
        sampleRemapperBins[0]->setTension(zero);
        sampleRemapperBins[0]->computePathData();
        //sampleRemapperBins[0]->pushSampleIntoFifo();

        sampleRemapperBins[1]->setLeftPoint(juce::Point<SampleType>(zero, zero));
        sampleRemapperBins[1]->setRightPoint(juce::Point<SampleType>(zero, zero));
        sampleRemapperBins[1]->setTension(zero);
        sampleRemapperBins[1]->computePathData();
        //sampleRemapperBins[1]->pushSampleIntoFifo();

        sampleRemapperBins[2]->setLeftPoint(juce::Point<SampleType>(minusOne, minusOne));
        sampleRemapperBins[2]->setRightPoint(juce::Point<SampleType>(zero, zero));
        sampleRemapperBins[2]->setTension(zero);
        sampleRemapperBins[2]->computePathData();
        //sampleRemapperBins[2]->pushSampleIntoFifo();

        nbOfActiveBins = 3;

        //sampleRemapperBins[0].setLeftPoint(juce::Point<SampleType>(one, one));
        //sampleRemapperBins[0].setRightPoint(juce::Point<SampleType>(1000000.0f, 1000000.0f));
        //sampleRemapperBins[0].setTension(one);
        //sampleRemapperBins[0].computePathData();

        //sampleRemapperBins[1].setLeftPoint(juce::Point<SampleType>(zero, zero));
        //sampleRemapperBins[1].setRightPoint(juce::Point<SampleType>(one, one));
        //sampleRemapperBins[1].setTension(one);
        //sampleRemapperBins[1].computePathData();

        //sampleRemapperBins[2].setLeftPoint(juce::Point<SampleType>(zero, zero));
        //sampleRemapperBins[2].setRightPoint(juce::Point<SampleType>(zero, zero));
        //sampleRemapperBins[2].setTension(one);
        //sampleRemapperBins[2].computePathData();

        //sampleRemapperBins[3].setLeftPoint(juce::Point<SampleType>(minusOne, minusOne));
        //sampleRemapperBins[3].setRightPoint(juce::Point<SampleType>(zero, zero));
        //sampleRemapperBins[3].setTension(one);
        //sampleRemapperBins[3].computePathData();

        //sampleRemapperBins[4].setLeftPoint(juce::Point<SampleType>(-1000000.0f, -1000000.0f));
        //sampleRemapperBins[4].setRightPoint(juce::Point<SampleType>(minusOne, minusOne));
        //sampleRemapperBins[4].setTension(one);
        //sampleRemapperBins[4].computePathData();

        //nbOfActiveBins = 3;
    };

    //===========================================================
    void prepare(const juce::dsp::ProcessSpec& spec) noexcept
    {
        sampleRate = spec.sampleRate;

        for (int bin = 0; bin < sampleRemapperBins.size(); ++bin)
        {
            sampleRemapperBins[bin]->prepare(spec);
            for (int fifo = 0; fifo < 5; ++fifo)
            {
                sampleRemapperBins[bin]->pushSampleIntoFifo();
                sampleRemapperBins[bin]->pullSampleFromFifo(sampleRemapperBins[bin]->getBinPathData());
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
                //jassert(binID < sampleRemapperBins.size());

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
                    output = -sampleRemapperBins[index].remapSample(-inputSample);

                }
                else if (inputSample > one)
                {
                    index = 0;
                    output = sampleRemapperBins[index].remapSample(inputSample);
                }
                else
                {
                    while ((!(sampleRemapperBins[index].isSampleInRange(inputSample))) && index < nbOfActiveBins) //Check if it is NOT in range.
                    {
                        jassert(index < sampleRemapperBins.size());
                        index++;
                    }

                    output = sampleRemapperBins[index].remapSample(inputSample);
                }

                jassert(index < sampleRemapperBins.size());

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
            while ((!(sampleRemapperBins[binID]->isSampleInRange(sample))) && binID < nbOfActiveBins) //Check if it is NOT in range.
            {
                jassert(binID < sampleRemapperBins.size());
                binID++;
            }

            output = sampleRemapperBins[binID]->remapSample(input);
        }

        return output;
    }

    //===========================================================
    void reset() noexcept
    {
        SampleType zero = static_cast<SampleType>(0.0);
        SampleType one = static_cast<SampleType>(1.0);
        SampleType minusOne = -one;

        //sampleRemapperBins.resize(DistortionConstants::WaveShaper<SampleType>::nbOfPointMax);
        
        sampleRemapperBins[0]->setLeftPoint(juce::Point<SampleType>(zero, zero));
        sampleRemapperBins[0]->setRightPoint(juce::Point<SampleType>(one, one));
        sampleRemapperBins[0]->setTension(zero);

        sampleRemapperBins[1]->setLeftPoint(juce::Point<SampleType>(zero, zero));
        sampleRemapperBins[1]->setRightPoint(juce::Point<SampleType>(zero, zero));
        sampleRemapperBins[1]->setTension(zero);

        sampleRemapperBins[2]->setLeftPoint(juce::Point<SampleType>(minusOne, minusOne));
        sampleRemapperBins[2]->setRightPoint(juce::Point<SampleType>(zero, zero));
        sampleRemapperBins[2]->setTension(zero);

        nbOfActiveBins = 3;

        for (int bin = 0; bin < sampleRemapperBins.size(); ++bin)
        {
            sampleRemapperBins[bin]->reset();
            
            //for (int fifo = 0; fifo < 10; ++fifo)
            //{
            //    sampleRemapperBins[bin]->pushSampleIntoFifo();
            //}


        }
    }

    void smoothedValuesSkip(int numSamplesToSkip)
    {
        for (int binIndex = 0; binIndex < nbOfActiveBins; ++binIndex)
        {
            sampleRemapperBins[binIndex]->smoothedValuesSkip(numSamplesToSkip);
        }
    }

    void getNextTensionAcrossVector()
    {
        //binLimPlusOne.getNextPointValue();
        //binLimPlusOne.getNextTensionValue();

        for (int binIndex = 0; binIndex < nbOfActiveBins; ++binIndex)
        {
            sampleRemapperBins[binIndex]->getNextPointValue();
            sampleRemapperBins[binIndex]->getNextTensionValue();
        }

        //binLimMinusOne.getNextPointValue();
        //binLimMinusOne.getNextTensionValue();
    };

    static void createValueTree(juce::ValueTree& vt, juce::UndoManager* undoManager)//, int distortionUnitID)
    {
        juce::Identifier id(SampleRemapperConstants::ParamStringID::sampleRemapper);
        juce::ValueTree vtSampleRemapper(id);
        vt.addChild(vtSampleRemapper, -1, nullptr);

        vtSampleRemapper.setProperty(juce::Identifier(SampleRemapperConstants::ParamStringID::isBipolar), false, nullptr);
        vtSampleRemapper.setProperty(juce::Identifier(SampleRemapperConstants::ParamStringID::selectedCurveID), 0, nullptr);
        vtSampleRemapper.setProperty(juce::Identifier(SampleRemapperConstants::ParamStringID::selectedPointID), 0, nullptr);
        vtSampleRemapper.setProperty(juce::Identifier(SampleRemapperConstants::ParamStringID::selectedTensionID), -1, nullptr);

        createValueTreePointUnipolar(vtSampleRemapper, undoManager);
    };

    static void createValueTreePointUnipolar(juce::ValueTree& vt, juce::UndoManager* undoManager)//, int distortionUnitID)
    {
        juce::Identifier id(SampleRemapperConstants::ParamStringID::srPoints);
        juce::ValueTree vtSampleRemapperPoint(id);
        vt.addChild(vtSampleRemapperPoint, -1, undoManager);

        SampleType zero = static_cast<SampleType>(0.0);
        SampleType one = static_cast<SampleType>(1.0);

        vtSampleRemapperPoint.addChild(createPoint(one, one, zero, 0, false, undoManager), -1, undoManager);
        vtSampleRemapperPoint.addChild(createPoint(zero, zero, zero, 0, false, undoManager), -1, undoManager);
        vtSampleRemapperPoint.addChild(createPoint(zero, zero, zero, 0, false, undoManager), -1, undoManager);
        vtSampleRemapperPoint.addChild(createPoint(-one, -one, zero, 0, false, undoManager), -1, undoManager);
    };

    static void createValueTreePointBipolar(juce::ValueTree& vt, juce::UndoManager* undoManager)//, int distortionUnitID)
    {
        juce::Identifier id(SampleRemapperConstants::ParamStringID::srPoints);
        juce::ValueTree vtSampleRemapperPoint(id);
        vt.addChild(vtSampleRemapperPoint, -1, undoManager);

        SampleType zero = static_cast<SampleType>(0.0);
        SampleType one = static_cast<SampleType>(1.0);

        vtSampleRemapperPoint.addChild(createPoint(one, one, zero, 0, false, undoManager), -1, undoManager);
        vtSampleRemapperPoint.addChild(createPoint(zero, zero, zero, 0, true, undoManager), -1, undoManager);
        vtSampleRemapperPoint.addChild(createPoint(-one, -one, zero, 0, false, undoManager), -1, undoManager);
    };

    static juce::ValueTree createPoint(float pointX, float pointY, float tension, int curveID, bool horizontalDragOn, juce::UndoManager* undoManager)
    {
        juce::Identifier id(SampleRemapperConstants::ParamStringID::point);
        juce::ValueTree vt(id);
        //juce::ValueTree vt(DistortionConstants::WaveShaperParamStringID::point);

        vt.setProperty(SampleRemapperConstants::ParamStringID::pointX, pointX, undoManager);
        vt.setProperty(SampleRemapperConstants::ParamStringID::pointY, pointY, undoManager);

        vt.setProperty(SampleRemapperConstants::ParamStringID::tension, tension, undoManager);
        vt.setProperty(SampleRemapperConstants::ParamStringID::horizontalDragOn, horizontalDragOn, undoManager);

        vt.setProperty(SampleRemapperConstants::ParamStringID::curveType, curveID, undoManager);

        return vt;
    };


    //Set Function
    void setNbOfActiveBins(const int newNbOfActiveBins) { nbOfActiveBins = newNbOfActiveBins; };

    //Get function
    SampleRemapperBin<SampleType>* getBin(int binID) { return sampleRemapperBins[binID]; };
    int getNbOfActiveBins() { return nbOfActiveBins; };

private:
    //SampleRemapperBin<SampleType> binLimPlusOne;
    //std::vector< SampleRemapperBin<SampleType> > sampleRemapperBins;
    juce::OwnedArray< SampleRemapperBin<SampleType> > sampleRemapperBins;
    //SampleRemapperBin<SampleType> binLimMinusOne;

    SampleType xLimMax = static_cast<SampleType>(DistortionConstants::WaveShaper<SampleType>::sampleLimitValue);
    
    int nbOfActiveBins;

    bool isBypassed = false;
    double sampleRate = 44100.0;
};