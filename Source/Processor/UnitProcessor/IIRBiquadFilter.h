/*
  ==============================================================================

    IIRBiquadFilter.h
    Created: 23 Oct 2021 10:55:53am
    Author:  CookiMonstor

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "../../DataStructure/MangledIdentifiers.h"

template <typename SampleType>
class IIRBiquadFilter
{
public:
    IIRBiquadFilter()
    {
        for (int biquadID = 0; biquadID < EQConstants::BiquadConstants<SampleType>::Orders::maxOrders; ++biquadID)
        {
            filter.add(new juce::dsp::IIR::Filter<SampleType>());
        }
    }

    IIRBiquadFilter(SampleType initFreq)
        : startingFreq(initFreq)
    {
        for (int biquadID = 0; biquadID < EQConstants::BiquadConstants::orderMaxValue; ++biquadID)
        {
            filter.add(new juce::dsp::IIR::Filter<SampleType>());
        }
    };

    //~IIRBiquadFilter();

    struct IIRBiquadParams
    {
        SampleType cutoff =  EQConstants::BiquadConstants<SampleType>::cutoffStartValue;
        SampleType q = EQConstants::BiquadConstants<SampleType>::qStartValue;
        SampleType gain = EQConstants::BiquadConstants<SampleType>::gainLinStartValue;
        int        type = EQConstants::BiquadConstants<SampleType>::typeStartValue;
    };

    struct FilterParams
    {
        IIRBiquadParams biquadParams;
        int          nbOfBiquads = EQConstants::BiquadConstants<SampleType>::orderStartValue;
        bool         isBypassed = EQConstants::BiquadConstants<SampleType>::isBypassedStartValue;
        bool         isActive = EQConstants::BiquadConstants<SampleType>::isActiveStartValue;
        int          id = 0;
    };

    void prepare(const juce::dsp::ProcessSpec& spec)
    {
        jassert(spec.sampleRate > 0);
        jassert(spec.numChannels > 0);

        sampleRate = spec.sampleRate;

        for (int biquadID = 0; biquadID < EQConstants::BiquadConstants<SampleType>::Orders::maxOrders; ++biquadID)
        {
            filter[biquadID]->prepare(spec);
        }

        reset();
    };

    void reset()
    {
        for (int biquadID = 0; biquadID < EQConstants::BiquadConstants<SampleType>::Orders::maxOrders; ++biquadID)
        {
            filter[biquadID]->reset();
            filter[biquadID]->coefficients = juce::dsp::IIR::Coefficients<SampleType>::makePeakFilter(sampleRate,
                startingFreq,
                EQConstants::BiquadConstants<SampleType>::gainLinStartValue,
                EQConstants::BiquadConstants<SampleType>::qStartValue);
        }

    };

    SampleType processSample(SampleType sample)
    {
        auto output = sample;
        
        if ((! filterParams.isBypassed) && filterParams.isActive)
        {
            for (int biquadID = 0; biquadID < filterParams.nbOfBiquads; ++biquadID)
            {
                output = filter[biquadID]->processSample(output);
            }
        }

        return output;
    };

    void setBiquadParam(const SampleType newFilterCutoff, const SampleType newFilterQ, const SampleType newFilterGain, const int newFilterType, const int newNbOfBiquad, const bool newIsBypassed, const bool newIsActive)
    {
        filterParams.biquadParams.cutoff = newFilterCutoff;
        filterParams.biquadParams.q = newFilterQ;
        filterParams.biquadParams.gain = newFilterGain;
        filterParams.biquadParams.type = newFilterType;
        filterParams.nbOfBiquads = newNbOfBiquad;
        filterParams.isBypassed = newIsBypassed;
        filterParams.isActive = newIsActive;

        setFilterCoefficient(newFilterCutoff, newFilterQ, newFilterGain, newFilterType);
    }

    void setOnOff(const bool newOnOff)
    {
        filterParams.onOff = newOnOff;
    }

    void setIsActive(const bool newIsActive)
    {
        filterParams.isActive = newIsActive;
    }

    void setFilterCoefficient(SampleType cutoff, SampleType q, SampleType gainLin, const int newFilterType)
    {
        jassert(cutoff >= (SampleType) 10.0);
        jassert(cutoff < static_cast<SampleType> (sampleRate / 2.0));

        switch (newFilterType)
        {
        case EQConstants::BiquadConstants<SampleType>::Types::peak : //Peak
            filter[0]->coefficients = juce::dsp::IIR::Coefficients<SampleType>::makePeakFilter(sampleRate,
                cutoff,
                q,
                gainLin);
            break;

        case EQConstants::BiquadConstants<SampleType>::Types::lowpass : //Low Pass
            filter[0]->coefficients = juce::dsp::IIR::Coefficients<SampleType>::makeLowPass(sampleRate,
                cutoff,
                q);
            break;

        case EQConstants::BiquadConstants<SampleType>::Types::highpass : //High Pass
            filter[0]->coefficients = juce::dsp::IIR::Coefficients<SampleType>::makeHighPass(sampleRate,
                cutoff,
                q);
            break;

        case EQConstants::BiquadConstants<SampleType>::Types::notch : //Notch
            filter[0]->coefficients = juce::dsp::IIR::Coefficients<SampleType>::makeNotch(sampleRate,
                cutoff,
                q);
            break;

        case EQConstants::BiquadConstants<SampleType>::Types::bandpass : //BandPass
            filter[0]->coefficients = juce::dsp::IIR::Coefficients<SampleType>::makeBandPass(sampleRate,
                cutoff,
                q);
            break;

        case EQConstants::BiquadConstants<SampleType>::Types::lowshelf : //Lowshelf
            filter[0]->coefficients = juce::dsp::IIR::Coefficients<SampleType>::makeLowShelf(sampleRate,
                cutoff,
                q,
                gainLin);
            break;

        case EQConstants::BiquadConstants<SampleType>::Types::highshelf : //Highshelf
            filter[0]->coefficients = juce::dsp::IIR::Coefficients<SampleType>::makeHighShelf(sampleRate,
                cutoff,
                q,
                gainLin);
            break;

        case -1: //Reset the Filter When Active is down (filter deleted on the UI).
            //filter[biquadID]->coefficients = { (SampleType)0.0, (SampleType)0.0, (SampleType)0.0, (SampleType)0.0, (SampleType)0.0 };
        default:
            //filter[biquadID]->coefficients = { (SampleType)1.0, (SampleType)0.0, (SampleType)0.0, (SampleType)0.0, (SampleType)0.0 };
            break;
        }

        for (int biquadID = 1; biquadID < filterParams.nbOfBiquads; ++biquadID)
        {
            filter[biquadID]->coefficients = filter[0]->coefficients;
        }
    }

    float getFilterMagnitude(double freq)
    {
        float magdB = 0.0f;
        double magLin = 1.0;

        for (int biquadID = 0; biquadID < filterParams.nbOfBiquads; ++biquadID)
        {
            magLin = filter[biquadID]->coefficients->getMagnitudeForFrequency(freq, sampleRate);
            magdB += juce::Decibels::gainToDecibels((float)magLin);
        }

        return magdB;
    }

private:
    juce::OwnedArray<juce::dsp::IIR::Filter<SampleType>> filter;

    FilterParams filterParams;

    SampleType startingFreq = EQConstants::BiquadConstants<SampleType>::cutoffStartValue;
    double sampleRate = 44100.0;
};