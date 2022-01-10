/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

//#include "PluginProcessor.h"
#include "PluginEditor.h"


//==============================================================================
MangledAudioProcessorEditor::MangledAudioProcessorEditor (MangledAudioProcessor& p)
    : AudioProcessorEditor (&p)
    , audioProcessor (p)
    , undoButton("Undo")
    , redoButton("Redo")
    //, mangledLabel("MANGLED")
    , masterGainLabel("Master Gain")
    , masterLimiterOnOffButton("Limiter", "On", AudioEngineConstants::Processor<float>::masterLimiterIsBypassedStartValue)
    , masterResetButton("Global Reset")
    , mainMenu(p.getMainLayerDataStruct())
    , bpm(p.bpm)
    , ppq(p.ppq)
    , ppqSample(p.ppqSample)
    , bufferSize(p.bufferSize)
{
    //setInterceptsMouseClicks(true, false);


    //Undo Redo
    undoButton.onClick = [this] {audioProcessor.getMainLayerDataStruct().getUndoManager().undo(); };
    redoButton.onClick = [this] {audioProcessor.getMainLayerDataStruct().getUndoManager().redo();
                                 audioProcessor.getMainLayerDataStruct().trigListener(); 
    };
    addAndMakeVisible(undoButton);
    addAndMakeVisible(redoButton);

    //Label
    mangledLabel.setText("MANGLED", juce::dontSendNotification);
    juce::Font boldFont{20, juce::Font::FontStyleFlags::bold };
    boldFont.setStyleFlags(juce::Font::FontStyleFlags::bold);
    mangledLabel.setFont(boldFont);
    mangledLabel.setColour(juce::Label::ColourIds::textColourId, AudioEngineConstants::UI::neonGreen);
    mangledLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(mangledLabel);

    masterGainLabel.setText("Master Gain", juce::dontSendNotification);
    masterGainLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(masterGainLabel);

    for (int i = 0; i < AudioEngineConstants::UI::stateString.size(); ++i)
    {
        //stateButton.add(new juce::TextButton(AudioEngineConstants::UI::stateString[i]));
        stateButton.add(new MangledDualStateButton(AudioEngineConstants::UI::stateString[i]));
        stateButton[i]->setClickingTogglesState(true);
        //stateButton[i]->setColour(juce::TextButton::ColourIds::buttonColourId, juce::Colours::grey);
        //stateButton[i]->setColour(juce::TextButton::ColourIds::buttonOnColourId, juce::Colours::black);
        //stateButton[i]->setRadioGroupId(StateButtons, juce::dontSendNotification);
        stateButton[i]->addListener(this);
        addAndMakeVisible(stateButton[i]);
    }

    stateButton[0]->setToggleState(true, juce::dontSendNotification);

    //Scope==============================================================================================================
    startTimerHz(AudioEngineConstants::UI::refreshRateHz);
    //startTimerHz(60);
    //Main Menu Tab Attachment and Listener

    mainMenuTab.getMainLayerTextButton()->addListener(this);
    //mainMenuTab.getPunchTextButton()->addListener(this);
    //mainMenuTab.getTailTextButton()->addListener(this);
    //mainMenuTab.getMainClipperTextButton()->addListener(this);
    //mainMenuTab.getOptionsTextButton()->addListener(this);

    addAndMakeVisible(mainMenuTab);

    //Menu==============================================================================================================
    addAndMakeVisible(mainMenu);

    linkParametersAttachment(audioProcessor.getMainLayerDataStruct().getAPVTSMainLayer());

    masterGainSlider.setSliderStyle(juce::Slider::SliderStyle::LinearHorizontal);
    masterGainSlider.setTextBoxStyle(juce::Slider::TextBoxRight, true, 50, 20);
    //masterGainSlider.setRange(-100.0f, +12.0f);
    //masterGainSlider.setValue(-12.0f);
    addAndMakeVisible(masterGainSlider);

    addAndMakeVisible(masterLimiterOnOffButton);

    masterResetButton.onClick = [this] {audioProcessor.getMainLayerDataStruct().resetAudioEngineParam(); };
    addAndMakeVisible(masterResetButton);

    //setUI();

    //mainMenu.getMainLayerMenu()->getStageIDButton(1)->setToggleState(false, juce::sendNotificationAsync);
    //mainMenu.getMainLayerMenu()->getStageIDButton(0)->setToggleState(true, juce::sendNotificationAsync);
    //mainMenu.getMainLayerMenu()->switchToStageMenu(0);

    setSize(AudioEngineConstants::UI::uiWidth, AudioEngineConstants::UI::uiHeight);
    setResizable(false, false);
}

MangledAudioProcessorEditor::~MangledAudioProcessorEditor()
{
    //audioProcessor.getMainLayerDataStruct().setSelectedStageID(0);
    stopTimer();
}

//==============================================================================
void MangledAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    //

    //const auto bounds = getLocalBounds().toFloat();
    //juce::Point<float> top = bounds.getTopLeft();

    //Violet -> Orange grad
    //juce::ColourGradient grad = juce::ColourGradient{ juce::Colours::darkorange.withMultipliedAlpha(0.6f),
    //                                                    bounds.getBottomLeft(),
    //                                                    juce::Colours::purple.withMultipliedAlpha(0.6f),
    //                                                    top,
    //                                                    false };
    //grad.addColour(0.5, juce::Colours::orangered.withMultipliedAlpha(0.6f));

    //g.setGradientFill(grad);

    g.setColour(juce::Colours::black);
    g.fillRect(getLocalBounds());
}

