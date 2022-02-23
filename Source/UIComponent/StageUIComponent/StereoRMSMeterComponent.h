/*
  ==============================================================================

    StereoRMSMeterComponent.h
    Created: 18 Nov 2021 10:57:19pm
    Author:  CookiMonstor

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "RMSMeter.h"
#include "../Controls/MangledLabel.h"
#include "../Controls/MangledSlider.h"
//==============================================================================
/*
*/
class StereoRMSMeterComponent  : public juce::Component
{
public:
    StereoRMSMeterComponent(juce::String initString);
    ~StereoRMSMeterComponent() override;

    void paint (juce::Graphics&) override;
    void resized() override;
    void setLeftRMSValue(float rmsValue)
    {
        juce::String stg = rmsValue < 0 ? (juce::String)rmsValue : "+" + (juce::String)rmsValue;
        int length = stg.length() - 5;
        stg = stg.dropLastCharacters(length);

        float rmsScaled = juce::jmap(rmsValue, -100.f, 12.0f, static_cast<float>(leftRMSMeter.getHeight()), 0.0f);
        double niquetamere = static_cast<double>(leftRMSMeter.getHeight());
        double colourPos = juce::jmap(static_cast<double>(rmsScaled), niquetamere, 0.0, 0.0, 1.0);
        leftRMSLabel.setColour(juce::Label::ColourIds::textColourId, gradient.getColourAtPosition(colourPos));
        leftRMSLabel.setText(stg, juce::dontSendNotification);
        leftRMSMeter.setRMSValue(rmsValue);
    };

    void setRightRMSValue(float rmsValue)
    {
        juce::String stg = rmsValue < 0 ? (juce::String)rmsValue : "+" + (juce::String)rmsValue;
        int length = stg.length() - 5;
        stg = stg.dropLastCharacters(length);

        float rmsScaled = juce::jmap(rmsValue, -100.f, 12.0f, static_cast<float>(rightRMSMeter.getHeight()), 0.0f);
        double colourPos = juce::jmap(static_cast<double>(rmsScaled), static_cast<double>(rightRMSMeter.getHeight()), 0.0, 0.0, 1.0);
        rightRMSLabel.setColour(juce::Label::ColourIds::textColourId, gradient.getColourAtPosition(colourPos));
        rightRMSLabel.setText(stg, juce::dontSendNotification);
        rightRMSMeter.setRMSValue(rmsValue);
    };

    void setLeftPeakValue(float peakValue)
    {
        juce::String stg = peakValue < 0 ? (juce::String)peakValue : "+" + (juce::String)peakValue;
        int length = stg.length() - 5;
        stg = stg.dropLastCharacters(length);

        float peakScaled = juce::jmap(peakValue, -100.f, 12.0f, static_cast<float>(leftRMSMeter.getHeight()), 0.0f);
        double colourPos = juce::jmap(static_cast<double>(peakScaled), static_cast<double>(leftRMSMeter.getHeight()), 0.0, 0.0, 1.0);
        leftPeakLabel.setColour(juce::Label::ColourIds::textColourId, gradient.getColourAtPosition(colourPos));
        leftPeakLabel.setText(stg, juce::dontSendNotification);
        leftRMSMeter.setPeakValue(peakValue);
    };

    void setRightPeakValue(float peakValue)
    {
        juce::String stg = peakValue < 0 ? (juce::String)peakValue : "+" + (juce::String)peakValue;
        int length = stg.length() - 5;
        stg = stg.dropLastCharacters(length);

        float peakScaled = juce::jmap(peakValue, -100.f, 12.0f, static_cast<float>(rightRMSMeter.getHeight()), 0.0f);
        double colourPos = juce::jmap(static_cast<double>(peakScaled), static_cast<double>(rightRMSMeter.getHeight()), 0.0, 0.0, 1.0);
        rightPeakLabel.setColour(juce::Label::ColourIds::textColourId, gradient.getColourAtPosition(colourPos));
        rightPeakLabel.setText(stg, juce::dontSendNotification);
        rightRMSMeter.setPeakValue(peakValue);
    };

    RMSMeter* getLeftRMSMeter() { return &leftRMSMeter; };
    RMSMeter* getRightRMSMeter() { return &rightRMSMeter; };
    MangledSlider* getGainSlider() { return &gainSlider; };

private:
    MangledLabel gainLabel;

    juce::Label leftPeakLabel;
    juce::Label rightPeakLabel;

    juce::Label leftRMSLabel;
    juce::Label rightRMSLabel;

    RMSMeter leftRMSMeter;
    RMSMeter rightRMSMeter;

    MangledSlider gainSlider;

    juce::ColourGradient gradient;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (StereoRMSMeterComponent)
};
