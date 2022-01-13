/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
//==============================================================================
MangledAudioProcessor::MangledAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
    , mainLayerDataStruct(*this, 
                        juce::Identifier("MangledDataStruct"),
                        MainLayerConstants::Processor<float>::nbOfStageMax,
                        EQConstants::Processor<float>::nbOfFilterMax,
                        DistortionConstants::Processor<float>::nbOfDUMax)
    , audioEngine(2)

#endif
{
    for (int i = 0; i < AudioEngineConstants::UI::stateString.size(); ++i)
    {
        state.add(new juce::ValueTree(AudioEngineConstants::UI::stateString[i]));
    }
}

MangledAudioProcessor::~MangledAudioProcessor()
{
}

//==============================================================================
const juce::String MangledAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool MangledAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool MangledAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool MangledAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double MangledAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int MangledAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int MangledAudioProcessor::getCurrentProgram()
{
    return 0;
}

void MangledAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String MangledAudioProcessor::getProgramName (int index)
{
    return {};
}

void MangledAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void MangledAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..

    audioEngine.prepareToPlay(sampleRate, samplesPerBlock);

    //setParams();
}

void MangledAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool MangledAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void MangledAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    // This is the place where you'd normally do the guts of your plugin's
    // audio processing...
    // Make sure to reset the state if your inner loop is processing
    // the samples and the outer loop is handling the channels.
    // Alternatively, you can process the samples with the channels
    // interleaved by keeping the same state.
    //for (int channel = 0; channel < totalNumInputChannels; ++channel)
    //{
    //    auto* channelData = buffer.getWritePointer (channel);

    //    // ..do something to the data...
    //}

    bufferSize.store(buffer.getNumSamples());

    if (mainLayerDataStruct.getShouldUpdate())
    {
        setParams();
        mainLayerDataStruct.setShouldUpdate(false);
    }

    if (this->getPlayHead() == nullptr)
    {
        ppq.store(0.0);
        ppqSample.store(0);
    }
    else
    {
        hostInformation.setPlayHead(this->getPlayHead());
        hostInformation.setSampleRate(getSampleRate());

        bpm.store(hostInformation.getBPM());
        ppq.store(hostInformation.getHostInfo().ppqPosition);
        ppqSample.store(hostInformation.getHostInfo().timeInSamples);

        if (!hostInformation.getHostInfo().isPlaying)
        {
            return;
        }
    }

    juce::dsp::AudioBlock<float> block(buffer);
    juce::dsp::ProcessContextReplacing<float> context(block);

    audioEngine.process(context);
    //audioEngine.processBuffer(buffer, midiMessages);

}

//==============================================================================
bool MangledAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* MangledAudioProcessor::createEditor()
{
    MangledAudioProcessorEditor* editor = new MangledAudioProcessorEditor(*this);

    //setParams();
    //editor->timerCallback2(3);
    //editor->timerCallback2(2);
    //editor->timerCallback2(1);

    int selectedStateID = mainLayerDataStruct.getSelectedStateID();
    int selectedStageID = mainLayerDataStruct.getSelectedStageID();
    int distortionUnitID = mainLayerDataStruct.getSelectedDistoUnitID();

    //editor->updateUI(0, 0);

    for (int stgID = 0; stgID < MainLayerConstants::Processor<float>::nbOfStageMax; ++stgID)
    {
        editor->updateScope(stgID);
        editor->updateRMSMeter(stgID);
        for (int duID = 0; duID < DistortionConstants::Processor<float>::nbOfDUMax; ++duID)
        {
            editor->updateDistortion(stgID, duID);
        }
    }

    editor->setUI();

    return editor;
    //return new MangledAudioProcessorEditor (*this);
}

