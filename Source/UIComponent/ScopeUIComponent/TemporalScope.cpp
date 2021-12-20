/*
  ==============================================================================

    TemporalScope.cpp
    Created: 3 Dec 2021 5:12:34pm
    Author:  CookiMonstor

  ==============================================================================
*/

#include <JuceHeader.h>
#include "TemporalScope.h"

//==============================================================================
TemporalScope::TemporalScope()
    : inputTemporalBuffer(2, 96000)
    , outputTemporalBuffer(2, 96000)
    , leftChannelScope(inputTemporalBuffer, outputTemporalBuffer, 0, leftInputPath, leftOutputPath)
    , rightChannelScope(inputTemporalBuffer, outputTemporalBuffer, 1, rightInputPath, rightOutputPath)
    , monoScope(inputTemporalBuffer, outputTemporalBuffer, monoInputPath, monoOutputPath)
{
    setInterceptsMouseClicks(false, true);
    setOpaque(true);

    inputTemporalBuffer.clear();
    outputTemporalBuffer.clear();

    addAndMakeVisible(leftChannelScope);
    addAndMakeVisible(rightChannelScope);

    addChildComponent(monoScope);
}

TemporalScope::~TemporalScope()
{
}

void TemporalScope::paint (juce::Graphics& g)
{
    g.setOpacity(1.0f);

    g.fillAll(juce::Colours::black);

}

void TemporalScope::resized()
{
    int rotaryPixelSize = ScopeConstants::UI::sliderHeight;
    int labelWidth = ScopeConstants::UI::labelWidth;
    int labelHeight = ScopeConstants::UI::labelHeight;
    int pixelSpace = ScopeConstants::UI::pixelSpace;

    int width = getWidth();
    int height = getHeight() ;
    
    monoScope.setBounds(0, 0, width, height);


    leftChannelScope.setBounds(0,0,width, height / 2);
    rightChannelScope.setBounds(0, leftChannelScope.getBottom() + 1, width, height / 2);


    leftChannelScope.computeGrid();
    rightChannelScope.computeGrid();


}

void TemporalScope::switchScope(bool buttonState)
{
    if (buttonState)
    {
        monoScope.setVisible(false);
        leftChannelScope.setVisible(true);
        rightChannelScope.setVisible(true);
    }
    else
    {
        leftChannelScope.setVisible(false);
        rightChannelScope.setVisible(false);
        monoScope.setVisible(true);

    }
}

void TemporalScope::computeGrid(int subdivision)
{
    leftChannelScope.computeGrid(subdivision);
    rightChannelScope.computeGrid(subdivision);
    monoScope.computeGrid(subdivision);
}

