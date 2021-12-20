/*
  ==============================================================================

    ScopeProcessor.h
    Created: 19 May 2021 4:41:13pm
    Author:  CookiMonstor

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "../../DataStructure/AudioSampleFifo.h"
#include "../../DataStructure/MangledIdentifiers.h"


template <typename SampleType>
class ScopeProcessor
{
public:
    ScopeProcessor(double sampleRate, int numOfAverages = 8, int fftOrder = 12, int numOfChannels = 2)
        : isBypassed(ScopeConstants::Processor<SampleType>::preEQIsBypassedStartValue)
        , sampleRate(sampleRate)
        , fft(fftOrder)
        , windowBuffer(1, fft.getSize())
        , inputSampleFifo(numOfChannels, inputSampleFifoSize)
        , fftTimeBuffer(numOfChannels, fft.getSize())
        , fftFreqBuffer(numOfChannels, fft.getSize() * 2)
        , averageSampleFifo(numOfChannels, 1 + ((fft.getSize() / 2) * numOfAverages))
        //FFT
        , averageBuffer(numOfChannels, ((fft.getSize() / 2)* numOfAverages))
        , resultsBufferFifo(numOfChannels, (fft.getSize() / 2)* resultBufferFifoSize)
        , resultBuffer(numOfChannels, fft.getSize() / 2)
        //, subviewIsBypassed(false)
        //
        //averageFFTSampleFifo(numOfChannels, 1 + ((fft.getSize() / 2) * numOfAverages)),
        //averageFFTBuffer(numOfChannels, ((fft.getSize() / 2)* numOfAverages)),
        //resultsFFTBufferFifo(numOfChannels, (fft.getSize() / 2)* resultBufferFifoSize),
        //resultFFTBuffer(numOfChannels, fft.getSize() / 2)
    {
        juce::dsp::WindowingFunction<SampleType>::fillWindowingTables(windowBuffer.getWritePointer(0), fft.getSize(),
            juce::dsp::WindowingFunction<SampleType>::blackman);


    }

    void prepare(const juce::dsp::ProcessSpec& spec)
    {
        jassert(spec.sampleRate > 0);
        jassert(spec.numChannels > 0);

        sampleRate = spec.sampleRate;


    }

    void reset()
    {
        inputSampleFifo.reset();
        resultsBufferFifo.reset();
        fftTimeBuffer.clear();
        fftFreqBuffer.clear();
        averageBuffer.clear();
        resultBuffer.clear();
    }

    void process(const juce::AudioBuffer<SampleType>& buffer)
    {
        if (isBypassed)
            return;

        if (inputSampleFifo.getFreeSpace() >= buffer.getNumSamples())
            inputSampleFifo.writeSamplesToFifo(buffer);

        while (inputSampleFifo.getNumReady() >= fft.getSize())
        {
            inputSampleFifo.copyWholeBuffer(fftTimeBuffer);
            inputSampleFifo.readSamplesFromFifo(resultBuffer); // Dummy Read
            //inputSampleFifo.readSamplesFromFifo(resultFFTBuffer); // Dummy Read

            for (int ch = 0; ch < fftTimeBuffer.getNumChannels(); ++ch)
            {
                //FFT Data
                juce::FloatVectorOperations::multiply(fftTimeBuffer.getWritePointer(ch),
                    windowBuffer.getReadPointer(0),
                    fft.getSize());

                fftFreqBuffer.clear();
                fftFreqBuffer.copyFrom(ch, 0, fftTimeBuffer, ch, 0, fftTimeBuffer.getNumSamples());

                fft.performFrequencyOnlyForwardTransform(fftFreqBuffer.getWritePointer(ch));

                resultBuffer.copyFrom(ch, 0, fftFreqBuffer, ch, 0, resultBuffer.getNumSamples());
            }

            if (averageSampleFifo.getFreeSpace() >= resultBuffer.getNumSamples())
                averageSampleFifo.writeSamplesToFifo(resultBuffer);

            if (averageSampleFifo.getFreeSpace() == 0)
            {
                const float numOfAverages = (float)averageBuffer.getNumSamples() / (float)resultBuffer.getNumSamples();
                const float ratio = 1.f / numOfAverages;

                averageSampleFifo.copyWholeBuffer(averageBuffer);
                averageSampleFifo.readSamplesFromFifo(resultBuffer); // Dummy Read
                averageBuffer.applyGain(ratio);

                resultBuffer.clear();
                for (int avg = 0; avg < numOfAverages; ++avg)
                    for (int ch = 0; ch < resultBuffer.getNumChannels(); ++ch)
                        resultBuffer.addFrom(ch, 0, averageBuffer,
                            ch, avg * resultBuffer.getNumSamples(),
                            resultBuffer.getNumSamples());

                writeBufferToResult(resultBuffer);
            }
        }
    }

    template <typename ProcessContext>
    void processBlock(const ProcessContext& context)
    {
        const auto& inputBlock = context.getInputBlock();
        auto& outputBlock = context.getOutputBlock();
        const auto numChannels = outputBlock.getNumChannels();
        const auto numSamples = outputBlock.getNumSamples();
        //const juce::dsp::AudioBlock<float> inputBlock;

        //inputBlock.copyTo(buffer);

        if (isBypassed || context.isBypassed)
        {
            return;
        }

        if (inputSampleFifo.getFreeSpace() >= inputBlock.getNumSamples())
            inputSampleFifo.writeSamplesToFifo(context);

        //Make the animation much smoother. No idea why.
        if (inputSampleFifo.getFreeSpace() >= inputBlock.getNumSamples())
            inputSampleFifo.writeSamplesToFifo(context);

        while (inputSampleFifo.getNumReady() >= fft.getSize())
        {
            inputSampleFifo.copyWholeBuffer(fftTimeBuffer);
            inputSampleFifo.readSamplesFromFifo(resultBuffer); // Dummy Read
            //inputSampleFifo.readSamplesFromFifo(resultFFTBuffer); // Dummy Read

            for (int ch = 0; ch < fftTimeBuffer.getNumChannels(); ++ch)
            {


                //FFT Data
                juce::FloatVectorOperations::multiply(fftTimeBuffer.getWritePointer(ch),
                    windowBuffer.getReadPointer(0),
                    fft.getSize());

                fftFreqBuffer.clear();
                fftFreqBuffer.copyFrom(ch, 0, fftTimeBuffer, ch, 0, fftTimeBuffer.getNumSamples());

                fft.performFrequencyOnlyForwardTransform(fftFreqBuffer.getWritePointer(ch));

                resultBuffer.copyFrom(ch, 0, fftFreqBuffer, ch, 0, resultBuffer.getNumSamples());
            }

            if (averageSampleFifo.getFreeSpace() >= resultBuffer.getNumSamples())
                averageSampleFifo.writeSamplesToFifo(resultBuffer);

            if (averageSampleFifo.getFreeSpace() == 0)
            {
                const float numOfAverages = (float)averageBuffer.getNumSamples() / (float)resultBuffer.getNumSamples();
                const float ratio = 1.f / numOfAverages;

                averageSampleFifo.copyWholeBuffer(averageBuffer);
                averageSampleFifo.readSamplesFromFifo(resultBuffer); // Dummy Read
                averageBuffer.applyGain(ratio);

                resultBuffer.clear();
                for (int avg = 0; avg < numOfAverages; ++avg)
                    for (int ch = 0; ch < resultBuffer.getNumChannels(); ++ch)
                        resultBuffer.addFrom(ch, 0, averageBuffer,
                            ch, avg * resultBuffer.getNumSamples(),
                            resultBuffer.getNumSamples());

                writeBufferToResult(resultBuffer);
            }
        }
    }

    enum
    {
        inputSampleFifoSize = 2*4096,
        resultBufferFifoSize = 32
    };

    //void setLowpassFilterBypass(const bool newLowpassIsBypassed)
    //{
    //    subviewIsBypassed = newLowpassIsBypassed;
    //}

    void setIsBypassed(bool newIsBypassed) 
    { 
        isBypassed = newIsBypassed; 
        //inputSampleFifo.getAbstractFifo()->reset();
        //averageSampleFifo.getAbstractFifo()->reset();
        //resultsBufferFifo.getAbstractFifo()->reset();
    }

    //void setDataTypeScope(const int newDataType) { dataType = newDataType; };

    int readBufferFromResult(juce::AudioBuffer<SampleType>& buffer) 
    {
        juce::SpinLock::ScopedLockType scopedLock(spinLock);
        return resultsBufferFifo.readSamplesFromFifo(buffer);
    }

    bool   getIsBypassedd() const noexcept { return isBypassed; };
    int    getNumChannels() const noexcept { return resultBuffer.getNumChannels(); };
    int    getNumSamples()  const noexcept { return resultBuffer.getNumSamples(); };
    int    getFFTSize()     const noexcept { return fft.getSize(); };
    double getSampleRate()  const noexcept { return sampleRate; };
    //int    getDataType() const noexcept { return dataType; };

private:
    std::atomic<bool> isBypassed;
    std::atomic<double> sampleRate;



    juce::dsp::FFT fft;
    juce::AudioBuffer<SampleType> windowBuffer;

    AudioSampleFifo<SampleType> inputSampleFifo;
    juce::AudioBuffer<SampleType> fftTimeBuffer;
    juce::AudioBuffer<SampleType> fftFreqBuffer;
    
    //Temporal
    AudioSampleFifo<SampleType> averageSampleFifo;
    juce::AudioBuffer<SampleType> averageBuffer;

    AudioSampleFifo<SampleType> resultsBufferFifo;
    juce::AudioBuffer<SampleType> resultBuffer;

    //FFT
    int dataType = 0;
    //AudioSampleFifo<float> averageFFTSampleFifo;
    //juce::AudioBuffer<float> averageFFTBuffer;

    //AudioSampleFifo<float> resultsFFTBufferFifo;
    //juce::AudioBuffer<float> resultFFTBuffer;

    juce::SpinLock spinLock;

    int writeBufferToResult(const juce::AudioBuffer<SampleType>& buffer)
    {
        juce::SpinLock::ScopedLockType scopedLock(spinLock);
        return resultsBufferFifo.writeSamplesToFifo(buffer);
    }

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ScopeProcessor)
};
