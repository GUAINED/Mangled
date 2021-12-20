/*
  ==============================================================================

    EQProcessor.h
    Created: 5 Apr 2021 8:25:31pm
    Author:  CookiMonstor

  ==============================================================================
*/

#pragma once

/*
  ==============================================================================

    PhaserProcessor.h
    Created: 2 Apr 2021 4:56:59pm
    Author:  CookiMonstor

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "IIRBiquadFilter.h"

//#include <JuceHeader.h>
#include "../../DataStructure/AudioSampleFifo.h"
//#include "../../DataStructure/MangledIdentifiers.h"

template <typename SampleType>
class EQFifo
{
public:
    EQFifo(double sampleRate, int numOfAverages = 8, int fftOrder = 12, int numOfChannels = 2)
        : isBypassed(ScopeConstants::preEQIsBypassedStartValue)
        , sampleRate(sampleRate)
        //, fft(fftOrder)
        //, windowBuffer(1, fft.getSize())
        , inputSampleFifo(numOfChannels, inputSampleFifoSize)
        , fftTimeBuffer(numOfChannels, EQConstants::UI::filterMagnitudeSize) //fft.getSize()
        //, fftFreqBuffer(numOfChannels, fft.getSize() * 2)
        , averageSampleFifo(numOfChannels, 1 + ((EQConstants::UI::filterMagnitudeSize / 2) * numOfAverages))
        //FFT
        , averageBuffer(numOfChannels, ((EQConstants::UI::filterMagnitudeSize / 2)* numOfAverages))
        , resultsBufferFifo(numOfChannels, (EQConstants::UI::filterMagnitudeSize / 2)* resultBufferFifoSize)
        , resultBuffer(numOfChannels, EQConstants::UI::filterMagnitudeSize / 2)
        //
        //averageFFTSampleFifo(numOfChannels, 1 + ((fft.getSize() / 2) * numOfAverages)),
        //averageFFTBuffer(numOfChannels, ((fft.getSize() / 2)* numOfAverages)),
        //resultsFFTBufferFifo(numOfChannels, (fft.getSize() / 2)* resultBufferFifoSize),
        //resultFFTBuffer(numOfChannels, fft.getSize() / 2)
    {
        //juce::dsp::WindowingFunction<SampleType>::fillWindowingTables(windowBuffer.getWritePointer(0), fft.getSize(),
        //    juce::dsp::WindowingFunction<SampleType>::blackman);
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
                ////FFT Data
                //juce::FloatVectorOperations::multiply(fftTimeBuffer.getWritePointer(ch),
                //    windowBuffer.getReadPointer(0),
                //    fft.getSize());

                //fftFreqBuffer.clear();
                //fftFreqBuffer.copyFrom(ch, 0, fftTimeBuffer, ch, 0, fftTimeBuffer.getNumSamples());

                //fft.performFrequencyOnlyForwardTransform(fftFreqBuffer.getWritePointer(ch));

                //resultBuffer.copyFrom(ch, 0, fftFreqBuffer, ch, 0, resultBuffer.getNumSamples());

                switch (dataType)
                {
                case 0:
                    //FFT Data
                    juce::FloatVectorOperations::multiply(fftTimeBuffer.getWritePointer(ch),
                        windowBuffer.getReadPointer(0),
                        fft.getSize());

                    fftFreqBuffer.clear();
                    fftFreqBuffer.copyFrom(ch, 0, fftTimeBuffer, ch, 0, fftTimeBuffer.getNumSamples());

                    fft.performFrequencyOnlyForwardTransform(fftFreqBuffer.getWritePointer(ch));

                    resultBuffer.copyFrom(ch, 0, fftFreqBuffer, ch, 0, resultBuffer.getNumSamples());
                    break;

                case 1:
                    //Temporal Data
                    resultBuffer.copyFrom(ch, 0, fftTimeBuffer, ch, 0, resultBuffer.getNumSamples());
                    break;

                default:
                    //Temporal Data
                    resultBuffer.copyFrom(ch, 0, fftTimeBuffer, ch, 0, resultBuffer.getNumSamples());
                    break;
                }
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
        //if (inputSampleFifo.getFreeSpace() >= inputBlock.getNumSamples())
        //    inputSampleFifo.writeSamplesToFifo(context);

        while (inputSampleFifo.getNumReady() >= EQConstants::UI::filterMagnitudeSize)
        {
            inputSampleFifo.copyWholeBuffer(fftTimeBuffer);
            inputSampleFifo.readSamplesFromFifo(resultBuffer); // Dummy Read
            //inputSampleFifo.readSamplesFromFifo(resultFFTBuffer); // Dummy Read

            for (int ch = 0; ch < fftTimeBuffer.getNumChannels(); ++ch)
            {
                //FFT Data
                //juce::FloatVectorOperations::multiply(fftTimeBuffer.getWritePointer(ch),
                //    windowBuffer.getReadPointer(0),
                //    fft.getSize());

                //fftFreqBuffer.clear();
                //fftFreqBuffer.copyFrom(ch, 0, fftTimeBuffer, ch, 0, fftTimeBuffer.getNumSamples());

                //fft.performFrequencyOnlyForwardTransform(fftFreqBuffer.getWritePointer(ch));

                resultBuffer.copyFrom(ch, 0, fftTimeBuffer, ch, 0, resultBuffer.getNumSamples()); //fftTimeBuffer
            }

            //if (averageSampleFifo.getFreeSpace() >= resultBuffer.getNumSamples())
            //    averageSampleFifo.writeSamplesToFifo(resultBuffer);

            //if (averageSampleFifo.getFreeSpace() == 0)
            //{
            //    const float numOfAverages = (float)averageBuffer.getNumSamples() / (float)resultBuffer.getNumSamples();
            //    const float ratio = 1.f / numOfAverages;

            //    averageSampleFifo.copyWholeBuffer(averageBuffer);
            //    averageSampleFifo.readSamplesFromFifo(resultBuffer); // Dummy Read
            //    averageBuffer.applyGain(ratio);

            //    resultBuffer.clear();
            //    for (int avg = 0; avg < numOfAverages; ++avg)
            //        for (int ch = 0; ch < resultBuffer.getNumChannels(); ++ch)
            //            resultBuffer.addFrom(ch, 0, averageBuffer,
            //                ch, avg * resultBuffer.getNumSamples(),
            //                resultBuffer.getNumSamples());

                writeBufferToResult(resultBuffer);
            //}
        }
    }

    enum
    {
        inputSampleFifoSize = 2 * EQConstants::UI::filterMagnitudeSize,
        resultBufferFifoSize = 4
    };

    void setIsBypassed(bool newIsBypassed)
    {
        isBypassed = newIsBypassed;
        //inputSampleFifo.getAbstractFifo()->reset();
        //averageSampleFifo.getAbstractFifo()->reset();
        //resultsBufferFifo.getAbstractFifo()->reset();
    }

    void setDataTypeScope(const int newDataType) { dataType = newDataType; };

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
    int    getDataType() const noexcept { return dataType; };

private:
    std::atomic<bool> isBypassed;
    std::atomic<double> sampleRate;

    //juce::dsp::FFT fft;
    //juce::AudioBuffer<SampleType> windowBuffer;

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

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(EQFifo)
};

template <typename SampleType>
class EQProcessor
{
public:
    //==============================================================================
    /** Constructor. */
    EQProcessor(int nbOfChannels, double sampleRate);

    //==============================================================================
    struct FilterParamsForUpdate
    {
        SampleType cutoff;
        SampleType q;
        SampleType gain;
        int        type;
        int        nbOfBiquads;
        bool       isBypassed;
        bool       isActive;
        int        id;
    };

    typedef SampleType (*pFilterMagnitude)[EQConstants::UI::filterMagnitudeSize];

    /** Update the state of the Phaser to avoid calling update() each param. This is
    * because we update all the parameters each slider movement

    */
    void updateState();

    //==============================================================================
    /** Initialises the processor. */
    void prepare(const juce::dsp::ProcessSpec& spec);

    /** Resets the internal state variables of the processor. */
    void reset();

    void processBuffer(juce::AudioBuffer<SampleType>& buffer);

    //==============================================================================
    /** Processes the input and output samples supplied in the processing context. */
    //template <typename SampleType>
    template <typename ProcessContext>
    void process(const ProcessContext& context)
    {
        ////Process with Process Context========================================================================
        const auto& inputBlock = context.getInputBlock();
        auto& outputBlock = context.getOutputBlock();
        const auto numChannels = outputBlock.getNumChannels();
        const auto numSamples = outputBlock.getNumSamples();

        jassert(inputBlock.getNumChannels() == numChannels);
        //jassert(inputBlock.getNumChannels() == lastOutput.size());
        jassert(inputBlock.getNumSamples() == numSamples);

        if (isBypassed || context.isBypassed)
        {
            outputBlock.copyFrom(inputBlock);
            return;
        }

        dryWet.pushDrySamples(inputBlock);

        for (size_t channelID = 0; channelID < numChannels; ++channelID)
        {
            auto* inputSamples = inputBlock.getChannelPointer(channelID);
            auto* outputSamples = outputBlock.getChannelPointer(channelID);

            for (size_t sample = 0; sample < numSamples; ++sample)
            {
                auto input = inputSamples[sample];
                auto output = input;

                for (int filterID = 0; filterID < EQConstants::Processor<SampleType>::nbOfFilterMax; ++filterID)
                {
                    output = filterBank[channelID][filterID].processSample(output);
                }

                outputSamples[sample] = output;

            }
        }

        dryWet.mixWetSamples(outputBlock);

    }

    //Set Function ==============================================================================================================================================================
    /** Sets the coefficient of one filter of the eq.
    */
    void setFilterCoefficient(SampleType newFilterCutoffEQ, SampleType newFilterQEQ, SampleType newFilterGainEQ, const int newFilterTypeEQ);

    /** Sets the Bypass of the filter.
    */
    void setFilterOnOff(const bool newFilterOnOff);

    void setMixParam(const SampleType newMix);

    /** Sets the Bypass of the EQ.
    */
    void setIsBypassed(const bool newIsBypassed);

    /** Sets the parameters for a given filterID of the EQ.
    */
    void setEQFilterParams(EQProcessor<SampleType>::FilterParamsForUpdate& newEQFilterParams);

    /** Compute each filter magnitude to display
    */
    void computeFiltersMagnitude(int filterID, bool filterIsActive);

    //Get Function====================================================================================
    juce::AudioBuffer<SampleType>* getFilterMagnitudeBuffer() { return &filterMagnitudeValue; };
    juce::AudioBuffer<SampleType>* getFilterSumMagnitudeBuffer() { return &filterSumMagnitudeValue; };
    //EQFifo<SampleType>* getEQFifo() { return &eqFifo; };
