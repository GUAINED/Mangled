/*
  ==============================================================================

    WaveShaperScope.cpp
    Created: 9 Apr 2021 2:09:04pm
    Author:  CookiMonstor

  ==============================================================================
*/

#include <JuceHeader.h>
#include "WaveShaperScope.h"

//==============================================================================
WaveShaperScope::WaveShaperScope(MainLayerDataStruct& mlDataStruct)
    : dataStruct(mlDataStruct)
{   
    setInterceptsMouseClicks(false, false);

    for (int pointID = 0; pointID < DistortionConstants::WaveShaper<float>::nbOfPointMax; ++pointID)
    {
        pointDraggerVector.add(new SelectablePointDragger(DistortionConstants::UI::Colour::pointDragger, DistortionConstants::UI::Colour::selectedPointDragger));
        addChildComponent(pointDraggerVector[pointID]);
    }

    for (int tensionID = 0; tensionID < DistortionConstants::WaveShaper<float>::nbOfPointMax - 1; ++tensionID)
    {
        tensionDraggerVector.add(new WaveShaperTensionDragger(juce::Colours::red));
        addChildComponent(tensionDraggerVector[tensionID]);

        binPathData.add(new juce::AudioBuffer<float>(3, DistortionConstants::UI::nbOfPointsPerBin + 1));
    }
}

WaveShaperScope::~WaveShaperScope()
{
}

void WaveShaperScope::paint(juce::Graphics& g)
{
    g.fillAll(DistortionConstants::UI::Colour::background);

    g.setColour(DistortionConstants::UI::Colour::innerGrid); 
    g.strokePath(innerGridPath, juce::PathStrokeType(DistortionConstants::UI::Colour::innerGridStrokeValue));

    g.setColour(DistortionConstants::UI::Colour::outerGrid);
    g.strokePath(outerGridPath, juce::PathStrokeType(DistortionConstants::UI::Colour::outerGridStrokeValue));

    g.setColour(DistortionConstants::UI::Colour::equation);
    g.strokePath(equationDistortionPath, juce::PathStrokeType(DistortionConstants::UI::Colour::equationPathStrokeValue));

    g.setColour(DistortionConstants::UI::Colour::unipolar);
    g.strokePath(unipolarSampleRemapPath, juce::PathStrokeType(DistortionConstants::UI::Colour::unipolarPathStrokeValue));

    g.setColour(bipolarColour);
    g.strokePath(bipolarSampleRemapPath, juce::PathStrokeType(DistortionConstants::UI::Colour::bipolarPathStrokeValue));

    g.setColour(DistortionConstants::UI::Colour::selectedCurve);
    g.strokePath(selectedCurveSampleRemapPath, juce::PathStrokeType(DistortionConstants::UI::Colour::selectedCurvePathStrokeValue));

    g.setColour(DistortionConstants::UI::Colour::bipolarSquare);
    g.setOpacity(DistortionConstants::UI::Colour::bipolarSquareColourOpacity);
    g.fillPath(isBipolarPath);

}

void WaveShaperScope::resized()
{
    drawInnerGrid();

    drawOuterGrid();

    drawIsBipolarRectangle();
}

void WaveShaperScope::drawInnerGrid()
{
    int numberOfLine = 9;
    //float diameter = 10.0f;
    float scopeWidth = (float)getWidth();
    //float halfScopeWidth = scopeWidth * 0.5f;
    float scopeHeight = (float)getHeight();
    //float halfScopeHeight = scopeHeight * 0.5f;
    float incWidth = scopeWidth / (float)numberOfLine;
    float incHeight = (float)getLocalBounds().getHeight() / (float)numberOfLine;
    //float halfIncWidth = incWidth * 0.5f;
    //float halfIncHeight = incHeight * 0.5f;

    innerGridPath.clear();

    for (int i = 0; i < numberOfLine; ++i)
    {
        innerGridPath.startNewSubPath(0.0f, ((float)i + 0.5f) * incHeight);
        innerGridPath.lineTo(scopeWidth, ((float)i + 0.5f) * incHeight);

        innerGridPath.startNewSubPath(((float)i + 0.5f) * incWidth, 0.0f);
        innerGridPath.lineTo(((float)i + 0.5f) * incWidth, scopeHeight);
    }
}

