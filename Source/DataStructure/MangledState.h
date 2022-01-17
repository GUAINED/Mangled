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
//#include "../Processor/UnitProcessor/EQProcessor.h"

template<typename SampleType>
class AudioEngineState : public juce::ValueTree::Listener
                       //, public AudioEngineState<SampleType>::EQState
                       //, public AudioEngineState<SampleType>::DistortionState
{
public:
    AudioEngineState(juce::AudioProcessor& processorToConnectTo,
        const juce::Identifier& name,
        int nbOfStages,
        int nbOfFiltersEQ,
        int nbOfDistortionUnit)
        : apvts(processorToConnectTo, &undoManager, name, createParametersLayout(nbOfStages, nbOfFiltersEQ, nbOfDistortionUnit))
        //, mainLayerState(apvts)
        //, stageState(apvts)
        //, fftState(apvts, &undoManager)
        //, eqState(apvts, &undoManager, this)
        //, phaserState(apvts)
        //, distortionState(apvts)
        , rootAudioEngine(AudioEngineConstants::ParamStringID::audioEngine)
    {
        rootAudioEngine.setProperty(AudioEngineConstants::ParamStringID::selectedState,
            AudioEngineConstants::Processor<float>::StateID::A,
            nullptr);
        MainLayerProcessor<SampleType>::createValueTree(rootAudioEngine, &undoManager);

        //apvts.state.addChild(rootMainLayer, 0, nullptr);
        apvts.state.addListener(this);
        rootAudioEngine.addListener(this);
        undoManager.clearUndoHistory();
        undoManager.beginNewTransaction();
    };
    
    ~AudioEngineState() {};

    class TemporalState
    {
    public:
        TemporalState(juce::AudioProcessorValueTreeState& mangledAPVTS)
            : apvts(mangledAPVTS)
        {
        };

        ~TemporalState() {};

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

        static void createParametersLayout(std::vector<std::unique_ptr<juce::RangedAudioParameter>>* plugInParameters, int stageID)
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

    private:
        juce::AudioProcessorValueTreeState& apvts;
    };

    class FFTState
    {
    public:
        FFTState(juce::AudioProcessorValueTreeState& mangledAPVTS, juce::UndoManager* undoManager)
            : apvts(mangledAPVTS)
            , pUndoManager(undoManager)
            
        {};
        ~FFTState() {};
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

        static void createParametersLayout(std::vector<std::unique_ptr<juce::RangedAudioParameter>>* plugInParameters, int stageID)
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

        static void createValueTree(juce::ValueTree& vt)
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
        juce::UndoManager* pUndoManager;
    };

    class EQState
    {
    public:
        EQState(juce::AudioProcessorValueTreeState& mangledAPVTS, juce::UndoManager* undoManager, AudioEngineState<SampleType>* listener)
            : apvts(mangledAPVTS)
            , pUndoManager(undoManager)
            , pListener()
        {};
        ~EQState() {};
        
        void resetEQParam(int stageID)
        {
            //Distortion Mix
            juce::String paramString = EQConstants::ParamStringID::GetParamStringID::mix(stageID);
            juce::Value apvtsValue = apvts.getParameterAsValue(paramString);
            apvtsValue = EQConstants::Processor<SampleType>::mixStartValue;

            //Distortion isBypassed
            paramString = EQConstants::ParamStringID::GetParamStringID::isBypassed(stageID);
            apvtsValue = apvts.getParameterAsValue(paramString);
            apvtsValue = EQConstants::Processor<SampleType>::isBypassedStartValue;

            for (int filterID = 0; filterID < EQConstants::Processor<SampleType>::nbOfFilterMax; ++filterID)
            {
                paramString = EQConstants::ParamStringID::GetParamStringID::filterCutoff(stageID, filterID);
                apvtsValue = apvts.getParameterAsValue(paramString);
                apvtsValue = EQConstants::BiquadConstants<SampleType>::cutoffStartValue;

                paramString = EQConstants::ParamStringID::GetParamStringID::filterQ(stageID, filterID);
                apvtsValue = apvts.getParameterAsValue(paramString);
                apvtsValue = EQConstants::BiquadConstants<SampleType>::qStartValue;

                paramString = EQConstants::ParamStringID::GetParamStringID::filterGain(stageID, filterID);
                apvtsValue = apvts.getParameterAsValue(paramString);
                apvtsValue = EQConstants::BiquadConstants<SampleType>::gainLinStartValue;

                paramString = EQConstants::ParamStringID::GetParamStringID::filterType(stageID, filterID);
                apvtsValue = apvts.getParameterAsValue(paramString);
                apvtsValue = EQConstants::BiquadConstants<SampleType>::typeStartValue;

                paramString = EQConstants::ParamStringID::GetParamStringID::filterOrder(stageID, filterID);
                apvtsValue = apvts.getParameterAsValue(paramString);
                apvtsValue = EQConstants::BiquadConstants<SampleType>::orderStartValue;

                paramString = EQConstants::ParamStringID::GetParamStringID::filterIsBypassed(stageID, filterID);
                apvtsValue = apvts.getParameterAsValue(paramString);
                apvtsValue = EQConstants::BiquadConstants<SampleType>::isBypassedStartValue;

                paramString = EQConstants::ParamStringID::GetParamStringID::filterIsActive(stageID, filterID);
                apvtsValue = apvts.getParameterAsValue(paramString);
                apvtsValue = ! EQConstants::BiquadConstants<SampleType>::isActiveStartValue;
            }

            //setSelectedFilterID(stageID, -1);
        };


        void setEQFilterCutOff(int stageID, int filterID, float newCutoff)
        {
            juce::String paramString = EQConstants::ParamStringID::GetParamStringID::filterCutoff(stageID, filterID);
            juce::Value cutoffValue = apvts.getParameterAsValue(paramString);

            cutoffValue = newCutoff;
        }
        void setEQFilterQ(int stageID, int filterID, float newQ)
        {
            juce::String paramString = EQConstants::ParamStringID::GetParamStringID::filterQ(stageID, filterID);
            juce::Value qValue = apvts.getParameterAsValue(paramString);

            qValue = newQ;
        }
        void setEQFilterGain(int stageID, int filterID, float newGain)
        {
            juce::String paramString = EQConstants::ParamStringID::GetParamStringID::filterGain(stageID, filterID);
            juce::Value gainValue = apvts.getParameterAsValue(paramString);

            gainValue = newGain;
        }
        void setEQFilterType(int stageID, int filterID, int filterType)
        {
            juce::String paramString = EQConstants::ParamStringID::GetParamStringID::filterType(stageID, filterID);
            juce::Value filterTypeValue = apvts.getParameterAsValue(paramString);

            filterTypeValue = filterType;
        }
        void setEQFilterIsBypassed(int stageID, int filterID, bool newIsBypassed)
        {
            juce::String paramString = EQConstants::ParamStringID::GetParamStringID::filterIsBypassed(stageID, filterID);
            juce::Value isBypassedValue = apvts.getParameterAsValue(paramString);

            isBypassedValue = newIsBypassed;
        }
        void setEQFilterIsActive(int stageID, int filterID, bool newIsActive)
        {
            juce::String paramString = EQConstants::ParamStringID::GetParamStringID::filterIsActive(stageID, filterID);
            juce::Value isActiveValue = apvts.getParameterAsValue(paramString);

            isActiveValue = newIsActive;
        }
        void setSelectedFilterID(int stageID, int filterID)
        {
            pUndoManager->beginNewTransaction();
            getEQVT(stageID).setPropertyExcludingListener(pListener, EQConstants::ParamStringID::selectedFilterID, filterID, pUndoManager);
        }
        void setMouseAddFilterEQ(const juce::MouseEvent& e, int filterID, float scopeWidth, float scopeHeight)
        {
            if (filterID == -1)
                return;

            int stageID = getSelectedStageID();

            int xPos = e.getPosition().getX();
            int yPos = e.getPosition().getY();

            // FQG VALUE
            float xPosRemap = juce::jmap((float)xPos, 0.0f, scopeWidth - 100.0f, 0.0f, 1.0f);
            float filterCutoff = juce::mapToLog10(xPosRemap, 10.0f, 22050.0f);

            float gainMin = 0.0f;
            float gainMax = 0.0f;

            switch (getScopeZoom(stageID))
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

            float filterGain = juce::jmap((float)yPos, 0.0f, scopeHeight, gainMax, gainMin);

            //undoManager.beginNewTransaction();
            setSelectedFilterID(stageID, filterID);
            setEQFilterCutOff(stageID, filterID, filterCutoff);
            setEQFilterGain(stageID, filterID, filterGain);
            setEQFilterIsBypassed(stageID, filterID, false);
            setEQFilterIsActive(stageID, filterID, false);
            addActiveFilterCount(stageID);
            //setEQFilterQ();
        }
        int findNextAvailableFilterIDEQ(int stageID)
        {
            int filterID = 0;
            juce::String paramString = EQConstants::ParamStringID::GetParamStringID::filterIsActive(stageID, filterID);
            bool isActive = !(*apvts.getRawParameterValue(paramString));

            while (isActive && (filterID < EQConstants::Processor<float>::nbOfFilterMax - 1))
            {
                filterID++;
                paramString = EQConstants::ParamStringID::GetParamStringID::filterIsActive(stageID, filterID);
                isActive = !(*apvts.getRawParameterValue(paramString));
            }

            //if (filterID == EQConstants::Processor<float>::nbOfFilterMax)
            if (filterID == EQConstants::Processor<float>::nbOfFilterMax - 1 && isActive)
            {
                filterID = -1;
            }

            return filterID;
        }
        int findNextActiveFilterDown(int stageID)
        {
            if (getNbOfActiveFilters(stageID) == 0)
                return -1;

            int filterID = getSelectedFilterID(stageID) - 1;
            juce::String paramString;
            bool isActive = true;
            //int count = 0;
            if (filterID < 0)
                filterID = EQConstants::Processor<float>::nbOfFilterMax - 1;

            paramString = EQConstants::ParamStringID::GetParamStringID::filterIsActive(stageID, filterID);
            isActive = (*apvts.getRawParameterValue(paramString));

            while (isActive && (filterID > -1))// EQConstants::Processor<float>::nbOfFilterMax)
            {
                //count++;
                filterID--;
                filterID = (filterID + EQConstants::Processor<float>::nbOfFilterMax - 1) % (EQConstants::Processor<float>::nbOfFilterMax - 1);
                paramString = EQConstants::ParamStringID::GetParamStringID::filterIsActive(stageID, filterID);
                isActive = (*apvts.getRawParameterValue(paramString));
            }

            return filterID;
        }
        int findNextActiveFilterUp(int stageID)
        {
            if (getNbOfActiveFilters(stageID) == 0)
                return -1;

            int filterID = getSelectedFilterID(stageID) + 1;
            juce::String paramString;
            bool isActive = true;
            if (filterID > EQConstants::Processor<float>::nbOfFilterMax - 1)
                filterID = 0;

            paramString = EQConstants::ParamStringID::GetParamStringID::filterIsActive(stageID, filterID);
            isActive = (*apvts.getRawParameterValue(paramString));

            while (isActive && (filterID < EQConstants::Processor<float>::nbOfFilterMax))
            {
                filterID++;
                filterID = filterID % (EQConstants::Processor<float>::nbOfFilterMax - 1);
                paramString = EQConstants::ParamStringID::GetParamStringID::filterIsActive(stageID, filterID);
                isActive = (*apvts.getRawParameterValue(paramString));
            }

            return filterID;
        }
        void addActiveFilterCount(int stageID)
        {
            int yolo = getNbOfActiveFilters(stageID);
            int nbOfActiveFilters = juce::jlimit(0, EQConstants::Processor<float>::nbOfFilterMax - 1, getNbOfActiveFilters(stageID) + 1);
            getEQVT(stageID).setPropertyExcludingListener(pListener, EQConstants::ParamStringID::nbOfActiveFilters, nbOfActiveFilters, pUndoManager);
        }
        void remodeActiveFilterCount(int stageID)
        {
            int nbOfActiveFilters = juce::jlimit(0, EQConstants::Processor<float>::nbOfFilterMax - 1, getNbOfActiveFilters(stageID) - 1);
            getEQVT(stageID).setPropertyExcludingListener(pListener, EQConstants::ParamStringID::nbOfActiveFilters, nbOfActiveFilters, pUndoManager);
        }
        void setMouseDragEventEQ(const juce::MouseEvent& e, float scopeWidth, float scopeHeight)
        {
            int stageID = getSelectedStageID();
            int filterID = getSelectedFilterID(stageID);

            if (filterID == -1)
                return;

            float xPos = static_cast<float>(e.getPosition().getX());
            float yPos = static_cast<float>(e.getPosition().getY());
            yPos = juce::jlimit(0.0f + scopeHeight / 14.0f, scopeHeight - scopeHeight / 14.0f, yPos);
            float filterCutoff = Conversion<float>::fromMouseToCutoff(xPos, 0.0f, scopeWidth - 100.0f, 10.0f, 22050.0f);
            setEQFilterCutOff(stageID, filterID, filterCutoff);

            int filterType = getEQFilterType(stageID, filterID);

            if (filterType == EQConstants::BiquadConstants<float>::Types::peak || filterType == EQConstants::BiquadConstants<float>::Types::lowshelf || filterType == EQConstants::BiquadConstants<float>::Types::highshelf)
            {

                float gainMin = 0.0f;
                float gainMax = 0.0f;

                switch (getScopeZoom(stageID))
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

                float filterGain = Conversion<float>::fromMouseToGain(yPos, scopeHeight, 0.0f, gainMin, gainMax);
                setEQFilterGain(stageID, filterID, filterGain);
            }
            else
            {
                float qMin = 0.0f;
                float qMax = 0.0f;

                switch (getScopeZoom(stageID))
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

                float filterQ = Conversion<float>::fromMouseToQ(yPos, scopeHeight - scopeHeight / 14.0f, 0.0f + scopeHeight / 14.0f, qMin, qMax);
                setEQFilterQ(stageID, filterID, filterQ);
            }
        }
        void setNbOfActiveFilter(int stageID, int nbOfActiveFilters)
        {
            getEQVT(stageID).setPropertyExcludingListener(pListener, EQConstants::ParamStringID::nbOfActiveFilters, nbOfActiveFilters, pUndoManager);
        }

        inline SampleType getFilterCutoff(int stageID, int filterID)
        {
            return *apvts.getRawParameterValue(EQConstants::ParamStringID::GetParamStringID::filterCutoff(stageID, filterID));
        };
        inline SampleType getFilterQ(int stageID, int filterID)
        {
            return *apvts.getRawParameterValue(EQConstants::ParamStringID::GetParamStringID::filterQ(stageID, filterID));
        };
        inline SampleType getFilterGain(int stageID, int filterID)
        {
            return *apvts.getRawParameterValue(EQConstants::ParamStringID::GetParamStringID::filterGain(stageID, filterID));
        };
        inline int getFilterOrder(int stageID, int filterID)
        {
            return ((int) *apvts.getRawParameterValue(EQConstants::ParamStringID::GetParamStringID::filterOrder(stageID, filterID))) + 1;
        };
        inline int getFilterType(int stageID, int filterID)
        {
            return ((int) *apvts.getRawParameterValue(EQConstants::ParamStringID::GetParamStringID::filterType(stageID, filterID)));
        };
        inline bool getFilterIsBypassed(int stageID, int filterID)
        {
            return *apvts.getRawParameterValue(EQConstants::ParamStringID::GetParamStringID::filterIsBypassed(stageID, filterID));
        };
        inline bool getFilterIsActive(int stageID, int filterID)
        {
            return !(*apvts.getRawParameterValue(EQConstants::ParamStringID::GetParamStringID::filterIsActive(stageID, filterID)));
        };
        inline bool getIsBypassed(int stageID)
        {
            return *apvts.getRawParameterValue(EQConstants::ParamStringID::GetParamStringID::isBypassed(stageID, filterID));
        };
        inline SampleType getMix(int stageID)
        {
            return *apvts.getRawParameterValue(EQConstants::ParamStringID::GetParamStringID::mix(stageID, filterID));
        };

        int getSelectedFilterID(int stageID)
        {
            juce::ValueTree vtEQ = getEQVT(stageID);

            return vtEQ.getProperty(EQConstants::ParamStringID::selectedFilterID);
        }
        int getEQFilterType(int stageID, int filterID)
        {
            juce::String paramString = EQConstants::ParamStringID::GetParamStringID::filterType(stageID, filterID);
            int filterType = (int)*apvts.getRawParameterValue(paramString);

            return filterType;
        }
        float getEQFilterCutoff(int stageID, int filterID)
        {
            juce::String paramString = EQConstants::ParamStringID::GetParamStringID::filterCutoff(stageID, filterID);
            float filterCutoff = *apvts.getRawParameterValue(paramString);

            return filterCutoff;
        }
        float getEQFilterQ(int stageID, int filterID)
        {
            juce::String paramString = EQConstants::ParamStringID::GetParamStringID::filterQ(stageID, filterID);
            float filterQ = *apvts.getRawParameterValue(paramString);

            return filterQ;
        }
        float getEQFilterGain(int stageID, int filterID)
        {
            juce::String paramString = EQConstants::ParamStringID::GetParamStringID::filterGain(stageID, filterID);
            float filterGain = *apvts.getRawParameterValue(paramString);

            return filterGain;
        }
        bool getEQFilterIsBypassed(int stageID, int filterID)
        {
            juce::String paramString = EQConstants::ParamStringID::GetParamStringID::filterIsBypassed(stageID, filterID);
            bool filterIsBypassed = *apvts.getRawParameterValue(paramString);

            return filterIsBypassed;
        }
        bool getEQFilterIsActive(int stageID, int filterID)
        {
            juce::String paramString = EQConstants::ParamStringID::GetParamStringID::filterIsActive(stageID, filterID);
            bool filterIsActive = !(*apvts.getRawParameterValue(paramString));

            return filterIsActive;
        }
        int getNbOfActiveFilters(int stageID)
        {
            int nbOfActiveFilter = getEQVT(stageID).getProperty(EQConstants::ParamStringID::nbOfActiveFilters);
            return nbOfActiveFilter;
        }

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
        juce::UndoManager* pUndoManager;
        AudioEngineState<SampleType>* pListener;
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
            , distortionUnitState(mangledAPVTS)
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
                DistortionUnitState::createParametersLayout(plugInParameters, stageID, distortionUnitID);
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
                : apvts(mangledAPVTS)
                //, distortionUnitState(mangledAPVTS)
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

            //juce::ValueTree& getRoot() {return root};

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
            //juce::ValueTree& root;
        };

    private:
        juce::AudioProcessorValueTreeState& apvts;
        DistortionUnitState distortionUnitState;
    };

    class StageState
    {
    public:
        StageState(juce::AudioProcessorValueTreeState& mangledAPVTS)
            : apvts(mangledAPVTS)

        {
        };
        ~StageState() {};

        inline bool getIsBypassed(int stageID)
        {
            return *apvts.getRawParameterValue(StageConstants::ParamStringID::GetParamStringID::isBypassed(stageID));
        };
        inline SampleType getInputGain(int stageID)
        {
            return *apvts.getRawParameterValue(StageConstants::ParamStringID::GetParamStringID::inputGain(stageID));
        };
        inline SampleType getOutputGain(int stageID)
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
            TemporalState::createParametersLayout(plugInParameters, stageID);

            //Scope===================================================================================================
            FFTState::createParametersLayout(plugInParameters, stageID);

            //EQ======================================================================================================
            EQState::createParametersLayout(plugInParameters, stageID, nbOfFiltersPerEQ);

            //Phaser==================================================================================================
            PhaserState::createParametersLayout(plugInParameters, stageID);

            //Distortion==============================================================================================
            DistortionState::createParametersLayout(plugInParameters, stageID, nbOfDistoUnitPerDisto);
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

    };

    class MainLayerState
    {
    public:
        MainLayerState(juce::AudioProcessorValueTreeState& mangledAPVTS)
            : apvts(mangledAPVTS)
        {
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
                StageState::createParametersLayout(plugInParameters, stageID, nbOfFiltersPerEQ, nbOfDistoUnitPerDisto);
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

        StageState* getStageState(int stageID) { return stageState[stageID]; };

    private:
        juce::AudioProcessorValueTreeState& apvts;
    };

    inline bool getMasterLimiterIsBypassed()
    {
        return *apvts.getRawParameterValue(AudioEngineConstants::ParamStringID::GetParamStringID::masterLimiterOnOff());
    }

    inline SampleType getMasterGain()
    {
        return *apvts.getRawParameterValue(AudioEngineConstants::ParamStringID::GetParamStringID::masterGain());
    }

    void valueTreePropertyChanged(juce::ValueTree& treeWhosePropertyHasChanged, const juce::Identifier& property) override
    {
        //juce::ValueTree parent = treeWhosePropertyHasChanged.getParent();
        shouldUpdate = true;
    }
    void valueTreeChildAdded(juce::ValueTree& parentTree, juce::ValueTree& childWhichHasBeenAdded) override
    {
        if (parentTree.getType() == juce::Identifier(SampleRemapperConstants::ParamStringID::srPoints))
        {
            if (parentTree.getParent().getProperty(juce::Identifier(SampleRemapperConstants::ParamStringID::isBipolar)))
            {
                shouldUpdate = true;
            }
            else
            {
                if (parentTree.getNumChildren() % 2 == 0)
                {
                    shouldUpdate = true;
                }
            }
        }
    }
    void valueTreeChildRemoved(juce::ValueTree& parentTree, juce::ValueTree& childWhichHasBeenRemoved, int indexFromWhichChildWasRemoved) override
    {
        if (parentTree.getType() == juce::Identifier(SampleRemapperConstants::ParamStringID::srPoints))
        {
            int nbOfChild = parentTree.getNumChildren();
            if (nbOfChild == 0)
                return;

            if (parentTree.getParent().getProperty(juce::Identifier(SampleRemapperConstants::ParamStringID::isBipolar)))
            {
                shouldUpdate = true;
            }
            else
            {
                if (nbOfChild % 2 == 0)
                {
                    shouldUpdate = true;
                }
            }
        }
    }
    void valueTreeChildOrderChanged(juce::ValueTree& parentTreeWhoseChildrenHaveMoved, int oldIndex, int newIndex) override
    {

    }
    void valueTreeParentChanged(juce::ValueTree& treeWhoseParentHasChanged) override
    {

    }
    void valueTreeRedirected(juce::ValueTree& treeWhichHasBeenChanged) override
    {

    }

    juce::AudioProcessorValueTreeState& getAPVTSMainLayer() { return apvts; };

    juce::UndoManager& getUndoManager() { return undoManager; };

    juce::ValueTree getRoot() { return rootAudioEngine; };

    //Create ValueTree============================================================================================================================
    juce::ValueTree createPointNoListener(float pointX, float pointY, float tension, int curveID, bool horizontalDragOn)
    {
        juce::ValueTree vt(SampleRemapperConstants::ParamStringID::point);

        vt.setPropertyExcludingListener(this, SampleRemapperConstants::ParamStringID::pointX, pointX, &undoManager);
        vt.setPropertyExcludingListener(this, SampleRemapperConstants::ParamStringID::pointY, pointY, &undoManager);

        vt.setPropertyExcludingListener(this, SampleRemapperConstants::ParamStringID::tension, tension, &undoManager);
        vt.setPropertyExcludingListener(this, SampleRemapperConstants::ParamStringID::horizontalDragOn, horizontalDragOn, &undoManager);

        vt.setPropertyExcludingListener(this, SampleRemapperConstants::ParamStringID::curveType, curveID, &undoManager);

        return vt;
    };

    //AudioEngine=================================================================================================================================
    void setSelectedState(int stateID)
    {
        rootAudioEngine.setPropertyExcludingListener(this, AudioEngineConstants::ParamStringID::selectedState, stateID, &undoManager);
    }
    void resetAudioEngineParam()
    {
        //undoManager.beginNewTransaction();

        juce::String paramString = AudioEngineConstants::ParamStringID::GetParamStringID::masterGain();
        juce::Value apvtsValue = apvts.getParameterAsValue(paramString);
        apvtsValue = AudioEngineConstants::Processor<float>::masterGainStartValue;

        resetMainLayerParam();
    }
    float getMasterGainValue()
    {
        juce::String paramString = AudioEngineConstants::ParamStringID::GetParamStringID::masterGain();
        float masterGain = *apvts.getRawParameterValue(paramString);

        return masterGain;
    }

    //MainLayer==================================================================================================================================
    void resetMainLayerParam()
    {
        juce::String paramString = MainLayerConstants::ParamStringID::GetParamStringID::isBypassed();
        juce::Value apvtsValue = apvts.getParameterAsValue(paramString);
        apvtsValue = MainLayerConstants::Processor<float>::isBypassedStartValue;

        for (int stageID = 0; stageID < MainLayerConstants::Processor<float>::nbOfStageMax; ++stageID)
        {
            resetStageParam(stageID);
        }
    }
    void resetStageParam(int stageID)
    {
        juce::String paramString = StageConstants::AutomationString::GetString::isBypassed(stageID);
        juce::Value apvtsValue = apvts.getParameterAsValue(paramString);
        apvtsValue = StageConstants::Processor<float>::isBypassedStartValue;

        resetScopeParam(stageID);
        resetEQParam(stageID);
        resetPhaserParam(stageID);
        resetDistortionParam(stageID);
    }

    //Set Function===============================================================================================================================
    void setSelectedStageID(int stageID)
    {
        getMainLayerVT().setProperty(StageConstants::ParamStringID::selectedStageID, stageID, &undoManager);
    }
    void setStageProcessorID(int stageID, int processorID)
    {
        juce::ValueTree vtStage = getStageVT(stageID);

        vtStage.setPropertyExcludingListener(this, StageConstants::ParamStringID::displayedProcessor, processorID, &undoManager);
    }

    //Scope======================================================================================================================================
    void resetScopeParam(int stageID)
    {
        juce::String paramString = ScopeConstants::ParamStringID::GetParamStringID::dataType(stageID);
        juce::Value apvtsValue = apvts.getParameterAsValue(paramString);
        apvtsValue = ScopeConstants::Processor<float>::modeStartValue;
    }
    void setScopeZoom(int stageID, int zoomID)
    {
        getScopeVT(stageID).setPropertyExcludingListener(this, ScopeConstants::ParamStringID::zoom, zoomID, &undoManager);
    }
    void setScopeIsBypassed(int stageID, bool isBypassed)
    {
        //getScopeVT(stageID).setPropertyExcludingListener(this, ScopeConstants::ParamStringID::scope::preEQIsBypassed, isBypassed, nullptr);
        //getScopeVT(stageID).setPropertyExcludingListener(this, ScopeConstants::ParamStringID::scope::postEQIsBypassed, isBypassed, nullptr);
        //getScopeVT(stageID).setProperty(ScopeConstants::ParamStringID::scope::postDistoIsBypassed, isBypassed, nullptr);

    }
    void setScopePreEQIsBypassed(int stageID, bool isBypassed)
    {
        //getScopeVT(stageID).setProperty(ScopeConstants::ParamStringID::scope::preEQIsBypassed, isBypassed, &undoManager);
    }
    void setScopePostEQIsBypassed(int stageID, bool isBypassed)
    {
        //getScopeVT(stageID).setProperty(ScopeConstants::ParamStringID::scope::postEQIsBypassed, isBypassed, &undoManager);
    }
    void setScopePostDistoIsBypassed(int stageID, bool isBypassed)
    {
        //getScopeVT(stageID).setProperty(ScopeConstants::ParamStringID::scope::postDistoIsBypassed, isBypassed, &undoManager);
    }

    //Set Function EQ============================================================================================================================
    void resetEQParam(int stageID)
    {
        //Distortion Mix
        juce::String paramString = EQConstants::ParamStringID::GetParamStringID::mix(stageID);
        juce::Value apvtsValue = apvts.getParameterAsValue(paramString);
        apvtsValue = EQConstants::Processor<SampleType>::mixStartValue;

        //Distortion isBypassed
        paramString = EQConstants::ParamStringID::GetParamStringID::isBypassed(stageID);
        apvtsValue = apvts.getParameterAsValue(paramString);
        apvtsValue = EQConstants::Processor<SampleType>::isBypassedStartValue;

        for (int filterID = 0; filterID < EQConstants::Processor<SampleType>::nbOfFilterMax; ++filterID)
        {
            paramString = EQConstants::ParamStringID::GetParamStringID::filterCutoff(stageID, filterID);
            apvtsValue = apvts.getParameterAsValue(paramString);
            apvtsValue = EQConstants::BiquadConstants<SampleType>::cutoffStartValue;

            paramString = EQConstants::ParamStringID::GetParamStringID::filterQ(stageID, filterID);
            apvtsValue = apvts.getParameterAsValue(paramString);
            apvtsValue = EQConstants::BiquadConstants<SampleType>::qStartValue;

            paramString = EQConstants::ParamStringID::GetParamStringID::filterGain(stageID, filterID);
            apvtsValue = apvts.getParameterAsValue(paramString);
            apvtsValue = EQConstants::BiquadConstants<SampleType>::gainLinStartValue;

            paramString = EQConstants::ParamStringID::GetParamStringID::filterType(stageID, filterID);
            apvtsValue = apvts.getParameterAsValue(paramString);
            apvtsValue = EQConstants::BiquadConstants<SampleType>::typeStartValue;

            paramString = EQConstants::ParamStringID::GetParamStringID::filterOrder(stageID, filterID);
            apvtsValue = apvts.getParameterAsValue(paramString);
            apvtsValue = EQConstants::BiquadConstants<SampleType>::orderStartValue;

            paramString = EQConstants::ParamStringID::GetParamStringID::filterIsBypassed(stageID, filterID);
            apvtsValue = apvts.getParameterAsValue(paramString);
            apvtsValue = EQConstants::BiquadConstants<SampleType>::isBypassedStartValue;

            paramString = EQConstants::ParamStringID::GetParamStringID::filterIsActive(stageID, filterID);
            apvtsValue = apvts.getParameterAsValue(paramString);
            apvtsValue = !EQConstants::BiquadConstants<SampleType>::isActiveStartValue;
        }

        //setSelectedFilterID(stageID, -1);
    };


    void setEQFilterCutOff(int stageID, int filterID, float newCutoff)
    {
        juce::String paramString = EQConstants::ParamStringID::GetParamStringID::filterCutoff(stageID, filterID);
        juce::Value cutoffValue = apvts.getParameterAsValue(paramString);

        cutoffValue = newCutoff;
    }
    void setEQFilterQ(int stageID, int filterID, float newQ)
    {
        juce::String paramString = EQConstants::ParamStringID::GetParamStringID::filterQ(stageID, filterID);
        juce::Value qValue = apvts.getParameterAsValue(paramString);

        qValue = newQ;
    }
    void setEQFilterGain(int stageID, int filterID, float newGain)
    {
        juce::String paramString = EQConstants::ParamStringID::GetParamStringID::filterGain(stageID, filterID);
        juce::Value gainValue = apvts.getParameterAsValue(paramString);

        gainValue = newGain;
    }
    void setEQFilterType(int stageID, int filterID, int filterType)
    {
        juce::String paramString = EQConstants::ParamStringID::GetParamStringID::filterType(stageID, filterID);
        juce::Value filterTypeValue = apvts.getParameterAsValue(paramString);

        filterTypeValue = filterType;
    }
    void setEQFilterIsBypassed(int stageID, int filterID, bool newIsBypassed)
    {
        juce::String paramString = EQConstants::ParamStringID::GetParamStringID::filterIsBypassed(stageID, filterID);
        juce::Value isBypassedValue = apvts.getParameterAsValue(paramString);

        isBypassedValue = newIsBypassed;
    }
    void setEQFilterIsActive(int stageID, int filterID, bool newIsActive)
    {
        juce::String paramString = EQConstants::ParamStringID::GetParamStringID::filterIsActive(stageID, filterID);
        juce::Value isActiveValue = apvts.getParameterAsValue(paramString);

        isActiveValue = newIsActive;
    }
    void setSelectedFilterID(int stageID, int filterID)
    {
        undoManager.beginNewTransaction();
        getEQVT(stageID).setPropertyExcludingListener(this, EQConstants::ParamStringID::selectedFilterID, filterID, &undoManager);
    }
    void setMouseAddFilterEQ(const juce::MouseEvent& e, int filterID, float scopeWidth, float scopeHeight)
    {
        if (filterID == -1)
            return;

        int stageID = getSelectedStageID();

        int xPos = e.getPosition().getX();
        int yPos = e.getPosition().getY();

        // FQG VALUE
        float xPosRemap = juce::jmap((float)xPos, 0.0f, scopeWidth - 100.0f, 0.0f, 1.0f);
        float filterCutoff = juce::mapToLog10(xPosRemap, 10.0f, 22050.0f);

        float gainMin = 0.0f;
        float gainMax = 0.0f;

        switch (getScopeZoom(stageID))
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

        float filterGain = juce::jmap((float)yPos, 0.0f, scopeHeight, gainMax, gainMin);

        //undoManager.beginNewTransaction();
        setSelectedFilterID(stageID, filterID);
        setEQFilterCutOff(stageID, filterID, filterCutoff);
        setEQFilterGain(stageID, filterID, filterGain);
        setEQFilterIsBypassed(stageID, filterID, false);
        setEQFilterIsActive(stageID, filterID, false);
        addActiveFilterCount(stageID);
        //setEQFilterQ();
    }
    int findNextAvailableFilterIDEQ(int stageID)
    {
        int filterID = 0;
        juce::String paramString = EQConstants::ParamStringID::GetParamStringID::filterIsActive(stageID, filterID);
        bool isActive = !(*apvts.getRawParameterValue(paramString));

        while (isActive && (filterID < EQConstants::Processor<float>::nbOfFilterMax - 1))
        {
            filterID++;
            paramString = EQConstants::ParamStringID::GetParamStringID::filterIsActive(stageID, filterID);
            isActive = !(*apvts.getRawParameterValue(paramString));
        }

        //if (filterID == EQConstants::Processor<float>::nbOfFilterMax)
        if (filterID == EQConstants::Processor<float>::nbOfFilterMax - 1 && isActive)
        {
            filterID = -1;
        }

        return filterID;
    }
    int findNextActiveFilterDown(int stageID)
    {
        if (getNbOfActiveFilters(stageID) == 0)
            return -1;

        int filterID = getSelectedFilterID(stageID) - 1;
        juce::String paramString;
        bool isActive = true;
        //int count = 0;
        if (filterID < 0)
            filterID = EQConstants::Processor<float>::nbOfFilterMax - 1;

        paramString = EQConstants::ParamStringID::GetParamStringID::filterIsActive(stageID, filterID);
        isActive = (*apvts.getRawParameterValue(paramString));

        while (isActive && (filterID > -1))// EQConstants::Processor<float>::nbOfFilterMax)
        {
            //count++;
            filterID--;
            filterID = (filterID + EQConstants::Processor<float>::nbOfFilterMax - 1) % (EQConstants::Processor<float>::nbOfFilterMax - 1);
            paramString = EQConstants::ParamStringID::GetParamStringID::filterIsActive(stageID, filterID);
            isActive = (*apvts.getRawParameterValue(paramString));
        }

        return filterID;
    }
    int findNextActiveFilterUp(int stageID)
    {
        if (getNbOfActiveFilters(stageID) == 0)
            return -1;

        int filterID = getSelectedFilterID(stageID) + 1;
        juce::String paramString;
        bool isActive = true;
        if (filterID > EQConstants::Processor<float>::nbOfFilterMax - 1)
            filterID = 0;

        paramString = EQConstants::ParamStringID::GetParamStringID::filterIsActive(stageID, filterID);
        isActive = (*apvts.getRawParameterValue(paramString));

        while (isActive && (filterID < EQConstants::Processor<float>::nbOfFilterMax))
        {
            filterID++;
            filterID = filterID % (EQConstants::Processor<float>::nbOfFilterMax - 1);
            paramString = EQConstants::ParamStringID::GetParamStringID::filterIsActive(stageID, filterID);
            isActive = (*apvts.getRawParameterValue(paramString));
        }

        return filterID;
    }
    void addActiveFilterCount(int stageID)
    {
        int yolo = getNbOfActiveFilters(stageID);
        int nbOfActiveFilters = juce::jlimit(0, EQConstants::Processor<float>::nbOfFilterMax - 1, getNbOfActiveFilters(stageID) + 1);
        getEQVT(stageID).setPropertyExcludingListener(this, EQConstants::ParamStringID::nbOfActiveFilters, nbOfActiveFilters, &undoManager);
    }
    void remodeActiveFilterCount(int stageID)
    {
        int nbOfActiveFilters = juce::jlimit(0, EQConstants::Processor<float>::nbOfFilterMax - 1, getNbOfActiveFilters(stageID) - 1);
        getEQVT(stageID).setPropertyExcludingListener(this, EQConstants::ParamStringID::nbOfActiveFilters, nbOfActiveFilters, &undoManager);
    }
    void setMouseDragEventEQ(const juce::MouseEvent& e, float scopeWidth, float scopeHeight)
    {
        int stageID = getSelectedStageID();
        int filterID = getSelectedFilterID(stageID);

        if (filterID == -1)
            return;

        float xPos = static_cast<float>(e.getPosition().getX());
        float yPos = static_cast<float>(e.getPosition().getY());
        yPos = juce::jlimit(0.0f + scopeHeight / 14.0f, scopeHeight - scopeHeight / 14.0f, yPos);
        float filterCutoff = Conversion<float>::fromMouseToCutoff(xPos, 0.0f, scopeWidth - 100.0f, 10.0f, 22050.0f);
        setEQFilterCutOff(stageID, filterID, filterCutoff);

        int filterType = getEQFilterType(stageID, filterID);

        if (filterType == EQConstants::BiquadConstants<float>::Types::peak || filterType == EQConstants::BiquadConstants<float>::Types::lowshelf || filterType == EQConstants::BiquadConstants<float>::Types::highshelf)
        {

            float gainMin = 0.0f;
            float gainMax = 0.0f;

            switch (getScopeZoom(stageID))
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

            float filterGain = Conversion<float>::fromMouseToGain(yPos, scopeHeight, 0.0f, gainMin, gainMax);
            setEQFilterGain(stageID, filterID, filterGain);
        }
        else
        {
            float qMin = 0.0f;
            float qMax = 0.0f;

            switch (getScopeZoom(stageID))
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

            float filterQ = Conversion<float>::fromMouseToQ(yPos, scopeHeight - scopeHeight / 14.0f, 0.0f + scopeHeight / 14.0f, qMin, qMax);
            setEQFilterQ(stageID, filterID, filterQ);
        }
    }
    void setNbOfActiveFilter(int stageID, int nbOfActiveFilters)
    {
        getEQVT(stageID).setPropertyExcludingListener(this, EQConstants::ParamStringID::nbOfActiveFilters, nbOfActiveFilters, &undoManager);
    }

    inline SampleType getFilterCutoff(int stageID, int filterID)
    {
        return *apvts.getRawParameterValue(EQConstants::ParamStringID::GetParamStringID::filterCutoff(stageID, filterID));
    };
    inline SampleType getFilterQ(int stageID, int filterID)
    {
        return *apvts.getRawParameterValue(EQConstants::ParamStringID::GetParamStringID::filterQ(stageID, filterID));
    };
    inline SampleType getFilterGain(int stageID, int filterID)
    {
        return *apvts.getRawParameterValue(EQConstants::ParamStringID::GetParamStringID::filterGain(stageID, filterID));
    };
    inline int getFilterOrder(int stageID, int filterID)
    {
        return ((int)*apvts.getRawParameterValue(EQConstants::ParamStringID::GetParamStringID::filterOrder(stageID, filterID))) + 1;
    };
    inline int getFilterType(int stageID, int filterID)
    {
        return ((int)*apvts.getRawParameterValue(EQConstants::ParamStringID::GetParamStringID::filterType(stageID, filterID)));
    };
    inline bool getFilterIsBypassed(int stageID, int filterID)
    {
        return *apvts.getRawParameterValue(EQConstants::ParamStringID::GetParamStringID::filterIsBypassed(stageID, filterID));
    };
    inline bool getFilterIsActive(int stageID, int filterID)
    {
        return !(*apvts.getRawParameterValue(EQConstants::ParamStringID::GetParamStringID::filterIsActive(stageID, filterID)));
    };
    inline bool getIsBypassed(int stageID)
    {
        return *apvts.getRawParameterValue(EQConstants::ParamStringID::GetParamStringID::isBypassed(stageID, filterID));
    };
    inline SampleType getMix(int stageID)
    {
        return *apvts.getRawParameterValue(EQConstants::ParamStringID::GetParamStringID::mix(stageID, filterID));
    };

    int getSelectedFilterID(int stageID)
    {
        juce::ValueTree vtEQ = getEQVT(stageID);

        return vtEQ.getProperty(EQConstants::ParamStringID::selectedFilterID);
    }
    int getEQFilterType(int stageID, int filterID)
    {
        juce::String paramString = EQConstants::ParamStringID::GetParamStringID::filterType(stageID, filterID);
        int filterType = (int)*apvts.getRawParameterValue(paramString);

        return filterType;
    }
    float getEQFilterCutoff(int stageID, int filterID)
    {
        juce::String paramString = EQConstants::ParamStringID::GetParamStringID::filterCutoff(stageID, filterID);
        float filterCutoff = *apvts.getRawParameterValue(paramString);

        return filterCutoff;
    }
    float getEQFilterQ(int stageID, int filterID)
    {
        juce::String paramString = EQConstants::ParamStringID::GetParamStringID::filterQ(stageID, filterID);
        float filterQ = *apvts.getRawParameterValue(paramString);

        return filterQ;
    }
    float getEQFilterGain(int stageID, int filterID)
    {
        juce::String paramString = EQConstants::ParamStringID::GetParamStringID::filterGain(stageID, filterID);
        float filterGain = *apvts.getRawParameterValue(paramString);

        return filterGain;
    }
    bool getEQFilterIsBypassed(int stageID, int filterID)
    {
        juce::String paramString = EQConstants::ParamStringID::GetParamStringID::filterIsBypassed(stageID, filterID);
        bool filterIsBypassed = *apvts.getRawParameterValue(paramString);

        return filterIsBypassed;
    }
    bool getEQFilterIsActive(int stageID, int filterID)
    {
        juce::String paramString = EQConstants::ParamStringID::GetParamStringID::filterIsActive(stageID, filterID);
        bool filterIsActive = !(*apvts.getRawParameterValue(paramString));

        return filterIsActive;
    }
    int getNbOfActiveFilters(int stageID)
    {
        int nbOfActiveFilter = getEQVT(stageID).getProperty(EQConstants::ParamStringID::nbOfActiveFilters);
        return nbOfActiveFilter;
    }
    void setMouseWheelEventEQ(const juce::MouseEvent& event, const juce::MouseWheelDetails& wheel, int filterID)
    {
        int stageID = getSelectedStageID();
        int filterType = getEQFilterType(stageID, filterID);

        if (filterType == EQConstants::BiquadConstants<float>::Types::peak || filterType == EQConstants::BiquadConstants<float>::Types::lowshelf || filterType == EQConstants::BiquadConstants<float>::Types::highshelf)
        {
            float rot = wheel.deltaY;

            juce::String paramString = EQConstants::ParamStringID::GetParamStringID::filterQ(stageID, filterID);
            float sliderValue = *apvts.getRawParameterValue(paramString);
            sliderValue += sliderValue * rot;
            setEQFilterQ(stageID, filterID, sliderValue);
        }
    }
    
    //Phaser=====================================================================================================================================
    void resetPhaserParam(int stageID)
    {
        //Phaser Centre Frequency
        juce::String paramString = PhaserConstants::ParamStringID::GetParamStringID::centerFrequency(stageID);
        juce::Value apvtsValue = apvts.getParameterAsValue(paramString);
        apvtsValue = PhaserConstants::Processor<float>::cfreqStartValue;

        //Phaser Feedback
        paramString = PhaserConstants::ParamStringID::GetParamStringID::feedback(stageID);
        apvtsValue = apvts.getParameterAsValue(paramString);
        apvtsValue = PhaserConstants::Processor<float>::feedbackStartValue;

        //Phaser Rate
        paramString = PhaserConstants::ParamStringID::GetParamStringID::rate(stageID);
        apvtsValue = apvts.getParameterAsValue(paramString);
        apvtsValue = PhaserConstants::Processor<float>::rateStartValue;

        //Phaser Depth
        paramString = PhaserConstants::ParamStringID::GetParamStringID::depth(stageID);
        apvtsValue = apvts.getParameterAsValue(paramString);
        apvtsValue = PhaserConstants::Processor<float>::depthStartValue;

        //Phaser Nb Of Stages
        paramString = PhaserConstants::ParamStringID::GetParamStringID::nbOfStages(stageID);
        apvtsValue = apvts.getParameterAsValue(paramString);
        apvtsValue = PhaserConstants::Processor<float>::nbOfStgStartValue;

        //Phaser Mix
        paramString = PhaserConstants::ParamStringID::GetParamStringID::mix(stageID);
        apvtsValue = apvts.getParameterAsValue(paramString);
        apvtsValue = PhaserConstants::Processor<float>::mixStartValue;

        //Phaser isBypassed
        paramString = PhaserConstants::ParamStringID::GetParamStringID::isBypassed(stageID);
        apvtsValue = apvts.getParameterAsValue(paramString);
        apvtsValue = PhaserConstants::Processor<float>::isBypassedStartValue;
    }

    //Set function SampleRemapper====================================================================================================================
    void resetDistortionParam(int stageID)
    {
        //Distortion Mix
        juce::String paramString = DistortionConstants::ParamStringID::GetParamStringID::mix(stageID);
        juce::Value apvtsValue = apvts.getParameterAsValue(paramString);
        apvtsValue = DistortionConstants::Processor<float>::mixStartValue;

        //Distortion isBypassed
        paramString = DistortionConstants::ParamStringID::GetParamStringID::isBypassed(stageID);
        apvtsValue = apvts.getParameterAsValue(paramString);
        apvtsValue = DistortionConstants::Processor<float>::isBypassedStartValue;

        for (int distortionUnitID = 0; distortionUnitID < 2; ++distortionUnitID)
        {
            resetDistortionDUParam(stageID, distortionUnitID);
        }
    }
    void resetDistortionDUParam(int stageID, int distortionUnitID)
    {
        bool duOnOffStartValue = true;

        //DU OnOff
        juce::String paramString = DistoUnitConstants::ParamStringID::GetParamStringID::isBypassed(stageID, distortionUnitID);
        juce::Value apvtsValue = apvts.getParameterAsValue(paramString);
        apvtsValue = duOnOffStartValue;

        getDistortionCircuitVT(stageID, distortionUnitID).setPropertyExcludingListener(this, DistortionCircuitConstants::ParamsID::equationType, 0, &undoManager);

        getDistortionCircuitVT(stageID, distortionUnitID).setPropertyExcludingListener(this, DistortionCircuitConstants::ParamsID::equationID, 0, &undoManager);

        //DU Routing
        paramString = DistoUnitConstants::ParamStringID::GetParamStringID::routing(stageID, distortionUnitID);
        apvtsValue = apvts.getParameterAsValue(paramString);
        apvtsValue = DistoUnitConstants::Processor<float>::routingStartValue;

        //DU PreGain
        paramString = DistoUnitConstants::ParamStringID::GetParamStringID::preGain(stageID, distortionUnitID);
        apvtsValue = apvts.getParameterAsValue(paramString);
        apvtsValue = DistoUnitConstants::Processor<float>::preGaindBStartValue;

        //DU Drive
        paramString = DistoUnitConstants::ParamStringID::GetParamStringID::drive(stageID, distortionUnitID);
        apvtsValue = apvts.getParameterAsValue(paramString);
        apvtsValue = DistoUnitConstants::Processor<float>::driveStartValue;

        //DU PostGain
        paramString = DistoUnitConstants::ParamStringID::GetParamStringID::postGain(stageID, distortionUnitID);
        apvtsValue = apvts.getParameterAsValue(paramString);
        apvtsValue = DistoUnitConstants::Processor<float>::postGaindBStartValue;

        //DU Warp
        paramString = DistoUnitConstants::ParamStringID::GetParamStringID::warp(stageID, distortionUnitID);
        apvtsValue = apvts.getParameterAsValue(paramString);
        apvtsValue = DistoUnitConstants::Processor<float>::warpStartValue;

        //Is Bipolar. Has to be done that way. OtherWise the undoManager Will begin a new transaction.
        getSampleRemapperVT(stageID, distortionUnitID).setProperty(SampleRemapperConstants::ParamStringID::isBipolar, false, &undoManager);

        //DU DCFilterOn
        paramString = DistoUnitConstants::ParamStringID::GetParamStringID::dcFilterIsBypassed(stageID, distortionUnitID);
        apvtsValue = apvts.getParameterAsValue(paramString);
        apvtsValue = DistoUnitConstants::Processor<float>::dcFilterIsBypassedStartValue;

        //DU Mix
        paramString = DistoUnitConstants::ParamStringID::GetParamStringID::mix(stageID, distortionUnitID);
        apvtsValue = apvts.getParameterAsValue(paramString);
        apvtsValue = DistoUnitConstants::Processor<float>::mixStartValue;

        resetSampleRemapper(stageID, distortionUnitID);
    }
    
    //SampleRemapper Function
    void switchFromUnipolarToBipolar(int stageID, int distortionUnitID)
    {
        juce::ValueTree vtWS = getSampleRemapperPointsVT(stageID, distortionUnitID);
        int nbOfPoints = getNbOfPoints(stageID, distortionUnitID);

        int pointID = nbOfPoints - 1;
        float pointX = getPointX(stageID, distortionUnitID, pointID);
        float pointY = getPointY(stageID, distortionUnitID, pointID);

        //undoManager.beginNewTransaction();

        while (!(pointX == 0.0f) && !(pointY == 0.0f) && (pointID > 0))//(pointID < nbOfPoints)
        {
            pointID--;
            pointX = getPointX(stageID, distortionUnitID, pointID);
            pointY = getPointY(stageID, distortionUnitID, pointID);
        }

        pointID--;
        pointX = getPointX(stageID, distortionUnitID, pointID);
        pointY = getPointY(stageID, distortionUnitID, pointID);

        while ((pointX == 0.0f) && (pointY == 0.0f) && (pointID > 0))
        {
            vtWS.removeChild(pointID, &undoManager);
            pointID--;
            pointX = getPointX(stageID, distortionUnitID, pointID);
            pointY = getPointX(stageID, distortionUnitID, pointID);
        }
        //zeroCrossingPointID;

        nbOfPoints = getNbOfPoints(stageID, distortionUnitID);

        if (nbOfPoints > 2)
        {
            for (int point = 1; point < nbOfPoints - 1; ++point)
            {
                setHorizontalDragOn(stageID, distortionUnitID, point, true);
            }
        }

        pointID = getSelectedPointID(stageID, distortionUnitID);

        //if (curveID == nbOfPoints / 2 - 1)
        {
            setSelectedIDCurve(stageID, distortionUnitID, pointID);
        }

        vtWS.sendPropertyChangeMessage(SampleRemapperConstants::ParamStringID::tension);
    }
    void switchFromBipolarToUnipolar(int stageID, int distortionUnitID)
    {
        juce::ValueTree vtWSPoints = getSampleRemapperPointsVT(stageID, distortionUnitID);
        int nbOfPoints = getNbOfPoints(stageID, distortionUnitID);
        int zeroCrossingPointID = 0;
        float zeroCPointX = getPointX(stageID, distortionUnitID, zeroCrossingPointID);

        //undoManager.beginNewTransaction();

        while ((zeroCPointX > 0.0f) && (zeroCrossingPointID < nbOfPoints))
        {
            zeroCrossingPointID++;
            zeroCPointX = getPointX(stageID, distortionUnitID, zeroCrossingPointID);
        }

        //zeroCrossingPointID;

        //zeroCPointX = getPointX(stageID, distortionUnitID, zeroCrossingPointID);
        float leftX = getPointX(stageID, distortionUnitID, zeroCrossingPointID);
        float leftY = getPointY(stageID, distortionUnitID, zeroCrossingPointID);
        float rightX = getPointX(stageID, distortionUnitID, zeroCrossingPointID - 1);
        float rightY = getPointY(stageID, distortionUnitID, zeroCrossingPointID - 1);

        for (int pointID = zeroCrossingPointID; pointID < nbOfPoints; ++pointID)
        {
            int nbOfChild = vtWSPoints.getNumChildren();
            vtWSPoints.removeChild(nbOfChild - 1, &undoManager);
        }

        //float pointY = 0.0f;
        int lastID = nbOfPoints - 1;
        float pointX = 0.0f;
        float pointY = juce::jmap(pointX, leftX, rightX, 0.0f, 1.0f);
        float tension = getTension(stageID, distortionUnitID, zeroCrossingPointID - 1);
        //pointY = std::pow(pointY, tension);
        pointY = juce::jmap(pointY, 0.0f, 1.0f, leftY, rightY);

        //tension = getTension(stageID, distortionUnitID, lastID);
        int curveID = getPointCurveType(stageID, distortionUnitID, zeroCrossingPointID - 1);
        curveID == 0 ? tension = -tension : tension = tension;
        vtWSPoints.addChild(createPointNoListener(pointX, pointY, 0.0f, 0, false), -1, &undoManager);
        vtWSPoints.addChild(createPointNoListener(pointX, -pointY, tension, curveID, false), -1, &undoManager);

        nbOfPoints = getNbOfPoints(stageID, distortionUnitID);
        lastID = nbOfPoints - 1 - 2; //We skip the 2 new zerocrossing points

        for (int pointID = lastID; pointID > 0; --pointID)
        {
            pointX = -getPointX(stageID, distortionUnitID, pointID);
            pointY = -getPointY(stageID, distortionUnitID, pointID);
            tension = getTension(stageID, distortionUnitID, pointID - 1);
            curveID = getPointCurveType(stageID, distortionUnitID, pointID - 1);

            curveID == 0 ? tension = -tension : tension = tension;

            vtWSPoints.addChild(createPointNoListener(pointX, pointY, tension, curveID, true), -1, &undoManager);
        }

        pointX = -getPointX(stageID, distortionUnitID, 0);
        pointY = -getPointY(stageID, distortionUnitID, 0);
        tension = 0.0f;
        vtWSPoints.addChild(createPointNoListener(pointX, pointY, tension, 0, true), -1, &undoManager);

        vtWSPoints.sendPropertyChangeMessage(SampleRemapperConstants::ParamStringID::tension);
    }

    //Value Tree Add/Remove point from SampleRemapper================================================================================================
    void setMouseAddPointWS(juce::MouseEvent& e, float scopeWidth, float scopeHeight)
    {
        int stageID = getSelectedStageID();
        int distortionUnitID = getSelectedDistoUnitID();
        int nbOfPoints = getNbOfPoints(stageID, distortionUnitID);
        //int halfNbOfPoints = nbOfPoints / 2;

        if (nbOfPoints == DistortionConstants::WaveShaper<float>::nbOfPointMax)
            return;

        float scopeMinLin = DistortionConstants::UI::scopeMinLin_f;
        float scopeMaxLin = DistortionConstants::UI::scopeMaxLin_f;
        juce::Point<float> point = e.getPosition().toFloat();
        float pointX = juce::jmap(point.getX(), 0.0f, scopeWidth, scopeMinLin, scopeMaxLin);
        float pointY = juce::jmap(point.getY(), scopeHeight, 0.0f, scopeMinLin, scopeMaxLin);
        //float tension = 1.0f;

        addPoint(stageID, distortionUnitID, pointX, pointY);
    }
    void setMouseDragEventWS(juce::MouseEvent& e, float scopeWidth, float scopeHeight)
    {
        int stageID = getSelectedStageID();
        int distortionUnitID = getSelectedDistoUnitID();
        int pointID = getSelectedPointID(stageID, distortionUnitID);
        int tensionID = getSelectedTensionID(stageID, distortionUnitID);
        float pointX = 0.0f;
        float pointY = 0.0f;

        float scopeMin = DistortionConstants::UI::scopeMinLin_f;
        float scopeMax = DistortionConstants::UI::scopeMaxLin_f;

        //undoManager.beginNewTransaction();

        if (pointID > -1)
        {
            pointX = juce::jmap((float)e.getPosition().getX(), 0.0f, scopeWidth, scopeMin, scopeMax);
            pointY = juce::jmap((float)e.getPosition().getY(), scopeHeight, 0.0f, scopeMin, scopeMax);
            juce::Point<float> newPoint(pointX, pointY);

            setSelectedPoint(stageID, distortionUnitID, newPoint);
        }
        else if (tensionID > -1)
        {
            float rightY = getPointY(stageID, distortionUnitID, tensionID);
            float leftY = getPointY(stageID, distortionUnitID, tensionID + 1);
            rightY = juce::jmap(rightY, scopeMin, scopeMax, scopeHeight, 0.0f);
            leftY = juce::jmap(leftY, scopeMin, scopeMax, scopeHeight, 0.0f);

            if (leftY != rightY)
            {
                float maxY = juce::jmax(rightY, leftY);
                float minY = juce::jmin(rightY, leftY);
                float mousePos = (float)e.getPosition().getY();
                mousePos = juce::jlimit(minY, maxY, mousePos);
                float tension = juce::jmap(mousePos, minY, maxY, -0.5f, 0.5f);

                setSelectedTension(stageID, distortionUnitID, tension);

            }
        }

        //getWaveShaperPointsVT(stageID, distortionUnitID).getChild(tensionID).sendPropertyChangeMessage(SampleRemapperConstants::ParamStringID::tension);


    }
    void setMouseUpWS()
    {
    }
    void addPoint(int stageID, int distortionUnitID, float pointX, float pointY)
    {
        juce::ValueTree vtWS = getSampleRemapperPointsVT(stageID, distortionUnitID);

        int nbOfChild = vtWS.getNumChildren();
        if (nbOfChild == DistortionConstants::WaveShaper<float>::nbOfPointMax)
            return;

        int childID = 0;
        int curveType = 0;
        float wsPointX = getPoint(stageID, distortionUnitID, childID).getX();

        while (!(wsPointX < pointX) && (childID < nbOfChild))
        {
            ++childID;
            wsPointX = getPoint(stageID, distortionUnitID, childID).getX();

        }

        undoManager.beginNewTransaction();

        float tensionChild = getTension(stageID, distortionUnitID, childID - 1);

        vtWS.addChild(createPointNoListener(pointX, pointY, tensionChild, 0, true), childID, &undoManager);

        setPointAndTensionIDNoTransaction(stageID, distortionUnitID, childID, -1);

        if (!getIsBipolar(stageID, distortionUnitID))
        {
            nbOfChild = getNbOfPoints(stageID, distortionUnitID);
            int mirrorID = nbOfChild - childID;
            curveType = getPointCurveType(stageID, distortionUnitID, childID - 1);
            tensionChild = getTension(stageID, distortionUnitID, childID - 1);

            curveType == 0 ? tensionChild = -tensionChild : tensionChild = tensionChild;

            vtWS.addChild(createPointNoListener(-pointX, -pointY, tensionChild, curveType, true), mirrorID, &undoManager);

            curveType = getPointCurveType(stageID, distortionUnitID, childID);
            //tensionChild = 0.0f;
            setPointCurveTypeNoListener(stageID, distortionUnitID, mirrorID - 1, 0);
            setTensionNoListener(stageID, distortionUnitID, mirrorID - 1, 0.0f);
        }

        setSelectedIDCurve(stageID, distortionUnitID, childID);
        getSampleRemapperPointsVT(stageID, distortionUnitID).getChild(childID).sendPropertyChangeMessage(SampleRemapperConstants::ParamStringID::selectedCurveID);
    }
    void deleteSelectedPoint(int stageID, int distortionUnitID)
    {
        int selectedID = getSelectedPointID(stageID, distortionUnitID);
        //int nbOfPoints = getNbOfPoints(stageID, distortionUnitID);
        juce::ValueTree vtWSPoints = getSampleRemapperPointsVT(stageID, distortionUnitID);
        int nbOfChildren = getSampleRemapperPointsVT(stageID, distortionUnitID).getNumChildren() - 1;
        bool horzDragOn = getHorizontalDragOn(stageID, distortionUnitID, selectedID);

        //if (selectedID == -1 || selectedID == 0 || selectedID == (nbOfPoints - 1))
        //    return;

        //HorizontalDragOn = true mean the point is either at -1 or +1 or at 0 on unipolar mode.
        if (selectedID == -1 || !horzDragOn)
            return;

        undoManager.beginNewTransaction();

        vtWSPoints.removeChild(selectedID, &undoManager);

        setSelectedIDPoint(stageID, distortionUnitID, selectedID - 1);
        setSelectedIDTension(stageID, distortionUnitID, -1);
        setSelectedIDCurve(stageID, distortionUnitID, selectedID - 1);

        bool isBipolar = getIsBipolar(stageID, distortionUnitID);

        if (!isBipolar)
        {
            int mirrorID = nbOfChildren - selectedID - 1;

            vtWSPoints.removeChild(mirrorID, &undoManager);

            int curveType = getPointCurveType(stageID, distortionUnitID, selectedID - 1);
            float tension = getTension(stageID, distortionUnitID, selectedID - 1);

            curveType == 0 ? tension = -tension : tension = tension;

            setPointCurveTypeNoListener(stageID, distortionUnitID, mirrorID - 1, curveType);
            setTensionNoListener(stageID, distortionUnitID, mirrorID - 1, tension);
        }
        //undoManager.beginNewTransaction

        getSampleRemapperPointsVT(stageID, distortionUnitID).getChild(selectedID - 1).sendPropertyChangeMessage(SampleRemapperConstants::ParamStringID::tension);
    }
    void resetSampleRemapper(int stageID, int distortionUnitID)
    {
        juce::ValueTree vtWS = getSampleRemapperVT(stageID, distortionUnitID);

        undoManager.beginNewTransaction();

        vtWS.removeAllChildren(&undoManager);

        setSelectedIDPoint(stageID, distortionUnitID, -1);
        setSelectedIDTension(stageID, distortionUnitID, -1);

        bool isBipolar = getIsBipolar(stageID, distortionUnitID);

        if (isBipolar)
        {
            SampleRemapper<float>::createValueTreePointBipolar(vtWS, &undoManager);
        }
        else
        {
            SampleRemapper<float>::createValueTreePointUnipolar(vtWS, &undoManager);
        }

    }
    void setDistoUnitID(int stageID, int distortionUnitID)
    {
        getDistortionVT(stageID).setProperty(DistoUnitConstants::ParamStringID::selectedDistoUnitID, distortionUnitID, nullptr);
    }
    void setSelectedDistoUnitID(int distortionUnitID)
    {
        int stageID = getSelectedStageID();
        getDistortionVT(stageID).setProperty(DistoUnitConstants::ParamStringID::selectedDistoUnitID, distortionUnitID, nullptr);
    }
    void setPoint(int stageID, int distortionUnitID, int pointID, juce::Point<float> newPoint)
    {
        //int everyPointButFirst = 0;
        juce::ValueTree vtWSPoint = getSampleRemapperPointsVT(stageID, distortionUnitID);
        int nbOfChildren = vtWSPoint.getNumChildren() - 1;

        float pointX = newPoint.getX();
        float pointY = newPoint.getY();

        //undoManager.beginNewTransaction();

        //setPointYNoListener(stageID, distortionUnitID, pointID, pointY);

        bool canDragHorizontal = getHorizontalDragOn(stageID, distortionUnitID, pointID);
        //We do not set X Value for the first and last Point. They are fixed at -1.0 and +1.0
        if (canDragHorizontal)
        {
            setPointX(stageID, distortionUnitID, pointID, pointX);
        }

        bool isBipolar = getIsBipolar(stageID, distortionUnitID);

        if (!isBipolar)
        {
            int mirrorID = nbOfChildren - pointID;

            setPointY(stageID, distortionUnitID, mirrorID, -pointY);

            if (canDragHorizontal)
                setPointX(stageID, distortionUnitID, mirrorID, -pointX);
        }

        setPointY(stageID, distortionUnitID, pointID, pointY);
        //vtWSPoint.sendPropertyChangeMessage(SampleRemapperConstants::ParamStringID::pointY); NoListener
    }
    void setPointX(int stageID, int distortionUnitID, int pointID, float newPointX)
    {
        newPointX = juce::jlimit(getPoint(stageID, distortionUnitID, pointID + 1).getX(),
            getPoint(stageID, distortionUnitID, pointID - 1).getX(),
            newPointX
        );

        getSampleRemapperPointsVT(stageID, distortionUnitID).getChild(pointID).setProperty(SampleRemapperConstants::ParamStringID::pointX, newPointX, &undoManager);
    }
    void setPointY(int stageID, int distortionUnitID, int pointID, float newPointY)
    {
        newPointY = juce::jlimit(DistortionConstants::WaveShaper<float>::pointYMinLin,
            DistortionConstants::WaveShaper<float>::pointYMaxLin,
            newPointY
        );

        getSampleRemapperPointsVT(stageID, distortionUnitID).getChild(pointID).setProperty(SampleRemapperConstants::ParamStringID::pointY, newPointY, &undoManager);
    }
    void setPointXNoListener(int stageID, int distortionUnitID, int pointID, float newPointX)
    {
        newPointX = juce::jlimit(getPoint(stageID, distortionUnitID, pointID + 1).getX(),
            getPoint(stageID, distortionUnitID, pointID - 1).getX(),
            newPointX
        );

        getSampleRemapperPointsVT(stageID, distortionUnitID).getChild(pointID).setPropertyExcludingListener(this, SampleRemapperConstants::ParamStringID::pointX, newPointX, &undoManager);
    }
    void setPointYNoListener(int stageID, int distortionUnitID, int pointID, float newPointY)
    {
        newPointY = juce::jlimit(DistortionConstants::WaveShaper<float>::pointYMinLin,
            DistortionConstants::WaveShaper<float>::pointYMaxLin,
            newPointY
        );

        getSampleRemapperPointsVT(stageID, distortionUnitID).getChild(pointID).setPropertyExcludingListener(this, SampleRemapperConstants::ParamStringID::pointY, newPointY, &undoManager);
    }
    void setSelectedPoint(int stageID, int distortionUnitID, juce::Point<float> newPoint)
    {
        int selectedID = getSelectedPointID(stageID, distortionUnitID);
        setPoint(stageID, distortionUnitID, selectedID, newPoint);
    }
    void setTension(int stageID, int distortionUnitID, int pointID, float newTension)
    {
        getSampleRemapperPointsVT(stageID, distortionUnitID).getChild(pointID).setPropertyExcludingListener(this, SampleRemapperConstants::ParamStringID::tension, newTension, &undoManager);

        bool isBipolar = getIsBipolar(stageID, distortionUnitID);

        if (!isBipolar)
        {
            int nbOfChildren = getSampleRemapperPointsVT(stageID, distortionUnitID).getNumChildren() - 2;
            int mirrorID = nbOfChildren - pointID;
            int curveType = getPointCurveType(stageID, distortionUnitID, pointID);
            float tension = 1.0f;

            if (curveType == DistortionConstants::WaveShaper<float>::CurveTypes::polynomial
                || curveType == DistortionConstants::WaveShaper<float>::CurveTypes::asinh)
            {
                tension = -newTension;
            }
            else
            {
                tension = newTension;
            }

            getSampleRemapperPointsVT(stageID, distortionUnitID).getChild(mirrorID).setPropertyExcludingListener(this, SampleRemapperConstants::ParamStringID::tension, tension, &undoManager);
        }

        getSampleRemapperPointsVT(stageID, distortionUnitID).getChild(pointID).sendPropertyChangeMessage(SampleRemapperConstants::ParamStringID::tension);
    }
    void setTensionNoListener(int stageID, int distortionUnitID, int pointID, float newTension)
    {
        getSampleRemapperPointsVT(stageID, distortionUnitID).getChild(pointID).setPropertyExcludingListener(this, SampleRemapperConstants::ParamStringID::tension, newTension, &undoManager);

        bool isBipolar = getIsBipolar(stageID, distortionUnitID);

        if (!isBipolar)
        {
            int nbOfChildren = getSampleRemapperPointsVT(stageID, distortionUnitID).getNumChildren() - 2;
            int mirrorID = nbOfChildren - pointID;
            int curveType = getPointCurveType(stageID, distortionUnitID, pointID);
            float tension = 1.0f;

            if (curveType == 0)
            {
                tension = -newTension;
            }
            else
            {
                tension = newTension;
            }

            getSampleRemapperPointsVT(stageID, distortionUnitID).getChild(mirrorID).setPropertyExcludingListener(this, SampleRemapperConstants::ParamStringID::tension, tension, &undoManager);
        }
    }
    void setSelectedTension(int stageID, int distortionUnitID, float newTension)
    {
        int selectedID = getSelectedTensionID(stageID, distortionUnitID);
        setTension(stageID, distortionUnitID, selectedID, newTension);
    }
    void setHorizontalDragOn(int stageID, int distortionUnitID, int pointID, bool horizontalDragOn)
    {
        juce::ValueTree vtWSPoints = getSampleRemapperPointsVT(stageID, distortionUnitID).getChild(pointID);

        vtWSPoints.setPropertyExcludingListener(this, SampleRemapperConstants::ParamStringID::horizontalDragOn, horizontalDragOn, nullptr);
    }
    void setPointCurveType(int stageID, int distortionUnitID, int pointID, int curveType)
    {
        if (pointID >= 0)
        {
            juce::ValueTree vtWSPoints = getSampleRemapperPointsVT(stageID, distortionUnitID).getChild(pointID);

            vtWSPoints.setPropertyExcludingListener(this, SampleRemapperConstants::ParamStringID::curveType, curveType, &undoManager);

            if (!getIsBipolar(stageID, distortionUnitID))
            {
                int nbOfChildren = getSampleRemapperPointsVT(stageID, distortionUnitID).getNumChildren() - 2;
                int mirrorID = nbOfChildren - pointID;
                vtWSPoints = getSampleRemapperPointsVT(stageID, distortionUnitID).getChild(mirrorID);
                vtWSPoints.setPropertyExcludingListener(this, SampleRemapperConstants::ParamStringID::curveType, curveType, &undoManager);

                float tension = getTension(stageID, distortionUnitID, pointID);
                setTensionNoListener(stageID, distortionUnitID, mirrorID, tension);
            }

            if (curveType == DistortionConstants::WaveShaper<float>::CurveTypes::polynomial)
            {
                float tension = getTension(stageID, distortionUnitID, pointID);
                setTensionNoListener(stageID, distortionUnitID, pointID, -tension);
            }
            getSampleRemapperPointsVT(stageID, distortionUnitID).getChild(pointID).sendPropertyChangeMessage(SampleRemapperConstants::ParamStringID::curveType);
            undoManager.beginNewTransaction();
        }
    }
    void setPointCurveTypeNoListener(int stageID, int distortionUnitID, int pointID, int curveType)
    {
        if (pointID >= 0)
        {
            juce::ValueTree vtWSPoints = getSampleRemapperPointsVT(stageID, distortionUnitID).getChild(pointID);

            vtWSPoints.setPropertyExcludingListener(this, SampleRemapperConstants::ParamStringID::curveType, curveType, &undoManager);

            //float tension = getTension(stageID, distortionUnitID, pointID);

            if (!getIsBipolar(stageID, distortionUnitID))
            {
                int nbOfChildren = getSampleRemapperPointsVT(stageID, distortionUnitID).getNumChildren() - 2;
                int mirrorID = nbOfChildren - pointID;
                vtWSPoints = getSampleRemapperPointsVT(stageID, distortionUnitID).getChild(mirrorID);
                vtWSPoints.setPropertyExcludingListener(this, SampleRemapperConstants::ParamStringID::curveType, curveType, &undoManager);
            }
        }
    }
    void setIsBipolar(int stageID, int distortionUnitID, bool isBipolar)
    {
        undoManager.beginNewTransaction();

        getSampleRemapperVT(stageID, distortionUnitID).setPropertyExcludingListener(this, SampleRemapperConstants::ParamStringID::isBipolar, isBipolar, &undoManager);

        isBipolar ? switchFromUnipolarToBipolar(stageID, distortionUnitID) : switchFromBipolarToUnipolar(stageID, distortionUnitID);

    }
    void setSelectedIDPoint(int stageID, int distortionUnitID, int pointID)
    {
        bool isBipolar = getIsBipolar(stageID, distortionUnitID);
        int nbOfPoint = getNbOfPoints(stageID, distortionUnitID);

        if (isBipolar)
        {
            getSampleRemapperVT(stageID, distortionUnitID).setPropertyExcludingListener(this, SampleRemapperConstants::ParamStringID::selectedPointID, pointID, &undoManager);
            //saveSelectedPoint();
        }
        else
        {
            if ((pointID < nbOfPoint / 2))
            {
                getSampleRemapperVT(stageID, distortionUnitID).setPropertyExcludingListener(this, SampleRemapperConstants::ParamStringID::selectedPointID, pointID, &undoManager);
                //saveSelectedPoint();
            }
        }
    }
    void setSelectedIDTension(int stageID, int distortionUnitID, int tensionID)
    {
        getSampleRemapperVT(stageID, distortionUnitID).setPropertyExcludingListener(this, SampleRemapperConstants::ParamStringID::selectedTensionID, tensionID, &undoManager);
    }
    void setSelectedIDCurve(int stageID, int distortionUnitID, int curveID)
    {
        int nbOfPoints = getNbOfPoints(stageID, distortionUnitID);

        getIsBipolar(stageID, distortionUnitID) ? nbOfPoints = nbOfPoints - 1 : nbOfPoints = nbOfPoints / 2 - 1;

        if (curveID == nbOfPoints)
        {
            curveID--;
        }

        if (curveID < 0)
            curveID = 0;

        getSampleRemapperVT(stageID, distortionUnitID).setPropertyExcludingListener(this, SampleRemapperConstants::ParamStringID::selectedCurveID, curveID, &undoManager);
    }
    void setPointAndTensionID(int stageID, int distortionUnitID, int pointID, int tensionID)
    {
        undoManager.beginNewTransaction();

        setSelectedIDPoint(stageID, distortionUnitID, pointID);
        setSelectedIDTension(stageID, distortionUnitID, tensionID);

        if (pointID != -1)
        {
            setSelectedIDCurve(stageID, distortionUnitID, pointID);
        }
        else if (tensionID != -1)
        {
            setSelectedIDCurve(stageID, distortionUnitID, tensionID);
        }

        //getWaveShaperVT(stageID, distortionUnitID).sendPropertyChangeMessage(SampleRemapperConstants::ParamStringID::selectedCurveID);
    }
    void setPointAndTensionIDNoTransaction(int stageID, int distortionUnitID, int pointID, int tensionID)
    {
        setSelectedIDPoint(stageID, distortionUnitID, pointID);
        setSelectedIDTension(stageID, distortionUnitID, tensionID);

        if (pointID != -1)
        {
            setSelectedIDCurve(stageID, distortionUnitID, pointID);
        }
        else if (tensionID != -1)
        {
            setSelectedIDCurve(stageID, distortionUnitID, tensionID);
        }
    }

    //void setDistortionCircuitEquation(int stageID, int distortionUnitID, int equationType, int equationID)
    //{
    //    //undoManager.beginNewTransaction();
    //    juce::ValueTree vt = getDistortionCircuitVT(stageID, distortionUnitID);
    //    //vt.setPropertyExcludingListener(this, DistortionCircuitConstants::ParamsID::equationType, equationType, &undoManager);
    //    vt.setProperty(DistortionCircuitConstants::ParamsID::equationType, equationType, &undoManager);
    //
    //    vt.setProperty(DistortionCircuitConstants::ParamsID::equationID, equationID, &undoManager);
    //    
    //}

    //Get function==============================================================================================================================
    inline juce::ValueTree getMainLayerVT()
    {
        return rootAudioEngine.getChildWithName(juce::Identifier(MainLayerConstants::ParamStringID::mainLayer));
    }
    inline juce::ValueTree getStageVT(int stageID)
    {
        juce::Identifier name(MainLayerConstants::ParamStringID::stage + (juce::String)stageID);

        return getMainLayerVT().getChildWithName(name);
    }
    inline juce::ValueTree getSelectedStageVT()
    {
        int selectedStageID = getSelectedStageID();

        return getStageVT(selectedStageID);
    }
    inline juce::ValueTree getScopeVT(int stageID)
    {
        return getStageVT(stageID).getChildWithName(ScopeConstants::ParamStringID::scope);
    }
    inline juce::ValueTree getEQVT(int stageID)
    {
        return getStageVT(stageID).getChildWithName(EQConstants::ParamStringID::eq);
    }
    inline juce::ValueTree getDistortionVT(int stageID)
    {
        return getStageVT(stageID).getChildWithName(DistortionConstants::ParamStringID::distortion);
    }
    inline juce::ValueTree getDistoUnitVT(int stageID, int distortionUnitID)
    {
        juce::Identifier name(DistoUnitConstants::ParamStringID::distoUnit + (juce::String)distortionUnitID);

        return getDistortionVT(stageID).getChildWithName(name);
    }
    inline juce::ValueTree getSelectedDistoUnitVT()
    {
        int selectedStageID = getSelectedStageID();
        juce::ValueTree distoVT = getDistortionVT(selectedStageID);
        int selectedDistoUnit = getSelectedDistoUnitID();

        return distoVT.getChild(selectedDistoUnit);
    }
    inline juce::ValueTree getSampleRemapperVT(int stageID, int distortionUnitID)
    {
        return getDistoUnitVT(stageID, distortionUnitID).getChildWithName(SampleRemapperConstants::ParamStringID::sampleRemapper);
    }
    inline juce::ValueTree getDistortionCircuitVT(int stageID, int distortionUnitID)
    {
        return getDistoUnitVT(stageID, distortionUnitID).getChildWithName(DistortionCircuitConstants::ParamsID::distortionCircuit);
    }
    inline juce::ValueTree getSampleRemapperPointsVT(int stageID, int distortionUnitID)
    {
        return getSampleRemapperVT(stageID, distortionUnitID).getChildWithName(SampleRemapperConstants::ParamStringID::srPoints);
    }

    //Get Function Audio Engine
    int getSelectedStateID()
    {
        return rootAudioEngine.getProperty(AudioEngineConstants::ParamStringID::selectedState);
    }

    //Get function Main Layer

    //Get Function Stage
    inline int getSelectedStageID()
    {
        return getMainLayerVT().getProperty(StageConstants::ParamStringID::selectedStageID);
    }
    inline int getStageProcessorID(int stageID)
    {
        return getStageVT(stageID).getProperty(StageConstants::ParamStringID::displayedProcessor);
    }
    inline int getSelectedStageProcessorID()
    {
        return getSelectedStageVT().getProperty(StageConstants::ParamStringID::displayedProcessor);
    }
    //Get Function Scope
    inline int getScopeZoom(int stageID)
    {
        juce::ValueTree vtScope = getScopeVT(stageID);

        return vtScope.getProperty(ScopeConstants::ParamStringID::zoom);
    }

    //Get Function EQ


    //Get Function Distortion
    bool getDistoUnitRouting(int stageID, int distortionUnitID)
    {
        juce::String paramString = DistoUnitConstants::ParamStringID::GetParamStringID::routing(stageID, distortionUnitID);
        bool distoRouting = *apvts.getRawParameterValue(paramString);

        return distoRouting;
    }
    float getDistoUnitDrive(int stageID, int distortionUnitID)
    {
        juce::String paramString = DistoUnitConstants::ParamStringID::GetParamStringID::drive(stageID, distortionUnitID);
        float distoEquationType = *apvts.getRawParameterValue(paramString);

        return distoEquationType;
    }
    int getSelectedDistoUnitID()
    {
        int selectedStageID = getSelectedStageID();
        juce::ValueTree distoVT = getDistortionVT(selectedStageID);

        return distoVT.getProperty(DistoUnitConstants::ParamStringID::selectedDistoUnitID);
    }
    int getDistortionUnitID(int stageID)
    {
        juce::ValueTree vtDistortion = getDistortionVT(stageID);

        return vtDistortion.getProperty(DistoUnitConstants::ParamStringID::selectedDistoUnitID);
    }
    int getNbOfPoints(int stageID, int distortionUnitID)
    {
        return getSampleRemapperPointsVT(stageID, distortionUnitID).getNumChildren();
    }
    float getPointX(int stageID, int distortionUnitID, int pointID)
    {
        return getSampleRemapperPointsVT(stageID, distortionUnitID).getChild(pointID).getProperty("PointX");
    }
    float getPointY(int stageID, int distortionUnitID, int pointID)
    {
        return getSampleRemapperPointsVT(stageID, distortionUnitID).getChild(pointID).getProperty("PointY");
    }
    juce::Point<float> getPoint(int stageID, int distortionUnitID, int pointID)
    {
        float pointX = getPointX(stageID, distortionUnitID, pointID);
        float pointY = getPointY(stageID, distortionUnitID, pointID);

        juce::Point<float> point(pointX, pointY);

        return point;
    }
    juce::Point<float> getSelectedPoint(int stageID, int distortionUnitID)
    {
        int selectedID = getSelectedPointID(stageID, distortionUnitID);
        return getPoint(stageID, distortionUnitID, selectedID);
    }
    float getTension(int stageID, int distortionUnitID, int pointID)
    {
        return getSampleRemapperPointsVT(stageID, distortionUnitID).getChild(pointID).getProperty(SampleRemapperConstants::ParamStringID::tension);
    }
    int getSelectedPointID(int stageID, int distortionUnitID)
    {
        return getSampleRemapperVT(stageID, distortionUnitID).getProperty(SampleRemapperConstants::ParamStringID::selectedPointID);
    }
    int getSelectedTensionID(int stageID, int distortionUnitID)
    {

        return getSampleRemapperVT(stageID, distortionUnitID).getProperty(SampleRemapperConstants::ParamStringID::selectedTensionID);
    }
    int getSelectedCurveID(int stageID, int distortionUnitID)
    {
        return getSampleRemapperVT(stageID, distortionUnitID).getProperty(SampleRemapperConstants::ParamStringID::selectedCurveID);
    }
    bool getHorizontalDragOn(int stageID, int distortionUnitID, int pointID)
    {
        return getSampleRemapperPointsVT(stageID, distortionUnitID).getChild(pointID).getProperty(SampleRemapperConstants::ParamStringID::horizontalDragOn);
    }
    int getPointCurveType(int stageID, int distortionUnitID, int pointID)
    {
        return getSampleRemapperPointsVT(stageID, distortionUnitID).getChild(pointID).getProperty(SampleRemapperConstants::ParamStringID::curveType);
    }
    bool getIsBipolar(int stageID, int distortionUnitID)
    {
        return getSampleRemapperVT(stageID, distortionUnitID).getProperty(SampleRemapperConstants::ParamStringID::isBipolar);
    }

    //int getDistortionCircuitEquationType(int stageID, int distortionUnitID)
    //{
    //    return getDistortionCircuitVT(stageID, distortionUnitID).getProperty(DistortionCircuitConstants::ParamsID::equationType);
    //}
    //
    //int getDistortionCircuitEquationID(int stageID, int distortionUnitID)
    //{
    //    return getDistortionCircuitVT(stageID, distortionUnitID).getProperty(DistortionCircuitConstants::ParamsID::equationID);
    //}

    void setShouldUpdate(bool newValue) { shouldUpdate = newValue; };
    bool getShouldUpdate() { return shouldUpdate; };

    void trigListener() { getRoot().sendPropertyChangeMessage(StageConstants::ParamStringID::selectedStageID); };
    //MainLayerState* getMainLayerState() { return &mainLayerState; };
    //EQState& getEQState() { return eqState; };
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

        parameters.push_back(std::make_unique<juce::AudioParameterFloat>(paramString,
            automationParamString,
            normalisableMasterGainRange,
            AudioEngineConstants::Processor<SampleType>::masterGainStartValue));

        paramString = AudioEngineConstants::ParamStringID::GetParamStringID::masterLimiterOnOff();
        automationParamString = AudioEngineConstants::AutomationString::GetString::masterLimiterOnOff();

        parameters.push_back(std::make_unique<juce::AudioParameterBool>(paramString,
            automationParamString,
            AudioEngineConstants::Processor<SampleType>::masterLimiterIsBypassedStartValue));

        MainLayerState::createParametersLayout(&parameters, nbOfStages, nbOfFiltersPerEQ, nbOfDistoUnitPerDisto);

        return { parameters.begin(), parameters.end() };
    };
    juce::UndoManager undoManager;
    juce::AudioProcessorValueTreeState apvts;
    //MainLayerState mainLayerState;
    //StageState stageState;
    //FFTState fftState;
    //EQState eqState;
    //PhaserState phaserState;
    //DistortionState distortionState;
    
    //juce::AudioProcessorValueTreeState apvts;
    juce::ValueTree rootAudioEngine;// = juce::ValueTree(AudioEngineConstants::ParamStringID::audioEngine);

    std::atomic<bool> shouldUpdate;
};