void MangledAudioProcessorEditor::resized()
{
    int rotaryPixelSize = 75;
    int labelWidth = rotaryPixelSize;
    int labelHeight = 20;
    int offset = 10;
    //auto buttonOnOffWidth = labelHeight + 10;

    //Menu Button===============================================================================================================================
    mainMenuTab.setBounds(0, 0, labelWidth + 30, labelHeight);
    mainMenu.setBounds(mainMenuTab.getX(), mainMenuTab.getBottom(), getLocalBounds().getWidth(), getLocalBounds().getHeight() - labelHeight);

    undoButton.setBounds(mainMenuTab.getRight(), 0, labelWidth - 20, labelHeight);
    redoButton.setBounds(undoButton.getRight(), 0, labelWidth - 20, labelHeight);

    //Test Label================================================================================================================================
    mangledLabel.setBounds(redoButton.getRight() + offset, mainMenuTab.getY(), labelWidth, labelHeight);
    stateButton[0]->setBounds(mangledLabel.getRight() + offset, mangledLabel.getY(), labelHeight, labelHeight);
    stateButton[1]->setBounds(stateButton[0]->getRight() + 1, mangledLabel.getY(), labelHeight, labelHeight);
    stateButton[2]->setBounds(stateButton[1]->getRight() + 1, mangledLabel.getY(), labelHeight, labelHeight);
    stateButton[3]->setBounds(stateButton[2]->getRight() + 1, mangledLabel.getY(), labelHeight, labelHeight);
    
    masterGainLabel.setBounds(stateButton.getLast()->getRight() + offset, mangledLabel.getY(), labelWidth, labelHeight);
    auto length = (getWidth() - masterGainLabel.getRight() - 2*labelWidth - 3);
    masterGainSlider.setBounds(masterGainLabel.getRight(), 0, length, labelHeight);
    masterLimiterOnOffButton.setBounds(masterGainSlider.getRight(), mainMenuTab.getY(), labelWidth, labelHeight);
    masterResetButton.setBounds(masterLimiterOnOffButton.getRight(), 0, labelWidth, labelHeight);

}

void MangledAudioProcessorEditor::buttonClicked(juce::Button* button)
{
    //Main Menu Button=========================================================================================
    //Display the selected Layer Menu--------------------------------------------------------------------------
    if (button == mainMenuTab.getMainLayerTextButton()) //Display Main Layer Menu
    {
        //audioProcessor.getAudioEngine()->setCurrentLayerProcessorID(AudioEngine::LayerProcessorID::mainLayer);
        mainMenu.switchToMainLayerMenu();
    }
    else if (button == stateButton[AudioEngineConstants::Processor<float>::A])
    {
        if (button->getToggleState())
        {
            int selectedState = audioProcessor.getMainLayerDataStruct().getSelectedState();
            audioProcessor.saveState(selectedState);
            audioProcessor.loadState(AudioEngineConstants::Processor<float>::A);
            audioProcessor.getMainLayerDataStruct().setSelectedState(AudioEngineConstants::Processor<float>::A);
        }
        else
        {
            stateButton[0]->setToggleState(true, juce::dontSendNotification);
        }

        stateButton[AudioEngineConstants::Processor<float>::B]->setToggleState(false, juce::dontSendNotification);
        stateButton[AudioEngineConstants::Processor<float>::C]->setToggleState(false, juce::dontSendNotification);
        stateButton[AudioEngineConstants::Processor<float>::D]->setToggleState(false, juce::dontSendNotification);
    }
    else if (button == stateButton[AudioEngineConstants::Processor<float>::B])
    {
        if (button->getToggleState())
        {
            int selectedState = audioProcessor.getMainLayerDataStruct().getSelectedState();
            audioProcessor.saveState(selectedState);
            audioProcessor.loadState(AudioEngineConstants::Processor<float>::B);
            audioProcessor.getMainLayerDataStruct().setSelectedState(AudioEngineConstants::Processor<float>::B);
        }
        else
        {
            stateButton[1]->setToggleState(true, juce::dontSendNotification);
        }
        stateButton[AudioEngineConstants::Processor<float>::A]->setToggleState(false, juce::dontSendNotification);
        stateButton[AudioEngineConstants::Processor<float>::C]->setToggleState(false, juce::dontSendNotification);
        stateButton[AudioEngineConstants::Processor<float>::D]->setToggleState(false, juce::dontSendNotification);
    }
    else if (button == stateButton[AudioEngineConstants::Processor<float>::C])
    {
        if (button->getToggleState())
        {
            int selectedState = audioProcessor.getMainLayerDataStruct().getSelectedState();
            audioProcessor.saveState(selectedState);
            audioProcessor.loadState(AudioEngineConstants::Processor<float>::C);
            audioProcessor.getMainLayerDataStruct().setSelectedState(AudioEngineConstants::Processor<float>::C);
        }
        else
        {
            stateButton[AudioEngineConstants::Processor<float>::C]->setToggleState(true, juce::dontSendNotification);
        }
        stateButton[AudioEngineConstants::Processor<float>::A]->setToggleState(false, juce::dontSendNotification);
        stateButton[AudioEngineConstants::Processor<float>::B]->setToggleState(false, juce::dontSendNotification);
        stateButton[AudioEngineConstants::Processor<float>::D]->setToggleState(false, juce::dontSendNotification);
    }
    else if (button == stateButton[AudioEngineConstants::Processor<float>::D])
    {
        if (button->getToggleState())
        {
            int selectedState = audioProcessor.getMainLayerDataStruct().getSelectedState();
            audioProcessor.saveState(selectedState);
            audioProcessor.loadState(AudioEngineConstants::Processor<float>::D);
            audioProcessor.getMainLayerDataStruct().setSelectedState(AudioEngineConstants::Processor<float>::D);
        }
        else
        {
            stateButton[AudioEngineConstants::Processor<float>::D]->setToggleState(true, juce::dontSendNotification);
        }
        stateButton[AudioEngineConstants::Processor<float>::A]->setToggleState(false, juce::dontSendNotification);
        stateButton[AudioEngineConstants::Processor<float>::B]->setToggleState(false, juce::dontSendNotification);
        stateButton[AudioEngineConstants::Processor<float>::C]->setToggleState(false, juce::dontSendNotification);
    }
    //else if (button == mainMenuTab.getPunchTextButton()) //Display Punch Layer Menu
    //{
    //    audioProcessor.getAudioEngine()->setCurrentLayerProcessorID(AudioEngine::LayerProcessorID::punch);
    //    mainMenu.switchToPunchMenu();
    //    button->setState(juce::Button::ButtonState::buttonDown);
    //}
    //else if (button == mainMenuTab.getTailTextButton()) //Display Tail Layer Menu
    //{
    //    audioProcessor.getAudioEngine()->setCurrentLayerProcessorID(AudioEngine::LayerProcessorID::tail);
    //    mainMenu.switchToTailMenu();
    //}
    //else if (button == mainMenuTab.getMainClipperTextButton()) //Display Main Clipper Menu
    //{
    //    audioProcessor.getAudioEngine()->setCurrentLayerProcessorID(AudioEngine::LayerProcessorID::mainClipper);
    //    mainMenu.switchToMainClipperMenu();
    //}
}

