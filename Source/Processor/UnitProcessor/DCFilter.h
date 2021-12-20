/*
  ==============================================================================

    DCFilter.h
    Created: 27 Sep 2021 2:43:10pm
    Author:  CookiMonstor

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "../../DataStructure/MangledIdentifiers.h"

template <typename SampleType>
class DCFilter
{
public:
    //==============================================================================
    /** Constructor. */
    DCFilter(int nbOfChannels = 2);

    //Set Function ==============================================================================================================================================================
    /** Sets the Bypass of the EQ.
    */
    void setIsBypassed(const bool newIsBypassed);


    //Get Function==================================================================================================================================================================

    /** Update the state of the Phaser to avoid calling update() each param. This is
    * because we update all the parameters each slider movement

    */
    void updateState();

    //==============================================================================
    /** Initialises the processor. */
    void prepare(const juce::dsp::ProcessSpec& spec);

    /** Resets the internal state variables of the processor. */
    void reset();

    //==============================================================================
    /** Processes the input and output samples supplied in the processing context. */
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

        //dryWet.pushDrySamples(inputBlock);

        for (int channel = 0; channel < numChannels; ++channel)
        {
            auto* inputSamples = inputBlock.getChannelPointer(channel);
            auto* outputSamples = outputBlock.getChannelPointer(channel);

            for (int sample = 0; sample < numSamples; ++sample)
            {
                auto input = inputSamples[sample];
                auto output = 0.0f;// = input;

                output = dcFilterBank[channel]->processSample(input);

                //jassert(!isnan(output));
                //if (isnan(output))
                //{
                //    int yolo = 1;
                //}

                outputSamples[sample] = output;
            }
        }

        //dryWet.mixWetSamples(outputBlock);
        //updateCounter = (updateCounter + (int)numSamples) % maxUpdateCounter;
    }

    void processBuffer(juce::AudioBuffer<float>& buffer);

private:
    //==============================================================================
    void update();

    //==============================================================================

    juce::OwnedArray<juce::dsp::IIR::Filter<SampleType>> dcFilterBank;

    SampleType dcFilterFreq = static_cast<SampleType>(DistortionConstants::DistortionUnit<float>::dcFilterCutoff);

    double R = 0.995;

    double sampleRate = 44100.0;
    int numberOfChannels;
    bool isBypassed = false;

    juce::dsp::DryWetMixer<SampleType> dryWet;
    SampleType mix = 1.0;

};
