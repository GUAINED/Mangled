/*
  ==============================================================================

    PhaserProcessor.cpp
    Created: 2 Apr 2021 4:56:59pm
    Author:  CookiMonstor

  ==============================================================================
*/

#include "PhaserProcessor.h"

//==============================================================================
template <typename SampleType>
CustomPhaser<SampleType>::CustomPhaser()
{
    //auto oscFunction = [](SampleType x) { return std::sin(x); };
    //osc.initialise(oscFunction);

    phaseShifterFreq.resize(maxNumStages);

    nbOfPhaseShifter = 8;

    for (auto n = 0; n < nbOfPhaseShifter; ++n)
    {
        phaseShifterFilters.add(new PhaseShifterFilter<SampleType>());
        //phaseShifterFilters[n]->setType(juce::dsp::FirstOrderTPTFilterType::allpass);
    }

    dryWet.setMixingRule(juce::dsp::DryWetMixingRule::linear);

    //customPhaserParams.rateHz = rate;
    //customPhaserParams.depth = depth;
    //customPhaserParams.centreHz = centreFrequency;
    //customPhaserParams.feedback = feedback;
    //customPhaserParams.numStages = numStages;
}

template <typename SampleType>
void CustomPhaser<SampleType>::setRate(SampleType newRateHz)
{
    jassert(juce::isPositiveAndBelow(newRateHz, static_cast<SampleType> (100.0)));

    //rate = newRateHz;
    //customPhaserParams.rateHz = rate;
    //update();
}

template <typename SampleType>
void CustomPhaser<SampleType>::setDepth(SampleType newDepth)
{
    jassert(juce::isPositiveAndNotGreaterThan(newDepth, static_cast<SampleType> (1.0)));

    //depth = newDepth;
    //customPhaserParams.depth = depth;
    //update();
}

