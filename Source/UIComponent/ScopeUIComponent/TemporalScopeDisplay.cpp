/*
  ==============================================================================

    TemporalScopeDisplay.cpp
    Created: 3 Dec 2021 8:53:59pm
    Author:  CookiMonstor

  ==============================================================================
*/

//#include <JuceHeader.h>
#include "TemporalScopeDisplay.h"

//==============================================================================
TemporalScopeDisplay::TemporalScopeDisplay(const juce::AudioBuffer<float>& inBuffer, const juce::AudioBuffer<float>& outBuffer, int chID, juce::Path& inPath, juce::Path& outPath)
    : inputBuffer(inBuffer)
    , outputBuffer(outBuffer)
    , channelID(chID)
    , inputPath(inPath)
    , outputPath(outPath)
{
    // In your constructor, you should add any child components, and
    // initialise any special settings that your component needs.

}

TemporalScopeDisplay::~TemporalScopeDisplay()
{
}

void TemporalScopeDisplay::paint (juce::Graphics& g)
{
    //g.setOpacity(1.0f);
    g.fillAll(juce::Colours::black);

    g.setColour(juce::Colours::darkgrey);
    g.setOpacity(0.3f);

    juce::PathStrokeType pst(1.0f);
    float dashLenght[2] = { 2.0f, 4.0f };
    juce::Path innerDottedGrid;
    pst.createDashedStroke(innerDottedGrid, innerGrid, dashLenght, 2);

    g.strokePath(innerDottedGrid, juce::PathStrokeType(1.0f));

    g.setColour(juce::Colours::grey);
    g.setOpacity(0.5f);
    g.strokePath(grid, juce::PathStrokeType(1.0f));

    g.setOpacity(0.5f);
    g.setColour(juce::Colours::grey);
    g.drawRect(getLocalBounds());

    //computeTemporalPath(g);

    g.setOpacity(1.0f);
    g.setColour(juce::Colours::darkgrey);
    g.strokePath(inputPath, juce::PathStrokeType(1.0f));

    g.setGradientFill(outputGradient);
    g.setOpacity(1.0f);
    g.strokePath(outputPath, juce::PathStrokeType(1.0f));
}

void TemporalScopeDisplay::resized()
{
    computeGrid();

    auto lim = std::pow(10.0, -0.01);
    const auto bounds = getLocalBounds().toFloat();
    inputGradient = juce::ColourGradient{ juce::Colours::darkred.darker(0.3f), bounds.getBottomLeft(), juce::Colours::darkred.darker(0.3f), bounds.getTopLeft(), false };
    outputGradient = juce::ColourGradient{ juce::Colours::red, bounds.getBottomLeft(), juce::Colours::red, bounds.getTopLeft(), false };
    double colourPos = juce::jmap(lim, -1.1, 1.1, 0.0, 1.0);
    inputGradient.addColour(colourPos, juce::Colours::darkgrey);
    outputGradient.addColour(colourPos, juce::Colours::white);
    colourPos = juce::jmap(-lim, -1.1, 1.1, 0.0, 1.0);
    inputGradient.addColour(colourPos, juce::Colours::darkgrey);
    outputGradient.addColour(colourPos, juce::Colours::white);
}

