/*
  ==============================================================================

    ScopeComponent.cpp
    Created: 8 Apr 2021 3:28:30pm
    Author:  CookiMonstor

  ==============================================================================
*/

#include <JuceHeader.h>
#include "ScopeDisplay.h"

//==============================================================================
ScopeDisplay::ScopeDisplay(MainLayerDataStruct& dataStruct)
    : preEQFFTBuffer(2,4096)
    , postEQFFTBuffer(2, 4096)
    , postDistoFFTBuffer(2, 4096)
    , dataStruct(dataStruct)
{
    setInterceptsMouseClicks(false, true);
    setOpaque(true);

    for (int filterID = 0; filterID < EQConstants::Processor<float>::nbOfFilterMax; ++filterID)
    {
        filterDraggerVector.add(new SelectablePointDragger(EQConstants::UI::FilterColour::filterColour[filterID],
            juce::Colours::ghostwhite));
        addChildComponent(filterDraggerVector[filterID]);

        filterDrawingBank[filterID].isActive = false;
        filterDrawingBank[filterID].colour = EQConstants::UI::FilterColour::filterColour[filterID];
    }

    filterSumDrawing.colour = juce::Colours::yellow;
    filterSumDrawing.opacity = 1.0f;
    filterSumDrawing.isBypassed = false;

    preEQFFTBuffer.clear();
    postEQFFTBuffer.clear();
    postDistoFFTBuffer.clear();
    
    isStereo = false;

    zoomComboBox.setLookAndFeel(&lafComboBox);
    zoomComboBox.addItem(ScopeConstants::UI::thirtydB::gridLabelStringArray[0], 1);
    zoomComboBox.addItem(ScopeConstants::UI::fifteendB::gridLabelStringArray[0], 2);
    zoomComboBox.addItem(ScopeConstants::UI::ninedB::gridLabelStringArray[0], 3);
    zoomComboBox.addItem(ScopeConstants::UI::sixdB::gridLabelStringArray[0], 4);
    zoomComboBox.addItem(ScopeConstants::UI::threedB::gridLabelStringArray[0], 5);
    zoomComboBox.setSelectedId(1, juce::dontSendNotification);
    zoomComboBox.addListener(this);
    gainMin = ScopeConstants::UI::thirtydB::Constants<float>::minGaindB;
    gainMax = ScopeConstants::UI::thirtydB::Constants<float>::maxGaindB;
    gainLabelInc = ScopeConstants::UI::thirtydB::Constants<float>::gainLabelIncdB;
    addAndMakeVisible(zoomComboBox);

    for (int i = 0; i < ScopeConstants::UI::thirtydB::gridLabelStringArray.size() - 1; ++i)
    {
        labelVector.add(new juce::Label());
        labelVector[i]->setText(ScopeConstants::UI::thirtydB::gridLabelStringArray[i + 1], juce::dontSendNotification);
        labelVector[i]->setJustificationType(juce::Justification::centred);
        labelVector[i]->setColour(juce::Label::ColourIds::textColourId, juce::Colours::white);
        addAndMakeVisible(labelVector[i]);
    }

    for (int i = 0; i < 4096; ++i)
    {
        xAxisf[i] = 0.0f;
    }

    for (int i = 0; i < EQConstants::UI::filterMagnitudeSize; ++i)
    {
        xAxisForFilterMagnitude[i] = 0.0f;
        freqForFilterMagnitude[i] = 0.0f;
    }
}

ScopeDisplay::~ScopeDisplay()
{
    zoomComboBox.setLookAndFeel(nullptr);
}

void ScopeDisplay::paint (juce::Graphics& g)
{
    //const auto bounds = getLocalBounds().toFloat();
    //juce::Point<float> top = bounds.getTopLeft();
    //juce::ColourGradient grad = juce::ColourGradient{ juce::Colours::blue.withMultipliedAlpha(0.9f),
    //                                                bounds.getBottomLeft(),
    //                                                juce::Colours::transparentBlack,
    //                                                top,
    //                                                false };

    //g.setGradientFill(grad);
    //g.fillRect(getLocalBounds());

    if (!isDisplayed)
        return;

    g.setOpacity(1.0f);

    g.fillAll(juce::Colours::black);

    drawLogGridScope(g);

    g.setColour(juce::Colours::cyan);
    //g.setOpacity(0.4f);
    juce::ColourGradient grad = juce::ColourGradient(juce::Colours::cyan, (float)getWidth() / 2.0f, 0.0f, juce::Colours::transparentWhite, (float)getWidth() / 2.0f, (float)getHeight(), false);
    g.setGradientFill(grad);
    g.fillPath(fftPreEQPath);// , juce::PathStrokeType(1.0f));
    g.setOpacity(1.0f);
    g.strokePath(fftPreEQPath, juce::PathStrokeType(2.0f));

    g.setColour(juce::Colours::orange);
    //g.setOpacity(0.2f);
    grad = juce::ColourGradient(juce::Colours::orange, (float)getWidth() / 2.0f, 0.0f, juce::Colours::transparentWhite, (float)getWidth() / 2.0f, (float)getHeight(), false);
    g.setGradientFill(grad);
    g.fillPath(fftPostEQPath);
    g.setOpacity(1.0f);
    g.strokePath(fftPostEQPath, juce::PathStrokeType(2.0f));

    g.setColour(juce::Colours::purple);
    //g.setOpacity(0.1f);
    grad = juce::ColourGradient(juce::Colours::purple, (float)getWidth() / 2.0f, 0.0f, juce::Colours::transparentWhite, (float)getWidth() / 2.0f, (float)getHeight(), false);
    g.setGradientFill(grad);
    g.fillPath(fftPostDistoPath);
    g.setOpacity(1.0f);
    g.strokePath(fftPostDistoPath, juce::PathStrokeType(1.0f));

    drawFilterResponse(g);
}