template <typename SampleType>
void CustomPhaser<SampleType>::setCentreFrequency(SampleType newCentreHz)
{
    jassert(juce::isPositiveAndBelow(newCentreHz, static_cast<SampleType> (sampleRate * 0.5)));

    centreFrequency = newCentreHz;
    normCentreFrequency = juce::mapFromLog10(centreFrequency, static_cast<SampleType> (20.0), static_cast<SampleType> (juce::jmin(20000.0, 0.49 * sampleRate)));
    //customPhaserParams.centreHz = centreFrequency;

    //int halfPhSh = nbOfPhaseShifter / 2;
    //SampleType freq = static_cast<SampleType>(0.5);
    //Test 1 meh
    
    //for (auto filterID = 0; filterID < halfPhSh; ++filterID)
    //{
    //    freq = ((float)(filterID + 1)) * normCentreFrequency / (halfPhSh + 1);
    //    phaseShifterFreq[filterID] = freq;

    //    freq = juce::mapToLog10(freq, static_cast<SampleType> (20.0),
    //                static_cast<SampleType> (juce::jmin(20000.0, 0.49 * sampleRate)));
    //    
    //    jassert(juce::isPositiveAndBelow(freq, static_cast<SampleType> (sampleRate * 0.5)));
    //    phaseShifterFilters[filterID]->setCutoffFrequency(freq);
    //}

    //int counter = 0;
    //for (auto filterID = halfPhSh; filterID < nbOfPhaseShifter; ++filterID)
    //{
    //    freq = ((float)(counter + 1)) * (static_cast<SampleType>(1.0) - normCentreFrequency) / (halfPhSh + 1) + normCentreFrequency;
    //    phaseShifterFreq[filterID] = freq;
    //    
    //    freq = juce::mapToLog10(freq, static_cast<SampleType> (20.0),
    //        static_cast<SampleType> (juce::jmin(20000.0, 0.49 * sampleRate)));
    //    
    //    jassert(juce::isPositiveAndBelow(freq, static_cast<SampleType> (sampleRate * 0.5)));
    //    phaseShifterFilters[filterID]->setCutoffFrequency(freq);

    //    counter++;
    //}

    //Test 2 nope
    //for (int filterID = 0; filterID < nbOfPhaseShifter; ++filterID)
    //{
    //    phaseShifterFilters[filterID]->setCutoffFrequency(std::pow(2.0f, filterID) * newCentreHz);
    //}
    // 
    //phaseShifterFilters[0]->setCutoffFrequency(newCentreHz);
    //phaseShifterFilters[1]->setCutoffFrequency(2.0f*newCentreHz);
    //phaseShifterFilters[2]->setCutoffFrequency(4.0f * newCentreHz);
    //phaseShifterFilters[3]->setCutoffFrequency(8.0f * newCentreHz);

    SampleType f1 = static_cast<SampleType>(0.0);
    SampleType f2 = static_cast<SampleType>(0.0);
    SampleType f3 = static_cast<SampleType>(0.0);
    SampleType f4 = static_cast<SampleType>(0.0);

    //Test 3
    switch(nbOfPhaseShifter)
    {
        case 1:
            phaseShifterFilters[0]->setCutoffFrequency(newCentreHz);
            break;

        case 2:
            f1 = juce::jlimit(static_cast<SampleType>(PhaserConstants::Processor<SampleType>::cfreqLimMin),
                              static_cast<SampleType>(PhaserConstants::Processor<SampleType>::cfreqLimMax),
                              centreFrequency / static_cast<SampleType>(2.0));
            f2 = juce::jlimit(static_cast<SampleType>(PhaserConstants::Processor<SampleType>::cfreqLimMin),
                static_cast<SampleType>(PhaserConstants::Processor<SampleType>::cfreqLimMax),
                centreFrequency * static_cast<SampleType>(2.0));
            phaseShifterFilters[0]->setCutoffFrequency(f1);
            phaseShifterFilters[1]->setCutoffFrequency(f2);
            break;
        case 3:
            f1 = juce::jlimit(static_cast<SampleType>(PhaserConstants::Processor<SampleType>::cfreqLimMin),
                static_cast<SampleType>(PhaserConstants::Processor<SampleType>::cfreqLimMax),
                centreFrequency / static_cast<SampleType>(3.0));
            f2 = juce::jlimit(static_cast<SampleType>(PhaserConstants::Processor<SampleType>::cfreqLimMin),
                static_cast<SampleType>(PhaserConstants::Processor<SampleType>::cfreqLimMax),
                centreFrequency * static_cast<SampleType>(3.0));
            phaseShifterFilters[0]->setCutoffFrequency(f1);
            phaseShifterFilters[1]->setCutoffFrequency(centreFrequency);
            phaseShifterFilters[2]->setCutoffFrequency(f2);
            break;
        case 4:
            f1 = juce::jlimit(static_cast<SampleType>(PhaserConstants::Processor<SampleType>::cfreqLimMin),
                static_cast<SampleType>(PhaserConstants::Processor<SampleType>::cfreqLimMax),
                centreFrequency / static_cast<SampleType>(4.0));
            f2 = juce::jlimit(static_cast<SampleType>(PhaserConstants::Processor<SampleType>::cfreqLimMin),
                static_cast<SampleType>(PhaserConstants::Processor<SampleType>::cfreqLimMax),
                centreFrequency / static_cast<SampleType>(2.0));
            f3 = juce::jlimit(static_cast<SampleType>(PhaserConstants::Processor<SampleType>::cfreqLimMin),
                static_cast<SampleType>(PhaserConstants::Processor<SampleType>::cfreqLimMax),
                centreFrequency * static_cast<SampleType>(2.0));
            f4 = juce::jlimit(static_cast<SampleType>(PhaserConstants::Processor<SampleType>::cfreqLimMin),
                static_cast<SampleType>(PhaserConstants::Processor<SampleType>::cfreqLimMax),
                centreFrequency * static_cast<SampleType>(4.0));
            phaseShifterFilters[0]->setCutoffFrequency(f1);
            phaseShifterFilters[1]->setCutoffFrequency(f2);
            phaseShifterFilters[2]->setCutoffFrequency(f3);
            phaseShifterFilters[3]->setCutoffFrequency(f4);
            break;
        default:
            phaseShifterFilters[0]->setCutoffFrequency(centreFrequency);
            phaseShifterFilters[1]->setCutoffFrequency(centreFrequency);
            phaseShifterFilters[2]->setCutoffFrequency(centreFrequency);
            phaseShifterFilters[3]->setCutoffFrequency(centreFrequency);
            break;
    }

}

template <typename SampleType>
void CustomPhaser<SampleType>::setFeedback(SampleType newFeedback)
{
    jassert(newFeedback >= static_cast<SampleType> (-1.0) && newFeedback <= static_cast<SampleType> (1.0));

    feedback = newFeedback;
    //customPhaserParams.feedback = feedback;
    //update();
}

