/*
  ==============================================================================

    PhaserProcessor.h
    Created: 2 Apr 2021 4:56:59pm
    Author:  CookiMonstor

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "../../../DataStructure/MangledIdentifiers.h"
#include "../Filters/PhaseShifterFilter.h"

template <typename SampleType>
class CustomPhaser
{
public:
    //==============================================================================
    /** Constructor. */
    CustomPhaser();

    struct Parameters
    {
        //SampleType rateHz;
        //SampleType depth;
        //SampleType centreHz;
        //SampleType feedback;
        //SampleType mix;
        float rateHz = PhaserConstants::Processor<SampleType>::rateStartValue;
        float depth = PhaserConstants::Processor<SampleType>::depthStartValue;
        float centreHz = PhaserConstants::Processor<SampleType>::cfreqStartValue;
        float feedback = PhaserConstants::Processor<SampleType>::feedbackStartValue;
        int numStages = PhaserConstants::Processor<SampleType>::nbOfStgStartValue;
        SampleType mix = PhaserConstants::Processor<SampleType>::mixStartValue;
        bool isBypassed = PhaserConstants::Processor<SampleType>::isBypassedStartValue;;
        int id = 0;

        inline Parameters& operator=(const Parameters& other)
        {
            // Guard self assignment
            if (this == &other)
                return *this;

            this->rateHz = other.rateHz;
            this->depth = other.depth;
            this->centreHz = other.centreHz;
            this->feedback = other.feedback;
            this->numStages = other.numStages;
            this->mix = other.mix;
            this->isBypassed = other.isBypassed;
            this->id = other.id;

            return *this;
        };

        inline bool operator==(const Parameters& rhs)
        {
            return ((rateHz == rhs.rateHz) &&
                (depth == rhs.depth) &&
                (centreHz == rhs.centreHz) &&
                (feedback == rhs.feedback) &&
                (numStages == rhs.numStages) &&
                (mix == rhs.mix) &&
                (isBypassed == rhs.isBypassed) &&
                (id == rhs.id)
                );
        };

        inline bool operator!=(const Parameters& rhs)
        {
            return !(*this == rhs);
        };
    };

    //==============================================================================
    /** Sets the rate (in Hz) of the LFO modulating the phaser all-pass filters. This
        rate must be lower than 100 Hz.
    */
    void setRate(SampleType newRateHz);

    /** Sets the volume (between 0 and 1) of the LFO modulating the phaser all-pass
        filters.
    */
    void setDepth(SampleType newDepth);

    /** Sets the centre frequency (in Hz) of the phaser all-pass filters modulation.
    */
    void setCentreFrequency(SampleType newCentreHz);

    /** Sets the feedback volume (between -1 and 1) of the phaser. Negative can be
        used to get specific phaser sounds.
    */
    void setFeedback(SampleType newFeedback);

    /** Sets the amount of dry and wet signal in the output of the phaser (between 0
        for full dry and 1 for full wet).
    */
    void setMix(SampleType newMix);

    /** Sets the number of stages of the phaser (2 - 4 - 6 - 8).
    */
    void setNumStages(const int newNumStages);

    /** Sets the Bypass of the phaser.
    */
    void setIsBypassed(const bool newIsBypassed);

    /** Sets the Bypass of the phaser.
    */
    void setPhaserParams(CustomPhaser<SampleType>::Parameters& newPhaserParams);


    /** Update the state of the Phaser to avoid calling update() each param. This is
    * because we update all the parameters each slider movement
    
    */
    void updateState();

    //==============================================================================
    /** Initialises the processor. */
    void prepare(const juce::dsp::ProcessSpec& spec);

    /** Resets the internal state variables of the processor. */
    void reset();

    //Get Function
    Parameters& getPhaserParams() { return params; };
    //void getPhaserParams();
    //==============================================================================
    /** Processes the input and output samples supplied in the processing context. */
    template <typename ProcessContext>
    void process(const ProcessContext& context) noexcept
    {
        const auto& inputBlock = context.getInputBlock();
        auto& outputBlock = context.getOutputBlock();
        const auto numChannels = outputBlock.getNumChannels();
        const auto numSamples = outputBlock.getNumSamples();

        jassert(inputBlock.getNumChannels() == numChannels);
        jassert(inputBlock.getNumChannels() == lastOutput.size());
        jassert(inputBlock.getNumSamples() == numSamples);

        auto inputNumSamples = inputBlock.getNumSamples();

        if (isBypassed || context.isBypassed)
        {
            smoothedValuesSkip(static_cast<int>(inputNumSamples));

            if (context.usesSeparateInputAndOutputBlocks())
                outputBlock.copyFrom(inputBlock);

            return;
        }

        //int numSamplesDown = 0;
        //auto counter = updateCounter;

        //for (size_t i = 0; i < numSamples; ++i)
        //{
        //    if (counter == 0)
        //        numSamplesDown++;

        //    counter++;

        //    if (counter == maxUpdateCounter)
        //        counter = 0;
        //}

        //if (numSamplesDown > 0)
        //{
        //    auto freqBlock = juce::dsp::AudioBlock<SampleType>(bufferFrequency).getSubBlock(0, (size_t)numSamplesDown);
        //    auto contextFreq = juce::dsp::ProcessContextReplacing<SampleType>(freqBlock);
        //    freqBlock.clear();

        //    osc.process(contextFreq);
        //    freqBlock.multiplyBy(oscVolume);
        //}

        //auto* freqSamples = bufferFrequency.getWritePointer(0);

        //for (int i = 0; i < numSamplesDown; ++i)
        //{
        //    auto lfo = juce::jlimit(static_cast<SampleType> (0.0),
        //        static_cast<SampleType> (1.0),
        //        freqSamples[i] + normCentreFrequency);
        //    
        //    freqSamples[i] = juce::mapToLog10(lfo, static_cast<SampleType> (20.0),
        //        static_cast<SampleType> (juce::jmin(20000.0, 0.49 * sampleRate)));
        //}

        //auto currentFrequency = allPassFilters[0]->getCutoffFrequency();
        dryWet.pushDrySamples(inputBlock);

        for (int channelID = 0; channelID < numChannels; ++channelID)
        {
            //counter = updateCounter;
            //int k = 0;

            //auto* inputSamples = inputBlock.getChannelPointer(channel);
            auto* outputSamples = outputBlock.getChannelPointer(channelID);

            //for (int n = 0; n < nbOfPhaseShifter; ++n)
            //{
            //    phaseShifterFilters[n]->setNextFrequency();
            //}

            for (int i = 0; i < numSamples; ++i)
            {
                //auto input = inputSamples[i];
                auto input = inputBlock.getSample(channelID, i);
                auto output = input - lastOutput[channelID];

                //if (i == 0 && counter != 0)
                //    for (int n = 0; n < numStages; ++n)
                //        allPassFilters[n]->setCutoffFrequency(currentFrequency);

                //if (counter == 0)
                //{
                //        for(int n = 0; n < numStages; ++n)
                //            allPassFilters[n]->setCutoffFrequency(freqSamples[k]);

                //    k++;
                //}
                jassert(nbOfPhaseShifter <= phaseShifterFilters.size());
                for (int n = 0; n < nbOfPhaseShifter; ++n)
                {
                    output = phaseShifterFilters[n]->processSample((int)channelID, output);
                } 

                outputSamples[i] = output;
                //outputBlock.getChannelPointer(channel)[i] = output;
                lastOutput[channelID] = output * feedbackVolume[channelID].getNextValue();

                //counter++;

                //if (counter == maxUpdateCounter)
                //    counter = 0;
            }
        }

        dryWet.mixWetSamples(outputBlock);
        //updateCounter = (updateCounter + (int)numSamples) % maxUpdateCounter;
    };
    
    void smoothedValuesSkip(int numSamplesToSkip)
    {
        for (int n = 0; n < nbOfPhaseShifter; ++n)
        {
            phaseShifterFilters[n]->smoothedValuesSkip(numSamplesToSkip);
        }
    };

    static void createParametersLayout(std::vector<std::unique_ptr<juce::RangedAudioParameter>>* plugInParameters, int stageID)
    {
        juce::NormalisableRange<SampleType> normalisableMixRange{ PhaserConstants::Processor<SampleType>::mixMin,
                                                                    PhaserConstants::Processor<SampleType>::mixMax,
                                                                    PhaserConstants::Processor<SampleType>::mixStartValue };
        //juce::NormalisableRange<SampleType> normalisableMixRange{ 0.0f,
        //                                                    100.0f,
        //                                                    0.3f };
        juce::String paramString = PhaserConstants::ParamStringID::GetParamStringID::centerFrequency(stageID);
        juce::String automationParamString = PhaserConstants::AutomationString::GetString::centerFrequency(stageID);

        plugInParameters->push_back(std::make_unique<juce::AudioParameterFloat>(paramString,
            automationParamString,
            PhaserConstants::Processor<SampleType>::cfreqMin,
            PhaserConstants::Processor<SampleType>::cfreqMax,
            PhaserConstants::Processor<SampleType>::cfreqStartValue));

        paramString = PhaserConstants::ParamStringID::GetParamStringID::feedback(stageID);
        automationParamString = PhaserConstants::AutomationString::GetString::feedback(stageID);
        plugInParameters->push_back(std::make_unique<juce::AudioParameterFloat>(paramString,
            automationParamString,
            PhaserConstants::Processor<SampleType>::feedbackMin,
            PhaserConstants::Processor<SampleType>::feedbackMax,
            PhaserConstants::Processor<SampleType>::feedbackStartValue));

        paramString = PhaserConstants::ParamStringID::GetParamStringID::rate(stageID);
        automationParamString = PhaserConstants::AutomationString::GetString::rate(stageID);
        plugInParameters->push_back(std::make_unique<juce::AudioParameterFloat>(paramString,
            automationParamString,
            PhaserConstants::Processor<SampleType>::rateMin,
            PhaserConstants::Processor<SampleType>::rateMax,
            PhaserConstants::Processor<SampleType>::rateStartValue));

        paramString = PhaserConstants::ParamStringID::GetParamStringID::depth(stageID);
        automationParamString = PhaserConstants::AutomationString::GetString::depth(stageID);
        plugInParameters->push_back(std::make_unique<juce::AudioParameterFloat>(paramString,
            automationParamString,
            PhaserConstants::Processor<SampleType>::depthMin,
            PhaserConstants::Processor<SampleType>::depthMax,
            PhaserConstants::Processor<SampleType>::depthStartValue));

        paramString = PhaserConstants::ParamStringID::GetParamStringID::mix(stageID);
        automationParamString = PhaserConstants::AutomationString::GetString::mix(stageID);
        plugInParameters->push_back(std::make_unique<juce::AudioParameterFloat>(paramString,
            automationParamString,
            PhaserConstants::Processor<SampleType>::mixMin,
            PhaserConstants::Processor<SampleType>::mixMax,
            PhaserConstants::Processor<SampleType>::mixStartValue));

        paramString = PhaserConstants::ParamStringID::GetParamStringID::nbOfStages(stageID);
        automationParamString = PhaserConstants::AutomationString::GetString::nbOfStages(stageID);
        plugInParameters->push_back(std::make_unique<juce::AudioParameterChoice>(paramString,
            automationParamString,
            PhaserConstants::UI::nbOfStgStringArray,
            PhaserConstants::Processor<SampleType>::nbOfStgStartValue));

        paramString = PhaserConstants::ParamStringID::GetParamStringID::isBypassed(stageID);
        automationParamString = PhaserConstants::AutomationString::GetString::isBypassed(stageID);
        plugInParameters->push_back(std::make_unique<juce::AudioParameterBool>(paramString,
            automationParamString,
            PhaserConstants::Processor<SampleType>::isBypassedStartValue));

    };


   /** class State
    {
    public:
        State(juce::AudioProcessorValueTreeState& state)//, CustomPhaser<SampleType>& proc)
        : apvts(state)
        //, processor(proc)
        {

        };

        ~State() {};

        void setPhaserParams(int stageID)// juce::AudioProcessorValueTreeState& apvts, int stageID)
        {
            //juce::AudioProcessorValueTreeState& apvts = mainLayerDataStruct.getAPVTSMainLayer();

            CustomPhaser<float>* pPhaserProcessor = audioEngine.getMainLayerProcessor()->getStageProcessor(stageID)->getPhaserProcessor();

            CustomPhaser<float>::Parameters newPhaserProcessorParams;

            juce::String paramString = PhaserConstants::ParamStringID::GetParamStringID::centerFrequency(stageID);
            newPhaserProcessorParams.centreHz = *apvts.getRawParameterValue(paramString);

            paramString = PhaserConstants::ParamStringID::GetParamStringID::feedback(stageID);
            newPhaserProcessorParams.feedback = *apvts.getRawParameterValue(paramString);

            paramString = PhaserConstants::ParamStringID::GetParamStringID::rate(stageID);
            newPhaserProcessorParams.rateHz = *apvts.getRawParameterValue(paramString);

            paramString = PhaserConstants::ParamStringID::GetParamStringID::depth(stageID);
            newPhaserProcessorParams.depth = *apvts.getRawParameterValue(paramString);

            paramString = PhaserConstants::ParamStringID::GetParamStringID::nbOfStages(stageID);
            newPhaserProcessorParams.numStages = (int)*apvts.getRawParameterValue(paramString);

            paramString = PhaserConstants::ParamStringID::GetParamStringID::mix(stageID);
            newPhaserProcessorParams.mix = *apvts.getRawParameterValue(paramString);

            paramString = PhaserConstants::ParamStringID::GetParamStringID::isBypassed(stageID);
            newPhaserProcessorParams.isBypassed = *apvts.getRawParameterValue(paramString);

            pPhaserProcessor->setPhaserParams(newPhaserProcessorParams);

        }


    private:
        juce::AudioProcessorValueTreeState& apvts;
        //CustomPhaser<SampleType>& processor;
    };
    */