void ScopeDisplay::resized()
{
    //int rotaryPixelSize = ScopeConstants::UI::sliderHeight;
    //int labelWidth = ScopeConstants::UI::labelWidth;
    //int labelHeight = ScopeConstants::UI::labelHeight;
    //int pixelSpace = ScopeConstants::UI::pixelSpace;

    //Display Dragger.
    for (int filterID = 0; filterID < EQConstants::Processor<float>::nbOfFilterMax; ++filterID)
    {
        filterDraggerVector[filterID]->setBounds(0, 0, 11, 11);
    }

    //Log Grid Path
    computeLogGridPath();
    computeGainGridPath();
    computeXAxisValueForFFTPath();
    computeFrequencyAndXAxisValueForFilterMagnitude();
}

void ScopeDisplay::computeLogGridPath()
{
    //Log Grid inner grid
    float width = (float)getWidth() - 100.0f;
    //float finc = 44100.0f / 4096.0f;
    float logMin = std::log10(10.0f);
    //Log Grid============================================================================================================================================================
    //Axis for 10, 100, 1000, 10000
    logAxisGridPath.clear();
    int numberOfDecade = 4;
    for (int decade = 1; decade < numberOfDecade + 1; ++decade)
    {
        float freq = std::pow(10.0f, (float)decade);

        float a = std::log10(freq) - logMin;
        float b = width - 1;
        float c = std::log10(22050.0f) - logMin;
        float xAxis = a * b / c;

        logAxisGridPath.startNewSubPath(xAxis, 0.0f);
        logAxisGridPath.lineTo(xAxis, (float)getBottom());
    }

    //inner Log grid filling
    innerLogAxisGridPath.clear();

    for (int decade = 1; decade < numberOfDecade + 0 ; ++decade)
    {
        //Log axis
        for (int i = 1; i < 10; ++i)
        {
            float freq = ((float) i) * std::pow(10.0f, (float) decade);

            float a = std::log10(freq) - logMin;
            float b = width;
            float c = std::log10(22050.0f) - logMin;
            float xAxis = a * b / c;
            //freq += finc;

            innerLogAxisGridPath.startNewSubPath(xAxis, 0.0f);
            innerLogAxisGridPath.lineTo(xAxis, (float)getBottom());
        }
    }

    float freq = (2.0f) * std::pow(10.0f, 4.0f);

    float a = std::log10(freq) - logMin;
    float b = width;
    float c = std::log10(22050.0f) - logMin;
    float xAxis = a * b / c;
    innerLogAxisGridPath.startNewSubPath(xAxis, 0.0f);
    innerLogAxisGridPath.lineTo(xAxis, (float)getBottom());

    //freq = (3.0f) * std::pow(10.0f, 4.0f);
    //a = std::log10(freq) - logMin;
    //b = width;
    //c = std::log10(22050.0f) - logMin;
    //xAxis = a * b / c;

    //innerLogAxisGridPath.startNewSubPath(xAxis, 0.0f);
    //innerLogAxisGridPath.lineTo(xAxis, (float)getBottom());
}

