/*
  ==============================================================================

    AudioSampleFifo.h
    Created: 19 May 2021 4:53:22pm
    Author:  CookiMonstor

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

template <typename SampleType>
class AudioSampleFifo
{
public:
    AudioSampleFifo(int nbOfChannelsInit = 2, int abstractFifoSize = 1024) : abstractFifo(abstractFifoSize)
                                                                           , nbOfChannels(nbOfChannelsInit)
    {
        jassert(abstractFifoSize > 0);
        jassert(nbOfChannels > 0);

        fifoBuffer.setSize(nbOfChannels, abstractFifo.getTotalSize(), false, false, true);
        fifoBuffer.clear();
    }

    int writeSamplesToFifo(const juce::AudioBuffer<SampleType>& buffer)
    {
        jassert(buffer.getNumChannels() == nbOfChannels);

        if (buffer.getNumSamples() > abstractFifo.getFreeSpace())
            return 0;

        int start1;
        int size1;
        int start2;
        int size2;

        abstractFifo.prepareToWrite(buffer.getNumSamples(), start1, size1, start2, size2);

        if (size1 > 0)
        {
            for (int channelID = 0; channelID < nbOfChannels; ++channelID)
            {
                fifoBuffer.copyFrom(channelID, start1, buffer, channelID, 0, size1);
            } 
        }

        if (size2 > 0)
        {
            for (int channelID = 0; channelID < nbOfChannels; ++channelID)
            {
                fifoBuffer.copyFrom(channelID, start2, buffer, channelID, size1, size2);
            }
        }

        abstractFifo.finishedWrite(size1 + size2);

        return size1 + size2;
    }

    template <typename ProcessContext>
    int writeSamplesToFifo(const ProcessContext& context)
    {
        int returnValue;
        const auto& inputBlock = context.getInputBlock();

        jassert(inputBlock.getNumChannels() == nbOfChannels);

        int yolo = inputBlock.getNumSamples();
        int yolo2 = abstractFifo.getFreeSpace();

        if (inputBlock.getNumSamples() > abstractFifo.getFreeSpace())
        {
            returnValue = 0;
        }
        else
        {
            int start1;
            int size1;
            int start2;
            int size2;

            abstractFifo.prepareToWrite(static_cast<int>(inputBlock.getNumSamples()), start1, size1, start2, size2);

            if (size1 > 0)
            {
                for (int channelID = 0; channelID < nbOfChannels; ++channelID)
                {
                    fifoBuffer.copyFrom(channelID, start1, inputBlock.getChannelPointer(channelID), size1);
                }  
            }


            if (size2 > 0)
            {
                for (int channelID = 0; channelID < nbOfChannels; ++channelID)
                {
                    fifoBuffer.copyFrom(channelID, start2, inputBlock.getChannelPointer(channelID), size2);
                } 
            }

            abstractFifo.finishedWrite(size1 + size2);

            returnValue = size1 + size2;
        }

        return returnValue;
    }

    int readSamplesFromFifo(juce::AudioBuffer<SampleType>& buffer)
    {
        jassert(buffer.getNumChannels() == nbOfChannels);

        int returnValue;

        if (abstractFifo.getNumReady() < buffer.getNumSamples())
        {
            returnValue = 0;
        }
        else
        {
            int start1;
            int size1;
            int start2;
            int size2;

            abstractFifo.prepareToRead(buffer.getNumSamples(), start1, size1, start2, size2);

            if (size1 > 0)
            {
                for (int channelID = 0; channelID < nbOfChannels; ++channelID)
                {
                    buffer.copyFrom(channelID, 0, fifoBuffer, channelID, start1, size1);
                }
            }


            if (size2 > 0)
            {
                for (int channelID = 0; channelID < nbOfChannels; ++channelID)
                {
                    buffer.copyFrom(channelID, size1, fifoBuffer, channelID, start2, size2);
                }    
            }

            abstractFifo.finishedRead(size1 + size2);

            returnValue = size1 + size2;
        }


        return returnValue;
    }

    int readSamplesFromFifo(SampleType* destination, int channel, int nbOfSamplesToRead)
    {
        //jassert(numberOfItems < buffer.getNumSamples());
        //jassert(channel >= 0 && channel < buffer.getNumChannels());

        int start1, size1, start2, size2;
        //fifo.setReadSize(numberOfItems);
        //fifo.prepareToRead(start1, size1, start2, size2);

        abstractFifo.prepareToRead(nbOfSamplesToRead, start1, size1, start2, size2);

        const auto* source = fifoBuffer.getReadPointer(channel);
        if (size1 > 0)
            std::copy_n(source + start1, size1, destination);
        if (size2 > 0)
            std::copy_n(source + start2, size2, destination + size1);
        //jassert(buffer->getNumChannels() == nbOfChannels);

        //int returnValue;

        //if (abstractFifo.getNumReady() < buffer->getNumSamples())
        //{
        //    returnValue = 0;
        //}
        //else
        //{
        //    int start1;
        //    int size1;
        //    int start2;
        //    int size2;

        //    abstractFifo.prepareToRead(buffer->getNumSamples(), start1, size1, start2, size2);

        //    if (size1 > 0)
        //    {
        //        //for (int channelID = 0; channelID < nbOfChannels; ++channelID)
        //        {
        //            buffer->copyFrom(channel, 0, fifoBuffer, channel, start1, size1);
        //        }
        //    }

        //    if (size2 > 0)
        //    {
        //        //for (int channelID = 0; channelID < nbOfChannels; ++channelID)
        //        {
        //            buffer->copyFrom(channel, size1, fifoBuffer, channel, start2, size2);
        //        }
        //    }

        //    abstractFifo.finishedRead(size1 + size2);

        //    returnValue = size1 + size2;
        //}

        //return returnValue;
        return 0;
    }

    int readSamplesFromFifo(juce::AudioBuffer<SampleType>* buffer)
    {
        jassert(buffer->getNumChannels() == nbOfChannels);

        int returnValue;

        if (abstractFifo.getNumReady() < buffer->getNumSamples())
        {
            returnValue = 0;
        }
        else
        {
            int start1;
            int size1;
            int start2;
            int size2;

            abstractFifo.prepareToRead(buffer->getNumSamples(), start1, size1, start2, size2);

            if (size1 > 0)
            {
                for (int channelID = 0; channelID < nbOfChannels; ++channelID)
                {
                    buffer->copyFrom(channelID, 0, fifoBuffer, channelID, start1, size1);
                }
            }


            if (size2 > 0)
            {
                for (int channelID = 0; channelID < nbOfChannels; ++channelID)
                {
                    buffer->copyFrom(channelID, size1, fifoBuffer, channelID, start2, size2);
                }
            }

            abstractFifo.finishedRead(size1 + size2);

            returnValue = size1 + size2;
        }

        return returnValue;
    }

    void setFifoSize(int newSize)
    {
        jassert(newSize > 0);
        abstractFifo.setTotalSize(newSize);
        fifoBuffer.setSize(nbOfChannels, abstractFifo.getTotalSize(), false, false, true);
    }

    int getFifoSize() const noexcept
    {
        return abstractFifo.getTotalSize();
    }

    void setNumOfChannels(int newNumOfChannels)
    {
        jassert(newNumOfChannels > 0);
        nbOfChannels = newNumOfChannels;
        fifoBuffer.setSize(nbOfChannels, abstractFifo.getTotalSize(), false, false, true);
    }

    int getNumOfChannels() const noexcept
    {
        return channels;
    }

    int getFreeSpace() const noexcept
    {
        return abstractFifo.getFreeSpace();
    }

    int getNumReady() const noexcept
    {
        return abstractFifo.getNumReady();
    }

    void reset()
    {
        fifoBuffer.clear();
        abstractFifo.reset();
    }

    void copyWholeBuffer(juce::AudioBuffer<SampleType>& buffer)
    {
        jassert(buffer.getNumChannels() == nbOfChannels);

        int start1;
        int size1;
        int start2;
        int size2;

        abstractFifo.prepareToRead(buffer.getNumSamples(), start1, size1, start2, size2);

        if (size1 > 0)
        {
            for (int channelID = 0; channelID < nbOfChannels; ++channelID)
                buffer.copyFrom(channelID, 0, fifoBuffer, channelID, start1, size1);
        }

        if (size2 > 0)
        {
            for (int channelID = 0; channelID < nbOfChannels; ++channelID)
            {
                buffer.copyFrom(channelID, size1, fifoBuffer, channelID, start2, size2);
            }
        }

        abstractFifo.finishedRead(0);
    }

    void copyWholeBuffer(juce::AudioBuffer<SampleType>& buffer, int numSampleToCopy)
    {
        jassert(buffer.getNumChannels() == nbOfChannels);

        int start1;
        int size1;
        int start2;
        int size2;

        abstractFifo.prepareToRead(numSampleToCopy, start1, size1, start2, size2);

        if (size1 > 0)
        {
            for (int channelID = 0; channelID < nbOfChannels; ++channelID)
                buffer.copyFrom(channelID, 0, fifoBuffer, channelID, start1, size1);
        }

        if (size2 > 0)
        {
            for (int channelID = 0; channelID < nbOfChannels; ++channelID)
            {
                buffer.copyFrom(channelID, size1, fifoBuffer, channelID, start2, size2);
            }
        }

        //abstractFifo.finishedRead(0);
    }

    juce::AbstractFifo* getAbstractFifo() { return &abstractFifo; };

private:
    juce::AbstractFifo abstractFifo;
    juce::AudioBuffer<SampleType> fifoBuffer;

    int nbOfChannels;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioSampleFifo)
};
