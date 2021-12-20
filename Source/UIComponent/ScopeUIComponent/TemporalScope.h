/*
  ==============================================================================

    TemporalScope.h
    Created: 3 Dec 2021 5:12:34pm
    Author:  CookiMonstor

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "../../DataStructure/MainLayerDataStruct.h"
#include "TemporalScopeDisplay.h"
#include "../GeneralUIComponent/OnOffButton.h"
//==============================================================================
/*
*/
class TemporalScope  : public juce::Component
                     
{
public:
    TemporalScope();
    ~TemporalScope() override;

    void paint (juce::Graphics&) override;
    void resized() override;

    void computeGrid(int subdivision = 4);
    void switchScope(bool buttonState);
    void computeStereoPath();
    void computeMonoPath();
    void setBPM(double newBPM) 
    { 
        bpm = newBPM;
        leftChannelScope.bpm = newBPM;
        rightChannelScope.bpm = newBPM;
        monoScope.bpm = newBPM;
    };
    void setPPQ(double newPPQ)
    { 
        ppq = newPPQ;
        leftChannelScope.ppq = newPPQ;
        rightChannelScope.ppq = newPPQ;
        monoScope.ppq = newPPQ;
    };
    void setPPQSample(juce::int64 newPPQSample)
    { 
        ppqSample = newPPQSample;
        leftChannelScope.ppqSample = newPPQSample;
        rightChannelScope.ppqSample = newPPQSample;
        monoScope.ppqSample = newPPQSample;
    };
    void setPreviousPPQSample(juce::int64 newPreviousPPQSample)
    {
        previousPPQSample = newPreviousPPQSample;
        leftChannelScope.previousPPQSample = newPreviousPPQSample;
        rightChannelScope.previousPPQSample = newPreviousPPQSample;
        monoScope.previousPPQSample = newPreviousPPQSample;
    };
    void setBufferSize(int newBufferSize)
    { 
        bufferSize = newBufferSize;
        leftChannelScope.bufferSize = newBufferSize;
        rightChannelScope.bufferSize = newBufferSize;
        monoScope.bufferSize = newBufferSize;
    };
    void setNbOfSamplesPerBeat(int newNbSamplePerBeat)
    {
        nbSamplePerBeat = newNbSamplePerBeat;
        leftChannelScope.nbSamplePerBeat = newNbSamplePerBeat;
        rightChannelScope.nbSamplePerBeat = newNbSamplePerBeat;
        monoScope.nbSamplePerBeat = newNbSamplePerBeat;
    };
    void setIsNormalized(bool newIsNormalized)
    {
        isNormalized = newIsNormalized;
        leftChannelScope.isNormalized = newIsNormalized;
        rightChannelScope.isNormalized = newIsNormalized;
        monoScope.isNormalized = newIsNormalized;
    };

    juce::AudioBuffer<float>* getInputTemporalBuffer() { return &inputTemporalBuffer; };
    juce::AudioBuffer<float>* getOutputTemporalBuffer() { return &outputTemporalBuffer; };

    TemporalScopeDisplay* getLeftScope() { return &leftChannelScope; };
    TemporalScopeDisplay* getRightScope() { return &rightChannelScope; };
    StereoToMonoTemporalScopeDisplay* getMonoScope() { return &monoScope; };


private:

    juce::AudioBuffer<float> inputTemporalBuffer;
    juce::AudioBuffer<float> outputTemporalBuffer;

    double bpm = 120.0;
    double ppq = 0.0;
    juce::int64 ppqSample = 0;
    juce::int64 previousPPQSample = 0;
    int bufferSize = 0;
    int nbSamplePerBeat = 22050;
    bool isNormalized = false;
    TemporalScopeDisplay leftChannelScope;
    juce::Path leftInputPath;
    juce::Path leftOutputPath;
    TemporalScopeDisplay rightChannelScope;
    juce::Path rightInputPath;
    juce::Path rightOutputPath;
    StereoToMonoTemporalScopeDisplay monoScope;
    juce::Path monoInputPath;
    juce::Path monoOutputPath;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TemporalScope)
};