void ScopeDisplay::computeGainGridPath()
{
    //Create the line for the filter gain.=================================================================================================================================
    float labelHeight = (float)ScopeConstants::UI::labelHeight;
    float labelWidth = ScopeConstants::UI::gainLabelWidth;
    float height = (float) getHeight();
    float xPosPath = (float)getRight() - 60.0f;
    float xPos = (float)getWidth() - 80.0f;
    float yPos = juce::jmap(3.0f*gainLabelInc, gainMin, gainMax, height, 0.0f);

    //Cb box
    gainAxisGridPath.clear();
    gainAxisGridPath.startNewSubPath(0.0f, yPos);
    gainAxisGridPath.lineTo(xPosPath, yPos);
    zoomComboBox.setBounds(xPosPath, yPos - 10.0f, labelWidth, labelHeight);

    //0 dB
    xPos = (float)getWidth() - 50.0f;
    yPos = juce::jmap(0.0f, gainMin, gainMax, height, 0.0f);
    gainAxisGridPath.startNewSubPath(0.0f, yPos);
    gainAxisGridPath.lineTo(xPosPath, yPos);
    labelVector[2]->setBounds(xPosPath, yPos - 10.0f, labelWidth, labelHeight);

    for (int i = 1 ; i < 3 ; ++i)
    {
        yPos = juce::jmap((float) (3 - i) * gainLabelInc, gainMin, gainMax, height, 0.0f);
        gainAxisGridPath.startNewSubPath(0.0f, yPos);
        gainAxisGridPath.lineTo(xPosPath, yPos);
        labelVector[i - 1]->setBounds(xPosPath, yPos - 10.0f, labelWidth, labelHeight);

        yPos = juce::jmap(-((float)i * gainLabelInc), gainMin, gainMax, height, 0.0f);
        gainAxisGridPath.startNewSubPath(0.0f, yPos);
        gainAxisGridPath.lineTo(xPosPath, yPos);
        labelVector[i + 2]->setBounds(xPosPath, yPos - 10.0f, labelWidth, labelHeight);
    }

    yPos = juce::jmap(-3.0f * gainLabelInc, gainMin, gainMax, height, 0.0f);
    gainAxisGridPath.startNewSubPath(0.0f, yPos);
    gainAxisGridPath.lineTo(xPosPath, yPos);
    labelVector[5]->setBounds(xPosPath, yPos - 10.0f, labelWidth, labelHeight);
}

void ScopeDisplay::computeXAxisValueForFFTPath()
{
    //Create the frequency axis for the FFT=================================================================================================================================
    float finc = 44100.0f / 4096.0f; //  ((float) xAxisf.size());
    float freq = finc;
    float width = (float)getWidth() - 100.0f;
    float logMin = std::log10(10.0f);

    for (int i = 1; i < EQConstants::UI::filterMagnitudeSize; ++i)
    {
        float a = std::log10(freq) - logMin;
        float b = width;
        float c = std::log10(22050.0f) - logMin;
        xAxisf[i] = a * b / c;
        freq += finc;
    }

    xAxisf[0] = -xAxisf[11];
}

void ScopeDisplay::computeFrequencyAndXAxisValueForFilterMagnitude()
{
    //Create the axis for the filter Magnitude.========================
    //float fInc = 22050.0 / EQConstants::UI::filterMagnitudeSize;

    int counter = 0;
    float fInc = (1.0f - 0.0f) / (float)(EQConstants::UI::nbOfPointsFirstBloc + 1);
    float min = 0.0f;
    float max = 10.0f;
    float freqC = fInc;
    float width = (float)getWidth() - 100.0f;
    float logMin = std::log10(10.0f);
    //freqForFilterMagnitude[0] = 0.0f;
    //xAxisForFilterMagnitude[0] = 0.0f;

    for (int i = 0; i < EQConstants::UI::nbOfPointsFirstBloc; ++i)
    {
        //float freq = juce::mapToLog10(freqC, min, max);
        float freq = juce::jmap(freqC, 0.0f, 1.0f, min, max);
        freqForFilterMagnitude[counter] = freq;
        freqC += fInc;
        counter++;
    }

    fInc = (1.0f - 0.0f) / (float)EQConstants::UI::nbOfPointsPerBloc;

    for (int bloc = 0; bloc < EQConstants::UI::nbOfBloc - 1; ++bloc)
    {
        min = std::pow(10.0f, (float)(bloc + 1));
        max = std::pow(10.0f, (float)(bloc + 2));

        freqC = 0.0f;
        for (int i = 0; i < EQConstants::UI::nbOfPointsPerBloc; ++i)
        {
            float freq = juce::mapToLog10(freqC, min, max);
            freqC += fInc;
            freqForFilterMagnitude[counter] = freq;
            counter++;
        }
    }

    min = 10000.0;
    max = 22050.0;

    freqC = 0.0f;

    for (int i = 0; i < EQConstants::UI::nbOfPointsPerBloc; ++i)
    {
        float freq = juce::mapToLog10(freqC, min, max);
        freqC += fInc;
        freqForFilterMagnitude[counter] = freq;
        counter++;
    }

    //xAxisForFilterMagnitude[0] = -500.0f;
    for (int i = 0; i < EQConstants::UI::filterMagnitudeSize; ++i)
    {
        float a = std::log10(freqForFilterMagnitude[i]) - logMin;
        float b = width;
        float c = std::log10(22050.0f) - logMin;
        xAxisForFilterMagnitude[i] = a * b / c;
    }
}

