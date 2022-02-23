/*
  ==============================================================================

    CurveEquation.h
    Created: 29 Oct 2021 3:18:27pm
    Author:  CookiMonstor

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "../DataStructure/MangledIdentifiers.h"

template <typename SampleType>
class CurveEquation
{
public:
    static inline SampleType identity(SampleType x, SampleType tension, SampleType leftX, SampleType leftY, SampleType rightX, SampleType rightY)
    {
        return x;
    }

    static inline SampleType polynomialOverOne(SampleType x, SampleType tension, SampleType leftX, SampleType leftY, SampleType rightX, SampleType rightY)
    {
        x = juce::jmap(x, leftX, rightX, static_cast<SampleType>(0.0), static_cast<SampleType>(1.0));

        x = std::pow(x, tension);

        x = juce::jmap(x, static_cast<SampleType>(0.0), static_cast<SampleType>(1.0), leftY, rightY);

        return x;
    }

    static inline SampleType polynomialBelowOne(SampleType x, SampleType tension, SampleType leftX, SampleType leftY, SampleType rightX, SampleType rightY)
    {
        x = juce::jmap(x, leftX, rightX, static_cast<SampleType>(0.0), static_cast<SampleType>(1.0));

        x = static_cast<SampleType>(1.0) - std::pow((static_cast<SampleType>(1.0) - x), static_cast<SampleType>(1.0) / tension);

        x = juce::jmap(x, static_cast<SampleType>(0.0), static_cast<SampleType>(1.0), leftY, rightY);

        return x;
    }

    static inline SampleType vertical(SampleType x, SampleType tension, SampleType leftX, SampleType leftY, SampleType rightX, SampleType rightY)
    {
        return (leftY - rightY) * static_cast<SampleType>(0.5);
    }
    static inline SampleType horizontal(SampleType x, SampleType tension, SampleType leftX, SampleType leftY, SampleType rightX, SampleType rightY)
    {
        return leftY;
    }
    static inline SampleType arcsinhOverOne(SampleType x, SampleType tension, SampleType leftX, SampleType leftY, SampleType rightX, SampleType rightY)
    {
        SampleType cste = PiecewiseFunctionConstants::Processor<SampleType>::aSinHCurveCste;

        x = juce::jmap(x, leftX, rightX, static_cast<SampleType>(0.0), static_cast<SampleType>(1.0));

        x = cste * std::asinh(std::pow(x, tension));

        x = juce::jmap(x, static_cast<SampleType>(0.0), static_cast<SampleType>(1.0), leftY, rightY);

        return x;
    }

    static inline SampleType arcsinhBelowOne(SampleType x, SampleType tension, SampleType leftX, SampleType leftY, SampleType rightX, SampleType rightY)
    {
        SampleType cste = PiecewiseFunctionConstants::Processor<SampleType>::aSinHCurveCste;

        x = juce::jmap(x, leftX, rightX, static_cast<SampleType>(0.0), static_cast<SampleType>(1.0));

        x = static_cast<SampleType>(1.0) - cste * std::asinh(std::pow(static_cast<SampleType>(1.0) - x, static_cast<SampleType>(1.0) / tension));;

        x = juce::jmap(x, static_cast<SampleType>(0.0), static_cast<SampleType>(1.0), leftY, rightY);

        return x;
    }

    static inline SampleType doubleCurveOverZero(SampleType x, SampleType tension, SampleType leftX, SampleType leftY, SampleType rightX, SampleType rightY)
    {
        SampleType tensionMin = PiecewiseFunctionConstants::Processor<SampleType>::tensionMin;
        SampleType tensionMax = PiecewiseFunctionConstants::Processor<SampleType>::tensionMax;
        SampleType midX = (rightX + leftX) * static_cast<SampleType>(0.5);
        SampleType midY = (rightY + leftY) * static_cast<SampleType>(0.5);
        SampleType tensionRemap = juce::jmap(tension, static_cast<SampleType>(-0.5), static_cast<SampleType>(0.5), static_cast<SampleType>(0.0), static_cast<SampleType>(1.0));
        tensionRemap = juce::mapToLog10(tensionRemap, tensionMin, tensionMax);

        if (x <= (rightX + leftX) * static_cast<SampleType>(0.5))
        {
            x = juce::jmap(x, leftX, midX, static_cast<SampleType>(0.0), static_cast<SampleType>(1.0));

            x = std::pow(x, tensionRemap);

            x = juce::jmap(x, static_cast<SampleType>(0.0), static_cast<SampleType>(1.0), leftY, midY);
        }
        else
        {
            x = juce::jmap(x, midX, rightX, static_cast<SampleType>(0.0), static_cast<SampleType>(1.0));

            x = static_cast<SampleType>(1.0) - std::pow(static_cast<SampleType>(1.0) - x,  tensionRemap);

            x = juce::jmap(x, static_cast<SampleType>(0.0), static_cast<SampleType>(1.0), midY, rightY);
        }

        return x;
    }

    static inline SampleType doubleCurveBelowZero(SampleType x, SampleType tension, SampleType leftX, SampleType leftY, SampleType rightX, SampleType rightY)
    {
        SampleType tensionMin = PiecewiseFunctionConstants::Processor<SampleType>::tensionMin;
        SampleType tensionMax = PiecewiseFunctionConstants::Processor<SampleType>::tensionMax;
        SampleType midX = (rightX + leftX) * static_cast<SampleType>(0.5);
        SampleType midY = (rightY + leftY) * static_cast<SampleType>(0.5);
        SampleType tensionRemap = juce::jmap(tension, static_cast<SampleType>(-0.5), static_cast<SampleType>(0.5), static_cast<SampleType>(0.0), static_cast<SampleType>(1.0));
        tensionRemap = juce::mapToLog10(tensionRemap, tensionMin, tensionMax);

        if (x <= (rightX + leftX) * static_cast<SampleType>(0.5))
        {
            x = juce::jmap(x, leftX, midX, static_cast<SampleType>(0.0), static_cast<SampleType>(1.0));

            x = std::pow(x, tensionRemap);
            
            x = juce::jmap(x, static_cast<SampleType>(0.0), static_cast<SampleType>(1.0), leftY, midY);
        }
        else
        {
            x = juce::jmap(x, midX, rightX, static_cast<SampleType>(0.0), static_cast<SampleType>(1.0));
            
            x = static_cast<SampleType>(1.0) - std::pow(static_cast<SampleType>(1.0) - x, tensionRemap);

            x = juce::jmap(x, static_cast<SampleType>(0.0), static_cast<SampleType>(1.0), midY, rightY);
        }

        return x;
    }

    static inline SampleType square(SampleType x, SampleType tension, SampleType leftX, SampleType leftY, SampleType rightX, SampleType rightY)
    {
        x = juce::jmap(x, leftX, rightX, static_cast<SampleType>(0.0), (static_cast<SampleType>(2.0) * (tension - static_cast<SampleType>(1.0)) + static_cast<SampleType>(1.0)) * juce::MathConstants<float>::pi);

        x = std::signbit(std::sin(x + static_cast<SampleType>(0.25) * juce::MathConstants<SampleType>::twoPi));

        x = juce::jmap(x, static_cast<SampleType>(0.0), static_cast<SampleType>(1.0), leftY, rightY);

        return x;
    }
    static inline SampleType triangle(SampleType x, SampleType tension, SampleType leftX, SampleType leftY, SampleType rightX, SampleType rightY)
    {
        x = juce::jmap(x, leftX, rightX, static_cast<SampleType>(0.0), static_cast<SampleType>(2.0) * (tension - static_cast<SampleType>(1.0)) + static_cast<SampleType>(1.0));

        float floorX = std::floor(x);

        x = std::pow(-static_cast<SampleType>(1.0), floorX) * (x - floorX - ((int)floorX % 2));

        x = juce::jmap(x, static_cast<SampleType>(0.0), static_cast<SampleType>(1.0), leftY, rightY);

        return x;
    }
    static inline SampleType stair(SampleType x, SampleType tension, SampleType leftX, SampleType leftY, SampleType rightX, SampleType rightY)
    {
        if (x == leftX)
            return leftX;

        x = juce::jmap(x, leftX, rightX, static_cast<SampleType>(1.0), tension + static_cast<SampleType>(1.0));

        float floorX = std::floor(x) + static_cast<SampleType>(1.0) / (tension + static_cast<SampleType>(1.0));

        x = juce::jmap(floorX, static_cast<SampleType>(1.0) / (tension + static_cast<SampleType>(1.0)), tension + static_cast<SampleType>(1.0) + static_cast<SampleType>(1.0) / (tension + static_cast<SampleType>(1.0)), leftY, rightY);

        return x;
    }
    static inline SampleType sin(SampleType x, SampleType tension, SampleType leftX, SampleType leftY, SampleType rightX, SampleType rightY)
    {
        x = juce::jmap(x, leftX, rightX, static_cast<SampleType>(0.0), (static_cast<SampleType>(2.0) * (tension - static_cast<SampleType>(1.0)) + static_cast<SampleType>(1.0)) * juce::MathConstants<float>::pi);

        x = -std::sin(x + static_cast<SampleType>(0.25) * juce::MathConstants<SampleType>::twoPi);

        x = juce::jmap(x, - static_cast<SampleType>(1.0), static_cast<SampleType>(1.0), leftY, rightY);

        return x;
    }
};



