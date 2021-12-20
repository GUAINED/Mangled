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
#include "IIRBiquadFilter.h"
class AbstractWriter
{
public:
    AbstractWriter() = default;

    void reset(int capacity, int samplesToRead) noexcept
    {
        jassert(capacity > 0);
        jassert(samplesToRead > 0 && samplesToRead < capacity);
        bufferSize = capacity;
        readSize = samplesToRead;
        writePos = 0;
        writePos.store(0);
    }

    void prepareToWrite(int numToWrite, int& startIndex1, int& blockSize1, int& startIndex2, int& blockSize2) const noexcept
    {
        const int ve = writePos.load();
        const int localReadSize = readSize.load();

        numToWrite = juce::jmin(numToWrite, bufferSize - localReadSize);

        if (numToWrite <= 0)
        {
            startIndex1 = 0;
            startIndex2 = 0;
            blockSize1 = 0;
            blockSize2 = 0;
        }
        else
        {
            startIndex1 = ve;
            startIndex2 = 0;
            blockSize1 = juce::jmin(bufferSize - ve, numToWrite);
            numToWrite -= blockSize1;
            blockSize2 = numToWrite <= 0 ? 0 : juce::jmin(numToWrite, bufferSize - localReadSize);
        }
    }

    void finishedWrite(int numWritten) noexcept
    {
        jassert(numWritten >= 0 && numWritten < bufferSize);

        int newEnd = writePos.load() + numWritten;

        if (newEnd >= bufferSize)
            newEnd -= bufferSize;

        writePos.store(newEnd);
    }

    void prepareToRead(int& startIndex1, int& blockSize1, int& startIndex2, int& blockSize2) const noexcept
    {
        const int localReadSize = readSize.load();

        int vs = writePos.load() - localReadSize;
        if (vs < 0) vs += bufferSize;

        int numWanted = localReadSize;

        startIndex1 = vs;
        startIndex2 = 0;
        blockSize1 = juce::jmin(bufferSize - vs, localReadSize);
        numWanted -= blockSize1;
        blockSize2 = numWanted <= 0 ? 0 : numWanted;
    }

    void setReadSize(int newValue) noexcept
    {
        jassert(newValue > 0 && newValue < bufferSize);
        readSize.store(newValue);
    }
private:
    int bufferSize;

    std::atomic <int> readSize;
    std::atomic <int> writePos;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AbstractWriter)
};

template <typename SampleType>
class RMSFifo
{
public:
    void reset(int numberOfChannels, int capacity)
    {
        fifo.reset(capacity, capacity - 1);
        buffer.clear();
        buffer.setSize(numberOfChannels, capacity, true, false, true);
    }

    template <typename ProcessContext>
    void push(const ProcessContext& context)
    {
        const auto& inputBlock = context.getInputBlock();

        const int numberOfItems = inputBlock.getNumSamples();
        jassert(numberOfItems < buffer.getNumSamples());
        jassert(inputBlock.getNumChannels() == buffer.getNumChannels());

        int start1, size1, start2, size2;
        fifo.prepareToWrite(numberOfItems, start1, size1, start2, size2);

        if (size1 > 0)
            for (auto channel = 0; channel < buffer.getNumChannels(); channel++)
                buffer.copyFrom(channel, start1, inputBlock.getChannelPointer(channel), size1);//channel, 0, size1
        if (size2 > 0)
            for (auto channel = 0; channel < buffer.getNumChannels(); channel++)
                buffer.copyFrom(channel, start2, inputBlock.getChannelPointer(channel), size2);//channel, size1, size2
        fifo.finishedWrite(size1 + size2);
    }

    void push(const juce::AudioBuffer<SampleType>& data)
    {
        const auto numberOfItems = data.getNumSamples();
        jassert(numberOfItems < buffer.getNumSamples());
        jassert(data.getNumChannels() == buffer.getNumChannels());

        int start1, size1, start2, size2;
        fifo.prepareToWrite(numberOfItems, start1, size1, start2, size2);

        if (size1 > 0)
            for (auto channel = 0; channel < buffer.getNumChannels(); channel++)
                buffer.copyFrom(channel, start1, data, channel, 0, size1);
        if (size2 > 0)
            for (auto channel = 0; channel < buffer.getNumChannels(); channel++)
                buffer.copyFrom(channel, start2, data, channel, size1, size2);
        fifo.finishedWrite(size1 + size2);
    }

    void push(const juce::AudioBuffer<SampleType>& data, int numberOfItems)
    {
        //const auto numberOfItems = data.getNumSamples();
        jassert(numberOfItems < buffer.getNumSamples());
        jassert(data.getNumChannels() == buffer.getNumChannels());

        int start1, size1, start2, size2;
        fifo.prepareToWrite(numberOfItems, start1, size1, start2, size2);

        if (size1 > 0)
            for (auto channel = 0; channel < buffer.getNumChannels(); channel++)
                buffer.copyFrom(channel, start1, data, channel, 0, size1);
        if (size2 > 0)
            for (auto channel = 0; channel < buffer.getNumChannels(); channel++)
                buffer.copyFrom(channel, start2, data, channel, size1, size2);
        fifo.finishedWrite(size1 + size2);
    }

