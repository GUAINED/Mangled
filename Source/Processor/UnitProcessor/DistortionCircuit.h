/*
  ==============================================================================

    DistortionCircuit.h
    Created: 13 Oct 2021 2:37:02pm
    Author:  CookiMonstor

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "DistortionEquation.h"
#include "DCFilter.h"

//==============================================================================
template <typename SampleType>
class DistortionCircuit
{
public:
    //==============================================================================
    DistortionCircuit()
    {
        distortionEquation = [](SampleType x, SampleType drive, SampleType warp) {return DistortionEquation<SampleType>::Sigmoid::tanh(x, drive, warp); };
        drive.setTargetValue(static_cast<SampleType>(2.0));
        warp.setTargetValue(static_cast<SampleType>(0.0));
        //dryWet.setMixingRule(juce::dsp::DryWetMixingRule::linear);
    }

    struct Parameters
    {
        int equationType;
        int equationID;
        SampleType drive;
        SampleType warp;

        inline Parameters& operator=(const Parameters& other)
        {
            // Guard self assignment
            if (this == &other)
                return *this;

            this->equationType = other.equationType;
            this->equationID = other.equationID;
            this->drive = other.drive;
            this->warp = other.warp;

            return *this;
        }

        inline bool operator==(const Parameters& rhs)
        {
            return ((equationType == rhs.equationType) &&
                (equationID == rhs.equationID) &&
                (drive == rhs.drive) &&
                (warp == rhs.warp)
                );
        };

        inline bool operator!=(const Parameters& rhs)
        {
            return !(*this == rhs);
        };
    };

    //==============================================================================
    void prepare(const juce::dsp::ProcessSpec& spec)
    {
        //juce::ignoreUnused (spec);
        //processorChain.prepare(spec);

        sampleRate = spec.sampleRate;
        rampDurationSeconds = spec.maximumBlockSize / sampleRate;
        //rampDurationSeconds = 10.0;
        //dryWet.prepare(spec);

        reset();
    }

    //==============================================================================
    template <typename ProcessContext>
    void process(const ProcessContext& context) noexcept
    {
        //juce::ignoreUnused(context);
        //processorChain.process(context);

        const auto& inputBlock = context.getInputBlock();
        auto& outputBlock = context.getOutputBlock();
        const auto numChannels = outputBlock.getNumChannels();
        const auto numSamples = outputBlock.getNumSamples();

        jassert(inputBlock.getNumChannels() == numChannels);
        jassert(inputBlock.getNumSamples() == numSamples);

        auto inputNumSamples = inputBlock.getNumSamples();

        //Bypass DistortionCircuit
        if (isBypassed || context.isBypassed)
        {
            smoothedValuesSkip(static_cast<int>(inputNumSamples));

            if (context.usesSeparateInputAndOutputBlocks())
                outputBlock.copyFrom(inputBlock);

            return;
        }

        SampleType driveSample = static_cast<SampleType>(0.0); 
        SampleType warpSample = static_cast<SampleType>(0.0);

        for (uint_fast32_t sampleIndex = 0; sampleIndex < numSamples; ++sampleIndex)
        {
            //auto* inputSamples = inputBlock.getChannelPointer(channel);
            //auto* outputSamples = outputBlock.getChannelPointer(channel);
            driveSample = drive.getNextValue();
            warpSample = warp.getNextValue();

            for (uint_fast32_t channel = 0; channel < numChannels; ++channel)
            {
                auto input = inputBlock.getSample(channel, sampleIndex);
                auto output = input;

                output = distortionEquation(input, driveSample, warpSample);

                outputBlock.getChannelPointer(channel)[sampleIndex] = output;
            }
        }

        //Mix
        //dryWet.mixWetSamples(outputBlock);
    }

    void processBuffer(juce::AudioBuffer<SampleType>& buffer)
    {
        //Bypass DistortionCircuit
        //if ((!isBypassed))//|| context.isBypassed)
        //{
        //    return;
        //}

        SampleType* pInputSample;
        SampleType input = static_cast<SampleType>(0.0);
        SampleType output = static_cast<SampleType>(0.0);
        SampleType driveSample = static_cast<SampleType>(0.0);
        SampleType warpSample = static_cast<SampleType>(0.0);

        for (size_t sampleIndex = 0; sampleIndex < buffer.getNumSamples(); ++sampleIndex)
        {
            //auto* inputSamples = inputBlock.getChannelPointer(channel);
            //auto* outputSamples = outputBlock.getChannelPointer(channel);
            driveSample = drive.getNextValue();
            warpSample = warp.getNextValue();

            for (size_t channel = 0; channel < buffer.getNumChannels(); ++channel)
            {
                pInputSample = buffer.getWritePointer(channel, sampleIndex);
                input = *pInputSample;

                output = distortionEquation(input, driveSample, warpSample);

                *pInputSample = output;
            }
        }
    }

    SampleType processSample(SampleType sample)
    {
        return distortionEquation(sample, drive.getTargetValue(), warp.getTargetValue());
    }

    //==============================================================================
    void reset() noexcept
    {
        //processorChain.reset();

        drive.reset(sampleRate, rampDurationSeconds);
        warp.reset(sampleRate, rampDurationSeconds);
    }

    /** Set the Equation of the waveShaper.
    */
    void setEquation(const int newEquationType, const int newEquationID)//newDistortionEquation, const int newDistortionEquationForNegativeValue);
    {
        equationTypeToEquationID(newEquationType, newEquationID);
    }

    /** Sets the Drive of the waveShaper.
    */
    void setDrive(SampleType newDrive)
    {
        //drive = newDrive;
        drive.setTargetValue(newDrive);
    }

    /** Sets the Drive of the waveShaper.
    */
    void setWarp(SampleType newWarp)
    {
        //drive = newDrive;
        warp.setTargetValue(newWarp);
    }

    /** Sets the amount of dry and wet signal in the output of the WaveShaper (between 0
    for full dry and 1 for full wet).
    */
    void setMixParam(SampleType newMix)
    {
        jassert(juce::isPositiveAndNotGreaterThan(newMix, static_cast<float> (1.0)));
    
        mix = newMix;
        //dryWet.setWetMixProportion(mix);
    }

    /** Sets the Bypass of the waveShaper.
    */
    void setIsBypassed(const bool newIsBypassed)
    {
        isBypassed = newIsBypassed;
    }

    /** Set the Parameters
    */
    void setParams(const Parameters& params)
    {
        setEquation(params.equationType, params.equationID);
        setDrive(params.drive);
        setWarp(params.warp);
    };

    void smoothedValuesSkip(int numSamplesToSkip)
    {
        drive.skip(numSamplesToSkip);
        warp.skip(numSamplesToSkip);
    }

    void equationTypeToEquationID(int eqaType, int eqaID)
    {
        switch (eqaType)
        {
            case DistortionCircuitConstants::Processor<SampleType>::EquationType::sigmoid :
                distortionEquation = sigmoidIDToEquation(eqaID);
                break;
            case DistortionCircuitConstants::Processor<SampleType>::EquationType::symetric :
                distortionEquation = symetricIDToEquation(eqaID);
                break;
            case DistortionCircuitConstants::Processor<SampleType>::EquationType::asymetric :
                distortionEquation = asymetricIDToEquation(eqaID);
                break;
            case DistortionCircuitConstants::Processor<SampleType>::EquationType::special :
                distortionEquation = specialIDToEquation(eqaID);
                break;
            default:
                distortionEquation = [](SampleType x, SampleType drive, SampleType warp) {return DistortionEquation<SampleType>::identity(x, drive, warp); };
                break;
        }
    }

    static std::function<SampleType(SampleType, SampleType, SampleType)> sigmoidIDToEquation(int equationID)
    {
        switch (equationID)
        {
        case DistortionCircuitConstants::Processor<SampleType>::SigmoidEquationID::none :
            return [](SampleType x, SampleType drive, SampleType warp) {return DistortionEquation<SampleType>::identity(x, drive, warp); };
            break;

        case DistortionCircuitConstants::Processor<SampleType>::SigmoidEquationID::tanh:
            return [](SampleType x, SampleType drive, SampleType warp) {return DistortionEquation<SampleType>::Sigmoid::tanh(x, drive, warp); };
            break;

        case DistortionCircuitConstants::Processor<SampleType>::SigmoidEquationID::parabolic :
            return [](SampleType x, SampleType drive, SampleType warp) {return DistortionEquation<SampleType>::Sigmoid::parabolic(x, drive, warp); };
            break;

        case DistortionCircuitConstants::Processor<SampleType>::SigmoidEquationID::hyperbolic :
            return [](SampleType x, SampleType drive, SampleType warp) {return DistortionEquation<SampleType>::Sigmoid::hyperbolic(x, drive, warp); };
            break;

        case DistortionCircuitConstants::Processor<SampleType>::SigmoidEquationID::asinh:
            return [](SampleType x, SampleType drive, SampleType warp) {return DistortionEquation<SampleType>::Sigmoid::asinh(x, drive, warp); };
            break;

        case DistortionCircuitConstants::Processor<SampleType>::SigmoidEquationID::unbSat1 :
                return [](SampleType x, SampleType drive, SampleType warp) {return DistortionEquation<SampleType>::Sigmoid::unbSat1(x, drive, warp); };
            break;

        case DistortionCircuitConstants::Processor<SampleType>::SigmoidEquationID::fuzz :
                return [](SampleType x, SampleType drive, SampleType warp) {return DistortionEquation<SampleType>::Sigmoid::fuzz(x, drive, warp); };
            break;
        case DistortionCircuitConstants::Processor<SampleType>::SigmoidEquationID::softClipper:
            return [](SampleType x, SampleType drive, SampleType warp) {return DistortionEquation<SampleType>::Sigmoid::softClipper(x, drive, warp); };
            break;
        default:
            return [](SampleType x, SampleType drive, SampleType warp) {return DistortionEquation<SampleType>::identity(x, drive, warp); };
            break;
        }
    }

    static std::function<SampleType(SampleType, SampleType, SampleType)> symetricIDToEquation(int equationID)
    {
        switch (equationID)
        {
        case DistortionCircuitConstants::Processor<SampleType>::SymetricEquationID::none:
            return [](SampleType x, SampleType drive, SampleType warp) {return DistortionEquation<SampleType>::identity(x, drive, warp); };
            break;
        case DistortionCircuitConstants::Processor<SampleType>::SymetricEquationID::hardclip:
            return [](SampleType x, SampleType drive, SampleType warp) {return DistortionEquation<SampleType>::Symetric::hardclip(x, drive, warp); };
            break;
        case DistortionCircuitConstants::Processor<SampleType>::SymetricEquationID::sin:
            return [](SampleType x, SampleType drive, SampleType warp) {return DistortionEquation<SampleType>::Symetric::sin(x, drive, warp); };
            break;
        case DistortionCircuitConstants::Processor<SampleType>::SymetricEquationID::sinh:
            return [](SampleType x, SampleType drive, SampleType warp) {return DistortionEquation<SampleType>::Symetric::sinh(x, drive, warp); };
            break;
        case DistortionCircuitConstants::Processor<SampleType>::SymetricEquationID::sinhAlt:
            return [](SampleType x, SampleType drive, SampleType warp) {return DistortionEquation<SampleType>::Symetric::sinhAlt(x, drive, warp); };
            break;
        case DistortionCircuitConstants::Processor<SampleType>::SymetricEquationID::tape:
            return [](SampleType x, SampleType drive, SampleType warp) {return DistortionEquation<SampleType>::Symetric::tape(x, drive, warp); };
            break;
        case DistortionCircuitConstants::Processor<SampleType>::SymetricEquationID::foldover:
            return [](SampleType x, SampleType drive, SampleType warp) {return DistortionEquation<SampleType>::Symetric::foldover(x, drive, warp); };
            break;
        default:
            return [](SampleType x, SampleType drive, SampleType warp) {return DistortionEquation<SampleType>::identity(x, drive, warp); };
            break;
        }
    }

    static std::function<SampleType(SampleType, SampleType, SampleType)> asymetricIDToEquation(int equationID)
    {
        switch (equationID)
        {
        case DistortionCircuitConstants::Processor<SampleType>::AsymetricEquationID::none:
            return [](SampleType x, SampleType drive, SampleType warp) {return DistortionEquation<SampleType>::identity(x, drive, warp); };
            break;
        case DistortionCircuitConstants::Processor<SampleType>::AsymetricEquationID::tubeSimulation:
            return [](SampleType x, SampleType drive, SampleType warp) {return DistortionEquation<SampleType>::Asymetric::tubeSimulation(x, drive, warp); };
            break;
        case DistortionCircuitConstants::Processor<SampleType>::AsymetricEquationID::distortionSimulation:
            return [](SampleType x, SampleType drive, SampleType warp) {return DistortionEquation<SampleType>::Asymetric::distortionSimulation(x, drive, warp); };
            break;
        case DistortionCircuitConstants::Processor<SampleType>::AsymetricEquationID::diode :
            return [](SampleType x, SampleType drive, SampleType warp) {return DistortionEquation<SampleType>::Asymetric::diode(x, drive, warp); };
            break;
        case DistortionCircuitConstants::Processor<SampleType>::AsymetricEquationID::diode2:
            return [](SampleType x, SampleType drive, SampleType warp) {return DistortionEquation<SampleType>::Asymetric::diode2(x, drive, warp); };
            break;

        default:
            return [](SampleType x, SampleType drive, SampleType warp) {return DistortionEquation<SampleType>::identity(x, drive, warp); };
            break;
        }
    }

    static std::function<SampleType(SampleType, SampleType, SampleType)> specialIDToEquation(int equationID)
    {
        switch (equationID)
        {
        case DistortionCircuitConstants::Processor<SampleType>::SpecialEquationID::none:
            return [](SampleType x, SampleType drive, SampleType warp) {return DistortionEquation<SampleType>::identity(x, drive, warp); };
            break;
        case DistortionCircuitConstants::Processor<SampleType>::SpecialEquationID::halfrect:
            return [](SampleType x, SampleType drive, SampleType warp) {return DistortionEquation<SampleType>::Special::halfrect(x, drive, warp); };
            break;

        case DistortionCircuitConstants::Processor<SampleType>::SpecialEquationID::fullrect:
            return [](SampleType x, SampleType drive, SampleType warp) {return DistortionEquation<SampleType>::Special::fullrect(x, drive, warp); };
            break;
        default:
            return [](SampleType x, SampleType drive, SampleType warp) {return DistortionEquation<SampleType>::identity(x, drive, warp); };
            break;
        }

    };

    //static void createValueTree(juce::ValueTree& vt)//, int distortionUnitID)
    //{
    //    juce::ValueTree vtDistortionCircuit(DistortionCircuitConstants::ParamsID::distortionCircuit);
    //    vt.addChild(vtDistortionCircuit, -1, nullptr);

    //    vtDistortionCircuit.setProperty(DistortionCircuitConstants::ParamsID::equationType, 0, nullptr);
    //    vtDistortionCircuit.setProperty(DistortionCircuitConstants::ParamsID::equationID, 0, nullptr);
    //};