//void MangledAudioProcessorEditor::comboBoxChanged(juce::ComboBox* comboBox)
//{
//    int stageID = audioProcessor.getMainLayerDataStruct().getSelectedStageID();
//    int distortionUnitID = audioProcessor.getMainLayerDataStruct().getSelectedDistoUnitID();
//
//    StageComponent* pStage = mainMenu.getMainLayerMenu()->getStageComponent(stageID);
//    ScopeDisplay* pScopeDisplay = pStage->getScope()->getScopeDisplay();
//    DistortionSliderComponent* pDistoSlider = pStage->getDistortionComponent()->getDistortionSlider(distortionUnitID);
//}

void MangledAudioProcessorEditor::linkParametersAttachment(juce::AudioProcessorValueTreeState& apvts)
{
    juce::String paramString = AudioEngineConstants::ParamStringID::GetParamStringID::masterLimiterOnOff();

    audioEngineAttachment.masterLimiterOnOff = std::make_unique<ButtonAttachment>(apvts,
        paramString,
        masterLimiterOnOffButton);

    paramString = AudioEngineConstants::ParamStringID::GetParamStringID::masterGain();

    audioEngineAttachment.masterGain = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts,
                                                                            paramString,
                                                                            masterGainSlider);

    paramString = MainLayerConstants::ParamStringID::GetParamStringID::isBypassed();
    mainLayerOnOffAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(audioProcessor.getMainLayerDataStruct().getAPVTSMainLayer(),
                                                                                                        paramString, 
                                                                                                        *mainMenuTab.getMainLayerOnOffButton());

    //paramString = getPostProcessingOnOffParamString();
    //mainLayerPostProcessingOnOffAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(audioProcessor.getMainLayerDataStruct().getAPVTSMainLayer(),
    //                                                                                                                paramString, 
    //                                                                                                                *mainMenu.getMainLayerMenu()->getStageMenuTab()->getPostProcessingOnOffButton());

    for (int stageID = 0; stageID < MainLayerConstants::Processor<float>::nbOfStageMax; ++stageID)
    {

        
        linkStageParametersAttachment(apvts, stageID);
    }
}

void MangledAudioProcessorEditor::linkStageParametersAttachment(juce::AudioProcessorValueTreeState& apvts, int stageID)
{
    StageComponent* pStageComponent = mainMenu.getMainLayerMenu()->getStageComponent(stageID);

    juce::String paramString = StageConstants::ParamStringID::GetParamStringID::isBypassed(stageID);
    mainLayerAttachment.stage[stageID].isBypassed = std::make_unique<ButtonAttachment>(apvts, paramString, *mainMenu.getMainLayerMenu()->getStageOnOffButton(stageID));

    paramString = ScopeConstants::ParamStringID::GetParamStringID::dataType(stageID);
    mainLayerAttachment.stage[stageID].scope.mode = std::make_unique<ComboBoxAttachment>(apvts,
        paramString,
        *pStageComponent->getDataToDisplayComboBox());

    paramString = StageConstants::ParamStringID::GetParamStringID::inputGain(stageID);
    mainLayerAttachment.stage[stageID].inputGain = std::make_unique<SliderAttachment>(apvts,
        paramString,
        *pStageComponent->getInputRMSMeter()->getGainSlider());

    paramString = StageConstants::ParamStringID::GetParamStringID::outputGain(stageID);
    mainLayerAttachment.stage[stageID].outputGain = std::make_unique<SliderAttachment>(apvts,
        paramString,
        *pStageComponent->getOutputRMSMeter()->getGainSlider());

    linkScopeParametersAttachment(apvts, pStageComponent, stageID);
    linkEQParametersAttachment(apvts, pStageComponent, stageID);
    linkPhaserParametersAttachment(apvts, pStageComponent, stageID);
    linkDistortionParametersAttachment(apvts, pStageComponent, stageID);
}

