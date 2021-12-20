/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

#include "DataStructure/UnitProcessorAttachmentStructure.h"

#include "PluginProcessor.h"
#include "Processor/UnitProcessor/EQProcessor.h"

#include "UIComponent/GeneralUIComponent/MainMenuTabComponent.h"
#include "UIComponent/GeneralUIComponent/MainMenuComponent.h"
#include "UIComponent/ScopeUIComponent/ScopeComponent.h"
#include "UIComponent/EQUIComponent/EQComponent.h"
#include "UIComponent/DistortionUIComponent/DistortionComponent.h"

//==============================================================================
/**
*/
class MangledAudioProcessorEditor  
    : public juce::AudioProcessorEditor
    , public juce::Button::Listener
    //, public juce::ComboBox::Listener
    , private juce::Timer
{
public:
    enum RadioButtonIds
    {
        LayerButtons = 1001
    };

    MangledAudioProcessorEditor (MangledAudioProcessor&);
    ~MangledAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

    //Mouse Listener
    //void mouseDown(const juce::MouseEvent& e);
    //void mouseDrag(const juce::MouseEvent& e);
    //void mouseUp(const juce::MouseEvent& e);
    //void mouseWheelMove(const juce::MouseEvent& event, const juce::MouseWheelDetails& wheel);

    //Button::Listener
    void buttonClicked(juce::Button* button) override;
    //void buttonStateChanged(juce::Button* button) override;

    //ComboBox::Listener
    //void comboBoxChanged(juce::ComboBox* comboBox) override;

    //Timer
    void timerCallback() override;

    std::atomic<double>& bpm;
    std::atomic<double>& ppq;
    std::atomic<juce::int64>& ppqSample;
    std::atomic<int>& bufferSize;
    void setUI();
private:
    void linkParametersAttachment(juce::AudioProcessorValueTreeState& apvts);
    void linkStageParametersAttachment(juce::AudioProcessorValueTreeState& apvts, int stageID);
    void linkScopeParametersAttachment(juce::AudioProcessorValueTreeState& apvts, StageComponent* pStageComponent, int stageID);
    void linkEQParametersAttachment(juce::AudioProcessorValueTreeState& apvts, StageComponent* pStageComponent, int stageID);
    void linkPhaserParametersAttachment(juce::AudioProcessorValueTreeState& apvts, StageComponent* pStageComponent, int stageID);
    void linkDistortionParametersAttachment(juce::AudioProcessorValueTreeState& apvts, StageComponent* pStageComponent, int stageID);


    MainMenuTabComponent mainMenuTab;

    juce::TextButton undoButton;
    juce::TextButton redoButton;

    juce::Slider masterGainSlider;

    OnOffButton masterLimiterOnOffButton;

    juce::TextButton masterResetButton;

    juce::Label mangledLabel;

    MainMenuComponent mainMenu;

    //Audio Engine Attachment
    AudioEngineAttachment audioEngineAttachment;

    //Main Menu Attachement
    std::unique_ptr<ButtonAttachment> mainLayerOnOffAttachment;

    //Main Layer Stage Attachment
    //std::unique_ptr<ButtonAttachment> mainLayerPostProcessingOnOffAttachment;

    //MainLayer All Stage Attachment
    MainLayerAttachment mainLayerAttachment;

    MangledAudioProcessor& audioProcessor;

    bool shouldDrag = false;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MangledAudioProcessorEditor)
};