void TemporalScopeDisplay::computeTemporalPath(juce::Graphics& g)
{
    juce::Path inputPath;
    juce::Path outputPath;

    inputPath.clear();
    outputPath.clear();

    //float start = inputBuffer.findMinMax(channelID, 0, inputBuffer.getNumSamples()).getStart();
    //float end = inputBuffer.findMinMax(channelID, 0, inputBuffer.getNumSamples()).getEnd();

    //float inputNormCoef = isNormalized ? juce::jmax(std::abs(start), std::abs(end))
    //                                   : 1.0f;
    float inputNormCoef = 1.0f;
    //start = outputBuffer.findMinMax(channelID, 0, inputBuffer.getNumSamples()).getStart();
    //end = outputBuffer.findMinMax(channelID, 0, inputBuffer.getNumSamples()).getEnd();

    //float outputNormCoef = isNormalized ? juce::jmax(std::abs(start), std::abs(end))
    //                                   : 1.0f;

    float outputNormCoef = 1.0f;
    float minLin = -1.1f;
    float maxLin = +1.1f;

    float width = static_cast<float>(getWidth());
    float height = static_cast<float>(getHeight());

    juce::int64 ppqSampleMod = (ppqSample + (juce::int64)bufferSize) % (juce::int64)nbSamplePerBeat - 1;
    if (ppqSampleMod < 0)
        return;

    float pos = juce::jmap((float)ppqSampleMod, 0.0f, (float)(nbSamplePerBeat - 1), 0.0f, width);

    //Input
    float inputMag = inputBuffer.getSample(channelID, 0);
    float outputMag = outputBuffer.getSample(channelID, 0);
    float inputMagRemap = juce::jmap(inputMag/inputNormCoef, minLin, maxLin, height, 0.0f);
    float outputMagRemap = juce::jmap(outputMag/outputNormCoef, minLin, maxLin, height, 0.0f);
    
    inputPath.startNewSubPath(pos, inputMagRemap);
    outputPath.startNewSubPath(pos, outputMagRemap);

    juce::int64 forLim = (juce::int64)nbSamplePerBeat - ppqSampleMod;
    jassert(juce::isPositiveAndBelow(forLim, inputBuffer.getNumSamples()));
    jassert(juce::isPositiveAndBelow(forLim, outputBuffer.getNumSamples()));
    jassert(juce::isPositiveAndBelow(forLim, nbSamplePerBeat));

    int inc = nbSamplePerBeat / (getWidth() + 1) + 1;
    //inc = 10;
    for (juce::int64 i = 1; i < forLim; i = i + inc)
    {
        pos = juce::jmap((float)i + (float)ppqSampleMod, 0.0f, (float)(nbSamplePerBeat - 1), 0.0f, width);

        inputMag = inputBuffer.getSample(channelID, i);
        outputMag = outputBuffer.getSample(channelID, i);
        inputMagRemap = juce::jmap(inputMag/inputNormCoef, minLin, maxLin, height, 0.0f);
        outputMagRemap = juce::jmap(outputMag/outputNormCoef, minLin, maxLin, height, 0.0f);
        
        inputPath.lineTo(pos, inputMagRemap);
        outputPath.lineTo(pos, outputMagRemap);
    }

    int count = 0;

    pos = juce::jmap((float)count, 0.0f, (float)(nbSamplePerBeat - 1), 0.0f, width);
    count+=inc;

    inputMag = inputBuffer.getSample(channelID, forLim);
    outputMag = outputBuffer.getSample(channelID, forLim);
    inputMagRemap = juce::jmap(inputMag/inputNormCoef, minLin, maxLin, height, 0.0f);
    outputMagRemap = juce::jmap(outputMag/outputNormCoef, minLin, maxLin, height, 0.0f);
    
    inputPath.startNewSubPath(pos, inputMagRemap);
    outputPath.startNewSubPath(pos, outputMagRemap);

    for (int i = forLim + 1; i < nbSamplePerBeat; i = i + inc)
    {
        jassert(juce::isPositiveAndBelow(i, inputBuffer.getNumSamples()));
        jassert(juce::isPositiveAndBelow(i, outputBuffer.getNumSamples()));
        jassert(juce::isPositiveAndBelow(i, nbSamplePerBeat));
        pos = juce::jmap((float)count, 0.0f, (float)(nbSamplePerBeat - 1), 0.0f, width);
        count+= inc;

        inputMag = inputBuffer.getSample(channelID, i);
        outputMag = outputBuffer.getSample(channelID, i);
        inputMagRemap = juce::jmap(inputMag/inputNormCoef, minLin, maxLin, height, 0.0f);
        outputMagRemap = juce::jmap(outputMag/outputNormCoef, minLin, maxLin, height, 0.0f);
        
        inputPath.lineTo(pos, inputMagRemap);
        outputPath.lineTo(pos, outputMagRemap);

    }
    
    g.setOpacity(1.0f);
    g.setColour(juce::Colours::darkgrey);
    g.strokePath(inputPath, juce::PathStrokeType(1.0f));

    g.setGradientFill(outputGradient);
    g.setOpacity(1.0f);
    g.strokePath(outputPath, juce::PathStrokeType(1.0f));
}

