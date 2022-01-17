/*
  ==============================================================================

    RMSProcessor.h
    Created: 18 Nov 2021 8:02:44pm
    Author:  CookiMonstor

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "../../DataStructure/AudioSampleFifo.h"
#include "../../DataStructure/NoReadLockAudioSampleFifo.h"
#include "IIRBiquadFilter.h"

template <typename SampleType>
class TemporalProcessor
{
public:
    TemporalProcessor() 
        //: rmsFifo(2, 1000)
    {
        lowpassFilter.add(new IIRBiquadFilter<SampleType>(0));
        lowpassFilter.add(new IIRBiquadFilter<SampleType>(0));
        peakLevels.resize(2);
    };

    void prepare(const juce::dsp::ProcessSpec& spec)//prepare(juce::dsp::ProcessSpec spec) //override
    {
        //juce::dsp::ProcessSpec spec{ sampleRate, static_cast<juce::uint32> (samplesPerBlock), 2 };
        //fxChain.prepare(spec);
        rmsLevels.clear();

        for (juce::uint32 i = 0; i < spec.numChannels; i++)
        {
            juce::LinearSmoothedValue<SampleType> rms{ static_cast<SampleType>(-100.0) };
            rms.reset(spec.sampleRate, 0.5);
            rmsLevels.emplace_back(std::move(rms));

            juce::LinearSmoothedValue<SampleType> peak{ static_cast<SampleType>(0.0) };
            peak.reset(spec.sampleRate, 0.5);
            peakLevels.emplace_back(std::move(peak));
        }
        rmsFifo.reset(spec.numChannels, static_cast<int>(spec.sampleRate) + 1);
        //rmsFifo.setFifoSize(static_cast<int>(spec.sampleRate) + 1);

        rmsWindowSize = static_cast<int> (spec.sampleRate * 50) / 1000;

        //rmsFifo.setFifoSize(5*(rmsWindowSize + 1));

        rmsCalculationBuffer.clear();
        rmsCalculationBuffer.setSize(spec.numChannels, static_cast<int>(spec.sampleRate) + 1, false, false, true);

        temporalCalculationBuffer.clear();
        temporalCalculationBuffer.setSize(spec.numChannels, spec.maximumBlockSize + 1, false, false, true);
    

        for (int channelID = 0; channelID < lowpassFilter.size(); ++channelID)
        {
            lowpassFilter[channelID]->prepare(spec);
            IIRBiquadFilter<SampleType>::FilterParams filterParams;
            filterParams.biquadParams.cutoff = static_cast<SampleType>(110.0);
            filterParams.biquadParams.q = static_cast<SampleType>(0.76);
            filterParams.biquadParams.gain = static_cast<SampleType>(0.0);
            filterParams.biquadParams.type = EQConstants::BiquadConstants<SampleType>::Types::lowpass;
            filterParams.nbOfBiquads = EQConstants::BiquadConstants<SampleType>::Orders::maxOrders;
            filterParams.isActive = true;
            filterParams.isBypassed = false;
            lowpassFilter[channelID]->setBiquadParam(filterParams);
        }
    }

    template <typename ProcessContext>
    void process(const ProcessContext& context) noexcept
    {
        if (isBypassed || context.isBypassed)
            return;
     
        const auto& inputBlock = context.getInputBlock();
        auto& outputBlock = context.getOutputBlock();
        const auto numChannels = outputBlock.getNumChannels();
        const auto numSamples = outputBlock.getNumSamples();
        const auto numSamples2 = inputBlock.getNumSamples();

        if (subViewIsBypassed)
        {
            rmsFifo.push(context);
            //rmsFifo.writeSamplesToFifo(context);
            //rmsFifo.writeSamplesToFifo(inputBlock);
        }
        else
        {
            for (int channelID = 0; channelID < numChannels; ++channelID)
            {
                //temporalCalculationBuffer.copyFrom(channelID, 0, inputBlock.getChannelPointer(channelID), numSamples);
                for (int sampleID = 0; sampleID < numSamples; ++sampleID)
                {
                    float sample = inputBlock.getSample(channelID, sampleID);
                    temporalCalculationBuffer.getWritePointer(channelID)[sampleID] = lowpassFilter[channelID]->processSample(sample);
                }
            }

            rmsFifo.push(temporalCalculationBuffer, numSamples);
            //rmsFifo.writeSamplesToFifo(temporalCalculationBuffer);
        }


        //rmsFifo.readSamplesFromFifo(rmsCalculationBuffer);

        //rmsFifo.writeSamplesToFifo(context);
        //rmsFifo.readSamplesFromFifo(rmsCalculationBuffer);
    }

    void reset()
    {
        //rmsFifo.reset();
    }

    SampleType getRmsLevel(const int channel)
    {
        jassert(channel >= 0 && channel < rmsCalculationBuffer.getNumChannels());
        rmsFifo.pull(rmsCalculationBuffer.getWritePointer(channel), channel, rmsWindowSize);
        
        //rmsFifo.readSamplesFromFifo(rmsCalculationBuffer.getWritePointer(channel), channel, rmsWindowSize);

        for (int i = 0; i < rmsCalculationBuffer.getNumSamples(); ++i)
        {
            SampleType yolo = rmsCalculationBuffer.getSample(0, i);
        }
        processLevelValue(rmsLevels[channel], juce::Decibels::gainToDecibels(rmsCalculationBuffer.getRMSLevel(channel, 0, rmsWindowSize)));
        
        juce::Range<SampleType> range = rmsCalculationBuffer.findMinMax(channel, 0, rmsWindowSize);

        peakLevels[channel].setCurrentAndTargetValue(juce::jmax(std::abs(range.getStart()), std::abs(range.getEnd())));

        SampleType yolo = rmsLevels[channel].getCurrentValue();

        return yolo;
    }

    SampleType getPeakLevel(const int channel)
    {
        return juce::Decibels::gainToDecibels(peakLevels[channel].getCurrentValue());
    }

    void processLevelValue(juce::LinearSmoothedValue<SampleType>& smoothedValue, const float value) const
    {
        if (isSmoothed)
        {
            if (value < smoothedValue.getCurrentValue())
            {
                smoothedValue.setTargetValue(value);
                return;
            }
        }
        smoothedValue.setCurrentAndTargetValue(value);
    }

    void pushRMSFifo(juce::AudioBuffer<SampleType>* pBuffer)
    {
        rmsFifo.pull(pBuffer->getWritePointer(0), 0,pBuffer->getNumSamples());
        rmsFifo.pull(pBuffer->getWritePointer(1), 1, pBuffer->getNumSamples());
        //rmsFifo.readSamplesFromFifo(pBuffer, 0, pBuffer->getNumSamples());
        //rmsFifo.readSamplesFromFifo(pBuffer, 1, pBuffer->getNumSamples());
    }

    void pushRMSFifo(juce::AudioBuffer<SampleType>* pBuffer, int nbOfItem)
    {
        rmsFifo.pull(pBuffer->getWritePointer(0), 0, nbOfItem);
        rmsFifo.pull(pBuffer->getWritePointer(1), 1, nbOfItem);
        //rmsFifo.readSamplesFromFifo(pBuffer->getWritePointer(0), 0, nbOfItem);
        //rmsFifo.readSamplesFromFifo(pBuffer->getWritePointer(1), 1, nbOfItem);
    }

    void setIsNormalized(bool newIsNormalized)
    {
        isNormalized = newIsNormalized;
    }

    void setSubViewIsBypassed(bool newSubViewIsBypassed)
    {
        subViewIsBypassed = newSubViewIsBypassed;
    }

    void setSubViewCutoff(SampleType newCutoff)
    {
        for (int filterID = 0; filterID < lowpassFilter.size(); ++filterID)
        {
            IIRBiquadFilter<SampleType>::IIRBiquadParams biquadParams;
            biquadParams.cutoff = newCutoff;
            biquadParams.q = static_cast<SampleType>(0.76);
            biquadParams.gain = static_cast<SampleType>(0.0);
            biquadParams.type = EQConstants::BiquadConstants<float>::Types::lowpass;

            lowpassFilter[filterID]->setFilterCoefficient(biquadParams);
        }
    }

    void setMonoViewIsBypassed(bool newMonoViewIsBypassed)
    {
        monoViewIsBypassed = newMonoViewIsBypassed;
    }

    void setIsBypassed(bool newIsBypassed)
    {
        isBypassed = newIsBypassed;
    }

    static void createParametersLayout(std::vector<std::unique_ptr<juce::RangedAudioParameter>>* plugInParameters, int stageID)
    {
        juce::NormalisableRange<SampleType> normalisableCutoffRange{ RMSConstants::Processor<SampleType>::subViewCutoffMin,
                                            RMSConstants::Processor<SampleType>::subViewCutoffMax,
                                             RMSConstants::Processor<SampleType>::subViewCutoffIncrement,
                                              RMSConstants::Processor<SampleType>::subViewCutoffStartValue }; //20 000Hz to avoid jassert in IIR filter at 44100Hz sr.
        normalisableCutoffRange.setSkewForCentre(RMSConstants::Processor<SampleType>::subViewCutoffStartValue);

        juce::String paramString = RMSConstants::ParamStringID::GetParamStringID::isNormalized(stageID);
        juce::String automationParamString = RMSConstants::AutomationString::GetString::isNormalized(stageID);
        plugInParameters->push_back(std::make_unique<juce::AudioParameterBool>(paramString,
            automationParamString,
            RMSConstants::Processor<SampleType>::isNormalizedStartValue));

        paramString = RMSConstants::ParamStringID::GetParamStringID::subViewIsBypassed(stageID);
        automationParamString = RMSConstants::AutomationString::GetString::subViewIsBypassed(stageID);
        plugInParameters->push_back(std::make_unique<juce::AudioParameterBool>(paramString,
            automationParamString,
            RMSConstants::Processor<SampleType>::subViewIsBypassedStartValue));

        paramString = RMSConstants::ParamStringID::GetParamStringID::subViewCutoff(stageID);
        automationParamString = RMSConstants::AutomationString::GetString::subViewCutoff(stageID);
        plugInParameters->push_back(std::make_unique<juce::AudioParameterFloat>(paramString,
            automationParamString,
            normalisableCutoffRange,
            RMSConstants::Processor<SampleType>::subViewCutoffStartValue));

        paramString = RMSConstants::ParamStringID::GetParamStringID::monoViewIsBypassed(stageID);
        automationParamString = RMSConstants::AutomationString::GetString::monoViewIsBypassed(stageID);
        plugInParameters->push_back(std::make_unique<juce::AudioParameterBool>(paramString,
            automationParamString,
            RMSConstants::Processor<SampleType>::monoViewIsBypassedStartValue));
    };
private:
    juce::OwnedArray<IIRBiquadFilter<SampleType>> lowpassFilter;
    NoReadLockAudioSampleFifo<SampleType> rmsFifo;
    //AudioSampleFifo<SampleType> rmsFifo;

    juce::AudioBuffer<SampleType> rmsCalculationBuffer;
    juce::AudioBuffer<SampleType> temporalCalculationBuffer;
    int rmsWindowSize = 50;
    std::vector<juce::LinearSmoothedValue<SampleType>> rmsLevels;
    std::vector<juce::LinearSmoothedValue<SampleType>> peakLevels;
    bool isSmoothed = false;

    bool subViewIsBypassed = true;
    bool monoViewIsBypassed = true;
    bool isNormalized = true;
    bool isBypassed = false;
};