private:
    //==============================================================================
    enum
    {
        preGainIndex,
        waveshaperIndex,
        postGainIndex
    };

    //Unsed--------------------------------------------------------------------------------------------------------------------
    //juce::dsp::ProcessorChain<juce::dsp::Gain<float>, juce::dsp::WaveShaper<float>, juce::dsp::Gain<float>> processorChain; // [1]

    //std::function<float(float, float)> distortionEquation;

    std::function<SampleType(SampleType, SampleType, SampleType)> distortionEquation;

    //juce::dsp::DryWetMixer<float> dryWet;

    juce::SmoothedValue<SampleType> drive;
    juce::SmoothedValue<SampleType> warp;

    SampleType mix = 1.0f;

    bool isBypassed = false;

    double sampleRate = 44100.0;
    double rampDurationSeconds = 0.0;
    //==============================================================================
    // Although clang supports C++17, their standard library still has no invoke_result
    // support. Remove the "|| JUCE_CLANG" once clang supports this properly!
    //#if (! JUCE_CXX17_IS_AVAILABLE) || (JUCE_CLANG && ! JUCE_WINDOWS)
    //    template <typename Functor>
    //    static CustomWaveShaper<typename std::result_of<Functor>, Functor> CreateWaveShaper(Functor functionToUse) { return { functionToUse }; }
    //#else
    //    template <typename Functor>
    //    static CustomWaveShaper<typename std::invoke_result<Functor>, Functor> CreateWaveShaper(Functor functionToUse) { return { functionToUse }; }
    //#endif
};