void TemporalScopeDisplay::computeTemporalPathPerFrame(juce::Graphics& g)
{
    if (previousPPQSample == ppqSample)
        return;

    juce::Path inputPath;
    juce::Path outputPath;

    inputPath.clear();
    outputPath.clear();

    //float start = inputBuffer.findMinMax(channelID, 0, inputBuffer.getNumSamples()).getStart();
    //float end = inputBuffer.findMinMax(channelID, 0, inputBuffer.getNumSamples()).getEnd();

    //float inputNormCoef = isNormalized ? juce::jmax(std::abs(start), std::abs(end))
    //                                   : 1.0f;
    float inputNormCoef = 1.0f;
    //start = outputBuffer.findMinMax(channelID, 0, inputBuffer.getNumSamples()).getStart();
    //end = outputBuffer.findMinMax(channelID, 0, inputBuffer.getNumSamples()).getEnd();

    //float outputNormCoef = isNormalized ? juce::jmax(std::abs(start), std::abs(end))
    //                                   : 1.0f;

    float outputNormCoef = 1.0f;
    float minLin = -1.1f;
    float maxLin = +1.1f;

    float width = static_cast<float>(getWidth());
    float height = static_cast<float>(getHeight());

    juce::int64 previousPPQSampleMod = (previousPPQSample  + (juce::int64)bufferSize) % (juce::int64)nbSamplePerBeat - 1;
    if (previousPPQSampleMod < 0)
        return;

    float pos = juce::jmap((float)previousPPQSampleMod, 0.0f, (float)(nbSamplePerBeat - 1), 0.0f, width);

    juce::int64 ppqSampleMod = (ppqSample  + (juce::int64)bufferSize ) % (juce::int64)nbSamplePerBeat - 1;
    if (ppqSampleMod < 0)
        return;

    //Input
    float inputMag = inputBuffer.getSample(channelID, 0);
    float outputMag = outputBuffer.getSample(channelID, 0);
    float inputMagRemap = juce::jmap(inputMag / inputNormCoef, minLin, maxLin, height, 0.0f);
    float outputMagRemap = juce::jmap(outputMag / outputNormCoef, minLin, maxLin, height, 0.0f);

    inputPath.startNewSubPath(pos, inputMagRemap);
    outputPath.startNewSubPath(pos, outputMagRemap);

    juce::int64 forLim = juce::jmin((juce::int64)nbSamplePerBeat - previousPPQSampleMod, ppqSampleMod);
    jassert(juce::isPositiveAndBelow(forLim, inputBuffer.getNumSamples()));
    jassert(juce::isPositiveAndBelow(forLim, outputBuffer.getNumSamples()));
    jassert(juce::isPositiveAndBelow(forLim, nbSamplePerBeat));

    if ((juce::int64)nbSamplePerBeat - previousPPQSampleMod > ppqSampleMod)
    {
        for (juce::int64 i = previousPPQSampleMod; i < ppqSampleMod; ++i)
        {
            pos = juce::jmap((float)i + (float)previousPPQSampleMod, 0.0f, (float)(nbSamplePerBeat - 1), 0.0f, width);

            inputMag = inputBuffer.getSample(channelID, i);
            outputMag = outputBuffer.getSample(channelID, i);
            inputMagRemap = juce::jmap(inputMag / inputNormCoef, minLin, maxLin, height, 0.0f);
            outputMagRemap = juce::jmap(outputMag / outputNormCoef, minLin, maxLin, height, 0.0f);

            inputPath.lineTo(pos, inputMagRemap);
            outputPath.lineTo(pos, outputMagRemap);
        }
    }
    else
    {
        for (juce::int64 i = previousPPQSampleMod; i < (juce::int64)nbSamplePerBeat - previousPPQSampleMod; ++i)
        {
            pos = juce::jmap((float)i + (float)previousPPQSampleMod, 0.0f, (float)(nbSamplePerBeat - 1), 0.0f, width);

            inputMag = inputBuffer.getSample(channelID, i);
            outputMag = outputBuffer.getSample(channelID, i);
            inputMagRemap = juce::jmap(inputMag / inputNormCoef, minLin, maxLin, height, 0.0f);
            outputMagRemap = juce::jmap(outputMag / outputNormCoef, minLin, maxLin, height, 0.0f);

            inputPath.lineTo(pos, inputMagRemap);
            outputPath.lineTo(pos, outputMagRemap);
        }

        int count = 0;

        pos = juce::jmap((float)count, 0.0f, (float)(nbSamplePerBeat - 1), 0.0f, width);
        count++;

        inputMag = inputBuffer.getSample(channelID, forLim);
        outputMag = outputBuffer.getSample(channelID, forLim);
        inputMagRemap = juce::jmap(inputMag / inputNormCoef, minLin, maxLin, height, 0.0f);
        outputMagRemap = juce::jmap(outputMag / outputNormCoef, minLin, maxLin, height, 0.0f);

        inputPath.startNewSubPath(pos, inputMagRemap);
        outputPath.startNewSubPath(pos, outputMagRemap);

        for (int i = (juce::int64)nbSamplePerBeat - previousPPQSampleMod + 1; i < ppqSampleMod; ++i)
        {
            jassert(juce::isPositiveAndBelow(i, inputBuffer.getNumSamples()));
            jassert(juce::isPositiveAndBelow(i, outputBuffer.getNumSamples()));
            jassert(juce::isPositiveAndBelow(i, nbSamplePerBeat));
            pos = juce::jmap((float)count, 0.0f, (float)(nbSamplePerBeat - 1), 0.0f, width);
            count++;

            inputMag = inputBuffer.getSample(channelID, i);
            outputMag = outputBuffer.getSample(channelID, i);
            inputMagRemap = juce::jmap(inputMag / inputNormCoef, minLin, maxLin, height, 0.0f);
            outputMagRemap = juce::jmap(outputMag / outputNormCoef, minLin, maxLin, height, 0.0f);

            inputPath.lineTo(pos, inputMagRemap);
            outputPath.lineTo(pos, outputMagRemap);

        }
    }


    g.setOpacity(1.0f);
    g.setColour(juce::Colours::darkgrey);
    g.strokePath(inputPath, juce::PathStrokeType(1.0f));

    g.setGradientFill(outputGradient);
    g.setOpacity(1.0f);
    g.strokePath(outputPath, juce::PathStrokeType(1.0f));

    previousPPQSample = ppqSample;
}