void TemporalScope::computeStereoPath()
{
    leftInputPath.clear();
    leftOutputPath.clear();
    rightInputPath.clear();
    rightOutputPath.clear();

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

    float width = static_cast<float>(leftChannelScope.getWidth());
    float height = static_cast<float>(leftChannelScope.getHeight());

    juce::int64 ppqSampleMod = (ppqSample + (juce::int64)bufferSize) % (juce::int64)nbSamplePerBeat - 1;
    if (ppqSampleMod < 0)
        return;

    float pos = juce::jmap((float)ppqSampleMod, 0.0f, (float)(nbSamplePerBeat - 1), 0.0f, width);

    //Left
    float inputMag = inputTemporalBuffer.getSample(1, 0);
    float outputMag = outputTemporalBuffer.getSample(1, 0);
    float inputMagRemap = juce::jmap(inputMag / inputNormCoef, minLin, maxLin, height, 0.0f);
    float outputMagRemap = juce::jmap(outputMag / outputNormCoef, minLin, maxLin, height, 0.0f);

    leftInputPath.startNewSubPath(pos, inputMagRemap);
    leftOutputPath.startNewSubPath(pos, outputMagRemap);

    inputMag = inputTemporalBuffer.getSample(0, 0);
    outputMag = outputTemporalBuffer.getSample(0, 0);
    inputMagRemap = juce::jmap(inputMag / inputNormCoef, minLin, maxLin, height, 0.0f);
    outputMagRemap = juce::jmap(outputMag / outputNormCoef, minLin, maxLin, height, 0.0f);

    rightInputPath.startNewSubPath(pos, inputMagRemap);
    rightOutputPath.startNewSubPath(pos, outputMagRemap);

    juce::int64 forLim = (juce::int64)nbSamplePerBeat - ppqSampleMod;
    jassert(juce::isPositiveAndBelow(forLim, inputTemporalBuffer.getNumSamples()));
    jassert(juce::isPositiveAndBelow(forLim, outputTemporalBuffer.getNumSamples()));
    jassert(juce::isPositiveAndBelow(forLim, nbSamplePerBeat));

    int inc = nbSamplePerBeat / (getWidth() + 1) + 1;
    //inc = 10;
    for (juce::int64 i = 1; i < forLim; i = i + inc)
    {
        pos = juce::jmap((float)i + (float)ppqSampleMod, 0.0f, (float)(nbSamplePerBeat - 1), 0.0f, width);

        inputMag = inputTemporalBuffer.getSample(1, i);
        outputMag = outputTemporalBuffer.getSample(1, i);
        inputMagRemap = juce::jmap(inputMag / inputNormCoef, minLin, maxLin, height, 0.0f);
        outputMagRemap = juce::jmap(outputMag / outputNormCoef, minLin, maxLin, height, 0.0f);

        leftInputPath.lineTo(pos, inputMagRemap);
        leftOutputPath.lineTo(pos, outputMagRemap);

        inputMag = inputTemporalBuffer.getSample(0, i);
        outputMag = outputTemporalBuffer.getSample(0, i);
        inputMagRemap = juce::jmap(inputMag / inputNormCoef, minLin, maxLin, height, 0.0f);
        outputMagRemap = juce::jmap(outputMag / outputNormCoef, minLin, maxLin, height, 0.0f);

        rightInputPath.lineTo(pos, inputMagRemap);
        rightOutputPath.lineTo(pos, outputMagRemap);
    }

    int count = 0;

    pos = juce::jmap((float)count, 0.0f, (float)(nbSamplePerBeat - 1), 0.0f, width);
    count += inc;

    inputMag = inputTemporalBuffer.getSample(1, forLim);
    outputMag = outputTemporalBuffer.getSample(1, forLim);
    inputMagRemap = juce::jmap(inputMag / inputNormCoef, minLin, maxLin, height, 0.0f);
    outputMagRemap = juce::jmap(outputMag / outputNormCoef, minLin, maxLin, height, 0.0f);

    leftInputPath.startNewSubPath(pos, inputMagRemap);
    leftOutputPath.startNewSubPath(pos, outputMagRemap);

    inputMag = inputTemporalBuffer.getSample(0, forLim);
    outputMag = outputTemporalBuffer.getSample(0, forLim);
    inputMagRemap = juce::jmap(inputMag / inputNormCoef, minLin, maxLin, height, 0.0f);
    outputMagRemap = juce::jmap(outputMag / outputNormCoef, minLin, maxLin, height, 0.0f);

    rightInputPath.startNewSubPath(pos, inputMagRemap);
    rightOutputPath.startNewSubPath(pos, outputMagRemap);

    for (int i = forLim + 1; i < nbSamplePerBeat; i = i + inc)
    {
        jassert(juce::isPositiveAndBelow(i, inputTemporalBuffer.getNumSamples()));
        jassert(juce::isPositiveAndBelow(i, outputTemporalBuffer.getNumSamples()));
        jassert(juce::isPositiveAndBelow(i, nbSamplePerBeat));
        pos = juce::jmap((float)count, 0.0f, (float)(nbSamplePerBeat - 1), 0.0f, width);
        count += inc;

        inputMag = inputTemporalBuffer.getSample(1, i);
        outputMag = outputTemporalBuffer.getSample(1, i);
        inputMagRemap = juce::jmap(inputMag / inputNormCoef, minLin, maxLin, height, 0.0f);
        outputMagRemap = juce::jmap(outputMag / outputNormCoef, minLin, maxLin, height, 0.0f);

        leftInputPath.lineTo(pos, inputMagRemap);
        leftOutputPath.lineTo(pos, outputMagRemap);

        inputMag = inputTemporalBuffer.getSample(0, i);
        outputMag = outputTemporalBuffer.getSample(0, i);
        inputMagRemap = juce::jmap(inputMag / inputNormCoef, minLin, maxLin, height, 0.0f);
        outputMagRemap = juce::jmap(outputMag / outputNormCoef, minLin, maxLin, height, 0.0f);

        rightInputPath.lineTo(pos, inputMagRemap);
        rightOutputPath.lineTo(pos, outputMagRemap);

    }
}