void ScopeDisplay::computeEQFilterFrequencyResponse(MainLayerDataStruct& mainLayerDataStruct, const float* filterMagnitudeValue, int filterID)
{
    int stageID = mainLayerDataStruct.getSelectedStageID();

    filterDrawingBank[filterID].isActive = mainLayerDataStruct.getEQFilterIsActive(stageID, filterID);

    if (!filterDrawingBank[filterID].isActive)
    {
        filterDraggerVector[filterID]->setVisible(false);
        filterDraggerVector[filterID]->setEnabled(false);
        filterDrawingBank[filterID].filterPath.clear();
        return;
    }
        
    filterDraggerVector[filterID]->setVisible(true);
    filterDraggerVector[filterID]->setEnabled(true);

    //auto mindB = EQConstants::UI::scopeMinGaindB_f;
    //auto maxdB = EQConstants::UI::scopeMaxGaindB_f;

    //double sr = 44100.0;
    float scopeHeight = static_cast<float>(getHeight());
    float draggerDiameter = filterDraggerVector[filterID]->getDiameter();
    float remappedMag = juce::jmap(filterMagnitudeValue[0], gainMin, gainMax, scopeHeight, 0.0f);

    bool filterIsBypassed = mainLayerDataStruct.getEQFilterIsBypassed(stageID, filterID);
    float filterCutOff = mainLayerDataStruct.getEQFilterCutoff(stageID, filterID);

    filterDrawingBank[filterID].filterPath.clear();
    filterDrawingBank[filterID].filterPath.startNewSubPath(xAxisForFilterMagnitude[0], remappedMag);
    filterDrawingBank[filterID].type = mainLayerDataStruct.getEQFilterType(stageID, filterID);
    filterDrawingBank[filterID].isBypassed = filterIsBypassed;
    filterDrawingBank[filterID].opacity = filterIsBypassed ? EQConstants::UI::opacityOff_f : EQConstants::UI::opacityOn_f;

    //Linear freq computation===================================================================================================================================================
    for (int i = 1; i < EQConstants::UI::filterMagnitudeSize; ++i)
    {
        remappedMag = juce::jmap(filterMagnitudeValue[i], gainMin, gainMax, scopeHeight, 0.0f);

        filterDrawingBank[filterID].filterPath.lineTo(xAxisForFilterMagnitude[i], remappedMag);
    }

    //FilterDragger SetBounds
    float xPos = 0.0f;
    float yPos = 0.0f;

    xPos = Conversion<float>::fromCutoffToXPosition(filterCutOff, 10.0f, 22050.0f, (float)getWidth() - 100.0f, 0.0f);

    xPos -= 0.5f * draggerDiameter;

    int filterType = mainLayerDataStruct.getEQFilterType(stageID, filterID);

    if (filterType == EQConstants::BiquadConstants<float>::Types::peak || 
        filterType == EQConstants::BiquadConstants<float>::Types::lowshelf || 
        filterType == EQConstants::BiquadConstants<float>::Types::highshelf)
    {
        float gain = mainLayerDataStruct.getEQFilterGain(stageID, filterID);

        //float gainMin = 0.0f;
        //float gainMax = 0.0f;
        switch (zoomComboBox.getSelectedId() - 1)
        {
        case ScopeConstants::UI::ZoomID::thirtydB:
            gainMin = ScopeConstants::UI::thirtydB::Constants<float>::minGaindB;
            gainMax = ScopeConstants::UI::thirtydB::Constants<float>::maxGaindB;
            break;

        case ScopeConstants::UI::ZoomID::fifteendB:
            gainMin = ScopeConstants::UI::fifteendB::Constants<float>::minGaindB;
            gainMax = ScopeConstants::UI::fifteendB::Constants<float>::maxGaindB;
            break;

        case ScopeConstants::UI::ZoomID::ninedB:
            gainMin = ScopeConstants::UI::ninedB::Constants<float>::minGaindB;
            gainMax = ScopeConstants::UI::ninedB::Constants<float>::maxGaindB;
            break;

        case ScopeConstants::UI::ZoomID::sixdB:
            gainMin = ScopeConstants::UI::sixdB::Constants<float>::minGaindB;
            gainMax = ScopeConstants::UI::sixdB::Constants<float>::maxGaindB;
            break;

        case ScopeConstants::UI::ZoomID::threedB:
            gainMin = ScopeConstants::UI::threedB::Constants<float>::minGaindB;
            gainMax = ScopeConstants::UI::threedB::Constants<float>::maxGaindB;
            break;

        default:
            gainMin = ScopeConstants::UI::thirtydB::Constants<float>::minGaindB;
            gainMax = ScopeConstants::UI::thirtydB::Constants<float>::maxGaindB;
            break;
        }

        yPos = Conversion<float>::fromGainToYPosition(gain, gainMin, gainMax, scopeHeight, 0.0f);
        yPos -= 0.5f * draggerDiameter;
    }
    else
    {
        float qMin = 0.0f;
        float qMax = 0.0f;

        switch (zoomComboBox.getSelectedId() - 1)
        {
        case ScopeConstants::UI::ZoomID::thirtydB:
            qMin = ScopeConstants::UI::thirtydB::Constants<float>::qMin;
            qMax = ScopeConstants::UI::thirtydB::Constants<float>::qMax;
            break;

        case ScopeConstants::UI::ZoomID::fifteendB:
            qMin = ScopeConstants::UI::fifteendB::Constants<float>::qMin;
            qMax = ScopeConstants::UI::fifteendB::Constants<float>::qMax;
            break;

        case ScopeConstants::UI::ZoomID::ninedB:
            qMin = ScopeConstants::UI::ninedB::Constants<float>::qMin;
            qMax = ScopeConstants::UI::ninedB::Constants<float>::qMax;
            break;

        case ScopeConstants::UI::ZoomID::sixdB:
            qMin = ScopeConstants::UI::sixdB::Constants<float>::qMin;
            qMax = ScopeConstants::UI::sixdB::Constants<float>::qMax;
            break;

        case ScopeConstants::UI::ZoomID::threedB:
            qMin = ScopeConstants::UI::threedB::Constants<float>::qMin;
            qMax = ScopeConstants::UI::threedB::Constants<float>::qMax;
            break;

        default:
            qMin = ScopeConstants::UI::thirtydB::Constants<float>::qMin;
            qMax = ScopeConstants::UI::thirtydB::Constants<float>::qMax;
            break;
        }
        
        yPos = Conversion<float>::fromQToYPositon(mainLayerDataStruct.getEQFilterQ(stageID, filterID), qMin, qMax, scopeHeight, 0.0f);
        yPos -= 0.5f * draggerDiameter;
    }

    if (filterType == EQConstants::BiquadConstants<float>::Types::notch || 
        filterType == EQConstants::BiquadConstants<float>::Types::bandpass) //We lock for bandpass and notch
        yPos = scopeHeight * 0.5f - draggerDiameter * 0.5f;

    filterDraggerVector[filterID]->setBounds(static_cast<int>(xPos),
                                             static_cast<int>(yPos),
                                             static_cast<int>(draggerDiameter),
                                             static_cast<int>(draggerDiameter));
}