    void pull(juce::AudioBuffer<SampleType>& data, int numberOfItems)
    {
        jassert(numberOfItems < buffer.getNumSamples());
        jassert(data.getNumChannels() == buffer.getNumChannels());

        int start1, size1, start2, size2;
        fifo.setReadSize(numberOfItems);
        fifo.prepareToRead(start1, size1, start2, size2);

        if (size1 > 0)
            for (auto channel = 0; channel < buffer.getNumChannels(); channel++)
                data.copyFrom(channel, 0, buffer, channel, start1, size1);
        if (size2 > 0)
            for (auto channel = 0; channel < buffer.getNumChannels(); channel++)
                data.copyFrom(channel, size1, buffer, channel, start2, size2);
    }

    void pull(float* destination, int channel, int numberOfItems)
    {
        jassert(numberOfItems < buffer.getNumSamples());
        jassert(channel >= 0 && channel < buffer.getNumChannels());

        int start1, size1, start2, size2;
        fifo.setReadSize(numberOfItems);
        fifo.prepareToRead(start1, size1, start2, size2);

        const auto* source = buffer.getReadPointer(channel);
        if (size1 > 0)
            std::copy_n(source + start1, size1, destination);
        if (size2 > 0)
            std::copy_n(source + start2, size2, destination + size1);
    }
private:
    AbstractWriter fifo;
    juce::AudioBuffer<SampleType> buffer;
};

template <typename SampleType>
class RMSProcessor
{
public:
    RMSProcessor() //: rmsFifo(2, 44101)
    {
        lowpassFilter.add(new IIRBiquadFilter<SampleType>());
        lowpassFilter.add(new IIRBiquadFilter<SampleType>());
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
        //rmsFifo.setFifoSize(spec.sampleRate + 1);

        rmsWindowSize = static_cast<int> (spec.sampleRate * 50) / 1000;
        rmsCalculationBuffer.clear();
        rmsCalculationBuffer.setSize(spec.numChannels, static_cast<int>(spec.sampleRate) + 1, false, false, true);

        temporalCalculationBuffer.clear();
        temporalCalculationBuffer.setSize(spec.numChannels, spec.maximumBlockSize, false, false, true);
    
        for (int channelID = 0; channelID < lowpassFilter.size(); ++channelID)
        {
            lowpassFilter[channelID]->prepare(spec);
            lowpassFilter[channelID]->setBiquadParam(static_cast<SampleType>(110.0),
                                                    static_cast<SampleType>(0.76),
                                                    static_cast<SampleType>(0.0),
                                                    EQConstants::BiquadConstants<SampleType>::Types::lowpass,
                                                    EQConstants::BiquadConstants<SampleType>::Orders::maxOrders,
                                                    false,
                                                    true);
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

        if (! subViewIsBypassed)
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

        }
        else
        {
            rmsFifo.push(context);
        }

        //rmsFifo.writeSamplesToFifo(context);
        //rmsFifo.readSamplesFromFifo(rmsCalculationBuffer);
    }

    void reset()
    {
        rmsFifo.reset();
    }

    SampleType getRmsLevel(const int channel)
    {
        jassert(channel >= 0 && channel < rmsCalculationBuffer.getNumChannels());
        rmsFifo.pull(rmsCalculationBuffer.getWritePointer(channel), channel, rmsWindowSize);
        processLevelValue(rmsLevels[channel], juce::Decibels::gainToDecibels(rmsCalculationBuffer.getRMSLevel(channel, 0, rmsWindowSize)));
        
        juce::Range<SampleType> range = rmsCalculationBuffer.findMinMax(0, channel, rmsWindowSize);

        peakLevels[channel].setCurrentAndTargetValue(juce::jmax(std::abs(range.getStart()), std::abs(range.getEnd())));

        return rmsLevels[channel].getCurrentValue();
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
    }

    void pushRMSFifo(juce::AudioBuffer<SampleType>* pBuffer, int nbOfItem)
    {
        rmsFifo.pull(pBuffer->getWritePointer(0), 0, nbOfItem);
        rmsFifo.pull(pBuffer->getWritePointer(1), 1, nbOfItem);
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
            lowpassFilter[filterID]->setFilterCoefficient(newCutoff, 
                                                        static_cast<SampleType>(0.76),
                                                        static_cast<SampleType>(0.0), 
                                                        EQConstants::BiquadConstants<float>::Types::lowpass);
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
private:
    juce::OwnedArray<IIRBiquadFilter<SampleType>> lowpassFilter;
    RMSFifo<SampleType> rmsFifo;
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