void WaveShaperScope::drawOuterGrid()
{
    float scopeWidth = (float)getWidth();
    float scopeHeight = (float)getHeight();

    float scopeMinLin = DistortionConstants::UI::scopeMinLin_f;
    float scopeMaxLin = DistortionConstants::UI::scopeMaxLin_f;

    float recX = juce::jmap(-1.0f, scopeMinLin, scopeMaxLin, 0.0f, scopeWidth);
    float recY = juce::jmap(1.0f, scopeMinLin, scopeMaxLin, scopeHeight, 0.0f);
    float recWidth = juce::jmap(1.0f, scopeMinLin, scopeMaxLin, 0.0f, scopeWidth) - recX;
    float recHeight = juce::jmap(-1.0f, scopeMinLin, scopeMaxLin, scopeHeight, 0.0f) - recY;

    float midVertLine = juce::jmap(0.0f, scopeMinLin, scopeMaxLin, 0.0f, scopeWidth);
    float midHorzLine = juce::jmap(0.0f, scopeMinLin, scopeMaxLin, scopeHeight, 0.0f);

    outerGridPath.clear();

    outerGridPath.startNewSubPath(recX, midHorzLine);
    outerGridPath.lineTo(recWidth + recX, midHorzLine);

    outerGridPath.startNewSubPath(midVertLine, recY);
    outerGridPath.lineTo(midVertLine, recHeight + recY);

    outerGridPath.addRectangle(recX, recY, recWidth, recHeight);
}

void WaveShaperScope::drawIsBipolarRectangle()
{
    float scopeWidth = (float)getWidth();
    float scopeHeight = (float)getHeight();

    float scopeMinLin = DistortionConstants::UI::scopeMinLin_f;
    float scopeMaxLin = DistortionConstants::UI::scopeMaxLin_f;

    float recX = juce::jmap(-1.0f, scopeMinLin, scopeMaxLin, 0.0f, scopeWidth);
    float recY = juce::jmap(1.0f, scopeMinLin, scopeMaxLin, scopeHeight, 0.0f);
    float recWidth = juce::jmap(0.0f, scopeMinLin, scopeMaxLin, 0.0f, scopeWidth) - recX;
    float recHeight = juce::jmap(-1.0f, scopeMinLin, scopeMaxLin, scopeHeight, 0.0f) - recY;

    isBipolarPath.clear();

    isBipolarPath.addRectangle(recX, recY, recWidth, recHeight);
}

void WaveShaperScope::drawSampleRemapperPath(juce::Path* path, const float* binXData, const float* binYData)//, float* eqBinYData)
{
    float scopeWidth = (float)getWidth();
    float scopeHeight = (float)getHeight();

    float xScope = juce::jmap(binXData[0], -1.125f, 1.125f, 0.0f, scopeWidth);
    float yScope = juce::jmap(binYData[0], -1.125f, 1.125f, scopeHeight, 0.0f);
    //float eqYScope = juce::jmap(eqBinYData[0], -1.125f, 1.125f, scopeHeight, 0.0f);

    path->startNewSubPath(xScope, yScope);
    //equationDistortionPath.startNewSubPath(xScope, eqYScope);

    for (int i = 1; i < 101; ++i)
    {
        xScope = juce::jmap(binXData[i], -1.125f, 1.125f, 0.0f, scopeWidth);
        yScope = juce::jmap(binYData[i], -1.125f, 1.125f, scopeHeight, 0.0f);
        //eqYScope = juce::jmap(eqBinYData[i], -1.125f, 1.125f, scopeHeight, 0.0f);

        jassert(!isnan(yScope));

        path->lineTo(xScope, yScope);
        //equationDistortionPath.lineTo(xScope, eqYScope);
    }
}

void WaveShaperScope::drawUnipolarPath(const float* binXData, const float* binYData)
{
    drawSampleRemapperPath(&unipolarSampleRemapPath, binXData, binYData);

}

void WaveShaperScope::drawBipolarPath(const float* binXData, const float* binYData)
{
    drawSampleRemapperPath(&bipolarSampleRemapPath, binXData, binYData);
}

