/*
  ==============================================================================

    WaveShaperPointProcessor.cpp
    Created: 6 Sep 2021 4:39:15pm
    Author:  CookiMonstor

  ==============================================================================
*/

#include "PiecewiseFunctionBin.h"

//template <typename SampleType>
//void WaveShaperPointProcessor<SampleType>::computeCurvePath()
//{
//    //float width = 100.0f;
//    //float height = 100.0f;
//
//    //float minLinValue = DistortionConstants::WaveShaper::scopeMinLinf;
//    //float maxLinValue = DistortionConstants::WaveShaper::scopeMaxLinf;
//
//    //float xLeft = leftPoint.getX();
//    //float yLeft = leftPoint.getY();
//    //float xRight = rightPoint.getX();
//    //float yRight = rightPoint.getY();
//
//    //float xLeftRemapped = juce::jmap(xLeft, minLinValue, maxLinValue, 0.0f, width);
//    //float yLeftRemapped = juce::jmap(xLeft, minLinValue, maxLinValue, height, 0.0f);
//
//    //float xRightRemapped = juce::jmap(xRight, minLinValue, maxLinValue, 0.0f, width);
//    //float yRightRemapped = juce::jmap(xRight, minLinValue, maxLinValue, height, 0.0f);
//
//    //int nbOfPoints = 50;
//    //SampleType inc = (xLeft - xRight) / (float) nbOfPoints;
//
//    //curvePath.clear();
//
//    //curvePath.startNewSubPath(xLeftRemapped, yLeftRemapped);
//
//    //if (inc == static_cast<SampleType>(0.0))
//    //{
//    //    curvePath.lineTo(xRightRemapped, yRightRemapped);
//    //}
//    //else
//    //{
//    //    for (int i = 0; i < nbOfPoints; ++i)
//    //    {
//    //        xLeft += inc;
//    //        yLeft = remapSample(xLeft);
//
//    //        xLeftRemapped = juce::jmap(xLeft, minLinValue, maxLinValue, 0.0f, width);
//    //        yLeftRemapped = juce::jmap(yLeft, minLinValue, maxLinValue, height, 0.0f);
//
//    //        curvePath.lineTo(xLeftRemapped, yLeftRemapped);
//    //    }
//    //}
//}