//==============================================================================
void MangledAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.

    //if (mainLayerDataStruct.getRoot().getProperty("IsOpening"))
    //{
    //    mainLayerDataStruct.getRoot().setProperty("IsOpening", false, nullptr);
    //    return;
    //}


    int selectedState = mainLayerDataStruct.getSelectedStateID();

    saveState(selectedState);

    juce::ValueTree vtData("ALLDATA");
    vtData.setProperty(AudioEngineConstants::ParamStringID::selectedState, selectedState, nullptr);
    for (int stateID = 0; stateID < AudioEngineConstants::Processor<float>::StateID::maxStateID; ++stateID)
    {
        //juce::ValueTree vtState(AudioEngineConstants::UI::stateString[stateID]);
        juce::ValueTree vtState = state[stateID]->getRoot();
        //vtState.copyPropertiesAndChildrenFrom(vtState, nullptr);
        vtData.addChild(vtState, stateID, nullptr);
    }


    //mainLayerDataStruct.setSelectedStageID(0);
    //mainLayerDataStruct.setDistoUnitID(0, 0);
    //mainLayerDataStruct.setDistoUnitID(1, 0);
    //mainLayerDataStruct.setDistoUnitID(2, 0);
    //mainLayerDataStruct.setDistoUnitID(3, 0);

    //auto state = mainLayerDataStruct.getAPVTSMainLayer().copyState();

    //juce::ValueTree vtData("ALLDATA");

    //vtData.addChild(state, 0, nullptr);
    //vtData.addChild(mainLayerDataStruct.getRoot(), 1, nullptr);

    std::unique_ptr<juce::XmlElement> xml(vtData.createXml());

    copyXmlToBinary(*xml, destData);
}

void MangledAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
     
    //std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));

    //if (xmlState.get() != nullptr)
    //{
    //    if (xmlState->hasTagName("ALLDATA"))
    //    {
    //        juce::XmlElement* xmlAPVTS = xmlState.get()->getChildByName(mainLayerDataStruct.getAPVTSMainLayer().state.getType());

    //        if (xmlAPVTS != nullptr)
    //        {
    //            mainLayerDataStruct.getAPVTSMainLayer().replaceState(juce::ValueTree::fromXml(*xmlAPVTS));
    //        }

    //        juce::XmlElement* xmlWS = xmlState.get()->getChildByName(mainLayerDataStruct.getRoot().getType());
    //        if (xmlWS != nullptr)
    //        {
    //            juce::ValueTree vtTest = juce::ValueTree::fromXml(*xmlWS);
    //            
    //            mainLayerDataStruct.getRoot().copyPropertiesAndChildrenFrom(vtTest, nullptr);

    //            //juce::ValueTree vtMainLayer = vtTest.getChildWithName(MainLayerApvtsIDString::stage + "0");
    //            //juce::ValueTree vtDisto = vtMainLayer.getChildWithName(MainLayerValueTreeID::distortion);
    //            //juce::ValueTree vtDU = vtDisto.getChildWithName(MainLayerValueTreeID::distoUnit + "0");
    //            //juce::ValueTree vtCircuit = vtDU.getChildWithName(MainLayerValueTreeID::distortionCircuit);
    //            //int vtType = vtCircuit.getProperty(MainLayerValueTreeID::equationType);
    //            //int vtID = vtCircuit.getProperty(MainLayerValueTreeID::equationID);

    //        }
    //    }
    //}

    std::unique_ptr<juce::XmlElement> xmlWholeState(getXmlFromBinary(data, sizeInBytes));
    int selectedState = -1;
    if (xmlWholeState.get() != nullptr)
    {
        if (xmlWholeState->hasTagName("ALLDATA"))
        {
            juce::ValueTree vtData = juce::ValueTree::fromXml(*xmlWholeState);
            selectedState = vtData.getProperty(AudioEngineConstants::ParamStringID::selectedState);
            for (int stateID = 0; stateID < AudioEngineConstants::Processor<float>::StateID::maxStateID; ++stateID)
            {
                state[stateID]->removeAllChildren(nullptr);
                //juce::XmlElement* xmlState = xmlWholeState.get()->getChildByName(AudioEngineConstants::UI::stateString[stateID]);
                juce::ValueTree vtState = vtData.getChildWithName(AudioEngineConstants::UI::stateString[stateID]);
                //if (xmlState != nullptr)
                if(vtState.isValid())
                {
                    //juce::XmlElement* xmlAPVTS = xmlState->getChildByName(mainLayerDataStruct.getAPVTSMainLayer().state.getType());
                    juce::ValueTree vtAPVTS = vtState.getChildWithName(mainLayerDataStruct.getAPVTSMainLayer().state.getType());
                    //if (xmlAPVTS != nullptr)
                    if(vtAPVTS.isValid())
                    {
                        //juce::ValueTree vtAPVTS = juce::ValueTree::fromXml(*xmlAPVTS);
                        vtState.removeChild(vtAPVTS, nullptr);
                        state[stateID]->addChild(vtAPVTS, 0, nullptr);
                    }

                    //juce::XmlElement* xmlVT = xmlState->getChildByName(mainLayerDataStruct.getRoot().getType());
                    //if (xmlVT != nullptr)
                    juce::ValueTree vtTest = vtState.getChildWithName(mainLayerDataStruct.getRoot().getType());
                    if(vtTest.isValid())
                    {
                        //juce::ValueTree vtTest = juce::ValueTree::fromXml(*xmlVT);
                        vtState.removeChild(vtTest, nullptr);
                        state[stateID]->addChild(vtTest, 1, nullptr);
                        //mainLayerDataStruct.getRoot().copyPropertiesAndChildrenFrom(vtTest, nullptr);
                    }
                }
            }
        }
    }

    if (selectedState >= AudioEngineConstants::Processor<float>::StateID::A
        && selectedState <= AudioEngineConstants::Processor<float>::StateID::D)
    {
        loadState(selectedState);
    }

    setParams();
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new MangledAudioProcessor();
}