private:
    //==============================================================================
    void update();

    //==============================================================================
    std::vector< std::vector < IIRBiquadFilter<SampleType> > > filterBank;

    //Filter Magnitude For UI.
    juce::AudioBuffer<double> freqForFilterMagnitude;

    juce::AudioBuffer<SampleType> filterMagnitudeValue;
    juce::AudioBuffer<SampleType> filterSumMagnitudeValue;
    //EQFifo<SampleType> eqFifo;
    double sampleRate = 44100.0;
    int numberOfChannels;
    bool isBypassed = false;

    juce::dsp::DryWetMixer<SampleType> dryWet;
};


//template <typename SampleType>
//// copy assignment
//EQProcessor<SampleType>::EQProcessorParam& operator=(const EQProcessor<SampleType>::EQProcessorParam& other)
//{
//    // Guard self assignment
//    if (this == &other)
//        return *this;
//
//    // assume *this manages a reusable resource, such as a heap-allocated buffer mArray
//    if (size != other.size) {         // resource in *this cannot be reused
//        delete[] mArray;              // release resource in *this
//        mArray = nullptr;
//        size = 0;                     // preserve invariants in case next line throws
//        mArray = new int[other.size]; // allocate resource in *this
//        size = other.size;
//    }
//
//    std::copy(other.mArray, other.mArray + other.size, mArray);
//    return *this;
//}
//template <typename SampleType>