/*
  ==============================================================================

    Conversion.h
    Created: 16 Nov 2021 2:09:15pm
    Author:  CookiMonstor

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

template <typename SampleType>
class Conversion
{
public:
    static inline SampleType fromMouseToCutoff(SampleType mouseXPosition, SampleType xPosCutoffMin, SampleType xPosCutoffMax, SampleType freqMin, SampleType freqMax) noexcept
    {
        SampleType cutoff = static_cast<SampleType>(0.0f);
        SampleType xPos = juce::jmap(mouseXPosition,
                                    xPosCutoffMin,
                                    xPosCutoffMax,
                                    static_cast<SampleType>(0.0),
                                    static_cast<SampleType>(1.0));

        cutoff = juce::mapToLog10(xPos, freqMin, freqMax);

        return cutoff;
    };

    static inline SampleType fromMouseToGain(SampleType mouseYPosition, SampleType yPosGainMin, SampleType yPosGainMax, SampleType gainMin, SampleType gainMax) noexcept
    {
        SampleType gain = static_cast<SampleType>(0.0);

        gain = juce::jmap(mouseYPosition,
            yPosGainMin,
            yPosGainMax,
            gainMin,
            gainMax);

        return gain;
    };

    static inline SampleType fromMouseToQ(SampleType mouseYPosition, SampleType yPosQMin, SampleType yPosQMax, SampleType qMin, SampleType qMax) noexcept
    {
        SampleType q = static_cast<SampleType>(0.0);
        SampleType yPos = juce::jmap(mouseYPosition,
            yPosQMin,
            yPosQMax,
            static_cast<SampleType>(0.0),
            static_cast<SampleType>(1.0));

        q = juce::mapToLog10(yPos, qMin, qMax);

        return q;
    };

    static inline SampleType fromCutoffToXPosition(SampleType cutoff, SampleType freqMin, SampleType freqMax, SampleType xPosCutoffMax, SampleType xPosCutoffMin) noexcept
    {
        SampleType xPosition = static_cast<SampleType>(0.0);
        //SampleType width = xPosCutoffMax - static_cast<SampleType>(100.0);
        //SampleType logMin = std::log10(freqMin);
        //SampleType a = std::log10(cutoff) - logMin;
        //SampleType b = width;
        //SampleType c = std::log10(freqMax) - logMin;

        //xPosition = a * b / c;

        xPosition = juce::mapFromLog10(cutoff, freqMin, freqMax);
        xPosition = juce::jmap(xPosition, 
            static_cast<SampleType>(0.0),
            static_cast<SampleType>(1.0),
            xPosCutoffMin,
            xPosCutoffMax);
        return xPosition;
    };

    static inline SampleType fromGainToYPosition(SampleType gain, SampleType gainMin, SampleType gainMax, SampleType yPosGainMin, SampleType yPosGainMax) noexcept
    {
        SampleType yPosition = static_cast<SampleType>(0.0);

        yPosition = juce::jmap(gain,
                               gainMin,
                               gainMax,
                               yPosGainMin,
                               yPosGainMax);

        return yPosition;
    };

    static inline SampleType fromQToYPositon(SampleType q, SampleType qMin, SampleType qMax, SampleType yPosQMin, SampleType yPosQMax) noexcept
    {
        SampleType yPosition = juce::mapFromLog10(q, qMin, qMax);
        yPosition = juce::jmap(yPosition,
                               static_cast<SampleType>(0.0),
                               static_cast<SampleType>(1.0),
                               yPosQMin,
                               yPosQMax);

        return yPosition;
    };
};