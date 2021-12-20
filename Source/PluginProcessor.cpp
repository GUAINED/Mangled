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
    setParams();
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

    if (mainLayerDataStruct.getRoot().getProperty("IsOpening"))
    {
        mainLayerDataStruct.getRoot().setProperty("IsOpening", false, nullptr);
        return;
    }


    auto state = mainLayerDataStruct.getAPVTSMainLayer().copyState();
    //if (state.isValid())
    //{
    //    int yolooo = 4;
    //}
    //std::unique_ptr<juce::XmlElement> xml(state.createXml());
    
    //std::unique_ptr < juce::XmlElement> xmlAPVTS(state.createXml());
    //std::unique_ptr < juce::XmlElement> xmlWS(mainLayerDataStruct.getRoot().createXml());
    //xml.addChildElement(xmlWS, 1);
    juce::ValueTree vtData("ALLDATA");

    vtData.addChild(state, 0, nullptr);
    vtData.addChild(mainLayerDataStruct.getRoot(), 1, nullptr);

    //int type = mainLayerDataStruct.getDistortionCircuitEquationType(0, 0);

    ////paramString = getDistortionDUEquationParamString(stageID, distortionUnitID);
    //int id = mainLayerDataStruct.getDistortionCircuitEquationID(0, 0);


    std::unique_ptr<juce::XmlElement> xml(vtData.createXml());

    copyXmlToBinary(*xml, destData);

    //juce::ValueTree vtTest = juce::ValueTree::fromXml(*xml);
    //if (vtTest.isValid())
    //{
    //    int yolo = 2;

    //}

    //    juce::ValueTree vtD = vtTest.getChildWithName(mainLayerDataStruct.getRoot().getType());


    //    juce::ValueTree vtMainLayer = vtD.getChildWithName(MainLayerApvtsIDString::stage + "0");
    //juce::ValueTree vtDisto = vtMainLayer.getChildWithName(MainLayerValueTreeID::distortion);
    //juce::ValueTree vtDU = vtDisto.getChildWithName(MainLayerValueTreeID::distoUnit + "0");
    //juce::ValueTree vtCircuit = vtDU.getChildWithName(MainLayerValueTreeID::distortionCircuit);
    //int vtType = vtCircuit.getProperty(MainLayerValueTreeID::equationType);
    //int vtID = vtCircuit.getProperty(MainLayerValueTreeID::equationID);
}

void MangledAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
     
    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));

    if (xmlState.get() != nullptr)
    {
        if (xmlState->hasTagName("ALLDATA"))
        {
            //mainLayerDataStruct.getAPVTSMainLayer().replaceState(juce::ValueTree::fromXml(*xmlState));
            //mainLayerDataStruct.getRoot().readFromData(data, sizeInBytes);
            //setParams();
            juce::XmlElement* xmlAPVTS = xmlState.get()->getChildByName(mainLayerDataStruct.getAPVTSMainLayer().state.getType());

            if (xmlAPVTS != nullptr)
            {
                mainLayerDataStruct.getAPVTSMainLayer().replaceState(juce::ValueTree::fromXml(*xmlAPVTS));
            }

            juce::XmlElement* xmlWS = xmlState.get()->getChildByName(mainLayerDataStruct.getRoot().getType());
            if (xmlWS != nullptr)
            {
                juce::ValueTree vtTest = juce::ValueTree::fromXml(*xmlWS);

                //juce::ValueTree vtMainLayer = vtTest.getChildWithName(MainLayerApvtsIDString::stage + "0");
                //juce::ValueTree vtDisto = vtMainLayer.getChildWithName(MainLayerValueTreeID::distortion);
                //juce::ValueTree vtDU = vtDisto.getChildWithName(MainLayerValueTreeID::distoUnit + "0");
                //juce::ValueTree vtCircuit = vtDU.getChildWithName(MainLayerValueTreeID::distortionCircuit);
                //int vtType = vtCircuit.getProperty(MainLayerValueTreeID::equationType);
                //int vtID = vtCircuit.getProperty(MainLayerValueTreeID::equationID);
                
                mainLayerDataStruct.getRoot().copyPropertiesAndChildrenFrom(vtTest, nullptr);

            }


        }
    }

    //mainLayerDataStruct.getRoot().setProperty("IsOpening", false, nullptr);
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

    //count++;
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
    audioEngine.getMainLayerProcessor()->getStageProcessor(stageID)->getPreEQScopeProcessor()->setIsBypassed((preEQIsBypassed || dataMask) || stageMask);
    
    paramString = ScopeConstants::ParamStringID::GetParamStringID::postEQIsBypassed(stageID);
    auto& postEQIsBypassed = *apvts.getRawParameterValue(paramString);
    audioEngine.getMainLayerProcessor()->getStageProcessor(stageID)->getPostEQScopeProcessor()->setIsBypassed((postEQIsBypassed || dataMask) || stageMask);
    
    paramString = ScopeConstants::ParamStringID::GetParamStringID::postDistoIsBypassed(stageID);
    auto& postDistoIsBypassed = *apvts.getRawParameterValue(paramString);
    audioEngine.getMainLayerProcessor()->getStageProcessor(stageID)->getPostDistoScopeProcessor()->setIsBypassed((postDistoIsBypassed || dataMask) || stageMask);
    
    paramString = ScopeConstants::ParamStringID::GetParamStringID::isNormalized(stageID);
    auto& isNormalized = *apvts.getRawParameterValue(paramString);
    audioEngine.getMainLayerProcessor()->getStageProcessor(stageID)->getInputRMSProcessor()->setIsNormalized(isNormalized);
    audioEngine.getMainLayerProcessor()->getStageProcessor(stageID)->getOutputRMSProcessor()->setIsNormalized(isNormalized);

    paramString = ScopeConstants::ParamStringID::GetParamStringID::subViewIsBypassed(stageID);
    auto& subViewIsBypassed = *apvts.getRawParameterValue(paramString);
    audioEngine.getMainLayerProcessor()->getStageProcessor(stageID)->getInputRMSProcessor()->setSubViewIsBypassed(subViewIsBypassed);
    audioEngine.getMainLayerProcessor()->getStageProcessor(stageID)->getOutputRMSProcessor()->setSubViewIsBypassed(subViewIsBypassed);

    paramString = ScopeConstants::ParamStringID::GetParamStringID::monoViewIsBypassed(stageID);
    auto& monoViewIsBypassed = *apvts.getRawParameterValue(paramString);
    audioEngine.getMainLayerProcessor()->getStageProcessor(stageID)->getInputRMSProcessor()->setMonoViewIsBypassed(monoViewIsBypassed);
    audioEngine.getMainLayerProcessor()->getStageProcessor(stageID)->getOutputRMSProcessor()->setMonoViewIsBypassed(monoViewIsBypassed);

    paramString = ScopeConstants::ParamStringID::GetParamStringID::subViewCutoff(stageID);
    auto& subViewCutoff = *apvts.getRawParameterValue(paramString);
    audioEngine.getMainLayerProcessor()->getStageProcessor(stageID)->getInputRMSProcessor()->setSubViewCutoff(subViewCutoff);
    audioEngine.getMainLayerProcessor()->getStageProcessor(stageID)->getOutputRMSProcessor()->setSubViewCutoff(subViewCutoff);


    audioEngine.getMainLayerProcessor()->getStageProcessor(stageID)->getInputRMSProcessor()->setIsBypassed(stageMask);
    audioEngine.getMainLayerProcessor()->getStageProcessor(stageID)->getOutputRMSProcessor()->setIsBypassed(stageMask);

}

void MangledAudioProcessor::setEQParams(juce::AudioProcessorValueTreeState& apvts, int stageID)
{
    //juce::AudioProcessorValueTreeState& apvts = mainLayerDataStruct.getAPVTSMainLayer();

    EQProcessor<float>* pEQProcessor = audioEngine.getMainLayerProcessor()->getStageProcessor(stageID)->getEQProcessor();

    EQProcessor<float>::FilterParamsForUpdate newEQFilterParams;

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
        newEQFilterParams.cutoff = *apvts.getRawParameterValue(paramString);

        paramString = EQConstants::ParamStringID::GetParamStringID::filterQ(stageID, filterID);
        newEQFilterParams.q = *apvts.getRawParameterValue(paramString);

        paramString = EQConstants::ParamStringID::GetParamStringID::filterGain(stageID, filterID);
        newEQFilterParams.gain = *apvts.getRawParameterValue(paramString);

        paramString = EQConstants::ParamStringID::GetParamStringID::filterOrder(stageID, filterID);
        newEQFilterParams.nbOfBiquads = (int)*apvts.getRawParameterValue(paramString);

        paramString = EQConstants::ParamStringID::GetParamStringID::filterIsActive(stageID, filterID);
        newEQFilterParams.isActive = !(*apvts.getRawParameterValue(paramString));

        paramString = EQConstants::ParamStringID::GetParamStringID::filterIsBypassed(stageID, filterID);
        newEQFilterParams.isBypassed = (*apvts.getRawParameterValue(paramString));

        paramString = EQConstants::ParamStringID::GetParamStringID::filterType(stageID, filterID);
        newEQFilterParams.isActive ? newEQFilterParams.type = (int)*apvts.getRawParameterValue(paramString)
            : newEQFilterParams.type = -1;

        newEQFilterParams.id = filterID;

        //if (&newEQFilterParams != &(stageProcessor->getEQProcessor()->getFilterWholeParams(filter)))
        //{
        pEQProcessor->setEQFilterParams(newEQFilterParams);
        //}

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

}