void TemporalScopeDisplay::computeGrid(int subdivision)
{
    grid.clear();
    innerGrid.clear();
    int width = getLocalBounds().getWidth();
    int height = getLocalBounds().getHeight();
    float x = 0.0f;
    float y = static_cast<float>(height);
    float inc = static_cast<float>(width) / static_cast<float>(subdivision);
    float innerInc = inc / 4.0f;
    //grid.startNewSubPath(0.0f, 0.0f);

    //grid.lineTo(x, y);

    for (int i = 1; i < subdivision; i++)
    {
        x = static_cast<float>(i) * inc;
        y = 0.0f;
        grid.startNewSubPath(x, y);

        y = static_cast<float>(height);
        grid.lineTo(x, y);

        for(int j = 1; j < 4; ++j)
        {
            x = static_cast<float>(j) * innerInc + (i - 1)*inc;
            y = 0.0f;
            innerGrid.startNewSubPath(x, y);

            y = static_cast<float>(height);
            innerGrid.lineTo(x, y);
            innerGrid.closeSubPath();
        }
    }

    for (int j = 1; j < 4; ++j)
    {

        x = static_cast<float>(j) * innerInc + (subdivision - 1) * inc;
        y = 0.0f;
        innerGrid.startNewSubPath(x, y);

        y = static_cast<float>(height);
        innerGrid.lineTo(x, y);
        innerGrid.closeSubPath();
    }

    x = 0.0f;
    y = static_cast<float>(height) / 2.0f;
    grid.startNewSubPath(x, y);
    x = width;
    grid.lineTo(x, y);

    inc = static_cast<float>(width) / static_cast<float>(16);

    //for (int i = 1; i < 16; i++)
    //{
    //    x = static_cast<float>(i) * inc;
    //    y = 0.0f;
    //    innerGrid.startNewSubPath(x, y);

    //    y = static_cast<float>(height);
    //    innerGrid.lineTo(x, y);
    //    innerGrid.closeSubPath();
    //}

    inc = static_cast<float>(height) / static_cast<float>(4);
    for (int i = 1; i < 4; i++)
    {
        if (i == 2)
            continue;

        x = 0.0f;
        y = static_cast<float>(i) * inc;
        innerGrid.startNewSubPath(x, y);

        x = static_cast<float>(width);
        innerGrid.lineTo(x, y);
        innerGrid.closeSubPath();
    }

}

