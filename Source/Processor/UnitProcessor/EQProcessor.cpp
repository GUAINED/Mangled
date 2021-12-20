/*
  ==============================================================================

    EQProcessor.cpp
    Created: 5 Apr 2021 8:25:31pm
    Author:  CookiMonstor

  ==============================================================================
*/

#include "EQProcessor.h"

//==============================================================================
template <typename SampleType>
EQProcessor<SampleType>::EQProcessor(int nbOfChannels = 2, double sampleRate = 44100.0)
    : numberOfChannels(nbOfChannels)
    , sampleRate(sampleRate)
    , freqForFilterMagnitude(1, EQConstants::UI::filterMagnitudeSize)
    , filterMagnitudeValue(EQConstants::Processor<SampleType>::nbOfFilterMax, EQConstants::UI::filterMagnitudeSize)
    , filterSumMagnitudeValue(1, EQConstants::UI::filterMagnitudeSize)
    //, eqFifo(sampleRate, 8 , 12, 1)//, 8, 12, 2)
{
    filterBank.resize(numberOfChannels);

    for (int channel = 0; channel < numberOfChannels; ++channel)
    {
        filterBank[channel].resize(EQConstants::Processor<SampleType>::nbOfFilterMax);
    }

    int counter = 0;
    double fInc = (1.0 - 0.0) / (double)(EQConstants::UI::nbOfPointsFirstBloc + 1);
    double min = 0.0;
    double max = 10.0;
    double freqC = fInc;

    for (int i = 0; i < EQConstants::UI::nbOfPointsFirstBloc; ++i)
    {
        double freq = juce::jmap(freqC,0.0,1.0, min, max);
        freqForFilterMagnitude.getWritePointer(0)[counter] = freq;
        freqC += fInc;
        counter++;
    }

    fInc = (1.0 - 0.0) / (double)EQConstants::UI::nbOfPointsPerBloc;

    for (int bloc = 0; bloc < EQConstants::UI::nbOfBloc - 1; ++bloc)
    {
        min = std::pow(10.0, (double)(bloc + 1));
        max = std::pow(10.0, (double)(bloc + 2));
        
        freqC = 0.0;

        for (int i = 0; i < EQConstants::UI::nbOfPointsPerBloc; ++i)
        {
            double freq = juce::mapToLog10(freqC, min, max);
            freqForFilterMagnitude.getWritePointer(0)[counter] = freq;
            freqC += fInc;
            counter++;
        }
    }

    min = 10000.0;
    max = 22050.0;
    freqC = 0.0;

    for (int i = 0; i < EQConstants::UI::nbOfPointsPerBloc; ++i)
    {
        double freq = juce::mapToLog10(freqC, min, max);
        freqC += fInc;
        freqForFilterMagnitude.getWritePointer(0)[counter] = freq;
        counter++;
    }

    filterMagnitudeValue.clear();
    filterSumMagnitudeValue.clear();

    dryWet.setMixingRule(juce::dsp::DryWetMixingRule::linear);
}

template <typename SampleType>
void EQProcessor<SampleType>::setFilterOnOff(const bool newFilterOnOff)
{
    //eqParams.filterParams[currentFilterID].filterOnOffEQ = newFilterOnOff;
}

template <typename SampleType>
void EQProcessor<SampleType>::setMixParam(const SampleType newMix)
{
    dryWet.setWetMixProportion(newMix / static_cast<SampleType>(100.0));
}

template <typename SampleType>
void EQProcessor<SampleType>::setIsBypassed(const bool newIsBypassed)
{
    isBypassed = newIsBypassed;
}

template <typename SampleType>
void EQProcessor<SampleType>::updateState()
{
    update();
}

//==============================================================================
template <typename SampleType>
void EQProcessor<SampleType>::prepare(const juce::dsp::ProcessSpec& spec)
{
    jassert(spec.sampleRate > 0);
    jassert(spec.numChannels > 0);

    numberOfChannels = (int) spec.numChannels;
    sampleRate = spec.sampleRate;

    dryWet.prepare(spec);

    for (int channelID = 0; channelID < numberOfChannels; ++channelID)
    {
        for (int filterID = 0; filterID < EQConstants::Processor<SampleType>::nbOfFilterMax; ++filterID)
        {
            filterBank[channelID][filterID].prepare(spec);
        }
    }

    //double fInc = sampleRate * 0.5 / (double) EQConstants::UI::filterMagnitudeSize;

    //double fInc = (1.0 - 0.0) / 500.0;
    //int counter = 0;
    //for (int bloc = 0; bloc < 3; ++bloc)
    //{
    //    double min = std::pow(10.0, (double)(bloc + 1));
    //    double max = std::pow(10.0, (double)(bloc + 2));

    //    double freqC = 0.0;
    //    for (int i = 0; i < 500; ++i)
    //    {
    //        //double freqNorm = juce::jmap(freqC, min, max, 0.0, 1.0);
    //        double freq = juce::mapToLog10(freqC, min, max);
    //        freqC += fInc;
    //        freqForFilterMagnitude[counter] = freq;
    //        counter++;
    //    }
    //}

    update();
    reset();
}