private:
    //==============================================================================
    void update();

    //==============================================================================
    juce::OwnedArray<PhaseShifterFilter<SampleType>> phaseShifterFilters;
    std::vector<SampleType> phaseShifterFreq;
    int nbOfPhaseShifter;

    //juce::dsp::Oscillator<SampleType> osc;
    //juce::OwnedArray<juce::dsp::FirstOrderTPTFilter<SampleType>> allPassFilters;
    //juce::SmoothedValue<SampleType, juce::ValueSmoothingTypes::Linear> oscVolume;
    std::vector<juce::SmoothedValue<SampleType, juce::ValueSmoothingTypes::Linear>> feedbackVolume{ 2 };
    juce::dsp::DryWetMixer<SampleType> dryWet;
    std::vector<SampleType> lastOutput{ 2 };

    //juce::AudioBuffer<SampleType> bufferFrequency;
    SampleType normCentreFrequency = 0.5;
    double sampleRate = 44100.0;
    bool isBypassed = true;

    int updateCounter = 0;
    static constexpr int maxUpdateCounter = 4;

    //SampleType rate = 1.0;
    //SampleType depth = 0.5;
    SampleType feedback = 0.0;
    SampleType mix = 0.5;
    SampleType centreFrequency = 1300.0;

    Parameters params;

    int numStages = 4;
    static constexpr int maxNumStages = 8;
};