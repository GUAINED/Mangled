/*
  ==============================================================================

    PhaseShifterFilter.h
    Created: 3 Nov 2021 3:02:05pm
    Author:  CookiMonstor

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "../../../DataStructure/MangledIdentifiers.h"
template <typename SampleType>
class PhaseShifterFilter
{
public:
    PhaseShifterFilter()
        : nbOfFilters(2)
    {
        //nbOfFilters = 2;
        for (auto filterID = 0; filterID < nbOfFilters; ++filterID)
        {
            allPassFilters.add(new juce::dsp::FirstOrderTPTFilter<SampleType>());
            allPassFilters[filterID]->setType(juce::dsp::FirstOrderTPTFilterType::allpass);
        }

        cutoff.setTargetValue(static_cast<SampleType>(1000.0));
    }

    void update()
    {

    }

    /** Initialises the processor. */
    void prepare(const juce::dsp::ProcessSpec& spec)
    {
        jassert(spec.sampleRate > 0);
        jassert(spec.numChannels > 0);

        sampleRate = spec.sampleRate;
        rampDurationSeconds = spec.maximumBlockSize / sampleRate;
        for (auto filterID = 0; filterID < nbOfFilters; ++filterID)
        {
            allPassFilters[filterID]->prepare(spec);
        }

        update();
        reset();
    }

    /** Resets the internal state variables of the processor. */
    void reset()
    {
        cutoff.reset(sampleRate, rampDurationSeconds);
        for (auto filterID = 0; filterID < nbOfFilters; ++filterID)
        {
            allPassFilters[filterID]->reset();
        }
    }

    //==============================================================================
    SampleType processSample(int channelID, SampleType sample)
    {
        SampleType output = sample;

        for (int filterID = 0; filterID < allPassFilters.size(); ++filterID)
        {
            jassert(filterID < allPassFilters.size());
            //SampleType yolo = allPassFilters[filterID]->processSample(channelID, output);
            jassert(channelID < 2);
            output = allPassFilters[filterID]->processSample(channelID, output);
        }

        return output;
    }

    void setCutoffFrequency(SampleType frequency)
    {
        for (int filterID = 0; filterID < nbOfFilters; ++filterID)
        {
            allPassFilters[filterID]->setCutoffFrequency(frequency);
        }

        //cutoff.setTargetValue(frequency);
        //SampleType f = std::sqrt(frequency) / static_cast<SampleType>(2.0);
        //SampleType g = std::sqrt(frequency) * static_cast<SampleType>(2.0);
        //allPassFilters[0]->setCutoffFrequency(f);
        //allPassFilters[1]->setCutoffFrequency(g);
    }

    void setNextFrequency()
    {
        //SampleType newCutoff = cutoff.getNextValue();

        //for (int filterID = 0; filterID < nbOfFilters; ++filterID)
        //{
        //    allPassFilters[filterID]->setCutoffFrequency(newCutoff);
        //}
    }

    void smoothedValuesSkip(int numSamplesToSkip)
    {
        cutoff.skip(numSamplesToSkip);
    }

private:
    int nbOfFilters;
    juce::OwnedArray<juce::dsp::FirstOrderTPTFilter<SampleType>> allPassFilters;
    juce::SmoothedValue<SampleType, juce::ValueSmoothingTypes::Multiplicative> cutoff;
    double sampleRate = 44100.0;
    double rampDurationSeconds = 0.0;
};