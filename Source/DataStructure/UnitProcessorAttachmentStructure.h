/*
  ==============================================================================

    UnitProcessorAttachmentStructure.h
    Created: 2 Jun 2021 8:32:28pm
    Author:  CookiMonstor

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
//#include "../Processor/UnitProcessor/EQProcessor.h"
//#include "../Processor/UnitProcessor/DistortionProcessor.h"
//#include "../Processor/LayerProcessor/MainLayerProcessor.h"
#include "MangledIdentifiers.h"
struct FilterAttachment
{
    std::unique_ptr<SliderAttachment> cutoff;
    std::unique_ptr<SliderAttachment> q;
    std::unique_ptr<SliderAttachment> gain;
    std::unique_ptr<ComboBoxAttachment> type;
    std::unique_ptr<ComboBoxAttachment> order;
    std::unique_ptr<ButtonAttachment> onOff;
    std::unique_ptr<ButtonAttachment> isActive;
};

struct PhaserAttachment
{
    std::unique_ptr<SliderAttachment> centreFrequency;
    std::unique_ptr<SliderAttachment> feedback;
    std::unique_ptr<SliderAttachment> rate;
    std::unique_ptr<SliderAttachment> depth;
    std::unique_ptr<SliderAttachment> mix;
    std::unique_ptr<ComboBoxAttachment> numStages;
    std::unique_ptr<ButtonAttachment> isBypassed;
};

struct DistortionUnitAttachment
{
    std::unique_ptr<ButtonAttachment> onOff;
    //std::unique_ptr<ComboBoxAttachment> sigmoidEQA;
    //std::unique_ptr<ComboBoxAttachment> symetricEQA;
    std::unique_ptr<ButtonAttachment> distortionProcessorFirst;
    //std::unique_ptr<ButtonAttachment> isBipolar;
    //std::unique_ptr<ButtonAttachment> hybridModeOnOff;
    std::unique_ptr<ButtonAttachment> dcFilterOnOff;
    std::unique_ptr<SliderAttachment> preGain;
    std::unique_ptr<SliderAttachment> drive;
    std::unique_ptr<SliderAttachment> warp;
    std::unique_ptr<SliderAttachment> postGain;
    std::unique_ptr<SliderAttachment> mix;
};

struct DistortionAttachment
{
    std::unique_ptr<ButtonAttachment> oversampling;
    std::unique_ptr<SliderAttachment> mix;
    std::unique_ptr<ButtonAttachment> onOff;

    DistortionUnitAttachment distortionUnit[DistortionConstants::Processor<float>::nbOfDUMax];
};
//typedef EQAttachment juce::OwnedArray<FilterAttachment>;

struct EQAttachment
{
    FilterAttachment filter[EQConstants::Processor<float>::nbOfFilterMax];//static_cast<int>(EQProcessor<float>::FilterID::maxFilterID)
    std::unique_ptr<SliderAttachment> mix;
    std::unique_ptr<ButtonAttachment> isBypassed;
};


struct ScopeAttachment
{
    std::unique_ptr<ComboBoxAttachment> mode;
    std::unique_ptr<ButtonAttachment> preEQIsBypassed;
    std::unique_ptr<ButtonAttachment> postEQIsBypassed;
    std::unique_ptr<ButtonAttachment> postDistoIsBypassed;
    std::unique_ptr<ButtonAttachment> isNormalized;
    std::unique_ptr<ButtonAttachment> monoViewIsBypassed;
    std::unique_ptr<ButtonAttachment> subViewIsBypassed;
    std::unique_ptr<SliderAttachment> subViewCutoff;
};

struct StageAttachment
{
    std::unique_ptr<ButtonAttachment> isBypassed;
    std::unique_ptr<SliderAttachment> inputGain;
    std::unique_ptr<SliderAttachment> outputGain;
    ScopeAttachment scope;
    EQAttachment eq;
    PhaserAttachment phaser;
    DistortionAttachment distortion;
};

struct MainLayerAttachment
{
    StageAttachment stage[MainLayerConstants::Processor<float>::nbOfStageMax];
};

struct AudioEngineAttachment
{
    std::unique_ptr<ButtonAttachment> masterLimiterOnOff;
    std::unique_ptr<SliderAttachment> masterGain;
};