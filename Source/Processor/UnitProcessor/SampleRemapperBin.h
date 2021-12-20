/*
  ==============================================================================

    WaveShaperPointProcessor.h
    Created: 6 Sep 2021 4:39:15pm
    Author:  CookiMonstor

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "../../DataStructure/MangledIdentifiers.h"
#include "CurveEquation.h"

template <typename SampleType>
class SampleRemapperBin
{
public:
    SampleRemapperBin()
        : leftPoint(static_cast<SampleType>(0.0), static_cast<SampleType>(0.0))
        , rightPoint(static_cast<SampleType>(1.0), static_cast<SampleType>(1.0))
        , smoothedLeftX(static_cast<SampleType>(0.0))
        , smoothedLeftY(static_cast<SampleType>(0.0))
        , smoothedRightX(static_cast<SampleType>(1.0))
        , smoothedRightY(static_cast<SampleType>(1.0))
        , tension(static_cast<SampleType>(0.0))
        , smoothedTension(tension)
        , curveID(0)
    {
        //setCurveEquation(curveID, tension);
        curveEquation = [](SampleType sample, SampleType tension, SampleType leftX, SampleType leftY, SampleType rightX, SampleType rightY) {return CurveEquation<SampleType>::identity(sample, tension, leftX, leftY, rightX, rightY); };
    
        for (int index = 0; index < DistortionConstants::UI::nbOfPointsPerBin; ++index)
        {
            binPathXData[index] = (SampleType) 0.0;
            binPathYData[index] = (SampleType)0.0;
            eqBinPathYData[index] = (SampleType)0.0;
        }
    }

    SampleRemapperBin(SampleType leftX, SampleType leftY, SampleType rightX, SampleType rightY, SampleType tensionInit = static_cast<SampleType>(1.0), int curveTypeInit = 1)
        : leftPoint(leftX, leftY)
        , rightPoint(rightX, rightY)
        , smoothedLeftX(leftX)
        , smoothedLeftY(leftY)
        , smoothedRightX(rightX)
        , smoothedRightY(rightY)
        , tension(tensionInit)
        , smoothedTension(tensionInit)
        , curveType(curveTypeInit)
        
    {
        curveEquation = [](SampleType sample, SampleType tension, SampleType leftX, SampleType leftY, SampleType rightX, SampleType rightY) {return CurveEquation<SampleType>::identity(sample, tension, leftX, leftY, rightX, rightY); };
        smoothedLeftX.setTargetValue(leftX);
        smoothedLeftY.setTargetValue(leftY);
        smoothedRightX.setTargetValue(rightX);
        smoothedRightY.setTargetValue(rightY);

        for (int index = 0; index < DistortionConstants::UI::nbOfPointsPerBin; ++index)
        {
            binPathXData[index] = (SampleType)0.0;
            binPathYData[index] = (SampleType)0.0;
            eqBinPathYData[index] = (SampleType)0.0;
        }

        //computePathData();
    }

    struct WaveShaperPointParam
    {
        juce::Point<SampleType> leftPoint;
        juce::Point<SampleType> rightPoint;
        SampleType tension = static_cast<SampleType>(1.0);
    };

    void prepare(const juce::dsp::ProcessSpec& spec) noexcept
    {
        sampleRate = spec.sampleRate;
        rampDurationSeconds = spec.maximumBlockSize / sampleRate;

        reset();
    }

    void reset() noexcept
    {
        smoothedLeftX.reset(sampleRate, rampDurationSeconds);
        smoothedLeftY.reset(sampleRate, rampDurationSeconds);
        smoothedRightX.reset(sampleRate, rampDurationSeconds);
        smoothedRightY.reset(sampleRate, rampDurationSeconds);

        smoothedTension.reset(sampleRate, rampDurationSeconds);
    }

    bool isSampleInRange(SampleType sample)
    {
        return ((leftPoint.getX() <= sample) && (sample < rightPoint.getX()));
    }

    SampleType remapSample(SampleType sample)
    {
        return curveEquation(sample, tension, leftPoint.getX(), leftPoint.getY(), rightPoint.getX(), rightPoint.getY());
    }

    void getNextPointValue()
    {
        leftPoint.setX(smoothedLeftX.getNextValue());
        leftPoint.setY(smoothedLeftY.getNextValue());
        rightPoint.setX(smoothedRightX.getNextValue());
        rightPoint.setY(smoothedRightY.getNextValue());
    }

    void getNextTensionValue()
    {
        tension = smoothedTension.getNextValue();
    };

    void smoothedValuesSkip(int numSamplesToSkip)
    {
        smoothedLeftX.skip(numSamplesToSkip);
        smoothedLeftY.skip(numSamplesToSkip);
        smoothedRightX.skip(numSamplesToSkip);
        smoothedRightY.skip(numSamplesToSkip);
        smoothedTension.skip(numSamplesToSkip);
    }

    void computePathData()
    {
        SampleType binLength = smoothedRightX.getTargetValue() - smoothedLeftX.getTargetValue();
        SampleType xValue = static_cast<SampleType>(0.0);
        SampleType yValue = static_cast<SampleType>(0.0);
        int nbOfPoints = DistortionConstants::UI::nbOfPointsPerBin;

        if (smoothedLeftX.getTargetValue() == smoothedRightX.getTargetValue())
        {
            SampleType maxY = juce::jmax(smoothedLeftY.getTargetValue(), smoothedRightY.getTargetValue());
            SampleType minY = juce::jmin(smoothedLeftY.getTargetValue(), smoothedRightY.getTargetValue());

            SampleType yInc = (maxY - minY) / (SampleType)nbOfPoints;

            if (maxY == smoothedRightY.getTargetValue())
            {
                yInc = -yInc;
            }

            xValue = smoothedLeftX.getTargetValue();
            yValue = smoothedRightY.getTargetValue();

            for (int pointID = 0; pointID < nbOfPoints; ++pointID)
            {
                binPathXData[pointID] = xValue;
                binPathYData[pointID] = yValue;

                yValue += yInc;
            }
        }
        else
        {
            SampleType xInc = (binLength) / (SampleType)(nbOfPoints - 1);

            xValue = smoothedRightX.getTargetValue();
            yValue = curveEquation(xValue,
                smoothedTension.getTargetValue(),
                smoothedLeftX.getTargetValue(),
                smoothedLeftY.getTargetValue(),
                smoothedRightX.getTargetValue(),
                smoothedRightY.getTargetValue());

            binPathXData[0] = xValue;
            binPathYData[0] = yValue;

            for (int pointID = 1; pointID < nbOfPoints - 1; ++pointID)
            {

                xValue -= xInc;
                yValue = curveEquation(xValue,
                    smoothedTension.getTargetValue(),
                    smoothedLeftX.getTargetValue(),
                    smoothedLeftY.getTargetValue(),
                    smoothedRightX.getTargetValue(),
                    smoothedRightY.getTargetValue());

                binPathXData[pointID] = xValue;
                binPathYData[pointID] = yValue;
            }

            xValue = smoothedLeftX.getTargetValue();
            yValue = curveEquation(xValue,
                smoothedTension.getTargetValue(),
                smoothedLeftX.getTargetValue(),
                smoothedLeftY.getTargetValue(),
                smoothedRightX.getTargetValue(),
                smoothedRightY.getTargetValue());

            binPathXData[100] = xValue;
            binPathYData[100] = yValue;
        }

        switch (curveID)
        {
        case DistortionConstants::WaveShaper<SampleType>::CurveTypes::polynomial:  //Tension
            xValue = binPathXData[50];
            yValue = binPathYData[50];
            break;

        case DistortionConstants::WaveShaper<SampleType>::CurveTypes::asinh:  //Tension
            xValue = binPathXData[50];
            yValue = binPathYData[50];
            break;

        case DistortionConstants::WaveShaper<SampleType>::CurveTypes::doubleCurve: //Sin
            xValue = (smoothedRightX.getTargetValue() + smoothedLeftX.getTargetValue()) * static_cast<SampleType>(0.5);
            yValue = juce::jmap(smoothedTension.getTargetValue(),
                static_cast<SampleType>(-0.5),
                static_cast<SampleType>(0.5),
                smoothedLeftY.getTargetValue(),
                smoothedRightY.getTargetValue());
            break;

        case DistortionConstants::WaveShaper<SampleType>::CurveTypes::triangle:
            xValue = (smoothedRightX.getTargetValue() + smoothedLeftX.getTargetValue()) * static_cast<SampleType>(0.5);
            yValue = juce::jmap(smoothedTension.getTargetValue(),
                static_cast<SampleType>(1.0),
                static_cast<SampleType>(15.0),
                smoothedLeftY.getTargetValue(),
                smoothedRightY.getTargetValue());
            break;

        case DistortionConstants::WaveShaper<SampleType>::CurveTypes::square:
            xValue = (smoothedRightX.getTargetValue() + smoothedLeftX.getTargetValue()) * static_cast<SampleType>(0.5);
            yValue = juce::jmap(smoothedTension.getTargetValue(),
                static_cast<SampleType>(1.0),
                static_cast<SampleType>(15.0),
                smoothedLeftY.getTargetValue(),
                smoothedRightY.getTargetValue());
            break;

        case DistortionConstants::WaveShaper<SampleType>::CurveTypes::stair:
            xValue = (smoothedRightX.getTargetValue() + smoothedLeftX.getTargetValue()) * static_cast<SampleType>(0.5);
            yValue = juce::jmap(smoothedTension.getTargetValue(),
                static_cast<SampleType>(1.0),
                static_cast<SampleType>(15.0),
                smoothedLeftY.getTargetValue(),
                smoothedRightY.getTargetValue());
            break;

        case DistortionConstants::WaveShaper<SampleType>::CurveTypes::sin:
            xValue = (smoothedRightX.getTargetValue() + smoothedLeftX.getTargetValue()) * static_cast<SampleType>(0.5);
            yValue = juce::jmap(smoothedTension.getTargetValue(),
                static_cast<SampleType>(1.0),
                static_cast<SampleType>(15.0),
                smoothedLeftY.getTargetValue(),
                smoothedRightY.getTargetValue());
            break;

        default:
            xValue = binPathXData[50];
            yValue = binPathYData[50];
            break;

        }

        binPathXData[101] = xValue;
        binPathYData[101] = yValue;
        
    }

    //Set function===============================================================================================================
    void setLeftPoint(juce::Point<SampleType> newPoint) 
    { 
        //SampleType x = juce::jmap(newPoint.getX(), -DistortionConstants::UI::scopeMinLin_f * 0.5f, DistortionConstants::UI::scopeMaxLin_f * 0.5f, -1.125f, 1.125f);
        //SampleType y = juce::jmap(newPoint.getY(), -DistortionConstants::UI::scopeMinLin_f * 0.5f, DistortionConstants::UI::scopeMaxLin_f * 0.5f, -1.125f, 1.125f);
        //smoothedLeftX.setTargetValue(x);
        //smoothedLeftY.setTargetValue(y);
        smoothedLeftX.setTargetValue(newPoint.getX());
        smoothedLeftY.setTargetValue(newPoint.getY());
    };

    void setRightPoint(juce::Point<SampleType> newPoint) 
    { 
        //SampleType x = juce::jmap(newPoint.getX(), -DistortionConstants::UI::scopeMinLin_f * 0.5f, DistortionConstants::UI::scopeMaxLin_f * 0.5f, -1.125f, 1.125f);
        //SampleType y = juce::jmap(newPoint.getY(), -DistortionConstants::UI::scopeMinLin_f * 0.5f, DistortionConstants::UI::scopeMaxLin_f * 0.5f, -1.125f, 1.125f);
        //smoothedRightX.setTargetValue(x);
        //smoothedRightY.setTargetValue(y);
        smoothedRightX.setTargetValue(newPoint.getX());
        smoothedRightY.setTargetValue(newPoint.getY());
    };

    void setCurveID(int newCurveID)
    {
        curveID = newCurveID;
    }

    void setTension(SampleType newTension)
    {
        SampleType tensionMin = DistortionConstants::WaveShaper<SampleType>::tensionMin;
        SampleType tensionMax = DistortionConstants::WaveShaper<SampleType>::tensionMax;
        SampleType tensionMapped = static_cast<SampleType>(0.5);

        switch (curveID)
        {
        case DistortionConstants::WaveShaper<SampleType>::CurveTypes::polynomial:
            if (smoothedLeftY.getTargetValue() > smoothedRightY.getTargetValue())
                newTension = -newTension;

            tensionMapped = juce::jmap(newTension, static_cast<SampleType>(-0.5), static_cast<SampleType>(0.5), static_cast<SampleType>(0.0), static_cast<SampleType>(1.0));
            tensionMapped = juce::mapToLog10(tensionMapped, tensionMin, tensionMax);
            break;

        case DistortionConstants::WaveShaper<SampleType>::CurveTypes::asinh:
            if (smoothedLeftY.getTargetValue() > smoothedRightY.getTargetValue())
                newTension = -newTension;

            tensionMapped = juce::jmap(newTension, static_cast<SampleType>(-0.5), static_cast<SampleType>(0.5), static_cast<SampleType>(0.0), static_cast<SampleType>(1.0));
            tensionMapped = juce::mapToLog10(tensionMapped, tensionMin, tensionMax);
            break;

        case DistortionConstants::WaveShaper<SampleType>::CurveTypes::doubleCurve:
            if (smoothedLeftY.getTargetValue() < smoothedRightY.getTargetValue())
                newTension = -newTension;
            //tension = juce::jmap(newTension, static_cast<SampleType>(-0.5), static_cast<SampleType>(0.5), static_cast<SampleType>(0.0), static_cast<SampleType>(1.0));
            //tension = juce::mapToLog10(tension, tensionMin, tensionMax);
            tensionMapped = newTension;
            break;

        case DistortionConstants::WaveShaper<SampleType>::CurveTypes::triangle:
            if (smoothedLeftY.getTargetValue() < smoothedRightY.getTargetValue())
                newTension = -newTension;
            tensionMapped = juce::jmap(newTension, static_cast<SampleType>(-0.5), static_cast<SampleType>(0.5), static_cast<SampleType>(1.0), static_cast<SampleType>(15.0));
            tensionMapped = std::floor(tensionMapped);
            break;

        case DistortionConstants::WaveShaper<SampleType>::CurveTypes::square:
            if (smoothedLeftY.getTargetValue() < smoothedRightY.getTargetValue())
                newTension = -newTension;
            tensionMapped = juce::jmap(newTension, static_cast<SampleType>(-0.5), static_cast<SampleType>(0.5), static_cast<SampleType>(1.0), static_cast<SampleType>(15.0));
            tensionMapped = std::floor(tensionMapped);
            break;

        case DistortionConstants::WaveShaper<SampleType>::CurveTypes::stair:
            if (smoothedLeftY.getTargetValue() < smoothedRightY.getTargetValue())
                newTension = -newTension;
            tensionMapped = juce::jmap(newTension, static_cast<SampleType>(-0.5), static_cast<SampleType>(0.5), static_cast<SampleType>(1.0), static_cast<SampleType>(15.0));
            tensionMapped = std::floor(tensionMapped);
            break;

        case DistortionConstants::WaveShaper<SampleType>::CurveTypes::sin:
            if (smoothedLeftY.getTargetValue() < smoothedRightY.getTargetValue())
                newTension = -newTension;
            tensionMapped = juce::jmap(newTension, static_cast<SampleType>(-0.5), static_cast<SampleType>(0.5), static_cast<SampleType>(1.0), static_cast<SampleType>(15.0));
            tensionMapped = std::floor(tensionMapped);
            break;

        default:
            tensionMapped = static_cast<SampleType>(1.0);
            break;
        }

        smoothedTension.setTargetValue(tensionMapped);
    };

    void setCurveEQA()
    {
        if (smoothedLeftX.getTargetValue() == smoothedRightX.getTargetValue())
        {
            curveEquation = [](SampleType sample, SampleType tension, SampleType leftX, SampleType leftY, SampleType rightX, SampleType rightY) {return CurveEquation<SampleType>::vertical(sample, tension, leftX, leftY, rightX, rightY); };
        }
        else if (smoothedLeftY.getTargetValue() == smoothedRightY.getTargetValue())
        {
            curveEquation = [](SampleType sample, SampleType tension, SampleType leftX, SampleType leftY, SampleType rightX, SampleType rightY) {return CurveEquation<SampleType>::horizontal(sample, tension, leftX, leftY, rightX, rightY); };
        }
        else
        {
            switch (curveID)
            {
            case DistortionConstants::WaveShaper<SampleType>::CurveTypes::polynomial:
                if (smoothedTension.getTargetValue() >= (SampleType) 1.0)
                {
                    curveEquation = [](SampleType sample, SampleType tension, SampleType leftX, SampleType leftY, SampleType rightX, SampleType rightY) {return CurveEquation<SampleType>::polynomialOverOne(sample, tension, leftX, leftY, rightX, rightY); };
                }
                else
                {
                    curveEquation = [](SampleType sample, SampleType tension, SampleType leftX, SampleType leftY, SampleType rightX, SampleType rightY) {return CurveEquation<SampleType>::polynomialBelowOne(sample, tension, leftX, leftY, rightX, rightY); };
                }
                break;

            case DistortionConstants::WaveShaper<SampleType>::CurveTypes::asinh:
                if (smoothedTension.getTargetValue() >= (SampleType) 1.0)
                {
                    curveEquation = [](SampleType sample, SampleType tension, SampleType leftX, SampleType leftY, SampleType rightX, SampleType rightY) {return CurveEquation<SampleType>::arcsinhOverOne(sample, tension, leftX, leftY, rightX, rightY); };
                }
                else
                {
                    curveEquation = [](SampleType sample, SampleType tension, SampleType leftX, SampleType leftY, SampleType rightX, SampleType rightY) {return CurveEquation<SampleType>::arcsinhBelowOne(sample, tension, leftX, leftY, rightX, rightY); };
                }
                break;

            case DistortionConstants::WaveShaper<SampleType>::CurveTypes::doubleCurve: //Sin
                if (smoothedTension.getTargetValue() >= (SampleType) 0.0)
                {
                    curveEquation = [](SampleType sample, SampleType tension, SampleType leftX, SampleType leftY, SampleType rightX, SampleType rightY) {return CurveEquation<SampleType>::doubleCurveOverZero(sample, tension, leftX, leftY, rightX, rightY); };
                }
                else
                {
                    curveEquation = [](SampleType sample, SampleType tension, SampleType leftX, SampleType leftY, SampleType rightX, SampleType rightY) {return CurveEquation<SampleType>::doubleCurveBelowZero(sample, tension, leftX, leftY, rightX, rightY); };
                }
                break;

            case DistortionConstants::WaveShaper<SampleType>::CurveTypes::triangle:
                curveEquation = [](SampleType sample, SampleType tension, SampleType leftX, SampleType leftY, SampleType rightX, SampleType rightY) {return CurveEquation<SampleType>::triangle(sample, tension, leftX, leftY, rightX, rightY); };
                break;

            case DistortionConstants::WaveShaper<SampleType>::CurveTypes::square:
                curveEquation = [](SampleType sample, SampleType tension, SampleType leftX, SampleType leftY, SampleType rightX, SampleType rightY) {return CurveEquation<SampleType>::square(sample, tension, leftX, leftY, rightX, rightY); };
                break;

            case DistortionConstants::WaveShaper<SampleType>::CurveTypes::stair:
                curveEquation = [](SampleType sample, SampleType tension, SampleType leftX, SampleType leftY, SampleType rightX, SampleType rightY) {return CurveEquation<SampleType>::stair(sample, tension, leftX, leftY, rightX, rightY); };
                break;

            case DistortionConstants::WaveShaper<SampleType>::CurveTypes::sin:
                curveEquation = [](SampleType sample, SampleType tension, SampleType leftX, SampleType leftY, SampleType rightX, SampleType rightY) {return CurveEquation<SampleType>::sin(sample, tension, leftX, leftY, rightX, rightY); };
                break;

            default:
                if (smoothedTension.getTargetValue() >= (SampleType)1.0)
                {
                    curveEquation = [](SampleType sample, SampleType tension, SampleType leftX, SampleType leftY, SampleType rightX, SampleType rightY) {return CurveEquation<SampleType>::polynomialOverOne(sample, tension, leftX, leftY, rightX, rightY); };
                }
                else
                {
                    curveEquation = [](SampleType sample, SampleType tension, SampleType leftX, SampleType leftY, SampleType rightX, SampleType rightY) {return CurveEquation<SampleType>::polynomialBelowOne(sample, tension, leftX, leftY, rightX, rightY); };
                }
                break;

            }
        }

    }

    void setBinParam(juce::Point<SampleType> newLeftPoint, juce::Point<SampleType> newRightPoint, SampleType newTension, int newCurveID)
    {
        setLeftPoint(newLeftPoint);
        setRightPoint(newRightPoint);
        setCurveID(newCurveID);
        setTension(newTension);
        setCurveEQA();
        computePathData();
    };

    void setEQBinData(int index, SampleType eqData) { eqBinPathYData[index] = eqData; };

    //Get Function
    SampleType getTargetLeftX() { return smoothedLeftX.getTargetValue(); };
    SampleType getTargetLeftY() { return smoothedLeftY.getTargetValue(); };
    SampleType getTargetRightX() { return smoothedRightX.getTargetValue(); };
    SampleType getTargetRightY() { return smoothedRightY.getTargetValue(); };
    juce::Point<SampleType>& getLeftPoint() { return leftPoint; };
    juce::Point<SampleType>& getRightPoint() { return rightPoint; };
    SampleType getTension() { return tension; };

    SampleType* getBinPathXData() { return binPathXData; };
    SampleType* getBinPathYData() { return binPathYData; };
    SampleType* getEQBinPathYData() { return eqBinPathYData; };

private:
    juce::Point<SampleType> leftPoint;
    juce::Point<SampleType> rightPoint;
    SampleType tension = static_cast<SampleType>(0.0);

    juce::SmoothedValue<SampleType> smoothedLeftX;
    juce::SmoothedValue<SampleType> smoothedLeftY;
    juce::SmoothedValue<SampleType> smoothedRightX;
    juce::SmoothedValue<SampleType> smoothedRightY;
    juce::SmoothedValue<SampleType> smoothedTension;

    std::function < SampleType(SampleType, SampleType, SampleType, SampleType, SampleType, SampleType) > curveEquation;

    int curveID = 0;

    double sampleRate = 44100.0;
    double rampDurationSeconds = 0.0;

    SampleType binPathXData[101 + 1];
    SampleType binPathYData[101 + 1];

    SampleType eqBinPathYData[101 + 1];
};