void ScopeDisplay::computeEQFilterSumFrequencyResponse(const float* filterSumMagnitudeValue, bool isAnyActiveFilter)
{
    filterSumDrawing.isActive = isAnyActiveFilter;

    if (!filterSumDrawing.isActive)
        return;

    //auto mindB = EQConstants::UI::scopeMinGaindB_f;
    //auto maxdB = EQConstants::UI::scopeMaxGaindB_f;

    //double sr = 44100.0;
    float scopeHeight = (float) getHeight();
    float remappedMag = juce::jmap(filterSumMagnitudeValue[0], gainMin, gainMax, scopeHeight, 0.0f);

    filterSumDrawing.filterPath.clear();
    filterSumDrawing.filterPath.startNewSubPath(xAxisForFilterMagnitude[0], remappedMag);

    //Linear freq computation===================================================================================================================================================
    for (int i = 1; i < EQConstants::UI::filterMagnitudeSize; ++i)
    {
        remappedMag = juce::jmap(filterSumMagnitudeValue[i], gainMin, gainMax, scopeHeight, 0.0f);

        filterSumDrawing.filterPath.lineTo(xAxisForFilterMagnitude[i], remappedMag);
    }
}

void ScopeDisplay::computeFFTPath()
{
    computeFFTPath(&preEQFFTBuffer, &fftPreEQPath);
    computeFFTPath(&postEQFFTBuffer, &fftPostEQPath);
    computeFFTPath(&postDistoFFTBuffer, &fftPostDistoPath);

}

void ScopeDisplay::computeFFTPath(juce::AudioBuffer<float>* fftBuffer, juce::Path* fftPath)
{
    auto rangeMindB = -100.0f;
    auto rangeMaxdB = 0.0f;

    //float sample = fftBuffer->getSample(0, 1);

    float sampleR = fftBuffer->getSample(0, 0);
    float sampleL = fftBuffer->getSample(0, 0);

    sampleR = std::isnan(sampleR) ? std::numeric_limits<float>::min()
                             : sampleR;

    sampleR = sampleR == 0.0f ? std::numeric_limits<float>::min()
                              : sampleR;

    sampleL = std::isnan(sampleR) ? std::numeric_limits<float>::min()
                                  : sampleL;

    sampleL = sampleL == 0.0f ? std::numeric_limits<float>::min()
                              : sampleL;

    int numSamples = fftBuffer->getNumSamples() / 2;
    float logNumSample = std::log10(static_cast<float>(2*numSamples));
    float factor = 20.0f;
    float avgSampleLR = factor * (log10f((sampleR + sampleL) * 0.5f) - logNumSample);

    auto height = (float) getHeight();

    fftPath->clear();
    fftPath->startNewSubPath(xAxisf[0],
        juce::jmap(avgSampleLR, rangeMindB, rangeMaxdB, (float)height, 0.0f));

    for (int sample = 1; sample < numSamples; ++sample)
    {
        sampleR = fftBuffer->getSample(0, sample);
        sampleR = isnan(sampleR)
            ? std::numeric_limits<float>::min()
            : sampleR;

        sampleR = sampleR == 0.0f
            ? std::numeric_limits<float>::min()
            : sampleR;

        sampleL = fftBuffer->getSample(1, sample);
        sampleL = isnan(sampleR)
            ? std::numeric_limits<float>::min()
            : sampleL;

        sampleL = sampleL == 0.0f
            ? std::numeric_limits<float>::min()
            : sampleL;

        avgSampleLR = factor * (log10f((sampleR + sampleL) * 0.5f) - logNumSample);

        fftPath->lineTo(xAxisf[sample], juce::jmap(avgSampleLR, rangeMindB, rangeMaxdB, (float)height, 0.0f));
    }

    fftPath->lineTo(xAxisf[numSamples - 1], juce::jmap(rangeMindB - 10.0f, rangeMindB, rangeMaxdB, (float)height, 0.0f));
    fftPath->lineTo(xAxisf[0], juce::jmap(rangeMindB - 10.0f, rangeMindB, rangeMaxdB, (float)height, 0.0f));
    fftPath->closeSubPath();

        
}

