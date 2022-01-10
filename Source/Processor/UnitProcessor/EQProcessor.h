/*
  ==============================================================================

    EQProcessor.h
    Created: 29 Dec 2021 5:14:06pm
    Author:  CookiMonstor

  ==============================================================================
*/

#pragma once

/*
  ==============================================================================

    EQProcessor.h
    Created: 5 Apr 2021 8:25:31pm
    Author:  CookiMonstor

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "IIRBiquadFilter.h"
#include "../../DataStructure/AudioSampleFifo.h"
#include "../../DataStructure/SampleFifo.h"
//#include "../../DataStructure/MangledIdentifiers.h"

template <typename SampleType>
class EQProcessor
{
public:
    //==============================================================================
    /** Constructor. */
    //EQProcessor(int nbOfChannels, double sampleRate);
    EQProcessor(int nbOfChannels = 2, double sampleRate = 44100.0)
        : numberOfChannels(nbOfChannels)
        , sampleRate(sampleRate)
        , freqForFilterMagnitude(1, EQConstants::UI::filterMagnitudeSize)
        , filterMagnitudeValue(EQConstants::Processor<SampleType>::nbOfFilterMax, EQConstants::UI::filterMagnitudeSize)
        , filterSumMagnitudeValue(1, EQConstants::UI::filterMagnitudeSize)
        , filterMagFifo(EQConstants::Processor<SampleType>::nbOfFilterMax, EQConstants::UI::filterMagnitudeSize)
        , filterSumMagFifo(1, EQConstants::UI::filterMagnitudeSize)
    {
        filterBank.resize(numberOfChannels);

        for (int channel = 0; channel < numberOfChannels; ++channel)
        {
            for (int filterID = 0; filterID < EQConstants::Processor<SampleType>::nbOfFilterMax; ++filterID)
            {
                filterBank[channel].add(new IIRBiquadFilter<SampleType>(filterID));
            }
            //filterBank[channel].resize(EQConstants::Processor<SampleType>::nbOfFilterMax);
        }

        int counter = 0;
        double fInc = (1.0 - 0.0) / (double)(EQConstants::UI::nbOfPointsFirstBloc + 1);
        double min = 0.0;
        double max = 10.0;
        double freqC = fInc;

        for (int i = 0; i < EQConstants::UI::nbOfPointsFirstBloc; ++i)
        {
            double freq = juce::jmap(freqC, 0.0, 1.0, min, max);
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

    /** Update the state of the Phaser to avoid calling update() each param. This is
    * because we update all the parameters each slider movement

    */
    void updateState()
    {
        update();
    };

    //==============================================================================
    /** Initialises the processor. */
    void prepare(const juce::dsp::ProcessSpec& spec)
    {
        jassert(spec.sampleRate > 0);
        jassert(spec.numChannels > 0);

        numberOfChannels = (int)spec.numChannels;
        sampleRate = spec.sampleRate;

        dryWet.prepare(spec);

        for (int channelID = 0; channelID < numberOfChannels; ++channelID)
        {
            for (int filterID = 0; filterID < EQConstants::Processor<SampleType>::nbOfFilterMax; ++filterID)
            {
                filterBank[channelID][filterID]->prepare(spec);
            }
        }

        //filterMagFifo.reset(EQConstants::Processor<SampleType>::nbOfFilterMax, static_cast<int>(spec.sampleRate) + 1);
        //filterSumMagFifo.reset(1, static_cast<int>(spec.sampleRate) + 1); //EQConstants::UI::filterMagnitudeSize
        filterMagFifo.setFifoSize(static_cast<int>(spec.sampleRate) + 1);
        filterSumMagFifo.setFifoSize(static_cast<int>(spec.sampleRate) + 1);
        update();
        reset();
    };

    /** Resets the internal state variables of the processor. */
    void reset()
    {
        dryWet.reset();
        //filterMagFifo.reset();
        //for (int channelID = 0; channelID < numberOfChannels; ++channelID)
        //{
        //    for (int filterID = 0; filterID < filterBank[channelID].size(); ++filterID)
        //    {
        //        filterBank[channelID][filterID].reset();
        //    }
        //}

    };

    void processBuffer(juce::AudioBuffer<SampleType>& buffer)

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
                pInputSample = buffer.getWritePointer(channel, sample);
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
    };

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
                    output = filterBank[channelID][filterID]->processSample(output);
                }

                outputSamples[sample] = output;

            }
        }

        dryWet.mixWetSamples(outputBlock);

    };

    //Set Function =================================================================
    void setMixParam(const SampleType newMix)
    {
        dryWet.setWetMixProportion(newMix / static_cast<SampleType>(100.0));
    };

    /** Sets the Bypass of the EQ.
    */
    void setIsBypassed(const bool newIsBypassed)
    {
        isBypassed = newIsBypassed;
    };

    /** Sets the parameters for a given filterID of the EQ.
    */
    void setEQFilterParams(typename IIRBiquadFilter<SampleType>::FilterParams filterParams)//EQProcessor<SampleType>::FilterParamsForUpdate
    {
        int filterID = filterParams.id;

        if (filterParams != filterBank[0][filterID]->getFilterParams())
        {
            for (int channelID = 0; channelID < numberOfChannels; ++channelID)
            {
                filterBank[channelID][filterID]->setBiquadParam(filterParams);
            }

            computeFiltersMagnitude(filterID, filterParams.isActive, filterParams.isBypassed);
        }
    };

    /** Compute each filter magnitude to display
    */
    void computeFiltersMagnitude(int filterID, bool isActive, bool isBypassed)
    {
        double freq = 0.0;
        SampleType filterMagnitudedB = static_cast<SampleType>(0.0);

        if (isActive)
        {
            if (isBypassed)
            {
                for (int i = 0; i < EQConstants::UI::filterMagnitudeSize; ++i)
                {
                    freq = freqForFilterMagnitude.getReadPointer(0)[i];

                    filterMagnitudedB = filterBank[0][filterID]->getFilterMagnitude(freq);

                    filterSumMagnitudeValue.getWritePointer(0)[i] -= filterMagnitudeValue.getWritePointer(filterID)[i];

                    filterMagnitudeValue.getWritePointer(filterID)[i] = filterMagnitudedB;

                }
            }
            else
            {
                for (int i = 0; i < EQConstants::UI::filterMagnitudeSize; ++i)
                {
                    freq = freqForFilterMagnitude.getReadPointer(0)[i];

                    filterMagnitudedB = filterBank[0][filterID]->getFilterMagnitude(freq);

                    filterSumMagnitudeValue.getWritePointer(0)[i] += filterMagnitudedB - filterMagnitudeValue.getWritePointer(filterID)[i];

                    filterMagnitudeValue.getWritePointer(filterID)[i] = filterMagnitudedB;

                }
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

        //for (int i = 0; i < filterSumMagnitudeValue.getNumSamples(); ++i)
        //{
        //    filterSumMagnitudeValue.getWritePointer(0)[i] = 0.0f;
        //}

    };

    //Get Function====================================================================================
    juce::AudioBuffer<SampleType>* getFilterMagnitudeBuffer() { return &filterMagnitudeValue; };
    juce::AudioBuffer<SampleType>* getFilterSumMagnitudeBuffer() { return &filterSumMagnitudeValue; };
    //EQFifo<SampleType>* getEQFifo() { return &eqFifo; };

    void pushFilterSumBufferIntoFifo()
    {
        //filterMagFifo.push(filterMagnitudeValue);
        //filterSumMagFifo.push(filterSumMagnitudeValue);
        filterMagFifo.writeSamplesToFifo(filterMagnitudeValue);
        filterSumMagFifo.writeSamplesToFifo(filterSumMagnitudeValue);
    };

    void pullFilterMagFifo(juce::AudioBuffer<SampleType>& buffer)
    {
        //filterMagFifo.pull(pBuffer, pBuffer->getNumSamples());
        filterMagFifo.readSamplesFromFifo(buffer);
    };

    void pullFilterSumMagFifo(juce::AudioBuffer<SampleType>& buffer)
    {
        //filterSumMagFifo.pull(pBuffer->getWritePointer(0), 0, pBuffer->getNumSamples());
        filterSumMagFifo.readSamplesFromFifo(buffer);
    };

    static void createParametersLayout(std::vector<std::unique_ptr<juce::RangedAudioParameter>>* plugInParameters, int stageID, int nbOfFiltersPerEQ)
    {
        juce::NormalisableRange<SampleType> normalisableMixRange{ EQConstants::Processor<SampleType>::mixMin, 
                                                                  EQConstants::Processor<SampleType>::mixMax,
                                                                  EQConstants::Processor<SampleType>::mixIncrement};

        juce::NormalisableRange<SampleType> normalisableFRange{ EQConstants::BiquadConstants<SampleType>::cutoffMin,
                                                        EQConstants::BiquadConstants<SampleType>::cutoffMax,
                                                         EQConstants::BiquadConstants<SampleType>::cutoffIncrement,
                                                          EQConstants::BiquadConstants<SampleType>::cutoffStartValue }; //20 000Hz to avoid jassert in IIR filter at 44100Hz sr.

        normalisableFRange.setSkewForCentre(EQConstants::BiquadConstants<SampleType>::cutoffStartValue);

        juce::NormalisableRange<SampleType> normalisableQRange{ EQConstants::BiquadConstants<SampleType>::qMin,
                                                            EQConstants::BiquadConstants<SampleType>::qMax,
                                                              EQConstants::BiquadConstants<SampleType>::qIncrement,
                                                               EQConstants::BiquadConstants<SampleType>::qStartValue };

        normalisableQRange.setSkewForCentre(EQConstants::BiquadConstants<SampleType>::qStartValue);

        //Mix EQ
        juce::String paramString = EQConstants::ParamStringID::GetParamStringID::mix(stageID);
        juce::String automationParamString = EQConstants::AutomationString::GetString::mix(stageID);
        plugInParameters->push_back(std::make_unique<juce::AudioParameterFloat>(paramString,
            automationParamString,
            normalisableMixRange,
            EQConstants::Processor<SampleType>::mixStartValue));

        //Bypass EQ
        paramString = EQConstants::ParamStringID::GetParamStringID::isBypassed(stageID);
        automationParamString = EQConstants::AutomationString::GetString::isBypassed(stageID);
        plugInParameters->push_back(std::make_unique<juce::AudioParameterBool>(paramString,
            automationParamString,
            EQConstants::Processor<SampleType>::isBypassedStartValue));

        for (int filterID = 0; filterID < nbOfFiltersPerEQ; ++filterID)
        {
            //Filter CutOff
            paramString = EQConstants::ParamStringID::GetParamStringID::filterCutoff(stageID, filterID);
            automationParamString = EQConstants::AutomationString::GetString::filterCutoff(stageID, filterID);
            plugInParameters->push_back(std::make_unique<juce::AudioParameterFloat>(paramString,
                automationParamString,
                normalisableFRange,
                EQConstants::BiquadConstants<SampleType>::cutoffStartValue));

            //Filter Q
            paramString = EQConstants::ParamStringID::GetParamStringID::filterQ(stageID, filterID);
            automationParamString = EQConstants::AutomationString::GetString::filterQ(stageID, filterID);
            plugInParameters->push_back(std::make_unique<juce::AudioParameterFloat>(paramString,
                automationParamString,
                normalisableQRange,
                EQConstants::BiquadConstants<SampleType>::qStartValue));

            //Filter Gain
            paramString = EQConstants::ParamStringID::GetParamStringID::filterGain(stageID, filterID);
            automationParamString = EQConstants::AutomationString::GetString::filterGain(stageID, filterID);
            plugInParameters->push_back(std::make_unique<juce::AudioParameterFloat>(paramString,
                automationParamString,
                EQConstants::BiquadConstants<SampleType>::gaindBMin,
                EQConstants::BiquadConstants<SampleType>::gaindBMax,
                EQConstants::BiquadConstants<SampleType>::gaindBStartValue));

            //Filter Type
            paramString = EQConstants::ParamStringID::GetParamStringID::filterType(stageID, filterID);
            automationParamString = EQConstants::AutomationString::GetString::filterType(stageID, filterID);
            plugInParameters->push_back(std::make_unique<juce::AudioParameterChoice>(paramString,
                automationParamString,
                EQConstants::UI::typeStringArray,
                EQConstants::BiquadConstants<SampleType>::typeStartValue));

            //Filter Order
            paramString = EQConstants::ParamStringID::GetParamStringID::filterOrder(stageID, filterID);
            automationParamString = EQConstants::AutomationString::GetString::filterOrder(stageID, filterID);
            plugInParameters->push_back(std::make_unique<juce::AudioParameterChoice>(paramString,
                automationParamString,
                EQConstants::UI::orderStringArray,
                EQConstants::BiquadConstants<SampleType>::orderStartValue));

            //On Off Filter
            paramString = EQConstants::ParamStringID::GetParamStringID::filterIsBypassed(stageID, filterID);
            automationParamString = EQConstants::AutomationString::GetString::filterIsBypassed(stageID, filterID);
            plugInParameters->push_back(std::make_unique<juce::AudioParameterBool>(paramString,
                automationParamString,
                EQConstants::BiquadConstants<SampleType>::isBypassedStartValue));

            //Is Active Filter
            paramString = EQConstants::ParamStringID::GetParamStringID::filterIsActive(stageID, filterID);
            automationParamString = EQConstants::AutomationString::GetString::filterIsActive(stageID, filterID);
            plugInParameters->push_back(std::make_unique<juce::AudioParameterBool>(paramString,
                automationParamString,
                !EQConstants::BiquadConstants<SampleType>::isActiveStartValue));
        }
    };

    static void createValueTree(juce::ValueTree& vt)
    {
        juce::ValueTree vtEQ(EQConstants::ParamStringID::eq);
        vt.addChild(vtEQ, -1, nullptr);

        vtEQ.setProperty(EQConstants::ParamStringID::selectedFilterID, -1, nullptr);
        vtEQ.setProperty(EQConstants::ParamStringID::nbOfActiveFilters, 0, nullptr);
    };

private:
    //==============================================================================
    void update() {};

    //==============================================================================
    std::vector < juce::OwnedArray < IIRBiquadFilter<SampleType> > > filterBank;

    //Filter Magnitude For UI.
    juce::AudioBuffer<double> freqForFilterMagnitude;

    juce::AudioBuffer<SampleType> filterMagnitudeValue;
    juce::AudioBuffer<SampleType> filterSumMagnitudeValue;

    AudioSampleFifo<SampleType > filterMagFifo;
    AudioSampleFifo<SampleType> filterSumMagFifo;

    //EQFifo<SampleType> eqFifo;
    double sampleRate = 44100.0;
    int numberOfChannels;
    bool isBypassed = false;

    juce::dsp::DryWetMixer<SampleType> dryWet;
};