void TemporalScope::computeMonoPath()
{
    monoInputPath.clear();
    monoOutputPath.clear();

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

    float width = static_cast<float>(monoScope.getWidth());
    float height = static_cast<float>(monoScope.getHeight());

    juce::int64 ppqSampleMod = (ppqSample + (juce::int64)bufferSize) % (juce::int64)nbSamplePerBeat - 1;
    if (ppqSampleMod < 0)
        return;

    float pos = juce::jmap((float)ppqSampleMod, 0.0f, (float)(nbSamplePerBeat - 1), 0.0f, width);

    //Left
    float inputMag = (inputTemporalBuffer.getSample(0, 0) + inputTemporalBuffer.getSample(1, 0)) * 0.5f;
    float outputMag = (outputTemporalBuffer.getSample(0, 0) + outputTemporalBuffer.getSample(1, 0)) * 0.5f;
    float inputMagRemap = juce::jmap(inputMag / inputNormCoef, minLin, maxLin, height, 0.0f);
    float outputMagRemap = juce::jmap(outputMag / outputNormCoef, minLin, maxLin, height, 0.0f);

    monoInputPath.startNewSubPath(pos, inputMagRemap);
    monoOutputPath.startNewSubPath(pos, outputMagRemap);

    //inputMag = (inputTemporalBuffer.getSample(0, 0) + inputTemporalBuffer.getSample(1, 0) ) * 0.5f;
    //outputMag = (outputTemporalBuffer.getSample(0, 0) + outputTemporalBuffer.getSample(1, 0)) * 0.5f;
    //inputMagRemap = juce::jmap(inputMag / inputNormCoef, minLin, maxLin, height, 0.0f);
    //outputMagRemap = juce::jmap(outputMag / outputNormCoef, minLin, maxLin, height, 0.0f);

    //rightInputPath.startNewSubPath(pos, inputMagRemap);
    //rightOutputPath.startNewSubPath(pos, outputMagRemap);

    juce::int64 forLim = (juce::int64)nbSamplePerBeat - ppqSampleMod;
    jassert(juce::isPositiveAndBelow(forLim, inputTemporalBuffer.getNumSamples()));
    jassert(juce::isPositiveAndBelow(forLim, outputTemporalBuffer.getNumSamples()));
    jassert(juce::isPositiveAndBelow(forLim, nbSamplePerBeat));

    int inc = nbSamplePerBeat / (getWidth() + 1) + 1;
    //inc = 10;
    for (juce::int64 i = 1; i < forLim; i = i + inc)
    {
        pos = juce::jmap((float)i + (float)ppqSampleMod, 0.0f, (float)(nbSamplePerBeat - 1), 0.0f, width);

        inputMag = (inputTemporalBuffer.getSample(0, i) + inputTemporalBuffer.getSample(1, i)) * 0.5f;
        outputMag = (outputTemporalBuffer.getSample(0, i) + outputTemporalBuffer.getSample(1, i)) * 0.5f;
        inputMagRemap = juce::jmap(inputMag / inputNormCoef, minLin, maxLin, height, 0.0f);
        outputMagRemap = juce::jmap(outputMag / outputNormCoef, minLin, maxLin, height, 0.0f);

        monoInputPath.lineTo(pos, inputMagRemap);
        monoOutputPath.lineTo(pos, outputMagRemap);
    }

    int count = 0;

    pos = juce::jmap((float)count, 0.0f, (float)(nbSamplePerBeat - 1), 0.0f, width);
    count += inc;

    inputMag = (inputTemporalBuffer.getSample(0, forLim) + inputTemporalBuffer.getSample(1, forLim)) * 0.5f;
    outputMag = (outputTemporalBuffer.getSample(0, forLim) + outputTemporalBuffer.getSample(1, forLim)) * 0.5f;
    inputMagRemap = juce::jmap(inputMag / inputNormCoef, minLin, maxLin, height, 0.0f);
    outputMagRemap = juce::jmap(outputMag / outputNormCoef, minLin, maxLin, height, 0.0f);

    monoInputPath.startNewSubPath(pos, inputMagRemap);
    monoOutputPath.startNewSubPath(pos, outputMagRemap);

    for (int i = forLim + 1; i < nbSamplePerBeat; i = i + inc)
    {
        jassert(juce::isPositiveAndBelow(i, inputTemporalBuffer.getNumSamples()));
        jassert(juce::isPositiveAndBelow(i, outputTemporalBuffer.getNumSamples()));
        jassert(juce::isPositiveAndBelow(i, nbSamplePerBeat));
        pos = juce::jmap((float)count, 0.0f, (float)(nbSamplePerBeat - 1), 0.0f, width);
        count += inc;

        inputMag = (inputTemporalBuffer.getSample(0, i) + inputTemporalBuffer.getSample(1, i)) * 0.5f;
        outputMag = (outputTemporalBuffer.getSample(0, i) + outputTemporalBuffer.getSample(1, i)) * 0.5f;
        inputMagRemap = juce::jmap(inputMag / inputNormCoef, minLin, maxLin, height, 0.0f);
        outputMagRemap = juce::jmap(outputMag / outputNormCoef, minLin, maxLin, height, 0.0f);

        monoInputPath.lineTo(pos, inputMagRemap);
        monoOutputPath.lineTo(pos, outputMagRemap);

    }
}