void ScopeDisplay::drawFilterResponse(juce::Graphics& g)
{
    if (filterSumDrawing.isActive)
    {
        for (int filterID = 0; filterID < EQConstants::Processor<float>::nbOfFilterMax; ++filterID)
        {
            if (filterID == currentFilterDragger)
                continue;

            g.setColour(filterDrawingBank[filterID].colour);
            g.setOpacity(filterDrawingBank[filterID].opacity);
            //, juce::AffineTransform::scale((float)getWidth() / (float)axis30000HzValue, 1.0f)
            g.strokePath(filterDrawingBank[filterID].filterPath, juce::PathStrokeType(1.0f));

        }

        if (!(currentFilterDragger == -1))
        {
            //OuterLayer
            g.setColour(filterDrawingBank[currentFilterDragger].colour);
            g.setOpacity(filterDrawingBank[currentFilterDragger].opacity);
            //, juce::AffineTransform::scale((float)getWidth() / (float)axis30000HzValue, 1.0f)
            g.strokePath(filterDrawingBank[currentFilterDragger].filterPath, juce::PathStrokeType(1.0f));

            //Fill inner.
            filterAreaOfEffectPath.clear();
            filterAreaOfEffectPath.addPath(filterDrawingBank[currentFilterDragger].filterPath);

            closeFilterAreaOfEffectPath(filterDrawingBank[currentFilterDragger].type, currentFilterDragger);

            float opacity = filterDrawingBank[currentFilterDragger].isBypassed ? EQConstants::UI::FilterColour::innerPathOpacityOff_f
                                                                            : EQConstants::UI::FilterColour::innerPathOpacityOn_f;

            g.setOpacity(opacity);
            //, juce::AffineTransform::scale((float)getWidth() / (float)axis30000HzValue, 1.0f)
            g.fillPath(filterAreaOfEffectPath);
        }

        g.setColour(EQConstants::UI::FilterColour::filterSumColour);
        g.setOpacity(1.0f);
        //, juce::AffineTransform::scale((float)getWidth() / (float)axis30000HzValue, 1.0f)
        g.strokePath(filterSumDrawing.filterPath, juce::PathStrokeType(EQConstants::UI::FilterColour::filterSumPathThickness_f));

    }
}

void ScopeDisplay::closeFilterAreaOfEffectPath(int filterType, int filterID)
{
    float length = filterDrawingBank[filterID].filterPath.getLength();
    //float xValueStartPath = filterDrawingBank[filterID].filterPath.getPointAlongPath(0.0f).getX();
    float xValueToReachToClose = filterDrawingBank[filterID].filterPath.getPointAlongPath(length).getX();

    switch (filterType)
    {
    case EQConstants::BiquadConstants<float>::Types::peak:
        filterAreaOfEffectPath.lineTo(xValueToReachToClose, (float)getHeight() * 0.5f);
        filterAreaOfEffectPath.lineTo(0.0f, (float)getHeight() * 0.5f);
        //filterAreaOfEffectPath.lineTo(xValueStartPath, (float)getHeight() / 2.0f);
        filterAreaOfEffectPath.closeSubPath();
        break;
    case EQConstants::BiquadConstants<float>::Types::lowpass:
        filterAreaOfEffectPath.lineTo(xValueToReachToClose, (float)getHeight());
        filterAreaOfEffectPath.lineTo(0.0f, (float)getHeight());
        filterAreaOfEffectPath.closeSubPath();
        break;
    case EQConstants::BiquadConstants<float>::Types::highpass:
        filterAreaOfEffectPath.lineTo(xValueToReachToClose, (float)getHeight());
        filterAreaOfEffectPath.lineTo(0.0f, (float)getHeight());
        //filterAreaOfEffectPath.lineTo(xValueStartPath, (float)getHeight() / 2.0f);
        filterAreaOfEffectPath.closeSubPath();
        break;
    case EQConstants::BiquadConstants<float>::Types::notch:
        filterAreaOfEffectPath.lineTo(xValueToReachToClose, (float)getHeight());
        filterAreaOfEffectPath.lineTo(0.0f, (float)getHeight());
        filterAreaOfEffectPath.closeSubPath();
        break;
    case EQConstants::BiquadConstants<float>::Types::bandpass:
        filterAreaOfEffectPath.lineTo(xValueToReachToClose, (float)getHeight());
        filterAreaOfEffectPath.lineTo(0.0f, (float)getHeight());
        filterAreaOfEffectPath.closeSubPath();
        break;
    case EQConstants::BiquadConstants<float>::Types::lowshelf:
        filterAreaOfEffectPath.lineTo(0.0f, (float)getHeight() * 0.5f);
        filterAreaOfEffectPath.closeSubPath();
        break;
    case EQConstants::BiquadConstants<float>::Types::highshelf:
        filterAreaOfEffectPath.lineTo(xValueToReachToClose, (float)getHeight() * 0.5f);
        filterAreaOfEffectPath.lineTo(0.0f, (float)getHeight() * 0.5f);
        filterAreaOfEffectPath.closeSubPath();
        break;
    default:
        filterAreaOfEffectPath.closeSubPath();
        break;
    }
}