void MangledAudioProcessorEditor::linkScopeParametersAttachment(juce::AudioProcessorValueTreeState& apvts, StageComponent* pStageComponent, int stageID)
{
    //juce::String rootString = MainLayerApvtsIDString::mainLayer + MainLayerApvtsIDString::stage;
    //juce::String stageIDString;
    //juce::String processorString;
    juce::String paramString;

    //stageIDString = rootString + ((juce::String)stageID);
    //Scope==============================================================================================================
    paramString = ScopeConstants::ParamStringID::GetParamStringID::preEQIsBypassed(stageID);
    mainLayerAttachment.stage[stageID].scope.preEQIsBypassed = std::make_unique<ButtonAttachment>(apvts,
        paramString,
        *pStageComponent->getScope()->getPreEQOnOffButton());

    paramString = ScopeConstants::ParamStringID::GetParamStringID::postEQIsBypassed(stageID);
    mainLayerAttachment.stage[stageID].scope.postEQIsBypassed = std::make_unique<ButtonAttachment>(apvts,
        paramString,
        *pStageComponent->getScope()->getPostEQOnOffButton());

    paramString = ScopeConstants::ParamStringID::GetParamStringID::postDistoIsBypassed(stageID);
    mainLayerAttachment.stage[stageID].scope.postDistoIsBypassed = std::make_unique<ButtonAttachment>(apvts,
        paramString,
        *pStageComponent->getScope()->getPostDistoOnOffButton());

    paramString = RMSConstants::ParamStringID::GetParamStringID::isNormalized(stageID);
    mainLayerAttachment.stage[stageID].scope.isNormalized = std::make_unique<ButtonAttachment>(apvts,
        paramString,
        *pStageComponent->getScope()->getIsNormalizedOnOffButton());

    paramString = RMSConstants::ParamStringID::GetParamStringID::monoViewIsBypassed(stageID);
    mainLayerAttachment.stage[stageID].scope.monoViewIsBypassed = std::make_unique<ButtonAttachment>(apvts,
        paramString,
        *pStageComponent->getScope()->getMonoViewOnOffButton());

    paramString = RMSConstants::ParamStringID::GetParamStringID::subViewIsBypassed(stageID);
    mainLayerAttachment.stage[stageID].scope.subViewIsBypassed = std::make_unique<ButtonAttachment>(apvts,
        paramString,
        *pStageComponent->getScope()->getSubViewOnOffButton());


    paramString = RMSConstants::ParamStringID::GetParamStringID::subViewCutoff(stageID);
    mainLayerAttachment.stage[stageID].scope.subViewCutoff = std::make_unique<SliderAttachment>(apvts,
        paramString,
        *pStageComponent->getScope()->getSubViewSlider());
}

void MangledAudioProcessorEditor::linkEQParametersAttachment(juce::AudioProcessorValueTreeState& apvts, StageComponent* pStageComponent, int stageID)
{
    //EQ=================================================================================================================
    EQComponent* pEQComponent = pStageComponent->getEQComponent();

    //Mix EQ
     juce::String paramString = EQConstants::ParamStringID::GetParamStringID::mix(stageID);
    mainLayerAttachment.stage[stageID].eq.mix = std::make_unique<SliderAttachment>(apvts,
        paramString,
        *pEQComponent->getMixEQSlider());

    //Bypass EQ
    paramString = EQConstants::ParamStringID::GetParamStringID::isBypassed(stageID);
    mainLayerAttachment.stage[stageID].eq.isBypassed = std::make_unique<ButtonAttachment>(apvts,
        paramString,
        *pEQComponent->getOnOffButton());

    for (int filterID = 0; filterID < EQConstants::Processor<float>::nbOfFilterMax; ++filterID)
    {
        EQSliderComponent* pEQSliderComponent = pEQComponent->getEQSliderComponent(filterID);

        //Filter CutOff
        paramString = EQConstants::ParamStringID::GetParamStringID::filterCutoff(stageID, filterID);
        mainLayerAttachment.stage[stageID].eq.filter[filterID].cutoff = std::make_unique<SliderAttachment>(apvts,
            paramString,
            *pEQSliderComponent->getFilterCutoffEQSlider());
        //Filter Q
        paramString = EQConstants::ParamStringID::GetParamStringID::filterQ(stageID, filterID);
        mainLayerAttachment.stage[stageID].eq.filter[filterID].q = std::make_unique<SliderAttachment>(apvts,
            paramString,
            *pEQSliderComponent->getFilterQEQSlider());
        //Filter Gain
        paramString = EQConstants::ParamStringID::GetParamStringID::filterGain(stageID, filterID);
        mainLayerAttachment.stage[stageID].eq.filter[filterID].gain = std::make_unique<SliderAttachment>(apvts,
            paramString,
            *pEQSliderComponent->getFilterGainEQSlider());
        //Filter Type
        paramString = EQConstants::ParamStringID::GetParamStringID::filterType(stageID, filterID);
        mainLayerAttachment.stage[stageID].eq.filter[filterID].type = std::make_unique<ComboBoxAttachment>(apvts,
            paramString,
            *pEQSliderComponent->getFilterTypeEQComboBox());
        //Filter Order
        paramString = EQConstants::ParamStringID::GetParamStringID::filterOrder(stageID, filterID);
        mainLayerAttachment.stage[stageID].eq.filter[filterID].order = std::make_unique<ComboBoxAttachment>(apvts,
            paramString,
            *pEQSliderComponent->getFilterOrderEQComboBox());
        //On Off Filter
        paramString = EQConstants::ParamStringID::GetParamStringID::filterIsBypassed(stageID, filterID);
        mainLayerAttachment.stage[stageID].eq.filter[filterID].onOff = std::make_unique<ButtonAttachment>(apvts,
            paramString,
            *pEQSliderComponent->getFilterOnOffEQTextButton());
        //Is Active Filter
        paramString = EQConstants::ParamStringID::GetParamStringID::filterIsActive(stageID, filterID);
        mainLayerAttachment.stage[stageID].eq.filter[filterID].isActive = std::make_unique<ButtonAttachment>(apvts,
            paramString,
            *pEQSliderComponent->getFilterIsActiveEQTextButton());

    }
}

