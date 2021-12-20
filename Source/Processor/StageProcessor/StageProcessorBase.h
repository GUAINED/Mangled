/*
  ==============================================================================

    StageProcessorBase.h
    Created: 19 Apr 2021 6:18:29pm
    Author:  CookiMonstor

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "../UnitProcessor/AllUnitProcessor.h"
#include "../UnitProcessor/GainProcessor.h"
#include "../UnitProcessor/RMSProcessor.h"
//==============================================================================
template <typename SampleType>
class StageProcessorBase //: //public juce::AudioProcessor//public ProcessorBase
{
public:
    StageProcessorBase(int nbOfChannels) :
        preEQScopeProcessor(44100.0),
        postEQScopeProcessor(44100.0),
        postDistoScopeProcessor(44100.0),
        eqProcessor(nbOfChannels, 44100.0)
    {
        inputGainProcessor.setGainDecibels(static_cast<SampleType>(0.0));
        outputGainProcessor.setGainDecibels(static_cast<SampleType>(0.0));
    }

    enum class UnitProcessorID
    {
        //preEQScopeIndex,
        eqID,
        //postEQScopeIndex,
        phaserID,
        distortionID,
        maxProcessorID
    };

    struct StageProcessorParams
    {
        bool onOff;
    };

    void prepare(const juce::dsp::ProcessSpec& spec)//prepare(juce::dsp::ProcessSpec spec) //override
    {
        //juce::dsp::ProcessSpec spec{ sampleRate, static_cast<juce::uint32> (samplesPerBlock), 2 };
        //fxChain.prepare(spec);
        //rmsLevels.clear();
        //for (auto i = 0; i < spec.numChannels; i++)
        //{
        //    juce::LinearSmoothedValue<float> rms{ -100.f };
        //    rms.reset(spec.sampleRate, 0.5);
        //    rmsLevels.emplace_back(std::move(rms));
        //}
        //rmsFifo.reset(spec.numChannels, spec.sampleRate + 1);
        //rmsWindowSize = static_cast<int> (spec.sampleRate * 50) / 1000;
        //rmsCalculationBuffer.clear();
        //rmsCalculationBuffer.setSize(spec.numChannels, static_cast<int>(spec.sampleRate) + 1);

        inputGainProcessor.prepare(spec);
        inputRMSProcessor.prepare(spec);

        preEQScopeProcessor.prepare(spec);
        eqProcessor.prepare(spec);
        postEQScopeProcessor.prepare(spec);

        phaserProcessor.prepare(spec);
        distortionProcessor.prepare(spec);
        postDistoScopeProcessor.prepare(spec);

        outputGainProcessor.prepare(spec);
        outputRMSProcessor.prepare(spec);
    }

    template <typename ProcessContext>
    void process(const ProcessContext& context) noexcept
    {
        if (isBypassed || context.isBypassed)
            return;

        inputGainProcessor.process(context);

        //rmsFifo.push(context);
        inputRMSProcessor.process(context);

        preEQScopeProcessor.processBlock(context);
        eqProcessor.process(context);
        postEQScopeProcessor.processBlock(context);

        phaserProcessor.process(context);

        distortionProcessor.process(context);
        postDistoScopeProcessor.processBlock(context);

        outputGainProcessor.process(context);
        outputRMSProcessor.process(context);

    }

    void processBuffer(juce::AudioBuffer<SampleType>& buffer, juce::MidiBuffer&) //override  process(juce::dsp::ProcessContextReplacing<float> context)
    {
        if (isBypassed)
            return;

        //fxChain.process(context);
        //juce::AudioBuffer<float> postEQBuffer;
        
        preEQScopeProcessor.process(buffer);
        eqProcessor.processBuffer(buffer);
        postEQScopeProcessor.process(buffer);
        //phaserProcessor.processBuffer(buffer);
        distortionProcessor.processBuffer(buffer);
        postDistoScopeProcessor.process(buffer);
    }

    void reset() //override
    {
        //fxChain.reset();
        inputGainProcessor.reset();

        preEQScopeProcessor.reset();
        eqProcessor.reset();
        postEQScopeProcessor.reset();

        phaserProcessor.reset();
        distortionProcessor.reset();
        postDistoScopeProcessor.reset();

        outputGainProcessor.reset();
    }

    //SampleType getRmsLevel(const int channel)
    //{
    //    jassert(channel >= 0 && channel < rmsCalculationBuffer.getNumChannels());
    //    rmsFifo.pull(rmsCalculationBuffer.getWritePointer(channel), channel, rmsWindowSize);
    //    processLevelValue(rmsLevels[channel], juce::Decibels::gainToDecibels(rmsCalculationBuffer.getRMSLevel(channel, 0, rmsWindowSize)));
    //    return rmsLevels[channel].getCurrentValue();
    //}

    //void processLevelValue(juce::LinearSmoothedValue<SampleType>& smoothedValue, const float value) const
    //{
    //    if (isSmoothed)
    //    {
    //        if (value < smoothedValue.getCurrentValue())
    //        {
    //            smoothedValue.setTargetValue(value);
    //            return;
    //        }
    //    }
    //    smoothedValue.setCurrentAndTargetValue(value);
    //}

    /** Set Input Gain.
    */
    void setInputGain(SampleType newInputGaindB) { inputGainProcessor.setGainDecibels(newInputGaindB); };

    /** Set Input Gain.
*/
    void setOutputGain(SampleType newOutputGaindB) { outputGainProcessor.setGainDecibels(newOutputGaindB); };

    /** Sets the parameters.
    */
    void setIsBypassed(bool newIsBypassed) { isBypassed = newIsBypassed; };

    const juce::String getName() const { return "Stage Processor Base"; };

    RMSProcessor<SampleType>* getInputRMSProcessor() { return &inputRMSProcessor; };
    ScopeProcessor<SampleType>*      getPreEQScopeProcessor()  { return &preEQScopeProcessor; };
    EQProcessor<SampleType>*         getEQProcessor()          { return &eqProcessor; };
    ScopeProcessor<SampleType>*      getPostEQScopeProcessor() { return &postEQScopeProcessor; };
    CustomPhaser<SampleType>*        getPhaserProcessor()      { return &phaserProcessor; };
    DistortionProcessor<SampleType>* getDistortionProcessor()  { return &distortionProcessor; };
    ScopeProcessor<SampleType>*      getPostDistoScopeProcessor() { return &postDistoScopeProcessor; };
    RMSProcessor<SampleType>* getOutputRMSProcessor() { return &outputRMSProcessor; };

private:
    GainProcessor<SampleType> inputGainProcessor;
    RMSProcessor<SampleType> inputRMSProcessor;

    ScopeProcessor<SampleType> preEQScopeProcessor;
    EQProcessor<SampleType> eqProcessor;
    ScopeProcessor<SampleType> postEQScopeProcessor;

    CustomPhaser<SampleType> phaserProcessor;
    DistortionProcessor<SampleType> distortionProcessor;
    ScopeProcessor<SampleType> postDistoScopeProcessor;

    GainProcessor<SampleType> outputGainProcessor;
    RMSProcessor<SampleType> outputRMSProcessor;
    bool isBypassed = true;

    //RMSFifo<SampleType> rmsFifo;
    //juce::AudioBuffer<float> rmsCalculationBuffer;
    //int rmsWindowSize = 50;
    //std::vector<juce::LinearSmoothedValue<SampleType>> rmsLevels;
    //bool isSmoothed = false;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(StageProcessorBase)
};