void ScopeDisplay::drawLogGridScope(juce::Graphics& g)
{
    g.setColour(juce::Colours::white);
    g.strokePath(logAxisGridPath, juce::PathStrokeType(1.0f));

    g.setColour(juce::Colours::grey);
    g.strokePath(innerLogAxisGridPath, juce::PathStrokeType(1.0f));

    g.strokePath(gainAxisGridPath, juce::PathStrokeType(1.0f));
}

void ScopeDisplay::setSelectedFilterDragger(int selectedFilter)
{
    for (int filter = 0; filter < EQConstants::Processor<float>::nbOfFilterMax; ++filter)
    {
        filterDraggerVector[filter]->setIsSelected(false);
    }

    currentFilterDragger = selectedFilter;

    if (!(currentFilterDragger == -1))
    {
        filterDraggerVector[currentFilterDragger]->setIsSelected(true);
    }
}

int  ScopeDisplay::findFilterDragger(const juce::MouseEvent& e)
{
    juce::Point<int> mousePos = e.getMouseDownPosition();
    int size = filterDraggerVector.size();
    int pointID = 0;
    juce::Rectangle<int> bounds = filterDraggerVector[pointID]->getBounds();
    bool isInside = bounds.contains(mousePos);

    while ((!isInside) && (pointID < size - 1))
    {
        pointID++;
        bounds = filterDraggerVector[pointID]->getBounds();
        isInside = bounds.contains(mousePos);

    }

    if (!isInside)
    {
        pointID = -1;
    }

    return pointID;
}

bool ScopeDisplay::isMouseOnScope(const juce::MouseEvent& e)
{
    juce::Rectangle<int> scopeBounds = getBounds();
    bool isMouseOnScope = scopeBounds.contains(e.getPosition());
    return isMouseOnScope;
}

int ScopeDisplay::updateUI(MainLayerDataStruct& mainLayerDataStruct, juce::AudioBuffer<float>* pFilterMagnitude, juce::AudioBuffer<float>* pFilterMagnitudeSum)
{
    int stageID = mainLayerDataStruct.getSelectedStageID();
    bool isAnyActiveFilter = false;
    int nbOfActiveFilter = 0;
    int selectedFilterID = mainLayerDataStruct.getSelectedFilterID(stageID);

    setSelectedFilterDragger(selectedFilterID);
    juce::String paramString = ScopeConstants::ParamStringID::GetParamStringID::preEQIsBypassed(stageID);
    if (*mainLayerDataStruct.getAPVTSMainLayer().getRawParameterValue(paramString))
    {
        fftPreEQPath.clear();
    }
    else
    {
        computeFFTPath(&preEQFFTBuffer, &fftPreEQPath);
    }
    paramString = ScopeConstants::ParamStringID::GetParamStringID::postEQIsBypassed(stageID);
    if (*mainLayerDataStruct.getAPVTSMainLayer().getRawParameterValue(paramString))
    {
        fftPostEQPath.clear();
    }
    else
    {
        computeFFTPath(&postEQFFTBuffer, &fftPostEQPath);
    }
    paramString = ScopeConstants::ParamStringID::GetParamStringID::postDistoIsBypassed(stageID);
    if (*mainLayerDataStruct.getAPVTSMainLayer().getRawParameterValue(paramString))
    {
        fftPostDistoPath.clear();
    }
    else
    {
        computeFFTPath(&postDistoFFTBuffer, &fftPostDistoPath);
    }
    

    for (int filterID = 0; filterID < EQConstants::Processor<float>::nbOfFilterMax; ++filterID)
    {
        //computeEQFilterFrequencyResponse(mainLayerDataStruct, pEQProcessor->getFilterMagnitudeValue(filterID), filterID);
        //computeEQFilterFrequencyResponse(mainLayerDataStruct, pEQProcessor->getPointerToFilterMagnitudeValue()[filterID], filterID);
        computeEQFilterFrequencyResponse(mainLayerDataStruct, pFilterMagnitude->getReadPointer(filterID), filterID);

        if (mainLayerDataStruct.getEQFilterIsActive(stageID, filterID))
            nbOfActiveFilter += 1;

        isAnyActiveFilter = isAnyActiveFilter || mainLayerDataStruct.getEQFilterIsActive(stageID, filterID);
    }

    computeEQFilterSumFrequencyResponse(pFilterMagnitudeSum->getReadPointer(0), isAnyActiveFilter);

    return nbOfActiveFilter;
}

