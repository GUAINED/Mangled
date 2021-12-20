/*
  ==============================================================================

    DCFilter.cpp
    Created: 27 Sep 2021 2:43:10pm
    Author:  CookiMonstor

  ==============================================================================
*/

#include "DCFilter.h"

//==============================================================================
template <typename SampleType>
DCFilter<SampleType>::DCFilter(int nbOfChannels):
    numberOfChannels(nbOfChannels)
{
    //numberOfChannels = nbOfChannels;

    //int maxNumberOfFilter = (int)FilterID::maxFilterID;

    //dcFilterBank.resize(numberOfChannels);

    for (int channel = 0; channel < numberOfChannels; ++channel)
    {
        dcFilterBank.add(new juce::dsp::IIR::Filter<SampleType>());

        //dcFilterBank[channel].coefficients = juce::dsp::IIR::Coefficients<SampleType>::makeHighPass(sampleRate, dcFilterFreq, 0.71f);
        //dcFilterBank[channel].coefficients = new juce::dsp::IIR::Coefficients<SampleType>( 1.0f, -1.0f, 1.0f, -R );
    }

    //dryWet.setMixingRule(juce::dsp::DryWetMixingRule::linear);
}

template <typename SampleType>
void DCFilter<SampleType>::setIsBypassed(const bool newIsBypassed)
{
    //for (int channel = 0; channel < dcFilterBank.size(); ++channel)
    //{
    //    dcFilterBank[channel]->reset();
    //}

    isBypassed = newIsBypassed;
}

template <typename SampleType>
void DCFilter<SampleType>::updateState()
{
    update();
}

//==============================================================================
template <typename SampleType>
void DCFilter<SampleType>::prepare(const juce::dsp::ProcessSpec& spec)
{
    jassert(spec.sampleRate > 0);
    jassert(spec.numChannels > 0);

    sampleRate = spec.sampleRate;
    numberOfChannels = spec.numChannels;
    //dryWet.prepare(spec);

    for (int channelID = 0; channelID < numberOfChannels; ++channelID)
    {
        dcFilterBank[channelID]->coefficients = juce::dsp::IIR::Coefficients<SampleType>::makeHighPass(sampleRate, dcFilterFreq, 0.71f);
    }

    update();
    reset();
}

template <typename SampleType>
void DCFilter<SampleType>::processBuffer(juce::AudioBuffer<float>& buffer)
{
    //Process with audioBuffer========================================================================

    //jassert(inputBlock.getNumChannels() == numChannels);
    ////jassert(inputBlock.getNumChannels() == lastOutput.size());
    //jassert(inputBlock.getNumSamples() == numSamples);

    if (isBypassed)// || context.isBypassed)
    {
        //outputBlock.copyFrom(inputBlock);
        return;
    }

    //dryWet.pushDrySamples(inputBlock);

    SampleType* pInputSample = (SampleType*)buffer.getWritePointer(0, 0);
    SampleType inputSample = (SampleType)*pInputSample;

    for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
    {

        for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
        {
            pInputSample = (SampleType*)buffer.getWritePointer(channel, sample);
            inputSample = (SampleType)*pInputSample;

            inputSample = dcFilterBank[channel]->processSample(inputSample);

            *pInputSample = inputSample;
        }
    }

    //dryWet.mixWetSamples(outputBlock);
}

template <typename SampleType>
void DCFilter<SampleType>::reset()
{

    for (int channelID = 0; channelID < numberOfChannels; ++channelID)
    {
        dcFilterBank[channelID]->reset();
    }

    //dryWet.reset();

}

template <typename SampleType>
void DCFilter<SampleType>::update()
{

}

//==============================================================================
template class DCFilter<float>;
template class DCFilter<double>;