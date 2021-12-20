/*
  ==============================================================================

    AudioEngine.h
    Created: 19 Apr 2021 6:18:29pm
    Author:  CookiMonstor

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "LayerProcessor/AllLayerProcessor.h"

//==============================================================================
template <typename SampleType>
class AudioEngine //: //public juce::AudioProcessor//public ProcessorBase
{
public:
    AudioEngine(int initNbOfChannels)
        : nbOfChannels(initNbOfChannels),
        mainLayerProcessor(initNbOfChannels)
    {
        masterGain.setGainDecibels(AudioEngineConstants::Processor<SampleType>::masterGainStartValue);

    }

    enum class LayerProcessorID
    {
        mainLayer,
        punch,
        tail,
        mainClipper,
        maxLayerProcessor
    };

    void prepareToPlay(double sampleRate, int samplesPerBlock) //override
    {
        juce::dsp::ProcessSpec spec{ sampleRate, static_cast<juce::uint32> (samplesPerBlock), 2 };
        
        mainLayerProcessor.prepare(spec);

        masterLimiter.prepare(spec);
        masterLimiter.setThreshold(AudioEngineConstants::Processor<SampleType>::masterLimiterThresholdValuedB);
        masterLimiter.setRelease(AudioEngineConstants::Processor<SampleType>::masterLimiterReleaseValueMs);

        masterGain.prepare(spec);
        masterGain.setRampDurationSeconds(spec.maximumBlockSize / spec.sampleRate);

    }

    template <typename ProcessContext>
    void process(const ProcessContext& context) noexcept
    {
        //juce::dsp::AudioBlock<SampleType> block(buffer);
        //juce::dsp::ProcessContextReplacing<SampleType> context(block);
        //fxChain.process(context);
        mainLayerProcessor.process(context);
        //tailProcessor.processBlock(buffer, midiBuffer);

        if(limiterIsBypassed)
            masterLimiter.process(context);

        masterGain.process(context);
    }

    void processBuffer(juce::AudioBuffer<SampleType>& buffer, juce::MidiBuffer& midiBuffer) //override
    {
        juce::dsp::AudioBlock<SampleType> block(buffer);
        juce::dsp::ProcessContextReplacing<SampleType> context(block);

        mainLayerProcessor.processBuffer(buffer, midiBuffer);

        masterGain.process(context);
    }

    void reset() //override
    {
        mainLayerProcessor.reset();

        masterLimiter.reset();

        masterGain.reset();
    }

    
    const juce::String getName() const { return "Audio Engine"; };
    
    //const juce::String getName() const override { return "Audio Engine"; };

    MainLayerProcessor<SampleType>* getMainLayerProcessor() { return &mainLayerProcessor; };

    void setParam(bool newMasterLimiterIsBypassed, SampleType newMasterGaindB)
    { 
        limiterIsBypassed = newMasterLimiterIsBypassed;
        masterGain.setGainDecibels(newMasterGaindB); 
    }

    juce::dsp::Gain<SampleType>* getMasterGain() { return &masterGain; };

private:

    //enum
    //{
    //    eqIndex,
    //    phaserIndex,
    //    distortionProcessorIndex,
    //    scopeIndex,
    //    maxProcessorIndex
    //};

    //juce::dsp::ProcessorChain<EQProcessor<float>, CustomPhaser<float>, DistortionProcessor> fxChain;

    int nbOfChannels = 2;

    MainLayerProcessor<SampleType>   mainLayerProcessor;
    //PunchProcessor       punchProcessor;
    //TailProcessor          tailProcessor;
    //MainClipperProcessor mainClipperProcessor;

    juce::dsp::Gain<SampleType> masterGain;
    juce::dsp::Limiter<SampleType> masterLimiter;
    bool limiterIsBypassed = true;
};