void MangledAudioProcessor::setParams()
{
    juce::AudioProcessorValueTreeState& apvts = mainLayerDataStruct.getAPVTSMainLayer();

    //Audio Engine=======================================================================================
    juce::String paramString = AudioEngineConstants::ParamStringID::GetParamStringID::masterLimiterOnOff();
    auto& newMasterLimiterIsBypassed = *apvts.getRawParameterValue(paramString);

    paramString = AudioEngineConstants::ParamStringID::GetParamStringID::masterGain();
    auto& newMasterGain = *apvts.getRawParameterValue(paramString);

    audioEngine.setParam(newMasterLimiterIsBypassed, newMasterGain);

    //Main Menu Tab======================================================================================
    MainLayerProcessor<float>* pMainLayerProcessor = audioEngine.getMainLayerProcessor();

    paramString = MainLayerConstants::ParamStringID::GetParamStringID::isBypassed();
    auto& mlIsBypassed = (*apvts.getRawParameterValue(paramString));
    pMainLayerProcessor->setIsBypassed(mlIsBypassed);

    //paramString = getPostProcessingOnOffParamString();
    //auto& postProcessorIsBypassed = (*apvts.getRawParameterValue(paramString));

    for (int stageID = 0; stageID < MainLayerConstants::Processor<float>::nbOfStageMax; ++stageID)//
    {
        setStageParams(pMainLayerProcessor, stageID);
    }
}

void MangledAudioProcessor::setStageParams(MainLayerProcessor<float>* pMainLayerProcessor, int stageID)
{
    juce::AudioProcessorValueTreeState& apvts = mainLayerDataStruct.getAPVTSMainLayer();

    StageProcessorBase<float>* pStageProcessor = pMainLayerProcessor->getStageProcessor(stageID);

    //Stage Processor On/Off=====================================================================
    juce::String paramString = StageConstants::ParamStringID::GetParamStringID::isBypassed(stageID);
    auto& stageIsBypassed = (*apvts.getRawParameterValue(paramString));
    pStageProcessor->setIsBypassed(stageIsBypassed);

    //Stage Processor Input Gain=================================================================
    paramString = StageConstants::ParamStringID::GetParamStringID::inputGain(stageID);
    auto& stageInputGain = (*apvts.getRawParameterValue(paramString));
    pStageProcessor->setInputGain(stageInputGain);

    //Stage Processor Output Gain================================================================
    paramString = StageConstants::ParamStringID::GetParamStringID::outputGain(stageID);
    auto& stageOutputGain = (*apvts.getRawParameterValue(paramString));
    pStageProcessor->setOutputGain(stageOutputGain);

    //Scope======================================================================================
    setScopeParams(apvts, stageID);

    //EQ=========================================================================================
    setEQParams(apvts, stageID);

    //Phaser=====================================================================================
    setPhaserParams(apvts, stageID);

    //Distortion=================================================================================
    setDistortionParams(apvts, stageID);
}

