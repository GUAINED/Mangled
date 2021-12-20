/*
  ==============================================================================

    RMSMeter.h
    Created: 18 Nov 2021 8:30:15pm
    Author:  CookiMonstor

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
/*
*/
class RMSMeter  : public juce::Component
{
public:
    RMSMeter(juce::ColourGradient& grad);
    ~RMSMeter() override;

    void paint (juce::Graphics&) override;
    void resized() override;

    void setRMSValue (float newValue) { rmsValue = newValue; };
    void setPeakValue(float newValue) { peakValue = newValue; };
private:
    float peakValue;
    float rmsValue;
	//std::function<float()> valueSupplier;
	juce::ColourGradient& gradient;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (RMSMeter)
};
//class VerticalGradientMeter : public Component, public Timer
//{
//public:
//	VerticalGradientMeter(std::function<float()>&& valueFunction) : valueSupplier(std::move(valueFunction))
//	{
//		startTimerHz(24);
//		grill = ImageCache::getFromMemory(BinaryData::MeterGrill_png, BinaryData::MeterGrill_pngSize);
//	}
//
//	void paint(Graphics& g) override
//	{
//
//	}
//
//	void resized() override
//	{
//
//	}
//
//	void paintOverChildren(::juce::Graphics& g) override
//	{
//		g.drawImage(grill, getLocalBounds().toFloat());
//	}
//
//	void timerCallback() override
//	{
//		repaint();
//	}
//private:
//
//	Image grill;
//};