void WaveShaperScope::drawSelectedCurvePath(const float* binXData, const float* binYData)
{
    float scopeWidth = (float)getWidth();
    float scopeHeight = (float)getHeight();

    float xScope = juce::jmap(binXData[0], -1.125f, 1.125f, 0.0f, scopeWidth);
    float yScope = juce::jmap(binYData[0], -1.125f, 1.125f, scopeHeight, 0.0f);

    selectedCurveSampleRemapPath.startNewSubPath(xScope, yScope);

    for (int i = 1; i < 101; ++i)
    {
        xScope = juce::jmap(binXData[i], -1.125f, 1.125f, 0.0f, scopeWidth);
        yScope = juce::jmap(binYData[i], -1.125f, 1.125f, scopeHeight, 0.0f);

        jassert(!isnan(yScope));

        selectedCurveSampleRemapPath.lineTo(xScope, yScope);
    }
}

void WaveShaperScope::setPointAndTension(int binID, bool pointVisible, float pointX, float pointY, bool tensionVisible, float tensionX, float tensionY)
{
    float scopeWidth = (float)getWidth();
    float scopeHeight = (float)getHeight();
    float scopeMinLin = DistortionConstants::UI::scopeMinLin_f;
    float scopeMaxLin = DistortionConstants::UI::scopeMaxLin_f;

    float diameter = 10.0f;
    float halfDiameter = diameter * 0.5f;

    float tensionXRemap = juce::jmap(tensionX, scopeMinLin, scopeMaxLin, 0.0f, scopeWidth) - halfDiameter;
    float tensionYRemap = juce::jmap(tensionY, scopeMinLin, scopeMaxLin, scopeHeight, 0.0f) - halfDiameter;

    float pointXRemap = juce::jmap(pointX, scopeMinLin, scopeMaxLin, 0.0f, scopeWidth) - halfDiameter;
    float pointYRemap = juce::jmap(pointY, scopeMinLin, scopeMaxLin, scopeHeight, 0.0f) - halfDiameter;

    pointDraggerVector[binID]->setBounds(static_cast<int>(pointXRemap),
        static_cast<int>(pointYRemap),
            static_cast<int>(diameter + 1.0f),
                static_cast<int>(diameter + 1.0f));
    pointDraggerVector[binID]->setVisible(pointVisible);

    tensionDraggerVector[binID]->setBounds(static_cast<int>(tensionXRemap),
        static_cast<int>(tensionYRemap),
            static_cast<int>(diameter + 1.0f),
                static_cast<int>(diameter + 1.0f));
    tensionDraggerVector[binID]->setVisible(tensionVisible);
}

void WaveShaperScope::setWaveShaperPointIsSelected(int selectedPointID)
{
    for (int pointID = 0; pointID < pointDraggerVector.size(); ++pointID)
    {
        pointDraggerVector[pointID]->setIsSelected(false);
    }

    if (selectedPointID == -1)
        return;

    pointDraggerVector[selectedPointID]->setIsSelected(true);
}

void WaveShaperScope::setWaveShaperTensionIsSelected(int selectedCurveID)
{
    for (int pointID = 0; pointID < tensionDraggerVector.size(); ++pointID)
    {
        //pointDraggerVector[pointID]->setIsSelected(false);
        tensionDraggerVector[pointID]->setColour(DistortionConstants::UI::Colour::tension);
    }

    pointDraggerVector.getLast()->setIsSelected(false);

    if (selectedCurveID == -1)
        return;

    tensionDraggerVector[selectedCurveID]->setColour(DistortionConstants::UI::Colour::selectedTension);
}

