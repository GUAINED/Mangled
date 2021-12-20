/*
  ==============================================================================

    TemporalScopeDisplay.h
    Created: 3 Dec 2021 8:53:59pm
    Author:  CookiMonstor

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
/*
*/
class TemporalScopeDisplay  : public juce::Component
{
public:
    TemporalScopeDisplay(const juce::AudioBuffer<float>& inBuffer, const juce::AudioBuffer<float>& outBuffer, int chID, juce::Path& inPath, juce::Path& outPath);
    ~TemporalScopeDisplay() override;

    void paint (juce::Graphics&) override;
    void resized() override;

    void computeGrid(int subdivisivon = 4);
    virtual void computeTemporalPath(juce::Graphics& g);
    void computeTemporalPathPerFrame(juce::Graphics& g);
    const juce::AudioBuffer<float>& inputBuffer;
    const juce::AudioBuffer<float>& outputBuffer;

    //Data
    double bpm = 120.0;
    double ppq = 0.0;
    juce::int64 ppqSample = 0;
    juce::int64 previousPPQSample = 0;
    int bufferSize = 0;
    int nbSamplePerBeat = 22050;

    int channelID = 0;

    juce::Path grid;
    juce::Path innerGrid;

    bool isNormalized = false;
    juce::ColourGradient inputGradient;
    juce::ColourGradient outputGradient;

    juce::Path& inputPath;
    juce::Path& outputPath;
private:
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TemporalScopeDisplay)
};

//class StereoToMonoTemporalScopeDisplay : public juce::Component
class StereoToMonoTemporalScopeDisplay : public TemporalScopeDisplay
{
public:
    StereoToMonoTemporalScopeDisplay(const juce::AudioBuffer<float>& inBuffer, const juce::AudioBuffer<float>& outBuffer, juce::Path& inPath, juce::Path& outPath);
    ~StereoToMonoTemporalScopeDisplay() override;

    void computeTemporalPath(juce::Graphics& g) override;

private:

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(StereoToMonoTemporalScopeDisplay)
};