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
#include "UIComponent/Controls/MangledSlider.h"
#include "UIComponent/Controls/MangledDualStateButton.h"
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
        LayerButtons = 1001,
        StateButtons = 1002
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
    void updateUI(int stageID, int distortionUnitID);
    std::atomic<double>& bpm;
    std::atomic<double>& ppq;
    std::atomic<juce::int64>& ppqSample;
    std::atomic<int>& bufferSize;
    void setUI();

    void updateScope(int stageID);
    void updateDistortion(int stageID, int distortionUnitID);
    void updateRMSMeter(int stageID);
private:
    void linkParametersAttachment(juce::AudioProcessorValueTreeState& apvts);
    void linkStageParametersAttachment(juce::AudioProcessorValueTreeState& apvts, int stageID);
    void linkScopeParametersAttachment(juce::AudioProcessorValueTreeState& apvts, StageComponent* pStageComponent, int stageID);
    void linkEQParametersAttachment(juce::AudioProcessorValueTreeState& apvts, StageComponent* pStageComponent, int stageID);
    void linkPhaserParametersAttachment(juce::AudioProcessorValueTreeState& apvts, StageComponent* pStageComponent, int stageID);
    void linkDistortionParametersAttachment(juce::AudioProcessorValueTreeState& apvts, StageComponent* pStageComponent, int stageID);

    void switchToSelectedID();
    void switchUI(int stageID, int processorID, int distortionUnitID);



    MainMenuTabComponent mainMenuTab;

    juce::TextButton undoButton;
    juce::TextButton redoButton;

    juce::OwnedArray<juce::TextButton> stateButton;
    //juce::TextButton stateAButton;
    //juce::TextButton stateBButton;
    juce::Label masterGainLabel;
    //juce::Slider masterGainSlider;
    MangledSlider masterGainSlider;
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
    bool editorLoadingDone = false;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MangledAudioProcessorEditor)
};