void WaveShaperScope::updateUI(MainLayerDataStruct& mainLayerDataStruct, int distortionUnitID, SampleRemapper<float>* pSM)
{
    int selectedStageID = mainLayerDataStruct.getSelectedStageID();
    //int selectedDistoUnitID = mainLayerDataStruct.getSelectedDistoUnitID();
    int selectedDistoUnitID = distortionUnitID;
    int selectedPointID = mainLayerDataStruct.getSelectedPointID(selectedStageID, selectedDistoUnitID);
    int selectedCurveID = mainLayerDataStruct.getSelectedCurveID(selectedStageID, selectedDistoUnitID);

    int nbOfPoints = mainLayerDataStruct.getNbOfPoints(selectedStageID, selectedDistoUnitID);

    bool pointVisible = true;
    bool tensionVisible;

    unipolarSampleRemapPath.clear();
    bipolarSampleRemapPath.clear();
    equationDistortionPath.clear();
    selectedCurveSampleRemapPath.clear();

    int tensionLen = tensionDraggerVector.size();

    for (int pointID = 0; pointID < tensionLen; ++pointID)
    {
        tensionDraggerVector[pointID]->setVisible(false);
        pointDraggerVector[pointID]->setVisible(false);
    }

    pointDraggerVector.getLast()->setVisible(false);

    setWaveShaperPointIsSelected(selectedPointID);

    bool isBipolar = mainLayerDataStruct.getIsBipolar(selectedStageID, selectedDistoUnitID);

    if (isBipolar)
    {
        bipolarColour = DistortionConstants::UI::Colour::unipolar;
        tensionVisible = true;
        isBipolarPath.clear();

        //if (selectedCurveID == nbOfPoints - 1)
        //{
        //    selectedCurveID -= 1;
            tensionDraggerVector[selectedCurveID]->setColour(DistortionConstants::UI::Colour::selectedTension);
        //}

        //Unipolar Path (red)
        for (int i = 0; i < nbOfPoints - 1; ++i)
        {
            if (i != selectedCurveID)
            {
                pSM->getBin(i)->pullSampleFromFifo(binPathData[i]);
                //drawUnipolarPath(pSM->getBin(i)->getBinPathData().getReadPointer(0), pSM->getBin(i)->getBinPathData().getReadPointer(1));
                drawUnipolarPath(binPathData[i]->getReadPointer(0), binPathData[i]->getReadPointer(1));
            }

            //setPointAndTension(i,
            //    pointVisible,
            //    pSM->getBin(i)->getBinPathData().getReadPointer(0)[0],
            //    pSM->getBin(i)->getBinPathData().getReadPointer(1)[0],
            //    true,
            //    pSM->getBin(i)->getBinPathData().getReadPointer(0)[101],
            //    pSM->getBin(i)->getBinPathData().getReadPointer(1)[101]);

            setPointAndTension(i,
                pointVisible,
                binPathData[i]->getReadPointer(0)[0],
                binPathData[i]->getReadPointer(1)[0],
                true,
                binPathData[i]->getReadPointer(0)[101],
                binPathData[i]->getReadPointer(1)[101]);
        }
    }
    else
    {
        bipolarColour = DistortionConstants::UI::Colour::bipolarOff;
        tensionVisible = false;
        drawIsBipolarRectangle();

        //if (selectedCurveID == nbOfPoints / 2 - 1)
        //{
        //    selectedCurveID -= 1;
            tensionDraggerVector[selectedCurveID]->setColour(DistortionConstants::UI::Colour::selectedTension);
        //}

        for (int i = 0; i < nbOfPoints / 2; ++i)
        {
            if (i != selectedCurveID)
            {
                pSM->getBin(i)->pullSampleFromFifo(binPathData[i]);
                //drawUnipolarPath(pSM->getBin(i)->getBinPathData().getReadPointer(0), pSM->getBin(i)->getBinPathData().getReadPointer(1));
                drawUnipolarPath(binPathData[i]->getReadPointer(0), binPathData[i]->getReadPointer(1));
            }

            //setPointAndTension(i,
            //    pointVisible,
            //    pSM->getBin(i)->getBinPathData().getReadPointer(0)[0],
            //    pSM->getBin(i)->getBinPathData().getReadPointer(1)[0],
            //    true,
            //    pSM->getBin(i)->getBinPathData().getReadPointer(0)[101],
            //    pSM->getBin(i)->getBinPathData().getReadPointer(1)[101]);

            setPointAndTension(i,
                pointVisible,
                binPathData[i]->getReadPointer(0)[0],
                binPathData[i]->getReadPointer(1)[0],
                true,
                binPathData[i]->getReadPointer(0)[101],
                binPathData[i]->getReadPointer(1)[101]);
        }

        tensionDraggerVector[nbOfPoints / 2 - 1]->setVisible(false);

        for (int i = nbOfPoints / 2; i < (nbOfPoints - 1); ++i)
        {
            if (i != selectedCurveID)
            {
                pSM->getBin(i)->pullSampleFromFifo(binPathData[i]);
                //drawUnipolarPath(pSM->getBin(i)->getBinPathData().getReadPointer(0), pSM->getBin(i)->getBinPathData().getReadPointer(1));
                drawUnipolarPath(binPathData[i]->getReadPointer(0), binPathData[i]->getReadPointer(1));
            }

            //setPointAndTension(i,
            //    tensionVisible,
            //    pSM->getBin(i)->getBinPathData().getReadPointer(0)[0],
            //    pSM->getBin(i)->getBinPathData().getReadPointer(1)[0],
            //    tensionVisible,
            //    pSM->getBin(i)->getBinPathData().getReadPointer(0)[101],
            //    pSM->getBin(i)->getBinPathData().getReadPointer(1)[101]);

            setPointAndTension(i,
                tensionVisible,
                binPathData[i]->getReadPointer(0)[0],
                binPathData[i]->getReadPointer(1)[0],
                tensionVisible,
                binPathData[i]->getReadPointer(0)[101],
                binPathData[i]->getReadPointer(1)[101]);
        }
    }

    //drawSelectedCurvePath(pSM->getBin(selectedCurveID)->getBinPathData().getReadPointer(0), pSM->getBin(selectedCurveID)->getBinPathData().getReadPointer(1));
    pSM->getBin(selectedCurveID)->pullSampleFromFifo(binPathData[selectedCurveID]);
    drawSelectedCurvePath(binPathData[selectedCurveID]->getReadPointer(0), binPathData[selectedCurveID]->getReadPointer(1));
    setWaveShaperTensionIsSelected(selectedCurveID);

    for (int i = 0; i < (nbOfPoints - 1); ++i)
    {
        //drawSampleRemapperPath(&equationDistortionPath, pSM->getBin(i)->getBinPathData().getReadPointer(0), pSM->getBin(i)->getBinPathData().getReadPointer(2));
    
        drawSampleRemapperPath(&equationDistortionPath, binPathData[i]->getReadPointer(0), binPathData[i]->getReadPointer(2));
    }

    //setPointAndTension(nbOfPoints - 1,
    //    tensionVisible,
    //    pSM->getBin(nbOfPoints - 2)->getBinPathData().getReadPointer(0)[100],
    //    pSM->getBin(nbOfPoints - 2)->getBinPathData().getReadPointer(1)[100],
    //    false,
    //    pSM->getBin(nbOfPoints - 2)->getBinPathData().getReadPointer(0)[101],
    //    pSM->getBin(nbOfPoints - 2)->getBinPathData().getReadPointer(1)[101]);

    setPointAndTension(nbOfPoints - 1,
        tensionVisible,
        binPathData[nbOfPoints - 2]->getReadPointer(0)[100],
        binPathData[nbOfPoints - 2]->getReadPointer(1)[100],
        false,
        binPathData[nbOfPoints - 2]->getReadPointer(0)[101],
        binPathData[nbOfPoints - 2]->getReadPointer(1)[101]);
}