void MangledAudioProcessor::setScopeParams(juce::AudioProcessorValueTreeState& apvts, int stageID)
{
    juce::String paramString = ScopeConstants::ParamStringID::GetParamStringID::dataType(stageID);
    int dataToDisplay = (int) *apvts.getRawParameterValue(paramString);
    int selectedStageID = mainLayerDataStruct.getSelectedStageID();

    //We Bypass FFT proc whenever the stage is not displayed or we display temporal data.
    bool stageMask = stageID == selectedStageID ? false : true;
    bool dataMask = dataToDisplay == 0 ? false : true;

    paramString = ScopeConstants::ParamStringID::GetParamStringID::preEQIsBypassed(stageID);
    auto& preEQIsBypassed = *apvts.getRawParameterValue(paramString);
    audioEngine.getMainLayerProcessor()->getStageProcessor(stageID)->getPreEQFFTProcessor()->setIsBypassed((preEQIsBypassed || dataMask) || stageMask);
    
    paramString = ScopeConstants::ParamStringID::GetParamStringID::postEQIsBypassed(stageID);
    auto& postEQIsBypassed = *apvts.getRawParameterValue(paramString);
    audioEngine.getMainLayerProcessor()->getStageProcessor(stageID)->getPostEQFFTProcessor()->setIsBypassed((postEQIsBypassed || dataMask) || stageMask);
    
    paramString = ScopeConstants::ParamStringID::GetParamStringID::postDistoIsBypassed(stageID);
    auto& postDistoIsBypassed = *apvts.getRawParameterValue(paramString);
    audioEngine.getMainLayerProcessor()->getStageProcessor(stageID)->getPostDistoFFTProcessor()->setIsBypassed((postDistoIsBypassed || dataMask) || stageMask);
    
    paramString = RMSConstants::ParamStringID::GetParamStringID::isNormalized(stageID);
    auto& isNormalized = *apvts.getRawParameterValue(paramString);
    audioEngine.getMainLayerProcessor()->getStageProcessor(stageID)->getInputTemporalProcessor()->setIsNormalized(isNormalized);
    audioEngine.getMainLayerProcessor()->getStageProcessor(stageID)->getOutputTemporalProcessor()->setIsNormalized(isNormalized);

    paramString = RMSConstants::ParamStringID::GetParamStringID::subViewIsBypassed(stageID);
    auto& subViewIsBypassed = *apvts.getRawParameterValue(paramString);
    audioEngine.getMainLayerProcessor()->getStageProcessor(stageID)->getInputTemporalProcessor()->setSubViewIsBypassed(subViewIsBypassed);
    audioEngine.getMainLayerProcessor()->getStageProcessor(stageID)->getOutputTemporalProcessor()->setSubViewIsBypassed(subViewIsBypassed);

    paramString = RMSConstants::ParamStringID::GetParamStringID::monoViewIsBypassed(stageID);
    auto& monoViewIsBypassed = *apvts.getRawParameterValue(paramString);
    audioEngine.getMainLayerProcessor()->getStageProcessor(stageID)->getInputTemporalProcessor()->setMonoViewIsBypassed(monoViewIsBypassed);
    audioEngine.getMainLayerProcessor()->getStageProcessor(stageID)->getOutputTemporalProcessor()->setMonoViewIsBypassed(monoViewIsBypassed);

    paramString = RMSConstants::ParamStringID::GetParamStringID::subViewCutoff(stageID);
    auto& subViewCutoff = *apvts.getRawParameterValue(paramString);
    audioEngine.getMainLayerProcessor()->getStageProcessor(stageID)->getInputTemporalProcessor()->setSubViewCutoff(subViewCutoff);
    audioEngine.getMainLayerProcessor()->getStageProcessor(stageID)->getOutputTemporalProcessor()->setSubViewCutoff(subViewCutoff);


    audioEngine.getMainLayerProcessor()->getStageProcessor(stageID)->getInputTemporalProcessor()->setIsBypassed(stageMask);
    audioEngine.getMainLayerProcessor()->getStageProcessor(stageID)->getOutputTemporalProcessor()->setIsBypassed(stageMask);

}

