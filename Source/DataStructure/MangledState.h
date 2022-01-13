/*
  ==============================================================================

    MangledAPVTS.h
    Created: 13 Jan 2022 8:39:04am
    Author:  CookiMonstor

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "MangledIdentifiers.h"
#include "../Processor/UnitProcessor/EQProcessor.h"

template<typename SampleType>
class MangledState : juce::ValueTree::Listener
{
public:
    MangledState(juce::AudioProcessor& processorToConnectTo,
        juce::UndoManager* pUndoManager,
        const juce::Identifier& name,
        int nbOfStages,
        int nbOfFiltersEQ,
        int nbOfDistortionUnit)
        : apvts(processorToConnectTo, pUndoManager, name, createParametersLayout(nbOfStages, nbOfFiltersEQ, nbOfDistortionUnit))
        , mainLayerState(apvts)
    {
    };
    
    ~MangledState() {};

    class ScopeState
    {
    public:
        ScopeState(juce::AudioProcessorValueTreeState& mangledAPVTS, int stgID)
            : apvts(mangledAPVTS)
            , stageID(stgID)
        {};
        ~ScopeState() {};
        inline int getDataType()
        {
            return (int)*apvts.getRawParameterValue(ScopeConstants::ParamStringID::GetParamStringID::dataType(stageID));
        };
        inline bool getPreEQIsBypassed()
        {
            return *apvts.getRawParameterValue(ScopeConstants::ParamStringID::GetParamStringID::preEQIsBypassed(stageID));
        };
        inline bool getPostEQIsBypassed()
        {
            return *apvts.getRawParameterValue(ScopeConstants::ParamStringID::GetParamStringID::postEQIsBypassed(stageID));
        };
        inline bool getPostDistortionIsBypassed()
        {
            return *apvts.getRawParameterValue(ScopeConstants::ParamStringID::GetParamStringID::postDistoIsBypassed(stageID));
        };
        inline bool getIsNormalized()
        {
            return *apvts.getRawParameterValue(ScopeConstants::ParamStringID::GetParamStringID::isNormalized(stageID));
        };
        inline bool getSubViewIsBypassed()
        {
            return *apvts.getRawParameterValue(ScopeConstants::ParamStringID::GetParamStringID::subViewIsBypassed(stageID));
        };
        inline bool getMonoViewIsBypassed()
        {
            return *apvts.getRawParameterValue(ScopeConstants::ParamStringID::GetParamStringID::monoViewIsBypassed(stageID));
        };
        inline bool getSubViewCutoff()
        {
            return *apvts.getRawParameterValue(ScopeConstants::ParamStringID::GetParamStringID::subViewCutoff(stageID));
        };

        static void createRMSParametersLayout(std::vector<std::unique_ptr<juce::RangedAudioParameter>>* plugInParameters, int stageID)
        {
            juce::NormalisableRange<SampleType> normalisableCutoffRange{ RMSConstants::Processor<SampleType>::subViewCutoffMin,
                                                RMSConstants::Processor<SampleType>::subViewCutoffMax,
                                                 RMSConstants::Processor<SampleType>::subViewCutoffIncrement,
                                                  RMSConstants::Processor<SampleType>::subViewCutoffStartValue }; //20 000Hz to avoid jassert in IIR filter at 44100Hz sr.
            normalisableCutoffRange.setSkewForCentre(RMSConstants::Processor<SampleType>::subViewCutoffStartValue);

            juce::String paramString = RMSConstants::ParamStringID::GetParamStringID::isNormalized(stageID);
            juce::String automationParamString = RMSConstants::AutomationString::GetString::isNormalized(stageID);
            plugInParameters->push_back(std::make_unique<juce::AudioParameterBool>(paramString,
                automationParamString,
                RMSConstants::Processor<SampleType>::isNormalizedStartValue));

            paramString = RMSConstants::ParamStringID::GetParamStringID::subViewIsBypassed(stageID);
            automationParamString = RMSConstants::AutomationString::GetString::subViewIsBypassed(stageID);
            plugInParameters->push_back(std::make_unique<juce::AudioParameterBool>(paramString,
                automationParamString,
                RMSConstants::Processor<SampleType>::subViewIsBypassedStartValue));

            paramString = RMSConstants::ParamStringID::GetParamStringID::subViewCutoff(stageID);
            automationParamString = RMSConstants::AutomationString::GetString::subViewCutoff(stageID);
            plugInParameters->push_back(std::make_unique<juce::AudioParameterFloat>(paramString,
                automationParamString,
                normalisableCutoffRange,
                RMSConstants::Processor<SampleType>::subViewCutoffStartValue));

            paramString = RMSConstants::ParamStringID::GetParamStringID::monoViewIsBypassed(stageID);
            automationParamString = RMSConstants::AutomationString::GetString::monoViewIsBypassed(stageID);
            plugInParameters->push_back(std::make_unique<juce::AudioParameterBool>(paramString,
                automationParamString,
                RMSConstants::Processor<SampleType>::monoViewIsBypassedStartValue));
        };

        static void createScopeParametersLayout(std::vector<std::unique_ptr<juce::RangedAudioParameter>>* plugInParameters, int stageID)
        {
            //Data Type
            juce::String paramString = ScopeConstants::ParamStringID::GetParamStringID::dataType(stageID);
            juce::String automationParamString = ScopeConstants::AutomationString::GetString::dataType(stageID);
            plugInParameters->push_back(std::make_unique<juce::AudioParameterChoice>(paramString,
                automationParamString,
                ScopeConstants::UI::modeStringArray,
                ScopeConstants::Processor<SampleType>::modeStartValue));

            paramString = ScopeConstants::ParamStringID::GetParamStringID::preEQIsBypassed(stageID);
            automationParamString = ScopeConstants::AutomationString::GetString::preEQIsBypassed(stageID);
            plugInParameters->push_back(std::make_unique<juce::AudioParameterBool>(paramString,
                automationParamString,
                ScopeConstants::Processor<SampleType>::preEQIsBypassedStartValue));

            paramString = ScopeConstants::ParamStringID::GetParamStringID::postEQIsBypassed(stageID);
            automationParamString = ScopeConstants::AutomationString::GetString::postEQIsBypassed(stageID);
            plugInParameters->push_back(std::make_unique<juce::AudioParameterBool>(paramString,
                automationParamString,
                ScopeConstants::Processor<SampleType>::postEQIsBypassedStartValue));

            paramString = ScopeConstants::ParamStringID::GetParamStringID::postDistoIsBypassed(stageID);
            automationParamString = ScopeConstants::AutomationString::GetString::postDistoIsBypassed(stageID);
            plugInParameters->push_back(std::make_unique<juce::AudioParameterBool>(paramString,
                automationParamString,
                ScopeConstants::Processor<SampleType>::postDistoIsBypassedStartValue));


        };

        static void createScopeValueTree(juce::ValueTree& vt)
        {
            juce::ValueTree vtScope(ScopeConstants::ParamStringID::scope);
            vt.addChild(vtScope, -1, nullptr);

            vtScope.setProperty(ScopeConstants::ParamStringID::zoom, -1, nullptr);
            //vtScope.setProperty(ScopeConstants::ParamStringID::scope::preEQIsBypassed, false, nullptr);
            //vtScope.setProperty(ScopeConstants::ParamStringID::scope::postEQIsBypassed, false, nullptr);
            //vtScope.setProperty(ScopeConstants::ParamStringID::scope::postDistoIsBypassed, false, nullptr);
        }

    private:
        juce::AudioProcessorValueTreeState& apvts;
        int stageID;
    };

    class EQState
    {
    public:
        EQState(juce::AudioProcessorValueTreeState& mangledAPVTS)
            : apvts(mangledAPVTS)
        {};
        ~EQState() {};

        static inline SampleType getFilterCutoff(int stageID, int filterID)
        {
            return *apvts.getRawParameterValue(EQConstants::ParamStringID::GetParamStringID::filterCutoff(stageID, filterID));
        };
        static inline SampleType getFilterQ(int stageID, int filterID)
        {
            return *apvts.getRawParameterValue(EQConstants::ParamStringID::GetParamStringID::filterQ(stageID, filterID));
        };
        static inline SampleType getFilterGain(int stageID, int filterID)
        {
            return *apvts.getRawParameterValue(EQConstants::ParamStringID::GetParamStringID::filterGain(stageID, filterID));
        };
        static inline int getFilterOrder(int stageID, int filterID)
        {
            return ((int) *apvts.getRawParameterValue(EQConstants::ParamStringID::GetParamStringID::filterOrder(stageID, filterID))) + 1;
        };
        static inline int getFilterType(int stageID, int filterID)
        {
            return ((int) *apvts.getRawParameterValue(EQConstants::ParamStringID::GetParamStringID::filterType(stageID, filterID))) + 1;
        };
        static inline bool getFilterIsBypassed(int stageID, int filterID)
        {
            return *apvts.getRawParameterValue(EQConstants::ParamStringID::GetParamStringID::filterIsBypassed(stageID, filterID));
        };
        static inline bool getFilterIsActive(int stageID, int filterID)
        {
            return *apvts.getRawParameterValue(EQConstants::ParamStringID::GetParamStringID::filterIsActive(stageID, filterID));
        };
        static inline bool getIsBypassed(int stageID)
        {
            return *apvts.getRawParameterValue(EQConstants::ParamStringID::GetParamStringID::isBypassed(stageID, filterID));
        };
        static inline SampleType getMix(int stageID)
        {
            return *apvts.getRawParameterValue(EQConstants::ParamStringID::GetParamStringID::mix(stageID, filterID));
        };

        static void createParametersLayout(std::vector<std::unique_ptr<juce::RangedAudioParameter>>* plugInParameters, int stageID, int nbOfFiltersPerEQ)
        {
            juce::NormalisableRange<SampleType> normalisableMixRange{ EQConstants::Processor<SampleType>::mixMin,
                                                                      EQConstants::Processor<SampleType>::mixMax,
                                                                      EQConstants::Processor<SampleType>::mixIncrement };

            juce::NormalisableRange<SampleType> normalisableFRange{ EQConstants::BiquadConstants<SampleType>::cutoffMin,
                                                            EQConstants::BiquadConstants<SampleType>::cutoffMax,
                                                             EQConstants::BiquadConstants<SampleType>::cutoffIncrement,
                                                              EQConstants::BiquadConstants<SampleType>::cutoffStartValue }; //20 000Hz to avoid jassert in IIR filter at 44100Hz sr.

            normalisableFRange.setSkewForCentre(EQConstants::BiquadConstants<SampleType>::cutoffStartValue);

            juce::NormalisableRange<SampleType> normalisableQRange{ EQConstants::BiquadConstants<SampleType>::qMin,
                                                                EQConstants::BiquadConstants<SampleType>::qMax,
                                                                  EQConstants::BiquadConstants<SampleType>::qIncrement,
                                                                   EQConstants::BiquadConstants<SampleType>::qStartValue };

            normalisableQRange.setSkewForCentre(EQConstants::BiquadConstants<SampleType>::qStartValue);

            //Mix EQ
            juce::String paramString = EQConstants::ParamStringID::GetParamStringID::mix(stageID);
            juce::String automationParamString = EQConstants::AutomationString::GetString::mix(stageID);
            plugInParameters->push_back(std::make_unique<juce::AudioParameterFloat>(paramString,
                automationParamString,
                normalisableMixRange,
                EQConstants::Processor<SampleType>::mixStartValue));

            //Bypass EQ
            paramString = EQConstants::ParamStringID::GetParamStringID::isBypassed(stageID);
            automationParamString = EQConstants::AutomationString::GetString::isBypassed(stageID);
            plugInParameters->push_back(std::make_unique<juce::AudioParameterBool>(paramString,
                automationParamString,
                EQConstants::Processor<SampleType>::isBypassedStartValue));

            for (int filterID = 0; filterID < nbOfFiltersPerEQ; ++filterID)
            {
                //Filter CutOff
                paramString = EQConstants::ParamStringID::GetParamStringID::filterCutoff(stageID, filterID);
                automationParamString = EQConstants::AutomationString::GetString::filterCutoff(stageID, filterID);
                plugInParameters->push_back(std::make_unique<juce::AudioParameterFloat>(paramString,
                    automationParamString,
                    normalisableFRange,
                    EQConstants::BiquadConstants<SampleType>::cutoffStartValue));

                //Filter Q
                paramString = EQConstants::ParamStringID::GetParamStringID::filterQ(stageID, filterID);
                automationParamString = EQConstants::AutomationString::GetString::filterQ(stageID, filterID);
                plugInParameters->push_back(std::make_unique<juce::AudioParameterFloat>(paramString,
                    automationParamString,
                    normalisableQRange,
                    EQConstants::BiquadConstants<SampleType>::qStartValue));

                //Filter Gain
                paramString = EQConstants::ParamStringID::GetParamStringID::filterGain(stageID, filterID);
                automationParamString = EQConstants::AutomationString::GetString::filterGain(stageID, filterID);
                plugInParameters->push_back(std::make_unique<juce::AudioParameterFloat>(paramString,
                    automationParamString,
                    EQConstants::BiquadConstants<SampleType>::gaindBMin,
                    EQConstants::BiquadConstants<SampleType>::gaindBMax,
                    EQConstants::BiquadConstants<SampleType>::gaindBStartValue));

                //Filter Type
                paramString = EQConstants::ParamStringID::GetParamStringID::filterType(stageID, filterID);
                automationParamString = EQConstants::AutomationString::GetString::filterType(stageID, filterID);
                plugInParameters->push_back(std::make_unique<juce::AudioParameterChoice>(paramString,
                    automationParamString,
                    EQConstants::UI::typeStringArray,
                    EQConstants::BiquadConstants<SampleType>::typeStartValue));

                //Filter Order
                paramString = EQConstants::ParamStringID::GetParamStringID::filterOrder(stageID, filterID);
                automationParamString = EQConstants::AutomationString::GetString::filterOrder(stageID, filterID);
                plugInParameters->push_back(std::make_unique<juce::AudioParameterChoice>(paramString,
                    automationParamString,
                    EQConstants::UI::orderStringArray,
                    EQConstants::BiquadConstants<SampleType>::orderStartValue));

                //On Off Filter
                paramString = EQConstants::ParamStringID::GetParamStringID::filterIsBypassed(stageID, filterID);
                automationParamString = EQConstants::AutomationString::GetString::filterIsBypassed(stageID, filterID);
                plugInParameters->push_back(std::make_unique<juce::AudioParameterBool>(paramString,
                    automationParamString,
                    EQConstants::BiquadConstants<SampleType>::isBypassedStartValue));

                //Is Active Filter
                paramString = EQConstants::ParamStringID::GetParamStringID::filterIsActive(stageID, filterID);
                automationParamString = EQConstants::AutomationString::GetString::filterIsActive(stageID, filterID);
                plugInParameters->push_back(std::make_unique<juce::AudioParameterBool>(paramString,
                    automationParamString,
                    !EQConstants::BiquadConstants<SampleType>::isActiveStartValue));
            }
        };

        static void createValueTree(juce::ValueTree& vt)
        {
            juce::ValueTree vtEQ(EQConstants::ParamStringID::eq);
            vt.addChild(vtEQ, -1, nullptr);

            vtEQ.setProperty(EQConstants::ParamStringID::selectedFilterID, -1, nullptr);
            vtEQ.setProperty(EQConstants::ParamStringID::nbOfActiveFilters, 0, nullptr);
        };


    private:
        juce::AudioProcessorValueTreeState& apvts;
    };

    class PhaserState
    {
    public:
        PhaserState(juce::AudioProcessorValueTreeState& mangledAPVTS)
            : apvts(mangledAPVTS)
        {};
        ~PhaserState() {};

        static inline SampleType getCentreFrequency(int stageID)
        {
            return *apvts.getRawParameterValue(PhaserConstants::ParamStringID::GetParamStringID::centerFrequency(stageID));
        };
        static inline int getNumberOfStages(int stageID)
        {
            return *apvts.getRawParameterValue(PhaserConstants::ParamStringID::GetParamStringID::nbOfStages(stageID));
        };
        static inline SampleType getDepth(int stageID)
        {
            return *apvts.getRawParameterValue(PhaserConstants::ParamStringID::GetParamStringID::depth(stageID));
        };
        static inline SampleType getRate(int stageID)
        {
            return *apvts.getRawParameterValue(PhaserConstants::ParamStringID::GetParamStringID::rate(stageID));
        };
        static inline SampleType getFeedback(int stageID)
        {
            return *apvts.getRawParameterValue(PhaserConstants::ParamStringID::GetParamStringID::feedback(stageID));
        };
        static inline SampleType getMix(int stageID)
        {
            return *apvts.getRawParameterValue(PhaserConstants::ParamStringID::GetParamStringID::mix(stageID));
        };
        static inline bool getIsBypassed(int stageID)
        {
            return *apvts.getRawParameterValue(PhaserConstants::ParamStringID::GetParamStringID::isBypassed(stageID));
        };

        static void createParametersLayout(std::vector<std::unique_ptr<juce::RangedAudioParameter>>* plugInParameters, int stageID)
        {
            juce::NormalisableRange<SampleType> normalisableMixRange{ PhaserConstants::Processor<SampleType>::mixMin,
                                                                        PhaserConstants::Processor<SampleType>::mixMax,
                                                                        PhaserConstants::Processor<SampleType>::mixStartValue };
            //juce::NormalisableRange<SampleType> normalisableMixRange{ 0.0f,
            //                                                    100.0f,
            //                                                    0.3f };
            juce::String paramString = PhaserConstants::ParamStringID::GetParamStringID::centerFrequency(stageID);
            juce::String automationParamString = PhaserConstants::AutomationString::GetString::centerFrequency(stageID);

            plugInParameters->push_back(std::make_unique<juce::AudioParameterFloat>(paramString,
                automationParamString,
                PhaserConstants::Processor<SampleType>::cfreqMin,
                PhaserConstants::Processor<SampleType>::cfreqMax,
                PhaserConstants::Processor<SampleType>::cfreqStartValue));

            paramString = PhaserConstants::ParamStringID::GetParamStringID::feedback(stageID);
            automationParamString = PhaserConstants::AutomationString::GetString::feedback(stageID);
            plugInParameters->push_back(std::make_unique<juce::AudioParameterFloat>(paramString,
                automationParamString,
                PhaserConstants::Processor<SampleType>::feedbackMin,
                PhaserConstants::Processor<SampleType>::feedbackMax,
                PhaserConstants::Processor<SampleType>::feedbackStartValue));

            paramString = PhaserConstants::ParamStringID::GetParamStringID::rate(stageID);
            automationParamString = PhaserConstants::AutomationString::GetString::rate(stageID);
            plugInParameters->push_back(std::make_unique<juce::AudioParameterFloat>(paramString,
                automationParamString,
                PhaserConstants::Processor<SampleType>::rateMin,
                PhaserConstants::Processor<SampleType>::rateMax,
                PhaserConstants::Processor<SampleType>::rateStartValue));

            paramString = PhaserConstants::ParamStringID::GetParamStringID::depth(stageID);
            automationParamString = PhaserConstants::AutomationString::GetString::depth(stageID);
            plugInParameters->push_back(std::make_unique<juce::AudioParameterFloat>(paramString,
                automationParamString,
                PhaserConstants::Processor<SampleType>::depthMin,
                PhaserConstants::Processor<SampleType>::depthMax,
                PhaserConstants::Processor<SampleType>::depthStartValue));

            paramString = PhaserConstants::ParamStringID::GetParamStringID::mix(stageID);
            automationParamString = PhaserConstants::AutomationString::GetString::mix(stageID);
            plugInParameters->push_back(std::make_unique<juce::AudioParameterFloat>(paramString,
                automationParamString,
                PhaserConstants::Processor<SampleType>::mixMin,
                PhaserConstants::Processor<SampleType>::mixMax,
                PhaserConstants::Processor<SampleType>::mixStartValue));

            paramString = PhaserConstants::ParamStringID::GetParamStringID::nbOfStages(stageID);
            automationParamString = PhaserConstants::AutomationString::GetString::nbOfStages(stageID);
            plugInParameters->push_back(std::make_unique<juce::AudioParameterChoice>(paramString,
                automationParamString,
                PhaserConstants::UI::nbOfStgStringArray,
                PhaserConstants::Processor<SampleType>::nbOfStgStartValue));

            paramString = PhaserConstants::ParamStringID::GetParamStringID::isBypassed(stageID);
            automationParamString = PhaserConstants::AutomationString::GetString::isBypassed(stageID);
            plugInParameters->push_back(std::make_unique<juce::AudioParameterBool>(paramString,
                automationParamString,
                PhaserConstants::Processor<SampleType>::isBypassedStartValue));

        };

    private:
        juce::AudioProcessorValueTreeState& apvts;
    };

    class DistortionState
    {
    public:
        DistortionState(juce::AudioProcessorValueTreeState& mangledAPVTS)
            : apvts(mangledAPVTS)
        {};
        ~DistortionState() {};

        static inline bool getOverSampling(int stageID)
        {
            return *apvts.getRawParameterValue(DistortionConstants::ParamStringID::GetParamStringID::overSampling(stageID));
        };
        static inline bool getIsBypassed(int stageID)
        {
            return *apvts.getRawParameterValue(DistortionConstants::ParamStringID::GetParamStringID::isBypassed(stageID));
        };
        static inline SampleType getMix(int stageID)
        {
            return *apvts.getRawParameterValue(DistortionConstants::ParamStringID::GetParamStringID::mix(stageID));
        };

        static void createParametersLayout(std::vector<std::unique_ptr<juce::RangedAudioParameter>>* plugInParameters, int stageID, int nbOfDistoUnitPerDisto)
        {
            juce::NormalisableRange<SampleType> normalisableMixRange{ DistortionConstants::Processor<SampleType>::mixMin,
                                                                DistortionConstants::Processor<SampleType>::mixMax,
                                                                DistortionConstants::Processor<SampleType>::mixIncrement };

            //OverSampling Distortion
            juce::String paramString = DistortionConstants::ParamStringID::GetParamStringID::overSampling(stageID);
            juce::String automationParamString = DistortionConstants::AutomationString::GetString::overSampling(stageID);
            plugInParameters->push_back(std::make_unique<juce::AudioParameterBool>(paramString,
                automationParamString,
                DistortionConstants::Processor<SampleType>::oversamplingStartValue));

            //Mix Distortion
            paramString = DistortionConstants::ParamStringID::GetParamStringID::mix(stageID);
            automationParamString = DistortionConstants::AutomationString::GetString::mix(stageID);
            plugInParameters->push_back(std::make_unique<juce::AudioParameterFloat>(paramString,
                automationParamString,
                normalisableMixRange,
                DistortionConstants::Processor<SampleType>::mixStartValue));

            //OnOff Distortion
            paramString = DistortionConstants::ParamStringID::GetParamStringID::isBypassed(stageID);
            automationParamString = DistortionConstants::AutomationString::GetString::isBypassed(stageID);
            plugInParameters->push_back(std::make_unique<juce::AudioParameterBool>(paramString,
                automationParamString,
                DistortionConstants::Processor<SampleType>::isBypassedStartValue));

            for (int distortionUnitID = 0; distortionUnitID < nbOfDistoUnitPerDisto; ++distortionUnitID)
            {
                DistortionUnitProcessor<SampleType>::createParametersLayout(plugInParameters, stageID, distortionUnitID);
            }
        };

        static void createValueTree(juce::ValueTree& vt, juce::UndoManager* undoManager)
        {
            //juce::Identifer id();
            juce::ValueTree vtDistortion(DistortionConstants::ParamStringID::distortion);
            vt.addChild(vtDistortion, -1, nullptr);

            vt.setProperty(DistoUnitConstants::ParamStringID::selectedDistoUnitID, 0, nullptr);

            for (int distortionUnit = 0; distortionUnit < 2; ++distortionUnit)
                DistortionUnitProcessor<SampleType>::createValueTree(vtDistortion, distortionUnit, undoManager);
        };

        class DistortionUnitState
        {
        public:
            DistortionUnitState(juce::AudioProcessorValueTreeState& mangledAPVTS)
                :apvts(mangledAPVTS)
                , distortionUnitState(mangledAPVTS)
                //, root(distortionUnitValueTreeRoot)
            {
                //create the Value tree for the SampleRemapper;
            };

            ~DistortionUnitState()
            {
            };
            
            static inline int getCircuitType(int stageID, int distortionUnitID)
            {
                return *apvts.getRawParameterValue(DistoUnitConstants::ParamStringID::GetParamStringID::equationType(stageID, distortionUnitID));
            };
            static inline int getSigmoidID(int stageID, int distortionUnitID)
            {
                return *apvts.getRawParameterValue(DistoUnitConstants::ParamStringID::GetParamStringID::sigmoidEQA(stageID, distortionUnitID));
            };
            static inline int getSymetricID(int stageID, int distortionUnitID)
            {
                return *apvts.getRawParameterValue(DistoUnitConstants::ParamStringID::GetParamStringID::symetricEQA(stageID, distortionUnitID));
            };
            static inline int getAsymetricID(int stageID, int distortionUnitID)
            {
                return *apvts.getRawParameterValue(DistoUnitConstants::ParamStringID::GetParamStringID::asymetricEQA(stageID, distortionUnitID));
            };
            static inline int getSpecialID(int stageID, int distortionUnitID)
            {
                return *apvts.getRawParameterValue(DistoUnitConstants::ParamStringID::GetParamStringID::specialEQA(stageID, distortionUnitID));
            };
            static inline SampleType getDCFilterIsBypassed(int stageID, int distortionUnitID)
            {
                return *apvts.getRawParameterValue(DistoUnitConstants::ParamStringID::GetParamStringID::dcFilterIsBypassed(stageID, distortionUnitID));
            };
            static inline SampleType getRouting(int stageID, int distortionUnitID)
            {
                return *apvts.getRawParameterValue(DistoUnitConstants::ParamStringID::GetParamStringID::routing(stageID, distortionUnitID));
            };
            static inline SampleType getPreGain(int stageID, int distortionUnitID)
            {
                return *apvts.getRawParameterValue(DistoUnitConstants::ParamStringID::GetParamStringID::preGain(stageID, distortionUnitID));
            };
            static inline SampleType getDrive(int stageID, int distortionUnitID)
            {
                return *apvts.getRawParameterValue(DistoUnitConstants::ParamStringID::GetParamStringID::drive(stageID, distortionUnitID));
            };
            static inline SampleType getPostGain(int stageID, int distortionUnitID)
            {
                return *apvts.getRawParameterValue(DistoUnitConstants::ParamStringID::GetParamStringID::postGain(stageID, distortionUnitID));
            };
            static inline SampleType getWarp(int stageID, int distortionUnitID)
            {
                return *apvts.getRawParameterValue(DistoUnitConstants::ParamStringID::GetParamStringID::warp(stageID, distortionUnitID));
            };
            static inline bool getIsBypassed(int stageID, int distortionUnitID)
            {
                return *apvts.getRawParameterValue(DistoUnitConstants::ParamStringID::GetParamStringID::isBypassed(stageID, distortionUnitID));
            };
            static inline SampleType getMix(int stageID, int distortionUnitID)
            {
                return *apvts.getRawParameterValue(DistoUnitConstants::ParamStringID::GetParamStringID::mix(stageID, distortionUnitID));
            };

            juce::ValueTree& getRoot() {return root};

            static void createParametersLayout(std::vector<std::unique_ptr<juce::RangedAudioParameter>>* plugInParameters, int stageID, int distortionUnitID)
            {
                juce::NormalisableRange<SampleType> normalisablePreGainRange{ DistoUnitConstants::Processor<SampleType>::preGaindBMin,
                                                              DistoUnitConstants::Processor<SampleType>::preGaindBMax,
                                                               DistoUnitConstants::Processor<SampleType>::preGaindBIncrement };

                juce::NormalisableRange<SampleType> normalisableDriveRange{ DistoUnitConstants::Processor<SampleType>::driveMin,
                                                                        DistoUnitConstants::Processor<SampleType>::driveMax,
                                                                         DistoUnitConstants::Processor<SampleType>::driveIncrement };

                juce::NormalisableRange<SampleType> normalisableWarpRange{ DistoUnitConstants::Processor<SampleType>::warpMin,
                                                                    DistoUnitConstants::Processor<SampleType>::warpMax,
                                                                     DistoUnitConstants::Processor<SampleType>::warpIncrement };

                juce::NormalisableRange<SampleType> normalisablePostGainRange{ DistoUnitConstants::Processor<SampleType>::postGaindBMin,
                                                                           DistoUnitConstants::Processor<SampleType>::postGaindBMax,
                                                                            DistoUnitConstants::Processor<SampleType>::postGaindBIncrement };

                juce::NormalisableRange<SampleType> normalisableMixRange{ DistoUnitConstants::Processor<SampleType>::mixMin,
                                                            DistoUnitConstants::Processor<SampleType>::mixMax,
                                                            DistoUnitConstants::Processor<SampleType>::mixIncrement };

                bool duOnOffStartValue = distortionUnitID == 0 ? DistoUnitConstants::Processor<SampleType>::isBypassedStartValue
                    : !DistoUnitConstants::Processor<SampleType>::isBypassedStartValue;

                juce::String paramString = DistoUnitConstants::ParamStringID::GetParamStringID::isBypassed(stageID, distortionUnitID);
                juce::String automationParamString = DistoUnitConstants::AutomationString::GetString::isBypassed(stageID, distortionUnitID);
                plugInParameters->push_back(std::make_unique<juce::AudioParameterBool>(paramString,
                    automationParamString,
                    duOnOffStartValue));

                //Distortion Equation
                paramString = DistoUnitConstants::ParamStringID::GetParamStringID::routing(stageID, distortionUnitID);
                automationParamString = DistoUnitConstants::AutomationString::GetString::routing(stageID, distortionUnitID);
                plugInParameters->push_back(std::make_unique<juce::AudioParameterBool>(paramString,
                    automationParamString,
                    DistoUnitConstants::Processor<SampleType>::routingStartValue));

                paramString = DistoUnitConstants::ParamStringID::GetParamStringID::equationType(stageID, distortionUnitID);
                automationParamString = DistoUnitConstants::AutomationString::GetString::equationType(stageID, distortionUnitID);
                plugInParameters->push_back(std::make_unique<juce::AudioParameterChoice>(paramString,
                    automationParamString,
                    DistortionConstants::UI::equationTypeStringArray,
                    0));

                paramString = DistoUnitConstants::ParamStringID::GetParamStringID::sigmoidEQA(stageID, distortionUnitID);
                automationParamString = DistoUnitConstants::AutomationString::GetString::sigmoidEQA(stageID, distortionUnitID);
                plugInParameters->push_back(std::make_unique<juce::AudioParameterChoice>(paramString,
                    automationParamString,
                    DistortionConstants::UI::sigmoidEQAStringArray,
                    1));

                paramString = DistoUnitConstants::ParamStringID::GetParamStringID::symetricEQA(stageID, distortionUnitID);
                automationParamString = DistoUnitConstants::AutomationString::GetString::symetricEQA(stageID, distortionUnitID);
                plugInParameters->push_back(std::make_unique<juce::AudioParameterChoice>(paramString,
                    automationParamString,
                    DistortionConstants::UI::symetricEQAStringArray,
                    1));

                paramString = DistoUnitConstants::ParamStringID::GetParamStringID::asymetricEQA(stageID, distortionUnitID);
                automationParamString = DistoUnitConstants::AutomationString::GetString::asymetricEQA(stageID, distortionUnitID);
                plugInParameters->push_back(std::make_unique<juce::AudioParameterChoice>(paramString,
                    automationParamString,
                    DistortionConstants::UI::asymetricEQAStringArray,
                    1));

                paramString = DistoUnitConstants::ParamStringID::GetParamStringID::specialEQA(stageID, distortionUnitID);
                automationParamString = DistoUnitConstants::AutomationString::GetString::specialEQA(stageID, distortionUnitID);
                plugInParameters->push_back(std::make_unique<juce::AudioParameterChoice>(paramString,
                    automationParamString,
                    DistortionConstants::UI::specialEQAStringArray,
                    1));

                //DC Filter OnOff
                paramString = DistoUnitConstants::ParamStringID::GetParamStringID::dcFilterIsBypassed(stageID, distortionUnitID);
                automationParamString = DistoUnitConstants::AutomationString::GetString::dcFilterIsBypassed(stageID, distortionUnitID);
                plugInParameters->push_back(std::make_unique<juce::AudioParameterBool>(paramString,
                    automationParamString,
                    DistoUnitConstants::Processor<SampleType>::dcFilterIsBypassedStartValue));

                //PreGain WaveShaper
                paramString = DistoUnitConstants::ParamStringID::GetParamStringID::preGain(stageID, distortionUnitID);
                automationParamString = DistoUnitConstants::AutomationString::GetString::preGain(stageID, distortionUnitID);
                plugInParameters->push_back(std::make_unique<juce::AudioParameterFloat>(paramString,
                    automationParamString,
                    normalisablePreGainRange,
                    DistoUnitConstants::Processor<SampleType>::preGaindBStartValue));

                //Drive WaveShaper
                paramString = DistoUnitConstants::ParamStringID::GetParamStringID::drive(stageID, distortionUnitID);
                automationParamString = DistoUnitConstants::AutomationString::GetString::drive(stageID, distortionUnitID);
                plugInParameters->push_back(std::make_unique<juce::AudioParameterFloat>(paramString,
                    automationParamString,
                    normalisableDriveRange,
                    DistoUnitConstants::Processor<SampleType>::driveStartValue));

                //Warp WaveShaper
                paramString = DistoUnitConstants::ParamStringID::GetParamStringID::warp(stageID, distortionUnitID);
                automationParamString = DistoUnitConstants::AutomationString::GetString::warp(stageID, distortionUnitID);
                plugInParameters->push_back(std::make_unique<juce::AudioParameterFloat>(paramString,
                    automationParamString,
                    normalisableWarpRange,
                    DistoUnitConstants::Processor<SampleType>::warpStartValue));

                //PostGain WaveShaper
                paramString = DistoUnitConstants::ParamStringID::GetParamStringID::postGain(stageID, distortionUnitID);
                automationParamString = DistoUnitConstants::AutomationString::GetString::postGain(stageID, distortionUnitID);
                plugInParameters->push_back(std::make_unique<juce::AudioParameterFloat>(paramString,
                    automationParamString,
                    normalisablePostGainRange,
                    DistoUnitConstants::Processor<SampleType>::postGaindBStartValue));

                //Mix WaveShaper
                paramString = DistoUnitConstants::ParamStringID::GetParamStringID::mix(stageID, distortionUnitID);
                automationParamString = DistoUnitConstants::AutomationString::GetString::mix(stageID, distortionUnitID);
                plugInParameters->push_back(std::make_unique<juce::AudioParameterFloat>(paramString,
                    automationParamString,
                    normalisableMixRange,
                    DistoUnitConstants::Processor<SampleType>::mixStartValue));
            };

            static void createValueTree(juce::ValueTree& vt, int distortionUnitID, juce::UndoManager* undoManager)
            {
                juce::Identifier id(DistoUnitConstants::ParamStringID::GetParamStringID::id(distortionUnitID));
                juce::ValueTree vtDistortionUnit(id);
                vt.addChild(vtDistortionUnit, -1, nullptr);

                SampleRemapper<SampleType>::createValueTree(vtDistortionUnit, undoManager);

                //DistortionCircuit<SampleType>::createValueTree(vtDistortionUnit);
            };

        private:
            juce::AudioProcessorValueTreeState& apvts;
            juce::ValueTree& root;
        };

    private:
        juce::AudioProcessorValueTreeState& apvts;
        DistortionUnitState distortionUnitState;
    };

    class StageState
    {
    public:
        StageState(juce::AudioProcessorValueTreeState& mangledAPVTS, int stgID)
            : apvts(mangledAPVTS)
            , scopeState(mangledAPVTS, stgID)
            , eqState(mangledAPVTS, stgID)
            , phaserState(mangledAPVTS, stgID)
            , stageID(stgID)
        {
        };
        ~StageState() {};

        inline bool getIsBypassed()
        {
            return *apvts.getRawParameterValue(StageConstants::ParamStringID::GetParamStringID::isBypassed(stageID));
        };
        inline SampleType getInputGain()
        {
            return *apvts.getRawParameterValue(StageConstants::ParamStringID::GetParamStringID::inputGain(stageID));
        };
        inline SampleType getOutputGain()
        {
            return *apvts.getRawParameterValue(StageConstants::ParamStringID::GetParamStringID::outputGain(stageID));
        };

        static void createParametersLayout(std::vector<std::unique_ptr<juce::RangedAudioParameter>>* plugInParameters, int stageID, int nbOfFiltersPerEQ, int nbOfDistoUnitPerDisto)
        {
            juce::NormalisableRange<SampleType> normalisableGainRange{ StageConstants::Processor<SampleType>::gainMin,
                                                          StageConstants::Processor<SampleType>::gainMax,
                                                           StageConstants::Processor<SampleType>::gaindBIncrement };

            //Stage OnOff
            juce::String paramString = StageConstants::ParamStringID::GetParamStringID::isBypassed(stageID);
            juce::String automationParamString = StageConstants::AutomationString::GetString::isBypassed(stageID);

            bool startValue = stageID == 0 ? StageConstants::Processor<SampleType>::isBypassedStartValue : !StageConstants::Processor<SampleType>::isBypassedStartValue;

            plugInParameters->push_back(std::make_unique<juce::AudioParameterBool>(paramString, automationParamString, startValue));

            paramString = StageConstants::ParamStringID::GetParamStringID::inputGain(stageID);
            automationParamString = StageConstants::AutomationString::GetString::inputGain(stageID);

            plugInParameters->push_back(std::make_unique<juce::AudioParameterFloat>(paramString,
                automationParamString,
                normalisableGainRange,
                StageConstants::Processor<SampleType>::gainStartValue));

            paramString = StageConstants::ParamStringID::GetParamStringID::outputGain(stageID);
            automationParamString = StageConstants::AutomationString::GetString::outputGain(stageID);

            plugInParameters->push_back(std::make_unique<juce::AudioParameterFloat>(paramString,
                automationParamString,
                normalisableGainRange,
                StageConstants::Processor<SampleType>::gainStartValue));

            //RMS=====================================================================================================
            RMSProcessor<SampleType>::createParametersLayout(plugInParameters, stageID);

            //Scope===================================================================================================
            ScopeProcessor<SampleType>::createParametersLayout(plugInParameters, stageID);

            //EQ======================================================================================================
            EQProcessor<SampleType>::createParametersLayout(plugInParameters, stageID, nbOfFiltersPerEQ);

            //Phaser==================================================================================================
            CustomPhaser<SampleType>::createParametersLayout(plugInParameters, stageID);

            //Distortion==============================================================================================
            DistortionProcessor<SampleType>::createParametersLayout(plugInParameters, stageID, nbOfDistoUnitPerDisto);
        };

        static void createValueTree(juce::ValueTree& vt, int stageID, juce::UndoManager* undoManager)
        {
            juce::Identifier id(MainLayerConstants::ParamStringID::stage + (juce::String)stageID);
            juce::ValueTree vtStage(id);
            vt.addChild(vtStage, -1, nullptr);

            vtStage.setProperty(StageConstants::ParamStringID::displayedProcessor, StageConstants::Processor<SampleType>::ProcessorID::distortion, nullptr);

            ScopeProcessor<SampleType>::createValueTree(vtStage);
            EQProcessor<SampleType>::createValueTree(vtStage);
            DistortionProcessor<SampleType>::createValueTree(vtStage, undoManager);
            //Need to Add Phaser.
        };

    private:
        juce::AudioProcessorValueTreeState& apvts;
        EQState eqState;
        PhaserState phaserState;
        DistortionState distortionState;
        int stageID;
    };

    class MainLayerState
    {
    public:
        MainLayerState(juce::AudioProcessorValueTreeState& apvts)
        {
            for (int stageID = 0; stageID < StageConstants::Processor<SampleType>::nbOfStagesMax; ++stageID)
            {
                stageState.add(new StageState(apvts, stageID));
            }
        };

        inline bool getIsBypassed()
        {
            return *apvts.getRawParameterValue(MainLayerConstants::ParamStringID::GetParamStringID::isBypassed());;
        };

        ~MainLayerState() {};

        static void createParametersLayout(std::vector<std::unique_ptr<juce::RangedAudioParameter>>* plugInParameters, int nbOfStages, int nbOfFiltersPerEQ, int nbOfDistoUnitPerDisto)
        {
            juce::String paramString = MainLayerConstants::ParamStringID::GetParamStringID::isBypassed();
            juce::String automationParamString = MainLayerConstants::AutomationString::GetString::isBypassed();

            plugInParameters->push_back(std::make_unique<juce::AudioParameterBool>(paramString,
                automationParamString,
                MainLayerConstants::Processor<float>::isBypassedStartValue));

            //Main Layer Stage Attachement========================================================================================================================================================
            //paramString = getPostProcessingOnOffParamString();
            //automationParamString = getPostProcessingOnOffParamAutomationString();
            //plugInParameters->push_back(std::make_unique<juce::AudioParameterBool>(paramString,
            //    automationParamString,
            //    true));

            //Main Layer All Stage All Parameters Layout==========================================================================================================================================
            for (int stageID = 0; stageID < nbOfStages; ++stageID)
            {
                StageProcessorBase<SampleType>::createParametersLayout(plugInParameters, stageID, nbOfFiltersPerEQ, nbOfDistoUnitPerDisto);
            }
        };

        static void createValueTree(juce::ValueTree& vt, juce::UndoManager* undoManager)
        {
            //vt.removeAllChildren(nullptr);
            juce::ValueTree vtMainLayer(MainLayerConstants::ParamStringID::mainLayer);
            //vt.setProperty("IsOpening", true, nullptr);
            vt.addChild(vtMainLayer, -1, nullptr);

            vtMainLayer.setProperty(juce::Identifier("Name"), MainLayerConstants::ParamStringID::mainLayer, nullptr);
            //vt.setProperty(AudioEngineConstants,0, nullptr);
            vtMainLayer.setProperty(StageConstants::ParamStringID::selectedStageID, 0, nullptr);

            for (int stage = 0; stage < 4; ++stage)
            {
                StageProcessorBase<SampleType>::createValueTree(vtMainLayer, stage, undoManager);
            }

        }

    private:
        juce::AudioProcessorValueTreeState& apvts;
        juce::OwnedArray<StageState> stageState;
    };

    inline bool getMasterLimiterIsBypassed()
    {
        return *apvts.getRawParameterValue(AudioEngineConstants::ParamStringID::GetParamStringID::masterLimiterOnOff());
    }

    inline SampleType getMasterGain()
    {
        return *apvts.getRawParameterValue(AudioEngineConstants::ParamStringID::GetParamStringID::masterGain());
    }

    void valueTreePropertyChanged(juce::ValueTree& treeWhosePropertyHasChanged, const juce::Identifier& property) override;
    void valueTreeChildAdded(juce::ValueTree& parentTree, juce::ValueTree& childWhichHasBeenAdded) override;
    void valueTreeChildRemoved(juce::ValueTree& parentTree, juce::ValueTree& childWhichHasBeenRemoved, int indexFromWhichChildWasRemoved) override;
    void valueTreeChildOrderChanged(juce::ValueTree& parentTreeWhoseChildrenHaveMoved, int oldIndex, int newIndex) override;
    void valueTreeParentChanged(juce::ValueTree& treeWhoseParentHasChanged) override;
    void valueTreeRedirected(juce::ValueTree& treeWhichHasBeenChanged) override;


    juce::AudioProcessorValueTreeState& getAPVTS() { return apvts; };

private:
    juce::AudioProcessorValueTreeState::ParameterLayout createParametersLayout(int nbOfStages, int nbOfFiltersPerEQ, int nbOfDistoUnitPerDisto)
    {
        std::vector<std::unique_ptr<juce::RangedAudioParameter>> parameters;

        //Audio Engine Master Gain=============================================================================================================================================================
        juce::NormalisableRange<SampleType> normalisableMasterGainRange{ AudioEngineConstants::Processor<SampleType>::masterGainMin,
                                                  AudioEngineConstants::Processor<SampleType>::masterGainMax,
                                                   AudioEngineConstants::Processor<SampleType>::masterGaindBIncrement };

        juce::String paramString = AudioEngineConstants::ParamStringID::GetParamStringID::masterGain();
        juce::String automationParamString = AudioEngineConstants::AutomationString::GetString::masterGain();

        plugInParameters->push_back(std::make_unique<juce::AudioParameterFloat>(paramString,
            automationParamString,
            normalisableMasterGainRange,
            AudioEngineConstants::Processor<SampleType>::masterGainStartValue));

        paramString = AudioEngineConstants::ParamStringID::GetParamStringID::masterLimiterOnOff();
        automationParamString = AudioEngineConstants::AutomationString::GetString::masterLimiterOnOff();

        plugInParameters->push_back(std::make_unique<juce::AudioParameterBool>(paramString,
            automationParamString,
            AudioEngineConstants::Processor<SampleType>::masterLimiterIsBypassedStartValue));

        MainLayerProcessor<SampleType>::createParametersLayout(plugInParameters, nbOfStages, nbOfFiltersPerEQ, nbOfDistoUnitPerDisto);

        return { parameters.begin(), parameters.end() };
    };

    juce::AudioProcessorValueTreeState apvts;
    MainLayerState mainLayerState;

    juce::AudioProcessorValueTreeState apvtsMainLayer;
    juce::ValueTree rootAudioEngine;

    std::atomic<bool> shouldUpdate;
};