void MangledAudioProcessorEditor::linkPhaserParametersAttachment(juce::AudioProcessorValueTreeState& apvts, StageComponent* pStageComponent, int stageID)
{
    //Phaser=====================================================================================================================
    PhaserComponent* pPhaserComponent = pStageComponent->getPhaserComponent();

    //Centre Freq Phaser
    juce::String paramString = PhaserConstants::ParamStringID::GetParamStringID::centerFrequency(stageID);
    mainLayerAttachment.stage[stageID].phaser.centreFrequency = std::make_unique<SliderAttachment>(apvts,
        paramString,
        *pPhaserComponent->getCentreFrequencyPhaserSlider());

    //Feedback Phaser
    paramString = PhaserConstants::ParamStringID::GetParamStringID::feedback(stageID);
    mainLayerAttachment.stage[stageID].phaser.feedback = std::make_unique<SliderAttachment>(apvts,
        paramString,
        *pPhaserComponent->getFeedbackPhaserSlider());

    //Rate Phaser
    paramString = PhaserConstants::ParamStringID::GetParamStringID::rate(stageID);
    mainLayerAttachment.stage[stageID].phaser.rate = std::make_unique<SliderAttachment>(apvts,
        paramString,
        *pPhaserComponent->getRatePhaserSlider());

    //Depth Phaser
    paramString = PhaserConstants::ParamStringID::GetParamStringID::depth(stageID);
    mainLayerAttachment.stage[stageID].phaser.depth = std::make_unique<SliderAttachment>(apvts,
        paramString,
        *pPhaserComponent->getDepthPhaserSlider());

    //Number Of Stages Phaser
    paramString = PhaserConstants::ParamStringID::GetParamStringID::nbOfStages(stageID);
    mainLayerAttachment.stage[stageID].phaser.numStages = std::make_unique<ComboBoxAttachment>(apvts,
        paramString,
        *pPhaserComponent->getNumStagesPhaserComboBox());

    //Mix Phaser
    paramString = PhaserConstants::ParamStringID::GetParamStringID::mix(stageID);
    mainLayerAttachment.stage[stageID].phaser.mix = std::make_unique<SliderAttachment>(apvts,
        paramString,
        *pPhaserComponent->getMixPhaserSlider());

    //Bypass Phaser
    paramString = PhaserConstants::ParamStringID::GetParamStringID::isBypassed(stageID);
    mainLayerAttachment.stage[stageID].phaser.isBypassed = std::make_unique<ButtonAttachment>(apvts,
        paramString,
        *pPhaserComponent->getPhaserOnOffButton());
}

void MangledAudioProcessorEditor::linkDistortionParametersAttachment(juce::AudioProcessorValueTreeState& apvts, StageComponent* pStageComponent, int stageID)
{
    //Distortion=====================================================================================================================
    DistortionComponent* pDistortionComponent = pStageComponent->getDistortionComponent();

    //OverSampling Distortion
    juce::String paramString = DistortionConstants::ParamStringID::GetParamStringID::overSampling(stageID);
    mainLayerAttachment.stage[stageID].distortion.oversampling = std::make_unique<ButtonAttachment>(apvts,
        paramString,
        *pDistortionComponent->getDistortionUnitMenuTab()->getOverSamplerOnOffButton());

    //Mix Distortion
    paramString = DistortionConstants::ParamStringID::GetParamStringID::mix(stageID);
    mainLayerAttachment.stage[stageID].distortion.mix = std::make_unique<SliderAttachment>(apvts,
        paramString,
        *pDistortionComponent->getDistortionUnitMenuTab()->getMixDistortionSlider());

    //Bypass Distortion
    paramString = DistortionConstants::ParamStringID::GetParamStringID::isBypassed(stageID);
    mainLayerAttachment.stage[stageID].distortion.onOff = std::make_unique<ButtonAttachment>(apvts,
        paramString,
        *pDistortionComponent->getDistortionUnitMenuTab()->getOnOffButton());

    for (int distortionUnitID = 0; distortionUnitID < DistortionConstants::Processor<float>::nbOfDUMax; ++distortionUnitID)
    {
        DistortionSliderComponent* pDistoUnitComponent = pDistortionComponent->getDistortionSlider(distortionUnitID);

        paramString = DistoUnitConstants::ParamStringID::GetParamStringID::isBypassed(stageID, distortionUnitID);
        mainLayerAttachment.stage[stageID].distortion.distortionUnit[distortionUnitID].onOff = std::make_unique<ButtonAttachment>(apvts,
            paramString,
            *pDistortionComponent->getDistortionUnitMenuTab()->getDistoUnitOnOffButton(distortionUnitID));

        //Equation Type
        paramString = DistoUnitConstants::ParamStringID::GetParamStringID::equationType(stageID, distortionUnitID);
        mainLayerAttachment.stage[stageID].distortion.distortionUnit[distortionUnitID].eqaType = std::make_unique<ComboBoxAttachment>(apvts,
            paramString,
            *pDistoUnitComponent->getEquationTypeComboBox());

        //Sigmoid Equation
        paramString = DistoUnitConstants::ParamStringID::GetParamStringID::sigmoidEQA(stageID, distortionUnitID);
        mainLayerAttachment.stage[stageID].distortion.distortionUnit[distortionUnitID].sigmoidEQA = std::make_unique<ComboBoxAttachment>(apvts,
            paramString,
            *pDistoUnitComponent->getSigmoidEquationComboBox());

        //Symetric Equation
        paramString = DistoUnitConstants::ParamStringID::GetParamStringID::symetricEQA(stageID, distortionUnitID);
        mainLayerAttachment.stage[stageID].distortion.distortionUnit[distortionUnitID].symetricEQA = std::make_unique<ComboBoxAttachment>(apvts,
            paramString,
            *pDistoUnitComponent->getSymetricEquationComboBox());

        //Asymetric Equation
        paramString = DistoUnitConstants::ParamStringID::GetParamStringID::asymetricEQA(stageID, distortionUnitID);
        mainLayerAttachment.stage[stageID].distortion.distortionUnit[distortionUnitID].asymetricEQA = std::make_unique<ComboBoxAttachment>(apvts,
            paramString,
            *pDistoUnitComponent->getAsymetricEquationComboBox());

        //Special Equation
        paramString = DistoUnitConstants::ParamStringID::GetParamStringID::specialEQA(stageID, distortionUnitID);
        mainLayerAttachment.stage[stageID].distortion.distortionUnit[distortionUnitID].specialEQA = std::make_unique<ComboBoxAttachment>(apvts,
            paramString,
            *pDistoUnitComponent->getSpecialEquationComboBox());

        //Distortion Routing
        paramString = DistoUnitConstants::ParamStringID::GetParamStringID::routing(stageID, distortionUnitID);
        mainLayerAttachment.stage[stageID].distortion.distortionUnit[distortionUnitID].distortionProcessorFirst = std::make_unique<ButtonAttachment>(apvts,
            paramString,
            *pDistoUnitComponent->getDistoProcFirstButton());

        //paramString = "Symetric" + (juce::String)(stageID)+ (juce::String)(distortionUnitID);
        //mainLayerAttachment.stage[stageID].distortion.distortionUnit[distortionUnitID].symetricEQA = std::make_unique<ComboBoxAttachment>(apvts,
        //    paramString,
        //    *pDistoUnitComponent->getSymetricEquationComboBox());

        //DC Filter
        paramString = DistoUnitConstants::ParamStringID::GetParamStringID::dcFilterIsBypassed(stageID, distortionUnitID);
        mainLayerAttachment.stage[stageID].distortion.distortionUnit[distortionUnitID].dcFilterOnOff = std::make_unique<ButtonAttachment>(apvts,
            paramString,
            *pDistoUnitComponent->getDCFilterOnOffButton());

        //PreGain WaveShaper
        paramString = DistoUnitConstants::ParamStringID::GetParamStringID::preGain(stageID, distortionUnitID);
        mainLayerAttachment.stage[stageID].distortion.distortionUnit[distortionUnitID].preGain = std::make_unique<SliderAttachment>(apvts,
            paramString,
            *pDistoUnitComponent->getPreGainWaveShaperSlider());

        //Drive WaveShaper
        paramString = DistoUnitConstants::ParamStringID::GetParamStringID::drive(stageID, distortionUnitID);
        mainLayerAttachment.stage[stageID].distortion.distortionUnit[distortionUnitID].drive = std::make_unique<SliderAttachment>(apvts,
            paramString,
            *pDistoUnitComponent->getDriveWaveShaperSlider());

        //Warp
        paramString = DistoUnitConstants::ParamStringID::GetParamStringID::warp(stageID, distortionUnitID);
        mainLayerAttachment.stage[stageID].distortion.distortionUnit[distortionUnitID].warp = std::make_unique<SliderAttachment>(apvts,
            paramString,
            *pDistoUnitComponent->getWarpWaveShaperSlider());

        //PostGain WaveShaper
        paramString = DistoUnitConstants::ParamStringID::GetParamStringID::postGain(stageID, distortionUnitID);
        mainLayerAttachment.stage[stageID].distortion.distortionUnit[distortionUnitID].postGain = std::make_unique<SliderAttachment>(apvts,
            paramString,
            *pDistoUnitComponent->getPostGainWaveShaperSlider());

        //Mix WaveShaper
        paramString = DistoUnitConstants::ParamStringID::GetParamStringID::mix(stageID, distortionUnitID);
        mainLayerAttachment.stage[stageID].distortion.distortionUnit[distortionUnitID].mix = std::make_unique<SliderAttachment>(apvts,
            paramString,
            *pDistoUnitComponent->getMixWaveShaperSlider());
    }
}