void MangledAudioProcessor::setEQParams(juce::AudioProcessorValueTreeState& apvts, int stageID)
{
    //juce::AudioProcessorValueTreeState& apvts = mainLayerDataStruct.getAPVTSMainLayer();

    EQProcessor<float>* pEQProcessor = audioEngine.getMainLayerProcessor()->getStageProcessor(stageID)->getEQProcessor();

    //EQProcessor<float>::FilterParamsForUpdate newEQFilterParams;
    IIRBiquadFilter<float>::FilterParams newEQFilterParams;

    //Mix EQ
    juce::String paramString = EQConstants::ParamStringID::GetParamStringID::mix(stageID);
    auto& newEQMix = *apvts.getRawParameterValue(paramString);
    pEQProcessor->setMixParam(newEQMix);

    //Bypass EQ
    paramString = EQConstants::ParamStringID::GetParamStringID::isBypassed(stageID);
    auto& newEQBypass = *apvts.getRawParameterValue(paramString);
    pEQProcessor->setIsBypassed(newEQBypass);

    for (int filterID = 0; filterID < EQConstants::Processor<float>::nbOfFilterMax; ++filterID)
    {
        paramString = EQConstants::ParamStringID::GetParamStringID::filterCutoff(stageID, filterID);
        newEQFilterParams.biquadParams.cutoff = *apvts.getRawParameterValue(paramString);

        paramString = EQConstants::ParamStringID::GetParamStringID::filterQ(stageID, filterID);
        newEQFilterParams.biquadParams.q = *apvts.getRawParameterValue(paramString);

        paramString = EQConstants::ParamStringID::GetParamStringID::filterGain(stageID, filterID);
        float gaindb = *apvts.getRawParameterValue(paramString);
        newEQFilterParams.biquadParams.gain = juce::Decibels::decibelsToGain(gaindb);

        paramString = EQConstants::ParamStringID::GetParamStringID::filterOrder(stageID, filterID);
        newEQFilterParams.nbOfBiquads = (int)*apvts.getRawParameterValue(paramString) + 1;

        paramString = EQConstants::ParamStringID::GetParamStringID::filterIsActive(stageID, filterID);
        newEQFilterParams.isActive = !(*apvts.getRawParameterValue(paramString));

        paramString = EQConstants::ParamStringID::GetParamStringID::filterIsBypassed(stageID, filterID);
        newEQFilterParams.isBypassed = (*apvts.getRawParameterValue(paramString));

        paramString = EQConstants::ParamStringID::GetParamStringID::filterType(stageID, filterID);
        newEQFilterParams.isActive ? newEQFilterParams.biquadParams.type = (int)*apvts.getRawParameterValue(paramString)
            : newEQFilterParams.biquadParams.type = -1;

        newEQFilterParams.id = filterID;

        pEQProcessor->setEQFilterParams(newEQFilterParams);
    }

    int selectedFilterID = mainLayerDataStruct.getSelectedFilterID(stageID);
    
    if (selectedFilterID != -1)
    {
        paramString = EQConstants::ParamStringID::GetParamStringID::filterIsActive(stageID, selectedFilterID);
        auto& isActive = (*apvts.getRawParameterValue(paramString));

        if (isActive)
        {
            mainLayerDataStruct.setSelectedFilterID(stageID, -1);
        }
    }

    pEQProcessor->pushFilterSumBufferIntoFifo();
}

void MangledAudioProcessor::setPhaserParams(juce::AudioProcessorValueTreeState& apvts, int stageID)
{
    //juce::AudioProcessorValueTreeState& apvts = mainLayerDataStruct.getAPVTSMainLayer();

    CustomPhaser<float>* pPhaserProcessor = audioEngine.getMainLayerProcessor()->getStageProcessor(stageID)->getPhaserProcessor();

    CustomPhaser<float>::Parameters newPhaserProcessorParams;

    juce::String paramString = PhaserConstants::ParamStringID::GetParamStringID::centerFrequency(stageID);
    newPhaserProcessorParams.centreHz = *apvts.getRawParameterValue(paramString);

    paramString = PhaserConstants::ParamStringID::GetParamStringID::feedback(stageID);
    newPhaserProcessorParams.feedback = *apvts.getRawParameterValue(paramString);

    paramString = PhaserConstants::ParamStringID::GetParamStringID::rate(stageID);
    newPhaserProcessorParams.rateHz = *apvts.getRawParameterValue(paramString);

    paramString = PhaserConstants::ParamStringID::GetParamStringID::depth(stageID);
    newPhaserProcessorParams.depth = *apvts.getRawParameterValue(paramString);

    paramString = PhaserConstants::ParamStringID::GetParamStringID::nbOfStages(stageID);
    newPhaserProcessorParams.numStages = (int)*apvts.getRawParameterValue(paramString);

    paramString = PhaserConstants::ParamStringID::GetParamStringID::mix(stageID);
    newPhaserProcessorParams.mix = *apvts.getRawParameterValue(paramString);

    paramString = PhaserConstants::ParamStringID::GetParamStringID::isBypassed(stageID);
    newPhaserProcessorParams.isBypassed = *apvts.getRawParameterValue(paramString);

    pPhaserProcessor->setPhaserParams(newPhaserProcessorParams);

}