int WaveShaperScope::findPoint(juce::MouseEvent& e)
{
    juce::Point<int> mousePos = e.getMouseDownPosition();
    int size = pointDraggerVector.size();
    int pointID = 0;
    juce::Rectangle<int> bounds = pointDraggerVector[pointID]->getBounds();
    bool isInside = bounds.contains(mousePos);

    while ((!isInside) && (pointID < size - 1))
    {
        pointID++;
        bounds = pointDraggerVector[pointID]->getBounds();
        isInside = bounds.contains(mousePos);

    }

    if (!isInside)
    {
        pointID = -1;
    }

    return pointID;
}

int WaveShaperScope::findTension(juce::MouseEvent& e)
{
    juce::Point<int> mousePos = e.getPosition();

    int size = tensionDraggerVector.size();
    int tensionID = 0;

    juce::Rectangle<int> bounds = tensionDraggerVector[tensionID]->getBounds();
    bool isInside = bounds.contains(mousePos);

    while ((!isInside) && (tensionID < size - 1))
    {
        tensionID++;
        bounds = tensionDraggerVector[tensionID]->getBounds();
        isInside = bounds.contains(mousePos);
    }

    if (!isInside)
    {
        tensionID = -1;
    }

    return tensionID;
}

bool WaveShaperScope::isMouseOnWSScope(const juce::MouseEvent& e)
{
    return getBounds().contains(e.getPosition());
}