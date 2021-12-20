/*
==============================================================================

GainProcessor.h
Created: 15 Nov 2021 1:47:18pm
Author:  CookiMonstor

==============================================================================
*/

#pragma once
#include <JuceHeader.h>

template <typename SampleType>
class GainProcessor
{
public:
    GainProcessor() noexcept = default;

    //==============================================================================
    /** Applies a new gain as a linear value. */
    void setGainLinear(SampleType newGain) noexcept { gain.setTargetValue(newGain); }

    /** Applies a new gain as a decibel value. */
    void setGainDecibels(SampleType newGainDecibels) noexcept { setGainLinear(juce::Decibels::decibelsToGain<SampleType>(newGainDecibels)); }

    /** Returns the current gain as a linear value. */
    SampleType getGainLinear() const noexcept { return gain.getTargetValue(); }

    /** Returns the current gain in decibels. */
    SampleType getGainDecibels() const noexcept { return juce::Decibels::gainToDecibels<SampleType>(getGainLinear()); }

    /** Sets the length of the ramp used for smoothing gain changes. */
    void setRampDurationSeconds(double newDurationSeconds) noexcept
    {
        if (rampDurationSeconds != newDurationSeconds)
        {
            rampDurationSeconds = newDurationSeconds;
            reset();
        }
    }

    /** Returns the ramp duration in seconds. */
    double getRampDurationSeconds() const noexcept { return rampDurationSeconds; }

    /** Returns true if the current value is currently being interpolated. */
    bool isSmoothing() const noexcept { return gain.isSmoothing(); }

    //==============================================================================
    /** Called before processing starts. */
    void prepare(const juce::dsp::ProcessSpec& spec) noexcept
    {
        sampleRate = spec.sampleRate;
        reset();
    }

    /** Resets the internal state of the gain */
    void reset() noexcept
    {
        if (sampleRate > 0)
            gain.reset(sampleRate, rampDurationSeconds);
    }

    //==============================================================================
    /** Returns the result of processing a single sample. */
    template <typename SampleType>
    SampleType JUCE_VECTOR_CALLTYPE processSample(SampleType s) noexcept
    {
        return s * gain.getNextValue();
    }

    /** Processes the input and output buffers supplied in the processing context. */
    template <typename ProcessContext>
    void process(const ProcessContext& context) noexcept
    {
        auto&& inBlock = context.getInputBlock();
        auto&& outBlock = context.getOutputBlock();

        jassert(inBlock.getNumChannels() == outBlock.getNumChannels());
        jassert(inBlock.getNumSamples() == outBlock.getNumSamples());

        auto len = inBlock.getNumSamples();
        auto numChannels = inBlock.getNumChannels();

        if (context.isBypassed)
        {
            gain.skip(static_cast<int> (len));

            if (context.usesSeparateInputAndOutputBlocks())
                outBlock.copyFrom(inBlock);

            return;
        }

        if (numChannels == 1)
        {
            auto* src = inBlock.getChannelPointer(0);
            auto* dst = outBlock.getChannelPointer(0);

            for (size_t i = 0; i < len; ++i)
                dst[i] = src[i] * gain.getNextValue();
        }
        else
        {
            JUCE_BEGIN_IGNORE_WARNINGS_MSVC(6255 6386)
                auto* gains = static_cast<SampleType*> (alloca(sizeof(SampleType) * len));

            for (size_t i = 0; i < len; ++i)
                gains[i] = gain.getNextValue();
            JUCE_END_IGNORE_WARNINGS_MSVC

                for (size_t chan = 0; chan < numChannels; ++chan)
                    juce::FloatVectorOperations::multiply(outBlock.getChannelPointer(chan),
                        inBlock.getChannelPointer(chan),
                        gains, static_cast<int> (len));
        }
    }

    void smoothedValuesSkip(int numSamplesToSkip)
    {
        gain.skip(numSamplesToSkip);
    }
private:
    //==============================================================================
    juce::SmoothedValue<SampleType> gain;
    double sampleRate = 0, rampDurationSeconds = 0;
};