void MangledAudioProcessor::setDistortionParams(juce::AudioProcessorValueTreeState& apvts, int stageID)
{
    //juce::AudioProcessorValueTreeState& apvts = mainLayerDataStruct.getAPVTSMainLayer();

    DistortionProcessor<float>::Parameters newDistortionProcessorParams;
    DistortionUnitProcessor<float>::Parameters newDistortionUnitProcessorParams;

    DistortionProcessor<float>* pDistortionProcessor = audioEngine.getMainLayerProcessor()->getStageProcessor(stageID)->getDistortionProcessor();

    //Over Sampling
    juce::String paramString = DistortionConstants::ParamStringID::GetParamStringID::overSampling(stageID);
    auto& newOverSampling = *apvts.getRawParameterValue(paramString);

    //Mix Distortion
    paramString = DistortionConstants::ParamStringID::GetParamStringID::mix(stageID);
    auto& newMixDistortionProcessor = *apvts.getRawParameterValue(paramString);

    //Bypass Distortion
    paramString = DistortionConstants::ParamStringID::GetParamStringID::isBypassed(stageID);
    auto& newIsBypassedDistortionProcessor = *apvts.getRawParameterValue(paramString);

    newDistortionProcessorParams.mix = newMixDistortionProcessor;
    newDistortionProcessorParams.isBypassed = newIsBypassedDistortionProcessor;
    pDistortionProcessor->setParams(newDistortionProcessorParams);

    for (int distortionUnitID = 0; distortionUnitID < DistortionConstants::Processor<float>::nbOfDUMax; ++distortionUnitID)
    {
        DistortionUnitProcessor<float>* pDistortionDU = pDistortionProcessor->getDistortionUnitProcessor(distortionUnitID);

        //Distortion First Unit OnOff
        paramString = DistoUnitConstants::ParamStringID::GetParamStringID::equationType(stageID, distortionUnitID);
        newDistortionUnitProcessorParams.circuit.equationType = *apvts.getRawParameterValue(paramString);

        switch (newDistortionUnitProcessorParams.circuit.equationType)
        {
        case static_cast<int>(DistortionCircuitConstants::Processor<float>::EquationType::sigmoid):
            paramString = DistoUnitConstants::ParamStringID::GetParamStringID::sigmoidEQA(stageID, distortionUnitID);
            newDistortionUnitProcessorParams.circuit.equationID = *apvts.getRawParameterValue(paramString);
            break;
        case static_cast<int>(DistortionCircuitConstants::Processor<float>::EquationType::symetric):
            paramString = DistoUnitConstants::ParamStringID::GetParamStringID::symetricEQA(stageID, distortionUnitID);
            newDistortionUnitProcessorParams.circuit.equationID = *apvts.getRawParameterValue(paramString);
            break;
        case static_cast<int>(DistortionCircuitConstants::Processor<float>::EquationType::asymetric):
            paramString = DistoUnitConstants::ParamStringID::GetParamStringID::asymetricEQA(stageID, distortionUnitID);
            newDistortionUnitProcessorParams.circuit.equationID = *apvts.getRawParameterValue(paramString);
            break;
        case static_cast<int>(DistortionCircuitConstants::Processor<float>::EquationType::special):
            paramString = DistoUnitConstants::ParamStringID::GetParamStringID::specialEQA(stageID, distortionUnitID);
            newDistortionUnitProcessorParams.circuit.equationID = *apvts.getRawParameterValue(paramString);
            break;
         default: 
            //paramString = DistoUnitConstants::ParamStringID::GetParamStringID::specialEQA(stageID, distortionUnitID);
            newDistortionUnitProcessorParams.circuit.equationID = 0;
            break;
        }

        //Distortion Routing
        paramString = DistoUnitConstants::ParamStringID::GetParamStringID::routing(stageID, distortionUnitID);
        newDistortionUnitProcessorParams.routing = *apvts.getRawParameterValue(paramString);

        //DC Filter On
        paramString = DistoUnitConstants::ParamStringID::GetParamStringID::dcFilterIsBypassed(stageID, distortionUnitID);
        newDistortionUnitProcessorParams.dcFilter.isBypassed = *apvts.getRawParameterValue(paramString);

        //PreGain
        paramString = DistoUnitConstants::ParamStringID::GetParamStringID::preGain(stageID, distortionUnitID);
        newDistortionUnitProcessorParams.preGain = *apvts.getRawParameterValue(paramString);

        //Drive
        paramString = DistoUnitConstants::ParamStringID::GetParamStringID::drive(stageID, distortionUnitID);
        newDistortionUnitProcessorParams.circuit.drive = *apvts.getRawParameterValue(paramString);

        //Warp
        paramString = DistoUnitConstants::ParamStringID::GetParamStringID::warp(stageID, distortionUnitID);
        newDistortionUnitProcessorParams.circuit.warp = *apvts.getRawParameterValue(paramString);

        //PostGain
        paramString = DistoUnitConstants::ParamStringID::GetParamStringID::postGain(stageID, distortionUnitID);
        newDistortionUnitProcessorParams.postGain = *apvts.getRawParameterValue(paramString);

        //Mix
        paramString = DistoUnitConstants::ParamStringID::GetParamStringID::mix(stageID, distortionUnitID);
        newDistortionUnitProcessorParams.mix = *apvts.getRawParameterValue(paramString);

        //Is Bypassed
        paramString = DistoUnitConstants::ParamStringID::GetParamStringID::isBypassed(stageID, distortionUnitID);
        newDistortionUnitProcessorParams.isBypassed = *apvts.getRawParameterValue(paramString);

        //OverSampling
        newDistortionUnitProcessorParams.overSampling = newOverSampling;

        //DistortionUnit ID
        newDistortionUnitProcessorParams.id = distortionUnitID;

        pDistortionDU->setDistortionUnitParams(newDistortionUnitProcessorParams);

        SampleRemapper<float>* pSampleRemapper = pDistortionDU->getSampleRemapper();

        int nbOfPoints = mainLayerDataStruct.getNbOfPoints(stageID, distortionUnitID);
        pSampleRemapper->setNbOfActiveBins(nbOfPoints - 1);

        juce::Point<float> leftPoint = mainLayerDataStruct.getPoint(stageID, distortionUnitID, 0);
        juce::Point<float> rightPoint(0.0f, 0.0f);
        //float tension = mainLayerDataStruct.getTension(stageID, distortionUnitID, 0);
        //int curveID = mainLayerDataStruct.getPointCurveType(stageID, distortionUnitID, 0);
        SampleRemapperBin<float>* pSRBin = nullptr;

        for (int binID = 0; binID < nbOfPoints - 1; ++binID)
        {
            pSRBin = pSampleRemapper->getBin(binID);
            SampleRemapperBin<float>::BinParams srBinParams;
            srBinParams.leftPoint = mainLayerDataStruct.getPoint(stageID, distortionUnitID, binID + 1);
            srBinParams.rightPoint = mainLayerDataStruct.getPoint(stageID, distortionUnitID, binID);
            srBinParams.tension = mainLayerDataStruct.getTension(stageID, distortionUnitID, binID);
            srBinParams.curveID = mainLayerDataStruct.getPointCurveType(stageID, distortionUnitID, binID);

            //if (srBinParams != pSRBin->getBinParams())
            //{
                pSRBin->setBinParam(srBinParams);
            //}
            pDistortionDU->computeDistortionEQAPathData(binID);
        }//SR Bin Loop
    }//Disto Unit loop
}