template <typename SampleType>
void CustomPhaser<SampleType>::setMix(SampleType newMix)
{
    jassert(juce::isPositiveAndNotGreaterThan(newMix/ static_cast<SampleType> (100.0), static_cast<SampleType> (1.0)));

    mix = newMix / static_cast<SampleType> (100.0);
    mix = juce::jmap(mix,
        static_cast<SampleType> (0.0),
        static_cast<SampleType> (1.0),
        static_cast<SampleType> (0.0),
        static_cast<SampleType> (0.5));
    dryWet.setWetMixProportion(mix);
    //update();
}

template <typename SampleType>
void CustomPhaser<SampleType>::setNumStages(const int newNumStages)
{
    numStages = (newNumStages * 2) + 2; //This function map 0 - 1 - 2 - 3 onto 2 - 4 - 6 - 8;
    nbOfPhaseShifter = newNumStages + 1;// (newNumStages * 2) + 2; //
}

template <typename SampleType>
void CustomPhaser<SampleType>::setIsBypassed(const bool newIsBypassed)
{
    isBypassed = newIsBypassed;
}

template <typename SampleType>
void CustomPhaser<SampleType>::updateState()
{
    update();
}

//==============================================================================
template <typename SampleType>
void CustomPhaser<SampleType>::prepare(const juce::dsp::ProcessSpec& spec)
{
    jassert(spec.sampleRate > 0);
    jassert(spec.numChannels > 0);

    sampleRate = spec.sampleRate;

    for (auto n = 0; n < nbOfPhaseShifter; ++n)
        phaseShifterFilters[n]->prepare(spec);

    dryWet.prepare(spec);
    feedbackVolume.resize(spec.numChannels);
    lastOutput.resize(spec.numChannels);

    auto specDown = spec;
    specDown.sampleRate /= (double)maxUpdateCounter;
    specDown.maximumBlockSize = specDown.maximumBlockSize / (juce::uint32)maxUpdateCounter + 1;

    //osc.prepare(specDown);
    //bufferFrequency.setSize(1, (int)specDown.maximumBlockSize, false, false, true);

    update();
    reset();
}

template <typename SampleType>
void CustomPhaser<SampleType>::reset()
{
    std::fill(lastOutput.begin(), lastOutput.end(), static_cast<SampleType> (0));

    for (auto n = 0; n < nbOfPhaseShifter; ++n)
        phaseShifterFilters[n]->reset();

    //osc.reset();
    dryWet.reset();

    //oscVolume.reset(sampleRate / (double)maxUpdateCounter, 0.05);

    for (auto& vol : feedbackVolume)
        vol.reset(sampleRate, 0.05);

    updateCounter = 0;
}

template <typename SampleType>
void CustomPhaser<SampleType>::update()
{
    //osc.setFrequency(rate);
    //oscVolume.setTargetValue(depth * (SampleType)0.5);
    dryWet.setWetMixProportion(mix);

    for (auto& vol : feedbackVolume)
        vol.setTargetValue(feedback);
}

//template <typename SampleType>
//CustomPhaser<SampleType>::CustomPhaserParams& CustomPhaser<SampleType>::getPhaserParams()
////void CustomPhaser<SampleType>::getPhaserParams()
//{
//    //CustomPhaser<SampleType>::CustomPhaserParams phaserProcessorParams;
//
//    //phaserProcessorParams.rateHz = rateHz;
//    //phaserProcessorParams.depth = depth;
//    //phaserProcessorParams.centreHz = centreHz;
//    //phaserProcessorParams.feedback = feedback;
//    //phaserProcessorParams.numStages = numStages;
//
//    //return &phaserProcessorParams;
//}

template <typename SampleType>
 void CustomPhaser<SampleType>::setPhaserParams(CustomPhaser<SampleType>::Parameters& newParams)
{
     if (params != newParams)
     {
         params = newParams;

         setNumStages(newParams.numStages);

         setCentreFrequency(newParams.centreHz);

         setFeedback(newParams.feedback);

         setMix(newParams.mix);

         setIsBypassed(newParams.isBypassed);
    }
}



//==============================================================================
template class CustomPhaser<float>;
template class CustomPhaser<double>;