void MangledAudioProcessorEditor::timerCallback()
{
    int stageID = audioProcessor.getMainLayerDataStruct().getSelectedStageID();
    int distortionUnitID = audioProcessor.getMainLayerDataStruct().getSelectedDistoUnitID();

    updateUI(stageID, distortionUnitID);
}

void MangledAudioProcessorEditor::updateUI(int stageID, int distortionUnitID)
{

    //setUI();
    //int stageID = 0;
    MainLayerDataStruct& mainLayerDataStruct = audioProcessor.getMainLayerDataStruct();
    //mainMenu.getMainLayerMenu()->switchToStageMenu(stageID);

    StageProcessorBase<float>* pStageProcessor = audioProcessor.getAudioEngine()->getMainLayerProcessor()->getStageProcessor(stageID);

    StageComponent* pStageComponent = mainMenu.getMainLayerMenu()->getStageComponent(stageID);
    ScopeDisplay* pScopeDisplay = pStageComponent->getScope()->getScopeDisplay();

    ScopeProcessor<float>* scopeProcessorPreEQ = pStageProcessor->getPreEQScopeProcessor();
    ScopeProcessor<float>* scopeProcessorPostEQ = pStageProcessor->getPostEQScopeProcessor();
    ScopeProcessor<float>* scopeProcessorPostDisto = pStageProcessor->getPostDistoScopeProcessor();

    juce::String paramString = ScopeConstants::ParamStringID::GetParamStringID::dataType(stageID);
    int dataType = (int)*mainLayerDataStruct.getAPVTSMainLayer().getRawParameterValue(paramString);

    if (dataType == 0)
    {

        scopeProcessorPreEQ->readBufferFromResult(*(pScopeDisplay->getPreEQFFTBuffer())); //Return a Value but fuck it
        scopeProcessorPostEQ->readBufferFromResult(*(pScopeDisplay->getPostEQFFTBuffer())); //Return a Value but fuck it
        scopeProcessorPostDisto->readBufferFromResult(*(pScopeDisplay->getPostDistoFFTBuffer()));

        //pScopeDisplay->computeFFTPath();

        //Draw Filter Response.============================================================================================================================================================================
        int currentFilterID = audioProcessor.getMainLayerDataStruct().getSelectedFilterID(stageID);

        //Param from processor.
        EQProcessor<float>* pEQProcessor = pStageProcessor->getEQProcessor();

        juce::AudioBuffer<float>& filterMag = pScopeDisplay->getFilterMagBuffer();
        //juce::AudioBuffer<float>* filterSumMag = pScopeDisplay->getFilterMagSumBuffer();
        juce::AudioBuffer<float>& filterSumMag = pScopeDisplay->getFilterMagSumBuffer();
        pEQProcessor->pullFilterMagFifo(filterMag);
        pEQProcessor->pullFilterSumMagFifo(filterSumMag);
        int nbOfActiveFilter = pScopeDisplay->updateUI(mainLayerDataStruct);

        //pStageComponent->getEQComponent()->setNbOfActiveFilter(nbOfActiveFilter);

        if (nbOfActiveFilter == 0)
        {
            pStageComponent->getEQComponent()->switchToEQSliderMenu(currentFilterID);
        }

        pScopeDisplay->repaint();
    }
    else
    {
        TemporalScope* pTempScope = pStageComponent->getScope()->getTemporalScope();

        paramString = RMSConstants::ParamStringID::GetParamStringID::isNormalized(stageID);
        auto& isNormalized = *mainLayerDataStruct.getAPVTSMainLayer().getRawParameterValue(paramString);

        pTempScope->setIsNormalized(!isNormalized);
        juce::int64 previousPPQ = pTempScope->getLeftScope()->ppqSample;
        pTempScope->setBPM(bpm.load());
        pTempScope->setPPQ(ppq.load());
        pTempScope->setPPQSample(ppqSample.load());
        pTempScope->setBufferSize(bufferSize.load());

        int nbSampleToPull = static_cast<int>((60.0 / bpm.load() * audioProcessor.getSampleRate()));
        pTempScope->setNbOfSamplesPerBeat(nbSampleToPull);
        juce::AudioBuffer<float>* pInputBuffer = pTempScope->getInputTemporalBuffer();
        juce::AudioBuffer<float>* pOutputBuffer = pTempScope->getOutputTemporalBuffer();

        pStageProcessor->getInputRMSProcessor()->pushRMSFifo(pInputBuffer, nbSampleToPull);
        pStageProcessor->getOutputRMSProcessor()->pushRMSFifo(pOutputBuffer, nbSampleToPull);

        paramString = RMSConstants::ParamStringID::GetParamStringID::monoViewIsBypassed(stageID);
        auto& monoView = *mainLayerDataStruct.getAPVTSMainLayer().getRawParameterValue(paramString);

        juce::int64 ppqSampleMod = (ppqSample.load() + (juce::int64)bufferSize) % (juce::int64)nbSampleToPull - 1;
        if (ppqSampleMod < 0)
            return;

        //float pos = juce::jmap((float)ppqSampleMod, 0.0f, (float)(nbSampleToPull - 1), 0.0f, (float) pTempScope->getLeftScope()->getWidth());
        ppqSampleMod = (previousPPQ + (juce::int64)bufferSize) % (juce::int64)nbSampleToPull - 1;
        if (ppqSampleMod < 0)
            return;
        //float pos2 = juce::jmap((float)ppqSampleMod, 0.0f, (float)(nbSampleToPull - 1), 0.0f, (float)pTempScope->getLeftScope()->getWidth());
        //pTempScope->setPPQ(ppq.load());
        //pTempScope->setPreviousPPQSample(previousPPQ);
        //pTempScope->setPPQSample(ppqSample.load());

        if (monoView)
        {
            pTempScope->computeStereoPath();
            //pTempScope->getLeftScope()->repaint((int) pos2, 0,(int) (pos2 + pos), pTempScope->getLeftScope()->getHeight());
            pTempScope->getLeftScope()->repaint();
            pTempScope->getRightScope()->repaint();
            //pTempScope->setPreviousPPQSample(ppqSample);
        }
        else
        {
            pTempScope->computeMonoPath();
            pTempScope->getMonoScope()->repaint();
        }


    }
    //Draw WaveShaperEquation.============================================================================================================================================================================
    //int distortionUnitID = audioProcessor.getMainLayerDataStruct().getSelectedDistoUnitID();
    SampleRemapper<float>* pSM = pStageProcessor->getDistortionProcessor()->getDistortionUnitProcessor(distortionUnitID)->getSampleRemapper();
    WaveShaperScope* pWaveShaperScope = pStageComponent->getDistortionComponent()->getDistortionSlider(distortionUnitID)->getScope();
    pWaveShaperScope->updateUI(mainLayerDataStruct, distortionUnitID, pSM);

    int pointID = audioProcessor.getMainLayerDataStruct().getSelectedCurveID(stageID, distortionUnitID);
    bool deleteEnable = audioProcessor.getMainLayerDataStruct().getHorizontalDragOn(stageID, distortionUnitID, pointID);

    pStageComponent->getDistortionComponent()->getDistortionSlider(distortionUnitID)->getDeleteWSPointButton()->setEnabled(deleteEnable);

    pWaveShaperScope->repaint();

    //RMS==================================================

    //srLabel.setText("SR: " + (juce::String)sr, juce::dontSendNotification);
    //pScopeDisplay->ppq = ppq;
    //juce::AudioBuffer<float>* pRMSBuffer = pScopeDisplay->getTemporalBuffer();
    //int nbSampleToPull = 1470;
    //pStageProcessor->getInputRMSProcessor()->pushRMSFifo(pRMSBuffer);
    //pStageProcessor->getOutputRMSProcessor()->pushRMSFifo(pRMSBuffer);
    //float rms = juce::Decibels::gainToDecibels(pRMSBuffer->getRMSLevel(0, 0, nbSampleToPull));
    //pStageComponent->getInputRMSMeter()->setLeftRMSValue(rms);
    //rms = juce::Decibels::gainToDecibels(pRMSBuffer->getRMSLevel(1, 0, nbSampleToPull));
    //pStageComponent->getInputRMSMeter()->setRightRMSValue(rms);

    //pStageComponent->getOutputRMSMeter()->setLeftRMSValue(rms);
    //pStageComponent->getOutputRMSMeter()->setRightRMSValue(rms);
    RMSProcessor<float>* pInputProc = pStageProcessor->getInputRMSProcessor();
    RMSProcessor<float>* pOutputProc = pStageProcessor->getOutputRMSProcessor();

    pStageComponent->getInputRMSMeter()->setLeftRMSValue(pInputProc->getRmsLevel(0));
    pStageComponent->getInputRMSMeter()->setRightRMSValue(pInputProc->getRmsLevel(1));
    pStageComponent->getOutputRMSMeter()->setLeftRMSValue(pOutputProc->getRmsLevel(0));
    pStageComponent->getOutputRMSMeter()->setRightRMSValue(pOutputProc->getRmsLevel(1));

    pStageComponent->getInputRMSMeter()->setLeftPeakValue(pInputProc->getPeakLevel(0));
    pStageComponent->getInputRMSMeter()->setRightPeakValue(pInputProc->getPeakLevel(1));
    pStageComponent->getOutputRMSMeter()->setLeftPeakValue(pOutputProc->getPeakLevel(0));
    pStageComponent->getOutputRMSMeter()->setRightPeakValue(pOutputProc->getPeakLevel(1));


    pStageComponent->getInputRMSMeter()->repaint();
    pStageComponent->getOutputRMSMeter()->repaint();

    editorLoadingDone = true;
}

