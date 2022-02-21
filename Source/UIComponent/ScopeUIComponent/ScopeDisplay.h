/*
  ==============================================================================

    ScopeComponent.h
    Created: 8 Apr 2021 3:28:30pm
    Author:  CookiMonstor

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
//#include "../../DataStructure/hostInformation.h"
#include "../../DataStructure/MainLayerDataStruct.h"
#include "../../DataStructure/MangledState.h"
#include "../../Functions/Conversion.h"
#include "../../Processor/UnitProcessor/EQ/EQProcessor.h"
#include "../GeneralUIComponent/SelectablePointDragger.h"
#include "../LookAndFeel/LookAndFeel_V4_ComboBox.h"

//==============================================================================
/*
*/
class ScopeDisplay : public juce::Component
                   , public juce::ComboBox::Listener
{
public:
    ScopeDisplay(AudioEngineState<float>& dataStruct);
    ~ScopeDisplay() override;

    void paint (juce::Graphics&) override;
    void resized() override;

    //ComboBox::Listener
    void comboBoxChanged(juce::ComboBox* comboBox) override;

    //Compute the log grid path.
    void computeLogGridPath();
    //Compute the log grid path.
    void computeGainGridPath();
    //Compute FFT Path.
    void computeXAxisValueForFFTPath();
    void computeFFTPath();
    void computeFFTPath(juce::AudioBuffer<float>* fftBuffer, juce::Path* fftPath);
    
    //Compute the filter response from the coefficient at the freqForFilterMagnitude.
    void computeFrequencyAndXAxisValueForFilterMagnitude();
    
    void computeEQFilterFrequencyResponse(int filterID); //, const float* filterMagnitudeValue
    void computeEQFilterSumFrequencyResponse(bool isAnyActiveFilter); //const float* filterSumMagnitudeValue, 
    void closeFilterAreaOfEffectPath(int filterType, int filterID);
    
    //Draw all the filter response.
    void drawFilterResponse(juce::Graphics& g);

    //Draw the log grid and dB Label.
    void drawLogGridScope(juce::Graphics& g);

    struct FilterDrawing
    {
        int filterID = 0;
        juce::Path filterPath = juce::Path();
        float opacity = 0.0f;
        juce::Colour colour = juce::Colours::black;
        int type = 0;
        bool isBypassed = true;
        bool isActive = false;
    };

    void setMonoToStereo(bool newIsStereo) { isStereo = newIsStereo; };

    void setSelectedFilterDragger(int selectedFilter);

    //void setIsDisplayed(bool newIsDisplayed) { isDisplayed = newIsDisplayed; };
    int  findFilterDragger(const juce::MouseEvent& e);
    bool isMouseOnScope(const juce::MouseEvent& e);

    void setGainLim(int zoomID);

    int updateUI();

    //Get function
    juce::AudioBuffer<float>* getPreEQFFTBuffer() { return &preEQFFTBuffer; };
    juce::AudioBuffer<float>* getPostEQFFTBuffer() { return &postEQFFTBuffer; };
    juce::AudioBuffer<float>* getPostDistoFFTBuffer() { return &postDistoFFTBuffer; };
    juce::AudioBuffer<float>& getFilterMagBuffer() { return filterMagBuffer; };

    juce::AudioBuffer<float>& getFilterMagSumBuffer() { return filterSumMagBuffer; };
    juce::ComboBox* getZoomComboBox() { return &zoomComboBox; };

private:
    //Value in pixel for xAxis and Hz for freq.======================
    float xAxisf[4096]; //FFT.
    float xAxisForFilterMagnitude[EQConstants::UI::filterMagnitudeSize]; //Filter Mag.
    float freqForFilterMagnitude[EQConstants::UI::filterMagnitudeSize];  //Filter Mag.
    
    //Fixed Path.======================================================================================================================
    juce::Path logAxisGridPath;      //Outer White Vertical Log Grid
    juce::Path innerLogAxisGridPath; //Inner Grey Vertical Log Grid.
    juce::Path gainAxisGridPath;     //Horizontal Log Grid.

    juce::Path fftPreEQPath;     //FFT Pre EQ Effect.
    juce::Path fftPostEQPath;    //FFT Post EQ Effect.
    juce::Path fftPostDistoPath; //FFT Post EQ Effect.

    juce::Path filterAreaOfEffectPath; //Fill for the area effect Of the filter.

    FilterDrawing filterDrawingBank[EQConstants::Processor<float>::nbOfFilterMax]; //Path for every filter.
    FilterDrawing filterSumDrawing;

    //Flag====================================================================================================================
    //bool isDisplayed = false;     //Bypass the Paint.
    int currentFilterDragger = 0; //Keep current filter Dragger.
    bool isStereo;

    //FFT Buffer===========================================================
    juce::AudioBuffer<float> preEQFFTBuffer;
    juce::AudioBuffer<float> postEQFFTBuffer;
    juce::AudioBuffer<float> postDistoFFTBuffer;

    juce::AudioBuffer<float> filterMagBuffer;
    juce::AudioBuffer<float> filterSumMagBuffer;
    //Filter Interface========================================================================================================================================
    juce::OwnedArray<SelectablePointDragger> filterDraggerVector; //Point to drag the filter.

    juce::ComboBox zoomComboBox;
    juce::OwnedArray<juce::Label> labelVector;
    float gainMin = 0.0f;
    float gainMax = 0.0f;
    float gainLabelInc = 0.0f;

    AudioEngineState<float>& dataStruct;
    LookAndFeel_V4_ComboBox lafComboBox;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ScopeDisplay)
};
