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
    IIRBiquadFilter(int filterID)
    {
        for (int biquadID = 0; biquadID < EQConstants::BiquadConstants<SampleType>::Orders::maxOrders; ++biquadID)
        {
            filter.add(new juce::dsp::IIR::Filter<SampleType>());
        }

        filterParams.biquadParams.cutoff = EQConstants::BiquadConstants<SampleType>::cutoffStartValue;
        filterParams.biquadParams.q = EQConstants::BiquadConstants<SampleType>::qStartValue;
        filterParams.biquadParams.gain = EQConstants::BiquadConstants<SampleType>::gainLinStartValue;
        filterParams.biquadParams.type = EQConstants::BiquadConstants<SampleType>::typeStartValue;
        filterParams.nbOfBiquads = EQConstants::BiquadConstants<SampleType>::orderStartValue + 1;
        filterParams.isBypassed = EQConstants::BiquadConstants<SampleType>::isBypassedStartValue;
        filterParams.isActive = EQConstants::BiquadConstants<SampleType>::isActiveStartValue;
        filterParams.id = filterID;
    }

    IIRBiquadFilter(int filterID, SampleType initFreq)
        : startingFreq(initFreq)
    {
        for (int biquadID = 0; biquadID < EQConstants::BiquadConstants::orderMaxValue; ++biquadID)
        {
            filter.add(new juce::dsp::IIR::Filter<SampleType>());
        }

        filterParams.biquadParams.cutoff = initFreq;
        filterParams.biquadParams.q = EQConstants::BiquadConstants<SampleType>::qStartValue;
        filterParams.biquadParams.gain = EQConstants::BiquadConstants<SampleType>::gainLinStartValue;
        filterParams.biquadParams.type = EQConstants::BiquadConstants<SampleType>::typeStartValue;
        filterParams.nbOfBiquads = EQConstants::BiquadConstants<SampleType>::orderStartValue + 1;
        filterParams.isBypassed = EQConstants::BiquadConstants<SampleType>::isBypassedStartValue;
        filterParams.isActive = EQConstants::BiquadConstants<SampleType>::isActiveStartValue;
        filterParams.id = filterID;
    };

    //~IIRBiquadFilter();

    struct IIRBiquadParams
    {
        SampleType cutoff =  EQConstants::BiquadConstants<SampleType>::cutoffStartValue;
        SampleType q = EQConstants::BiquadConstants<SampleType>::qStartValue;
        SampleType gain = EQConstants::BiquadConstants<SampleType>::gainLinStartValue;
        int        type = EQConstants::BiquadConstants<SampleType>::typeStartValue;

        inline IIRBiquadParams& operator=(const IIRBiquadParams& other)
        {
            // Guard self assignment
            if (this == &other)
                return *this;

            this->cutoff = other.cutoff;
            this->q = other.q;
            this->gain = other.gain;
            this->type = other.type;

            return *this;
        }

        inline bool operator==(const IIRBiquadParams& rhs)
        {
            return (
                (cutoff == rhs.cutoff) &&
                (q == rhs.q) &&
                (gain == rhs.gain) &&
                (type == rhs.type)
                );
        };

        inline bool operator!=(const IIRBiquadParams& rhs)
        {
            return !(*this == rhs);
        };
    };

    struct FilterParams
    {
    public:
        IIRBiquadParams biquadParams;
        int  nbOfBiquads = EQConstants::BiquadConstants<SampleType>::orderStartValue;
        bool isBypassed = EQConstants::BiquadConstants<SampleType>::isBypassedStartValue;
        bool isActive = EQConstants::BiquadConstants<SampleType>::isActiveStartValue;
        int  id = 0;

        inline FilterParams& operator=(const FilterParams& other)
        {
            // Guard self assignment
            if (this == &other)
                return *this;

            this->biquadParams = other.biquadParams;
            this->nbOfBiquads = other.nbOfBiquads;
            this->isBypassed = other.isBypassed;
            this->isActive = other.isActive;
            this->id = other.id;

            return *this;
        }

        inline bool operator==(const FilterParams& rhs)
        {
            return (
                (biquadParams == rhs.biquadParams) &&
                (nbOfBiquads == rhs.nbOfBiquads) &&
                (isBypassed == rhs.isBypassed) &&
                (isActive == rhs.isActive) &&
                (id == rhs.id)
                );
        };

        inline bool operator!=(const FilterParams& rhs)
        {
            return !(*this == rhs);
        };
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

    void setBiquadParam(const FilterParams newFilterParams)
    {
        filterParams = newFilterParams;

        setFilterCoefficient(filterParams.biquadParams);
    }

    void setOnOff(const bool newOnOff)
    {
        filterParams.onOff = newOnOff;
    }

    void setIsActive(const bool newIsActive)
    {
        filterParams.isActive = newIsActive;
    }

    void setFilterCoefficient(const IIRBiquadParams biquadParams)
    {
        jassert(biquadParams.cutoff >= (SampleType) 10.0);
        jassert(biquadParams.cutoff < static_cast<SampleType> (sampleRate / 2.0));

        switch (biquadParams.type)
        {
        case EQConstants::BiquadConstants<SampleType>::Types::peak : //Peak
            filter[0]->coefficients = juce::dsp::IIR::Coefficients<SampleType>::makePeakFilter(sampleRate,
                biquadParams.cutoff,
                biquadParams.q,
                biquadParams.gain / filterParams.nbOfBiquads);
            break;

        case EQConstants::BiquadConstants<SampleType>::Types::lowpass : //Low Pass
            filter[0]->coefficients = juce::dsp::IIR::Coefficients<SampleType>::makeLowPass(sampleRate,
                biquadParams.cutoff,
                biquadParams.q);
            break;

        case EQConstants::BiquadConstants<SampleType>::Types::highpass : //High Pass
            filter[0]->coefficients = juce::dsp::IIR::Coefficients<SampleType>::makeHighPass(sampleRate,
                biquadParams.cutoff,
                biquadParams.q);
            break;

        case EQConstants::BiquadConstants<SampleType>::Types::notch : //Notch
            filter[0]->coefficients = juce::dsp::IIR::Coefficients<SampleType>::makeNotch(sampleRate,
                biquadParams.cutoff,
                biquadParams.q);
            break;

        case EQConstants::BiquadConstants<SampleType>::Types::bandpass : //BandPass
            filter[0]->coefficients = juce::dsp::IIR::Coefficients<SampleType>::makeBandPass(sampleRate,
                biquadParams.cutoff,
                biquadParams.q);
            break;

        case EQConstants::BiquadConstants<SampleType>::Types::lowshelf : //Lowshelf
            filter[0]->coefficients = juce::dsp::IIR::Coefficients<SampleType>::makeLowShelf(sampleRate,
                biquadParams.cutoff,
                biquadParams.q,
                biquadParams.gain);
            break;

        case EQConstants::BiquadConstants<SampleType>::Types::highshelf : //Highshelf
            filter[0]->coefficients = juce::dsp::IIR::Coefficients<SampleType>::makeHighShelf(sampleRate,
                biquadParams.cutoff,
                biquadParams.q,
                biquadParams.gain);
            break;

        case -1: //Reset the Filter When Active is down (filter deleted on the UI).
            //filter[biquadID]->coefficients = { (SampleType)0.0, (SampleType)0.0, (SampleType)0.0, (SampleType)0.0, (SampleType)0.0 };
        default:
            filter[0]->coefficients = juce::dsp::IIR::Coefficients<SampleType>::makePeakFilter(sampleRate,
                biquadParams.cutoff,
                biquadParams.q,
                biquadParams.gain); 
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

    FilterParams& getFilterParams() { return filterParams; };

    void setMagHasBeenRemoved(bool newMagHasBeenRemoved) { magHasBeenRemoved = newMagHasBeenRemoved; };
    bool getMagHasBeenRemoved() { return magHasBeenRemoved; };

private:
    juce::OwnedArray<juce::dsp::IIR::Filter<SampleType>> filter;

    FilterParams filterParams;

    SampleType startingFreq = EQConstants::BiquadConstants<SampleType>::cutoffStartValue;

    bool magHasBeenRemoved = false;

    double sampleRate = 44100.0;
};