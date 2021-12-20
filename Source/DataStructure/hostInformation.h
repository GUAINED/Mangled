/*
  ==============================================================================

    hostInformation.h
    Created: 11 May 2021 1:31:00pm
    Author:  CookiMonstor

  ==============================================================================
*/

#pragma once
#include<JuceHeader.h>

class hostInformation 
{
public:
    void setPlayHead(juce::AudioPlayHead* newPlayHead);
    void setSampleRate(double newSampleRate) { sampleRate = newSampleRate; };

    double& getBPM() { return bpm; };
    double& getSampleRate() { return sampleRate; };
    juce::AudioPlayHead* getPlayHead() { return playHead; };
    juce::AudioPlayHead::CurrentPositionInfo getHostInfo() { return hostInfo; };
private:
    juce::AudioPlayHead* playHead = nullptr;
    juce::AudioPlayHead::CurrentPositionInfo hostInfo;
    double bpm = 0.0;
    double sampleRate = 44100.0;
};