void MangledAudioProcessorEditor::setUI()
{
    int selectedStageID = audioProcessor.getMainLayerDataStruct().getSelectedStageID();
    StageComponent* pStageComponent = nullptr;
    int distortionUnitID = audioProcessor.getMainLayerDataStruct().getSelectedDistoUnitID();
    int curveID = 0;
    bool isEQ = false;

    int circuitType = 0;
    int circuitID = 0;

    mainMenu.getMainLayerMenu()->switchToStageMenu(selectedStageID);
    mainMenu.getMainLayerMenu()->setButtonOn(selectedStageID, juce::dontSendNotification);
    
    for (int stageID = 0; stageID < MainLayerConstants::Processor<float>::nbOfStageMax; ++stageID)
    {
       pStageComponent = mainMenu.getMainLayerMenu()->getStageComponent(stageID);

       juce::String paramString = ScopeConstants::ParamStringID::GetParamStringID::dataType(stageID);
       int dataToDisplay = (int)*(audioProcessor.getMainLayerDataStruct().getAPVTSMainLayer().getRawParameterValue(paramString));

       pStageComponent->getScope()->switchScope(dataToDisplay);
       
       //isEQ = audioProcessor.getMainLayerDataStruct().getIsEQ(stageID);
       //pStageComponent->getDisplayEQButton()->setToggleState(isEQ, juce::dontSendNotification);
       //pStageComponent->getDisplayDistoButton()->setToggleState(! isEQ, juce::dontSendNotification);
       //isEQ ? pStageComponent->switchWaveShaperToEQ() : pStageComponent->switchEQToWaveShaper();

       //int filterID = audioProcessor.getMainLayerDataStruct().getSelectedFilterID(stageID);
       //pStageComponent->getEQComponent()->switchToEQSliderMenu(filterID);

       //distortionUnitID = audioProcessor.getMainLayerDataStruct().getDistortionUnitID(stageID);
       //bool firstDUDisplayed = distortionUnitID == 0 ? true : false;
       //pStageComponent->getDistortionComponent()->getDistortionUnitMenuTab()->getDistoUnitIDButton(0)->setToggleState(firstDUDisplayed, juce::dontSendNotification);
       //pStageComponent->getDistortionComponent()->getDistortionUnitMenuTab()->getDistoUnitIDButton(1)->setToggleState(!firstDUDisplayed, juce::dontSendNotification);

       //pStageComponent->getDistortionComponent()->switchDistortionUnit(distortionUnitID);

       //curveID = audioProcessor.getMainLayerDataStruct().getSelectedCurveID(stageID, distortionUnitID);
       //int curveType = audioProcessor.getMainLayerDataStruct().getPointCurveType(stageID, distortionUnitID, curveID);
       //pStageComponent->getDistortionComponent()->getDistortionSlider(distortionUnitID)->getCurveTypeComboBox()->setSelectedId(curveType + 1, juce::dontSendNotification);

       for (int distoUID = 0; distoUID < DistortionConstants::Processor<float>::nbOfDUMax; ++distoUID)
       {
           curveID = audioProcessor.getMainLayerDataStruct().getSelectedCurveID(stageID, distoUID);
           int curveType = audioProcessor.getMainLayerDataStruct().getPointCurveType(stageID, distoUID, curveID);
           pStageComponent->getDistortionComponent()->getDistortionSlider(distoUID)->getCurveTypeComboBox()->setSelectedId(curveType + 1, juce::dontSendNotification);

           circuitType = audioProcessor.getMainLayerDataStruct().getDistortionCircuitEquationType(stageID, distoUID);
           circuitID = audioProcessor.getMainLayerDataStruct().getDistortionCircuitEquationID(stageID, distoUID);
           pStageComponent->getDistortionComponent()->getDistortionSlider(distoUID)->setUI(circuitID, circuitType);
       }
    }
}

//void MangledAudioProcessorEditor::buttonStateChanged(juce::Button* button)
//{
//}
//
//void MangledAudioProcessorEditor::eqProcessorParamsUpdate()
//{
//}
//
//void MangledAudioProcessorEditor::distortionProcessorParamsUpdate()
//{
//}