void ScopeDisplay::comboBoxChanged(juce::ComboBox* comboBox)
{
    int stageID = dataStruct.getSelectedStageID();
    if (comboBox == &zoomComboBox)
    {
        int zoomID = zoomComboBox.getSelectedId() - 1;
        dataStruct.setScopeZoom(stageID, zoomID);

        setGainLim(zoomID);
        computeGainGridPath();
    }
}

void ScopeDisplay::setGainLim(int zoomID)
{
    switch (zoomID)
    {
    case ScopeConstants::UI::ZoomID::thirtydB:
        gainMin = ScopeConstants::UI::thirtydB::Constants<float>::minGaindB;
        gainMax = ScopeConstants::UI::thirtydB::Constants<float>::maxGaindB;
        gainLabelInc = ScopeConstants::UI::thirtydB::Constants<float>::gainLabelIncdB;
        for (int i = 0; i < labelVector.size(); ++i)
        {
            labelVector[i]->setText(ScopeConstants::UI::thirtydB::gridLabelStringArray[i + 1], juce::dontSendNotification);
        }
        break;

    case ScopeConstants::UI::ZoomID::fifteendB:
        gainMin = ScopeConstants::UI::fifteendB::Constants<float>::minGaindB;
        gainMax = ScopeConstants::UI::fifteendB::Constants<float>::maxGaindB;
        gainLabelInc = ScopeConstants::UI::fifteendB::Constants<float>::gainLabelIncdB;
        for (int i = 0; i < labelVector.size(); ++i)
        {
            labelVector[i]->setText(ScopeConstants::UI::fifteendB::gridLabelStringArray[i + 1], juce::dontSendNotification);
        }
        break;

    case ScopeConstants::UI::ZoomID::ninedB:
        gainMin = ScopeConstants::UI::ninedB::Constants<float>::minGaindB;
        gainMax = ScopeConstants::UI::ninedB::Constants<float>::maxGaindB;
        gainLabelInc = ScopeConstants::UI::ninedB::Constants<float>::gainLabelIncdB;
        for (int i = 0; i < labelVector.size(); ++i)
        {
            labelVector[i]->setText(ScopeConstants::UI::ninedB::gridLabelStringArray[i + 1], juce::dontSendNotification);
        }
        break;

    case ScopeConstants::UI::ZoomID::sixdB:
        gainMin = ScopeConstants::UI::sixdB::Constants<float>::minGaindB;
        gainMax = ScopeConstants::UI::sixdB::Constants<float>::maxGaindB;
        gainLabelInc = ScopeConstants::UI::sixdB::Constants<float>::gainLabelIncdB;
        for (int i = 0; i < labelVector.size(); ++i)
        {
            labelVector[i]->setText(ScopeConstants::UI::sixdB::gridLabelStringArray[i + 1], juce::dontSendNotification);
        }
        break;

    case ScopeConstants::UI::ZoomID::threedB:
        gainMin = ScopeConstants::UI::threedB::Constants<float>::minGaindB;
        gainMax = ScopeConstants::UI::threedB::Constants<float>::maxGaindB;
        gainLabelInc = ScopeConstants::UI::threedB::Constants<float>::gainLabelIncdB;
        for (int i = 0; i < labelVector.size(); ++i)
        {
            labelVector[i]->setText(ScopeConstants::UI::threedB::gridLabelStringArray[i + 1], juce::dontSendNotification);
        }
        break;

    default:
        gainMin = ScopeConstants::UI::thirtydB::Constants<float>::minGaindB;
        gainMax = ScopeConstants::UI::thirtydB::Constants<float>::maxGaindB;
        gainLabelInc = ScopeConstants::UI::thirtydB::Constants<float>::gainLabelIncdB;
        for (int i = 0; i < labelVector.size(); ++i)
        {
            labelVector[i]->setText(ScopeConstants::UI::thirtydB::gridLabelStringArray[i + 1], juce::dontSendNotification);
        }
        break;
    }
}