template <typename SampleType>
void EQProcessor<SampleType>::processBuffer(juce::AudioBuffer<SampleType>& buffer)
{
    //Process with audioBuffer========================================================================

    //jassert(inputBlock.getNumChannels() == numChannels);
    //jassert(inputBlock.getNumChannels() == lastOutput.size());
    //jassert(inputBlock.getNumSamples() == numSamples);

    if (isBypassed)// || context.isBypassed)
    {
        //outputBlock.copyFrom(inputBlock);
        return;
    }

    //dryWet.pushDrySamples(inputBlock);

    SampleType* pInputSample = buffer.getWritePointer(0, 0);
    SampleType inputSample = *pInputSample;
    SampleType inputSampleSave = inputSample;

    for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
    {

        for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
        {
            pInputSample =  buffer.getWritePointer(channel, sample);
            inputSample = *pInputSample;
            inputSampleSave = inputSample;

            for (int filter = 0; filter < EQConstants::Processor<SampleType>::nbOfFilterMax; ++filter)
            {
                inputSample = filterBank[channel][filter].processSample(inputSample);
            }

            *pInputSample = inputSample;// *mix + inputSampleSave * ((SampleType)1.0 - mix);
        }
    }

    //dryWet.mixWetSamples(outputBlock);
}

template <typename SampleType>
void EQProcessor<SampleType>::reset()
{
    dryWet.reset();

    //for (int channelID = 0; channelID < numberOfChannels; ++channelID)
    //{
    //    for (int filterID = 0; filterID < filterBank[channelID].size(); ++filterID)
    //    {
    //        filterBank[channelID][filterID].reset();
    //    }
    //}

}

template <typename SampleType>
void EQProcessor<SampleType>::update()
{

}

template <typename SampleType>
void EQProcessor<SampleType>::setEQFilterParams(EQProcessor<SampleType>::FilterParamsForUpdate& newEQFilterParams)
{
    //Order ===================================================================================================================================
    int filterID = newEQFilterParams.id;
    int nbOfBiquads = newEQFilterParams.nbOfBiquads + 1;
    SampleType gainLin = juce::Decibels::decibelsToGain(newEQFilterParams.gain / nbOfBiquads);
    SampleType cutoff = newEQFilterParams.cutoff;
    SampleType q = newEQFilterParams.q;

    for (int channelID = 0; channelID < numberOfChannels; ++channelID)
    {
        filterBank[channelID][newEQFilterParams.id].setBiquadParam(cutoff, q, gainLin, newEQFilterParams.type, newEQFilterParams.nbOfBiquads + 1 ,newEQFilterParams.isBypassed, newEQFilterParams.isActive);
    }

    computeFiltersMagnitude(newEQFilterParams.id, newEQFilterParams.isActive);
}

template <typename SampleType>
void EQProcessor<SampleType>::computeFiltersMagnitude(int filterID, bool filterIsActive)
{
    double freq = 0.0;
    SampleType filterMagnitudeSumdB = static_cast<SampleType>(0.0);

    if (filterIsActive)
    {
        for (int i = 0; i < EQConstants::UI::filterMagnitudeSize; ++i)
        {
            freq = freqForFilterMagnitude.getReadPointer(0)[i];

            filterMagnitudeSumdB = filterBank[0][filterID].getFilterMagnitude(freq);

            filterSumMagnitudeValue.getWritePointer(0)[i] += filterMagnitudeSumdB - filterMagnitudeValue.getWritePointer(filterID)[i];

            filterMagnitudeValue.getWritePointer(filterID)[i] = filterMagnitudeSumdB;
        }
    }
    else
    {
        for (int i = 0; i < EQConstants::UI::filterMagnitudeSize; ++i)
        {
            filterSumMagnitudeValue.getWritePointer(0)[i] -= filterMagnitudeValue.getWritePointer(filterID)[i];

            filterMagnitudeValue.getWritePointer(filterID)[i] = 0.0f;
        }
    }

    //juce::dsp::AudioBlock<SampleType> block(filterSumMagnitudeValue);
    //juce::dsp::ProcessContextReplacing<SampleType> context(block);

    //eqFifo.processBlock(context);
}
//==============================================================================
template class EQProcessor<float>;
template class EQProcessor<double>;