//==============================================================================
StereoToMonoTemporalScopeDisplay::StereoToMonoTemporalScopeDisplay(const juce::AudioBuffer<float>& inBuffer, const juce::AudioBuffer<float>& outBuffer, juce::Path& inPath, juce::Path& outPath)
    : TemporalScopeDisplay(inBuffer, outBuffer, 0, inPath, outPath)
    //: inputBuffer(inBuffer)
    //, outputBuffer(outBuffer)
{
    // In your constructor, you should add any child components, and
    // initialise any special settings that your component needs.

}

StereoToMonoTemporalScopeDisplay::~StereoToMonoTemporalScopeDisplay()
{
}

void StereoToMonoTemporalScopeDisplay::computeTemporalPath(juce::Graphics& g)
{
    juce::Path inputPath;
    juce::Path outputPath;

    inputPath.clear();
    outputPath.clear();

    //float start = inputBuffer.findMinMax(channelID, 0, inputBuffer.getNumSamples()).getStart();
    //float end = inputBuffer.findMinMax(channelID, 0, inputBuffer.getNumSamples()).getEnd();

    //float inputNormCoef = isNormalized ? juce::jmax(std::abs(start), std::abs(end))
    //                                   : 1.0f;
    float inputNormCoef = 1.0f;
    //start = outputBuffer.findMinMax(channelID, 0, inputBuffer.getNumSamples()).getStart();
    //end = outputBuffer.findMinMax(channelID, 0, inputBuffer.getNumSamples()).getEnd();

    //float outputNormCoef = isNormalized ? juce::jmax(std::abs(start), std::abs(end))
    //                                   : 1.0f;

    float outputNormCoef = 1.0f;
    float minLin = -1.1f;
    float maxLin = +1.1f;

    float width = static_cast<float>(getWidth());
    float height = static_cast<float>(getHeight());

    juce::int64 ppqSampleMod = (ppqSample + (juce::int64)bufferSize) % (juce::int64)nbSamplePerBeat - 1;
    if (ppqSampleMod < 0)
        return;

    float pos = juce::jmap((float)ppqSampleMod, 0.0f, (float)(nbSamplePerBeat - 1), 0.0f, width);

    //Input
    float inputMag = (inputBuffer.getSample(0, 0) + inputBuffer.getSample(1, 0))*0.5f;
    float outputMag = (outputBuffer.getSample(0, 0) + outputBuffer.getSample(1, 0)) * 0.5f;
    float inputMagRemap = juce::jmap(inputMag / inputNormCoef, minLin, maxLin, height, 0.0f);
    float outputMagRemap = juce::jmap(outputMag / outputNormCoef, minLin, maxLin, height, 0.0f);

    inputPath.startNewSubPath(pos, inputMagRemap);
    outputPath.startNewSubPath(pos, outputMagRemap);

    juce::int64 forLim = (juce::int64)nbSamplePerBeat - ppqSampleMod;
    jassert(juce::isPositiveAndBelow(forLim, inputBuffer.getNumSamples()));
    jassert(juce::isPositiveAndBelow(forLim, outputBuffer.getNumSamples()));
    jassert(juce::isPositiveAndBelow(forLim, nbSamplePerBeat));

    for (juce::int64 i = 1; i < forLim; ++i)
    {
        pos = juce::jmap((float)i + (float)ppqSampleMod, 0.0f, (float)(nbSamplePerBeat - 1), 0.0f, width);

        inputMag = (inputBuffer.getSample(0, i) + inputBuffer.getSample(1, i)) * 0.5f;
        outputMag = (outputBuffer.getSample(0, i) + outputBuffer.getSample(1, i)) * 0.5f;
        inputMagRemap = juce::jmap(inputMag / inputNormCoef, minLin, maxLin, height, 0.0f);
        outputMagRemap = juce::jmap(outputMag / outputNormCoef, minLin, maxLin, height, 0.0f);

        inputPath.lineTo(pos, inputMagRemap);
        outputPath.lineTo(pos, outputMagRemap);
    }

    int count = 0;

    pos = juce::jmap((float)count, 0.0f, (float)(nbSamplePerBeat - 1), 0.0f, width);
    count++;

    inputMag = (inputBuffer.getSample(0, forLim) + inputBuffer.getSample(1, forLim)) * 0.5f;
    outputMag = (outputBuffer.getSample(0, forLim) + outputBuffer.getSample(1, forLim)) * 0.5f;
    inputMagRemap = juce::jmap(inputMag / inputNormCoef, minLin, maxLin, height, 0.0f);
    outputMagRemap = juce::jmap(outputMag / outputNormCoef, minLin, maxLin, height, 0.0f);

    inputPath.startNewSubPath(pos, inputMagRemap);
    outputPath.startNewSubPath(pos, outputMagRemap);

    for (int i = forLim + 1; i < nbSamplePerBeat; ++i)
    {
        jassert(juce::isPositiveAndBelow(i, inputBuffer.getNumSamples()));
        jassert(juce::isPositiveAndBelow(i, outputBuffer.getNumSamples()));
        jassert(juce::isPositiveAndBelow(i, nbSamplePerBeat));
        pos = juce::jmap((float)count, 0.0f, (float)(nbSamplePerBeat - 1), 0.0f, width);
        count++;

        inputMag = (inputBuffer.getSample(0, i) + inputBuffer.getSample(1, i)) * 0.5f;
        outputMag = (outputBuffer.getSample(0, i) + outputBuffer.getSample(1, i)) * 0.5f;
        inputMagRemap = juce::jmap(inputMag / inputNormCoef, minLin, maxLin, height, 0.0f);
        outputMagRemap = juce::jmap(outputMag / outputNormCoef, minLin, maxLin, height, 0.0f);

        inputPath.lineTo(pos, inputMagRemap);
        outputPath.lineTo(pos, outputMagRemap);

    }

    g.setOpacity(1.0f);
    //g.setColour(juce::Colours::darkgrey);
    g.setGradientFill(inputGradient);
    g.strokePath(inputPath, juce::PathStrokeType(1.0f));
    g.setGradientFill(outputGradient);
    //g.setOpacity(1.0f);
    g.strokePath(outputPath, juce::PathStrokeType(1.0f));
}