void MangledAudioProcessor::setPhaserParams(juce::AudioProcessorValueTreeState& apvts, int stageID)
{
    //juce::AudioProcessorValueTreeState& apvts = mainLayerDataStruct.getAPVTSMainLayer();

    CustomPhaser<float>* pPhaserProcessor = audioEngine.getMainLayerProcessor()->getStageProcessor(stageID)->getPhaserProcessor();

    CustomPhaser<float>::CustomPhaserParams newPhaserProcessorParams;

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

    if (&newPhaserProcessorParams != &(pPhaserProcessor->getPhaserParams()))
    {
        pPhaserProcessor->setPhaserParams(newPhaserProcessorParams);
    }
    paramString = PhaserConstants::ParamStringID::GetParamStringID::mix(stageID);
    auto& newPhaserMix = *apvts.getRawParameterValue(paramString);
    pPhaserProcessor->setMix(newPhaserMix);

    paramString = PhaserConstants::ParamStringID::GetParamStringID::isBypassed(stageID);
    auto& newPhaserIsBypassed = *apvts.getRawParameterValue(paramString);
    pPhaserProcessor->setIsBypassed(newPhaserIsBypassed);
}

void MangledAudioProcessor::setDistortionParams(juce::AudioProcessorValueTreeState& apvts, int stageID)
{
    //juce::AudioProcessorValueTreeState& apvts = mainLayerDataStruct.getAPVTSMainLayer();

    DistortionProcessor<float>::DistortionParams newDistortionProcessorParams;
    DistortionUnitProcessor<float>::DistortionUnitParams newDistortionUnitProcessorParams;

    DistortionProcessor<float>* pDistortionProcessor = audioEngine.getMainLayerProcessor()->getStageProcessor(stageID)->getDistortionProcessor();

    //Over Sampling
    juce::String paramString = getDistortionOverSamplingParamString(stageID);
    auto& newOverSampling = *apvts.getRawParameterValue(paramString);

    //Mix Distortion
    paramString = getDistortionMixParamString(stageID);
    auto& newMixDistortionProcessor = *apvts.getRawParameterValue(paramString);

    //Bypass Distortion
    paramString = getDistortionIsBypassedParamString(stageID);
    auto& newIsBypassedDistortionProcessor = *apvts.getRawParameterValue(paramString);

    newDistortionProcessorParams.mix = newMixDistortionProcessor;
    newDistortionProcessorParams.isBypassed = newIsBypassedDistortionProcessor;
    pDistortionProcessor->setDistortionParams(newDistortionProcessorParams);

    for (int distortionUnitID = 0; distortionUnitID < static_cast<int>(DistortionProcessor<float>::DistortionUnitID::maxDistortionUnitID); ++distortionUnitID)
    {
        DistortionUnitProcessor<float>* pDistortionDU = pDistortionProcessor->getDistortionUnitProcessor(distortionUnitID);

        //Distortion First Unit OnOff
        paramString = getDistortionDUIsBypassedParamString(stageID, distortionUnitID);
        newDistortionUnitProcessorParams.isBypassed = *apvts.getRawParameterValue(paramString);

        //paramString = (juce::String)("Sigmoid") + (juce::String)(stageID)+juce::String(distortionUnitID);
        //auto& sigmoidEQA = *apvts.getRawParameterValue(paramString);
        //
        //if (sigmoidEQA == 0)
        //{
        //    int yolo = 1;
        //}
        //
        //paramString = (juce::String)("Symetric") + (juce::String)(stageID)+juce::String(distortionUnitID);
        //auto& symetricEQA = *apvts.getRawParameterValue(paramString);

        //if (symetricEQA == 1)
        //{
        //    int yolo = 2;
        //}
        //Distortion Routing
        paramString = getDistortionDURoutingParamString(stageID, distortionUnitID);
        newDistortionUnitProcessorParams.routing = *apvts.getRawParameterValue(paramString);

        //paramString = getDistortionDUEquationParamString(stageID, distortionUnitID);
        newDistortionUnitProcessorParams.distortionEquationType = mainLayerDataStruct.getDistortionCircuitEquationType(stageID, distortionUnitID);

        //paramString = getDistortionDUEquationParamString(stageID, distortionUnitID);
        newDistortionUnitProcessorParams.distortionEquationID = mainLayerDataStruct.getDistortionCircuitEquationID(stageID, distortionUnitID);

        //DC Filter On
        paramString = getDistortionDUDCFilterIsBypassedParamString(stageID, distortionUnitID);
        newDistortionUnitProcessorParams.dcFilterIsBypassed = *apvts.getRawParameterValue(paramString);

        //PreGain WaveShaper
        paramString = getDistortionDUPreGainParamString(stageID, distortionUnitID);
        newDistortionUnitProcessorParams.preGain = *apvts.getRawParameterValue(paramString);

        //Drive WaveShaper
        paramString = getDistortionDUDriveParamString(stageID, distortionUnitID);
        newDistortionUnitProcessorParams.drive = *apvts.getRawParameterValue(paramString);

        //Warp WaveShaper
        paramString = getDistortionDUWarpParamString(stageID, distortionUnitID);
        newDistortionUnitProcessorParams.warp = *apvts.getRawParameterValue(paramString);

        //PostGain WaveShaper
        paramString = getDistortionDUPostGainParamString(stageID, distortionUnitID);
        newDistortionUnitProcessorParams.postGain = *apvts.getRawParameterValue(paramString);

        //Mix WaveShaper
        paramString = getDistortionDUMixParamString(stageID, distortionUnitID);
        newDistortionUnitProcessorParams.mix = *apvts.getRawParameterValue(paramString);

        //OverSampling
        newDistortionUnitProcessorParams.oversampling = newOverSampling;

        //DistortionUnit ID
        newDistortionUnitProcessorParams.distortionUnitID = distortionUnitID;

        //if (&newDistortionUnitProcessorParams != &(stageProcessor->getDistortionProcessor()->getDistortionUnitParams(distortionUnit)))
        {
            pDistortionDU->setDistortionUnitParams(newDistortionUnitProcessorParams);
        }

        SampleRemapper<float>* pSampleRemapper = pDistortionDU->getSampleRemapper();

        int nbOfPoints = mainLayerDataStruct.getNbOfPoints(stageID, distortionUnitID);
        pSampleRemapper->setNbOfActiveBins(nbOfPoints - 1 + 2);

        juce::Point<float> leftPoint = mainLayerDataStruct.getPoint(stageID, distortionUnitID, 0);
        juce::Point<float> rightPoint(0.0f, 0.0f);
        float tension = mainLayerDataStruct.getTension(stageID, distortionUnitID, 0);
        int curveID = mainLayerDataStruct.getPointCurveType(stageID, distortionUnitID, 0);
        SampleRemapperBin<float>* pSRBin = nullptr;
       // bool isBipolar = mainLayerDataStruct.getIsBipolar(stageID, distortionUnitID);

        for (int binID = 0; binID < nbOfPoints - 1; ++binID)
        {
            pSRBin = pSampleRemapper->getBin(binID);
            leftPoint = mainLayerDataStruct.getPoint(stageID, distortionUnitID, binID + 1);
            rightPoint = mainLayerDataStruct.getPoint(stageID, distortionUnitID, binID);
            tension = mainLayerDataStruct.getTension(stageID, distortionUnitID, binID);
            curveID = mainLayerDataStruct.getPointCurveType(stageID, distortionUnitID, binID);

            //pSampleRemapper->setWaveShaperPoint(binID, leftPoint, rightPoint, tension, curveType);

            pSRBin->setBinParam(leftPoint, rightPoint, tension, curveID);
        }

        pDistortionDU->computeDistortionEQAPathData();

        //pDistortionDU->getDCFilter()->reset();
    }//Disto Unit loop
}
