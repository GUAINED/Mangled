/*
  ==============================================================================

    Identifiers.h
    Created: 23 Sep 2021 3:17:48pm
    Author:  CookiMonstor

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

typedef juce::AudioProcessorValueTreeState::SliderAttachment SliderAttachment;
typedef juce::AudioProcessorValueTreeState::ButtonAttachment ButtonAttachment;
typedef juce::AudioProcessorValueTreeState::ComboBoxAttachment ComboBoxAttachment;

namespace AudioEngineConstants
{
    template <typename SampleType>
    class Processor
    {
    public:
        static constexpr bool masterLimiterIsBypassedStartValue = false;
        static constexpr SampleType masterGainStartValue = static_cast<SampleType>(-12.0);
        static constexpr SampleType masterGainMin = static_cast<SampleType>(-100.0);
        static constexpr SampleType masterGainMax = static_cast<SampleType>(12.0);
        static constexpr SampleType masterGaindBIncrement = static_cast<SampleType>(0.1);
        static constexpr SampleType masterLimiterReleaseValueMs = static_cast<SampleType>(400.0);
        static constexpr SampleType masterLimiterThresholdValuedB = static_cast<SampleType>(-3.0);
        static constexpr int nbOfStates = 4;


        enum StateID
        {
            A,
            B,
            C,
            D,
            maxStateID
        };
    };

    class UI
    {
    public:
        static constexpr int uiWidth = 800;
        static constexpr int uiHeight = 600;
        static constexpr int refreshRateHz = 30;
        static juce::StringArray stateString;
        static const juce::Colour neonGreen;
        static const juce::Colour darkerNeonGreen;
        static const juce::Colour darkNeonGreen;
        static const juce::Colour darkestNeonGreen;

        

    };

    namespace ParamStringID
    {
        static const juce::String masterLimiterOnOff{ "MASTERLIMITERONOFF" };
        static const juce::String masterGain{ "MASTERGAIN" };
        static const juce::String mainLayerOnOff;
        static const juce::Identifier audioEngine{ "AUDIOENGINE" };
        static const juce::Identifier selectedState{ "STATE" };

        class GetParamStringID
        {
        public:
            static inline juce::String masterLimiterOnOff()
            {
                juce::String paramString = AudioEngineConstants::ParamStringID::masterLimiterOnOff;

                return paramString;
            };
            static inline juce::String masterGain()
            {
                juce::String paramString = AudioEngineConstants::ParamStringID::masterGain;

                return paramString;
            };
        };
    };

    namespace AutomationString
    {
        static const juce::String masterLimiterOnOff{ "Master Limiter On Off" };
        static const juce::String masterGain{ "Master Gain" };

        class GetString
        {
        public:
            static inline juce::String masterLimiterOnOff()
            {
                juce::String paramString = AudioEngineConstants::AutomationString::masterLimiterOnOff;

                return paramString;
            };
            static inline juce::String masterGain()
            {
                juce::String paramString = AudioEngineConstants::AutomationString::masterGain;

                return paramString;
            };
        };
    };
};

namespace MainLayerConstants
{
    template <typename SampleType>
    class Processor
    {
    public:
        static constexpr bool isBypassedStartValue = false;
        static constexpr int nbOfStageMax = 4;
    };

    namespace ParamStringID
    {
        static const juce::String mainLayer{ "ML" };
        static const juce::String isBypassed{ "ISBYPASSED" };
        static const juce::String stage{ "S" };
        static const juce::String postProcessor{ "PP" };

        class GetParamStringID
        {
        public:
            static inline juce::String isBypassed()
            {
                juce::String paramString = MainLayerConstants::ParamStringID::mainLayer
                    + MainLayerConstants::ParamStringID::isBypassed;

                return paramString;
            };
            static inline juce::String postProcessingOnOff()
            {
                juce::String paramString = MainLayerConstants::ParamStringID::mainLayer
                    + MainLayerConstants::ParamStringID::postProcessor
                    + MainLayerConstants::ParamStringID::isBypassed;

                return paramString;
            };
        };
    };

    namespace AutomationString
    {
        static const juce::String mainLayer{ "Main Layer " };
        static const juce::String stage{ "Stage " };
        static const juce::String postProcessor{ "Post Processing " };
        static const juce::String isBypassed{ "Is Bypassed" };

        class GetString
        {
        public:
            static inline juce::String isBypassed()
            {
                juce::String paramString = MainLayerConstants::AutomationString::mainLayer
                    + MainLayerConstants::AutomationString::isBypassed;

                return paramString;
            };
            static inline juce::String postProcessingOnOff()
            {
                juce::String paramString = MainLayerConstants::AutomationString::mainLayer
                    + MainLayerConstants::AutomationString::postProcessor
                    + MainLayerConstants::AutomationString::isBypassed;

                return paramString;
            };
        };
    };
};

namespace StageConstants
{
    template <typename SampleType>
    class Processor
    {
    public:
        static constexpr SampleType gainStartValue = static_cast<SampleType>(0.0);
        static constexpr SampleType gainMin = static_cast<SampleType>(-100.0);
        static constexpr SampleType gainMax = static_cast<SampleType>(12.0);
        static constexpr SampleType gaindBIncrement = static_cast<SampleType>(0.1);
        static constexpr bool isBypassedStartValue = false;
    };

    class UI
    {
    public:
        static constexpr int sliderHeight = 75;
        static constexpr int labelWidth = 75;
        static constexpr int labelHeight = 20;
        static constexpr int pixelSpace = 10;
    };

    namespace ParamStringID
    {
        static const juce::String inputGain{ "INPUTGAIN" };
        static const juce::String outputGain{ "OUTPUTGAIN" };
        static const juce::String isBypassed{ "ISBYPASSED" };

        static const juce::Identifier selectedStageID("SelectedStageID");
        static const juce::Identifier isEQ("IsEQ");

        class GetParamStringID
        {
        public:
            static inline juce::String inputGain(int stageID) 
            {
                juce::String paramString = MainLayerConstants::ParamStringID::mainLayer
                    + MainLayerConstants::ParamStringID::stage
                    + (juce::String)stageID
                    + StageConstants::ParamStringID::inputGain;

                return paramString;
            };
            static inline juce::String outputGain(int stageID)
            {
                juce::String paramString = MainLayerConstants::ParamStringID::mainLayer
                    + MainLayerConstants::ParamStringID::stage
                    + (juce::String)stageID
                    + StageConstants::ParamStringID::outputGain;

                return paramString;
            };
            static inline juce::String isBypassed(int stageID)
            {
                juce::String paramString = MainLayerConstants::ParamStringID::mainLayer
                    + MainLayerConstants::ParamStringID::stage
                    + (juce::String)stageID
                    + StageConstants::ParamStringID::isBypassed;

                return paramString;
            };
        };
    };

    namespace AutomationString
    {
        static const juce::String inputGain{ "Input Gain" };
        static const juce::String outputGain{ "Output Gain" };
        static const juce::String isBypassed{ "Is Bypassed" };

        class GetString
        {
        public:
            static inline juce::String inputGain(int stageID)
            {
                juce::String paramString = MainLayerConstants::AutomationString::mainLayer
                                         + MainLayerConstants::AutomationString::stage
                                         + (juce::String)stageID + " "
                                         + StageConstants::AutomationString::inputGain;

                return paramString;
            };
            static inline juce::String outputGain(int stageID) 
            {
                juce::String paramString = MainLayerConstants::AutomationString::mainLayer
                    + MainLayerConstants::AutomationString::stage
                    + (juce::String)stageID + " "
                    + StageConstants::AutomationString::outputGain;

                return paramString;
            };
            static inline juce::String isBypassed(int stageID)
            {
                juce::String paramString = MainLayerConstants::AutomationString::mainLayer
                    + MainLayerConstants::AutomationString::stage
                    + (juce::String)stageID + " "
                    + StageConstants::AutomationString::isBypassed;

                return paramString;
            };
        };
    };
};

namespace RMSConstants
{
    template <typename SampleType>
    class Processor
    {
    public:
        static constexpr SampleType subViewCutoffMin = static_cast<SampleType>(10.0);
        static constexpr SampleType subViewCutoffMax = static_cast<SampleType>(300.0);
        static constexpr SampleType subViewCutoffIncrement = static_cast<SampleType>(0.01 * 0.5);
        static constexpr SampleType subViewCutoffStartValue = static_cast<SampleType>(110.0);
        static constexpr bool isNormalizedStartValue = true;
        static constexpr bool subViewIsBypassedStartValue = true;
        static constexpr bool monoViewIsBypassedStartValue = true;
    };

    class UI
    {

    };

    namespace ParamStringID
    {
        static const juce::String rms{ "RMS" };
        static const juce::String isNormalized{ "ISNORMALIZED" };
        static const juce::String subView{ "SUBVIEW" };
        static const juce::String cutoff{ "CUTOFF" };
        static const juce::String monoView{ "MONOVIEW" };
        static const juce::String isBypassed{ "ISBYPASSED" };

        class GetParamStringID
        {
        public:
            static inline juce::String isNormalized(int stageID)
            {
                juce::String paramString = MainLayerConstants::ParamStringID::mainLayer
                    + MainLayerConstants::ParamStringID::stage
                    + (juce::String)stageID
                    + RMSConstants::ParamStringID::rms
                    + RMSConstants::ParamStringID::isNormalized;

                return paramString;
            };
            static inline juce::String subViewIsBypassed(int stageID)
            {
                juce::String paramString = MainLayerConstants::ParamStringID::mainLayer
                    + MainLayerConstants::ParamStringID::stage
                    + (juce::String)stageID
                    + RMSConstants::ParamStringID::rms
                    + RMSConstants::ParamStringID::subView
                    + RMSConstants::ParamStringID::isBypassed;

                return paramString;
            };
            static inline juce::String subViewCutoff(int stageID)
            {
                juce::String paramString = MainLayerConstants::ParamStringID::mainLayer
                    + MainLayerConstants::ParamStringID::stage
                    + (juce::String)stageID
                    + RMSConstants::ParamStringID::rms
                    + RMSConstants::ParamStringID::subView
                    + RMSConstants::ParamStringID::cutoff;

                return paramString;
            };
            static inline juce::String monoViewIsBypassed(int stageID)
            {
                juce::String paramString = MainLayerConstants::ParamStringID::mainLayer
                    + MainLayerConstants::ParamStringID::stage
                    + (juce::String)stageID
                    + RMSConstants::ParamStringID::rms
                    + RMSConstants::ParamStringID::monoView
                    + RMSConstants::ParamStringID::isBypassed;

                return paramString;
            };
        };
    };

    namespace AutomationString
    {
        static const juce::String rms{ "RMS " };
        static const juce::String monoView{ "Mono View " };
        static const juce::String subView{ "Sub View " };
        static const juce::String cutoff{ "Cutoff" };
        static const juce::String isNormalized{ "Is Normalized" };
        static const juce::String isBypassed{ "Is Bypassed" };

        class GetString
        {
        public:
            static inline juce::String isNormalized(int stageID)
            {
                juce::String paramString = MainLayerConstants::AutomationString::mainLayer
                    + MainLayerConstants::AutomationString::stage
                    + (juce::String)stageID + " "
                    + RMSConstants::AutomationString::rms
                    + RMSConstants::AutomationString::isNormalized;

                return paramString;
            };
            static inline juce::String subViewIsBypassed(int stageID)
            {
                juce::String paramString = MainLayerConstants::AutomationString::mainLayer
                    + MainLayerConstants::AutomationString::stage
                    + (juce::String)stageID + " "
                    + RMSConstants::AutomationString::rms
                    + RMSConstants::AutomationString::subView
                    + RMSConstants::AutomationString::isBypassed;

                return paramString;
            };
            static inline juce::String subViewCutoff(int stageID)
            {
                juce::String paramString = MainLayerConstants::AutomationString::mainLayer
                    + MainLayerConstants::AutomationString::stage
                    + (juce::String)stageID + " "
                    + RMSConstants::AutomationString::rms
                    + RMSConstants::AutomationString::subView
                    + RMSConstants::AutomationString::cutoff;

                return paramString;
            };
            static inline juce::String monoViewIsBypassed(int stageID)
            {
                juce::String paramString = MainLayerConstants::AutomationString::mainLayer
                    + MainLayerConstants::AutomationString::stage
                    + (juce::String)stageID + " "
                    + RMSConstants::AutomationString::rms
                    + RMSConstants::AutomationString::monoView
                    + RMSConstants::AutomationString::isBypassed;

                return paramString;
            };
        };
    };
};

namespace ScopeConstants
{
    template <typename SampleType>
    class Processor
    {
    public:

        static constexpr int modeStartValue = 0;
        static constexpr bool preEQIsBypassedStartValue = false;
        static constexpr bool postEQIsBypassedStartValue = false;
        static constexpr bool postDistoIsBypassedStartValue = false;
    };

    class UI
    {
    public:
        static constexpr int sliderHeight = 75;
        static constexpr int labelWidth = 75;
        static constexpr float gainLabelWidth = 50.0f;
        static constexpr int labelHeight = 20;
        static constexpr int pixelSpace = 10;

        enum ZoomID
        {
            thirtydB,
            fifteendB,
            ninedB,
            sixdB,
            threedB,
            maxZoomID
        };

        class thirtydB
        {
        public:
            template <typename SampleType>
            class Constants
            {
            public:
                static constexpr SampleType minGaindB = static_cast<SampleType>(-35.0);
                static constexpr SampleType maxGaindB = static_cast<SampleType>(35.0);
                static constexpr SampleType gainLabelIncdB = static_cast<SampleType>(10.0);
                static constexpr SampleType qMin = static_cast<SampleType>(0.025);
                static constexpr SampleType qMax = static_cast<SampleType>(40.0);
            };

            static juce::StringArray gridLabelStringArray;// { "30 dB", "+20 dB", "+10 dB", "+ 0 dB", "-10 dB", "-20 dB", "-30 dB" };

        };

        class fifteendB
        {
        public:
            template <typename SampleType>
            class Constants
            {
            public:
                static constexpr SampleType minGaindB = static_cast<SampleType>(-17.5);
                static constexpr SampleType maxGaindB = static_cast<SampleType>(17.5);
                static constexpr SampleType gainLabelIncdB = static_cast<SampleType>(5.0);
                static constexpr SampleType qMin = static_cast<SampleType>(0.13);
                static constexpr SampleType qMax = static_cast<SampleType>(7.5);
            };

            static juce::StringArray gridLabelStringArray;// ("15 dB", "+10 dB", "+5 dB", "+ 0 dB", "-5 dB", "-10 dB", "-15 dB");
        };

        class ninedB
        {
        public:
            template <typename SampleType>
            class Constants
            {
            public:
                static constexpr SampleType minGaindB = static_cast<SampleType>(-10.5);
                static constexpr SampleType maxGaindB = static_cast<SampleType>(10.5);
                static constexpr SampleType gainLabelIncdB = static_cast<SampleType>(3.0);
                static constexpr SampleType qMin = static_cast<SampleType>(0.3);
                static constexpr SampleType qMax = static_cast<SampleType>(3.3);
            };

            static juce::StringArray gridLabelStringArray;// ("9 dB", "+6 dB", "+3 dB", "+ 0 dB", "-3 dB", "-6 dB", "-9 dB");
        };

        class sixdB
        {
        public:
            template <typename SampleType>
            class Constants
            {
            public:
                static constexpr SampleType minGaindB = static_cast<SampleType>(-7.0);
                static constexpr SampleType maxGaindB = static_cast<SampleType>(7.0);
                static constexpr SampleType gainLabelIncdB = static_cast<SampleType>(2.0);
                static constexpr SampleType qMin = static_cast<SampleType>(0.44);
                static constexpr SampleType qMax = static_cast<SampleType>(2.2);
            };

            static juce::StringArray gridLabelStringArray;// ("6 dB", "+4 dB", "+2 dB", "+ 0 dB", "-2 dB", "-4 dB", "-6 dB");

        };

        class threedB
        {
        public:
            template <typename SampleType>
            class Constants
            {
            public:
                static constexpr SampleType minGaindB = static_cast<SampleType>(-3.5);
                static constexpr SampleType maxGaindB = static_cast<SampleType>(3.5);
                static constexpr SampleType gainLabelIncdB = static_cast<SampleType>(1.0);
                static constexpr SampleType qMin = static_cast<SampleType>(0.65);
                static constexpr SampleType qMax = static_cast<SampleType>(1.4);
            };

            static juce::StringArray gridLabelStringArray;// ("3 dB", "+2 dB", "+1 dB", "+ 0 dB", "-1 dB", "-2 dB", "-3 dB");
        };

        static juce::StringArray modeStringArray;// ("FFT", "Temporal", "RMS", "Sound Field");
    };

    namespace ParamStringID
    {
        //static const juce::String scope{ "SCOPE" };
        static const juce::String mode{ "MODE" };
        static const juce::String preEQ{ "PREEQ" };
        static const juce::String postEQ{ "POSTEQ" };
        static const juce::String postDisto{ "POSTDISTO" };
        static const juce::String isNormalized{ "ISNORMALIZED" };
        static const juce::String subView{ "SUBVIEW" };
        static const juce::String cutoff{ "CUTOFF" };
        static const juce::String monoView{ "MONOVIEW" };
        static const juce::String isBypassed{ "ISBYPASSED" };

        static const juce::Identifier scope("Scope");
        static const juce::Identifier zoom("Zoom");

        class GetParamStringID
        {
        public:
            static inline juce::String preEQIsBypassed(int stageID)
            {
                juce::String paramString = MainLayerConstants::ParamStringID::mainLayer
                    + MainLayerConstants::ParamStringID::stage
                    + (juce::String)stageID
                    + ScopeConstants::ParamStringID::scope
                    + ScopeConstants::ParamStringID::preEQ
                    + ScopeConstants::ParamStringID::isBypassed;

                return paramString;
            };
            static inline juce::String postEQIsBypassed(int stageID)
            {
                juce::String paramString = MainLayerConstants::ParamStringID::mainLayer
                    + MainLayerConstants::ParamStringID::stage
                    + (juce::String)stageID
                    + ScopeConstants::ParamStringID::scope
                    + ScopeConstants::ParamStringID::postEQ
                    + ScopeConstants::ParamStringID::isBypassed;

                return paramString;
            };
            static inline juce::String postDistoIsBypassed(int stageID)
            {
                juce::String paramString = MainLayerConstants::ParamStringID::mainLayer
                    + MainLayerConstants::ParamStringID::stage
                    + (juce::String)stageID
                    + ScopeConstants::ParamStringID::scope
                    + ScopeConstants::ParamStringID::postDisto
                    + ScopeConstants::ParamStringID::isBypassed;

                return paramString;
            };
            static inline juce::String dataType(int stageID)
            {
                juce::String paramString = MainLayerConstants::ParamStringID::mainLayer
                    + MainLayerConstants::ParamStringID::stage
                    + (juce::String)stageID
                    + ScopeConstants::ParamStringID::scope
                    + ScopeConstants::ParamStringID::mode;

                return paramString;
            };
        };
    };

    namespace AutomationString
    {
        static const juce::String scope{ "Scope " };
        static const juce::String mode{ "Mode" };
        static const juce::String preEQ{ "Pre EQ " };
        static const juce::String postEQ{ "Post EQ " };
        static const juce::String postDisto{ "Post Distortion " };
        static const juce::String monoView{ "Mono View " };
        static const juce::String subView{ "Sub View " };
        static const juce::String cutoff{ "Cutoff" };
        static const juce::String isNormalized{ "Is Normalized" };
        static const juce::String isBypassed{ "Is Bypassed" };

        class GetString
        {
        public:
            static inline juce::String preEQIsBypassed(int stageID)
            {
                juce::String paramString = MainLayerConstants::AutomationString::mainLayer
                    + MainLayerConstants::AutomationString::stage
                    + (juce::String)stageID + " "
                    + ScopeConstants::AutomationString::scope
                    + ScopeConstants::AutomationString::preEQ
                    + ScopeConstants::AutomationString::isBypassed;

                return paramString;
            };
            static inline juce::String postEQIsBypassed(int stageID)
            {
                juce::String paramString = MainLayerConstants::AutomationString::mainLayer
                    + MainLayerConstants::AutomationString::stage
                    + (juce::String)stageID + " "
                    + ScopeConstants::AutomationString::scope
                    + ScopeConstants::AutomationString::postEQ
                    + ScopeConstants::AutomationString::isBypassed;

                return paramString;
            };
            static inline juce::String postDistoIsBypassed(int stageID)
            {
                juce::String paramString = MainLayerConstants::AutomationString::mainLayer
                    + MainLayerConstants::AutomationString::stage
                    + (juce::String)stageID + " "
                    + ScopeConstants::AutomationString::scope
                    + ScopeConstants::AutomationString::postDisto
                    + ScopeConstants::AutomationString::isBypassed;

                return paramString;
            };
            static inline juce::String dataType(int stageID)
            {
                juce::String paramString = MainLayerConstants::AutomationString::mainLayer
                    + MainLayerConstants::AutomationString::stage
                    + (juce::String)stageID + " "
                    + ScopeConstants::AutomationString::scope
                    + ScopeConstants::AutomationString::mode;

                return paramString;
            };
        };
    };
}

namespace EQConstants
{
    template <typename SampleType>
    class Processor
    {
    public:
        static constexpr SampleType mixMin = static_cast<SampleType>(0.0);
        static constexpr SampleType mixMax = static_cast<SampleType>(100.0);
        static constexpr SampleType mixIncrement = static_cast<SampleType>(0.01);
        static constexpr SampleType mixStartValue = static_cast<SampleType>(100.0);
        static constexpr int nbOfFilterMax = 14;
        static constexpr bool isBypassedStartValue = false;
    };

    template <typename SampleType>
    class BiquadConstants
    {
    public:
        static constexpr SampleType gaindBMin = static_cast<SampleType>(-30.0);
        static constexpr SampleType gaindBMax = static_cast<SampleType>(30.0);
        static constexpr SampleType qMin = static_cast<SampleType>(0.025);
        static constexpr SampleType qMax = static_cast<SampleType>(40.0);
        static constexpr SampleType gaindBStartValue = static_cast<SampleType>(0.0);
        static constexpr SampleType gainLinStartValue = static_cast<SampleType>(1.0);
        static constexpr SampleType qStartValue = static_cast<SampleType>(1.0);
        static constexpr SampleType cutoffMin = static_cast<SampleType>(10.0);
        static constexpr SampleType cutoffMax = static_cast<SampleType>(20000.0);
        static constexpr SampleType cutoffStartValue = static_cast<SampleType>(1000.0);
        static constexpr SampleType cutoffIncrement = static_cast<SampleType>(0.01);
        static constexpr SampleType qIncrement = static_cast<SampleType>(0.001);

        static constexpr int typeStartValue = 0;
        static constexpr int orderStartValue = 0;
        static constexpr bool isBypassedStartValue = true;
        static constexpr bool isActiveStartValue = false; //Link to the "Delete" button. Button = on means filter not active.

        enum Types
        {
            peak,
            lowpass,
            highpass,
            notch,
            bandpass,
            lowshelf,
            highshelf,
            maxTypes
        };

        enum Orders
        {
            first,
            second,
            third,
            fourth,
            maxOrders
        };
    };

    class UI
    {
    public:
        static constexpr int sliderHeight = 75;
        static constexpr int labelWidth = 75;
        static constexpr int labelHeight = 20;
        static constexpr int pixelSpace = 10;
        static constexpr float opacityOff_f = 0.3f;
        static constexpr float opacityOn_f = 1.0f;
        static juce::StringArray typeStringArray; //("Peak", "Low Pass", "High Pass", "Notch", "Band Pass", "LowShelf", "HighShelf")
        static juce::StringArray orderStringArray;

        static constexpr int numberOfDecade = 4;
        static constexpr int numberOfGainLine = 7;
        static constexpr int nbOfBloc = 4;
        static constexpr int nbOfPointsFirstBloc = 100;
        static constexpr int nbOfPointsPerBloc = 600;
        static constexpr int filterMagnitudeSize = nbOfPointsPerBloc * nbOfBloc + nbOfPointsFirstBloc;
        static constexpr int numberOfValuePerDecade = filterMagnitudeSize / numberOfDecade; //Must be an integer

        class FilterColour
        {
        public:
            static juce::Array<juce::Colour> filterColour;
            static const juce::Colour filterSumColour;
            static constexpr float filterSumPathThickness_f = 2.5f;

            static constexpr float innerPathOpacityOff_f = 0.3f;
            static constexpr float innerPathOpacityOn_f = 0.5f;
        };


    };

    namespace ParamStringID
    {
        static const juce::String filter{ "FILTER" };
        static const juce::String cutoff{ "CUTOFF" };
        static const juce::String q{ "Q" };
        static const juce::String gain{ "GAIN" };
        static const juce::String type{ "TYPE" };
        static const juce::String order{ "ORDER" };
        static const juce::String onOff{ "ONOFF" };
        static const juce::String isActive{ "ISACTIVE" };
        static const juce::String mix{ "MIX" };
        static const juce::String isBypassed{ "ISBYPASSED" };

        static const juce::Identifier eq("EQ");
        static const juce::Identifier selectedFilterID("SelectedFilterID");
        static const juce::Identifier nbOfActiveFilters("NbOfActiveFilters");

        class GetParamStringID
        {
        public:
            static inline juce::String mix(int stageID)
            {
                juce::String paramString = MainLayerConstants::ParamStringID::mainLayer
                    + MainLayerConstants::ParamStringID::stage
                    + (juce::String)stageID
                    + EQConstants::ParamStringID::eq
                    + EQConstants::ParamStringID::mix;

                return paramString;
            };
            static inline juce::String isBypassed(int stageID)
            {
                juce::String paramString = MainLayerConstants::ParamStringID::mainLayer
                    + MainLayerConstants::ParamStringID::stage
                    + (juce::String)stageID
                    + EQConstants::ParamStringID::eq
                    + EQConstants::ParamStringID::isBypassed;

                return paramString;
            };
            static inline juce::String filterCutoff(int stageID, int filterID)
            {
                juce::String paramString = MainLayerConstants::ParamStringID::mainLayer
                    + MainLayerConstants::ParamStringID::stage
                    + (juce::String)stageID
                    + EQConstants::ParamStringID::eq
                    + EQConstants::ParamStringID::filter
                    + (juce::String)filterID
                    + EQConstants::ParamStringID::cutoff;

                return paramString;
            };
            static inline juce::String filterQ(int stageID, int filterID)
            {
                juce::String paramString = MainLayerConstants::ParamStringID::mainLayer
                    + MainLayerConstants::ParamStringID::stage
                    + (juce::String)stageID
                    + EQConstants::ParamStringID::eq
                    + EQConstants::ParamStringID::filter
                    + (juce::String)filterID
                    + EQConstants::ParamStringID::q;

                return paramString;
            };
            static inline juce::String filterGain(int stageID, int filterID)
            {
                juce::String paramString = MainLayerConstants::ParamStringID::mainLayer
                    + MainLayerConstants::ParamStringID::stage
                    + (juce::String)stageID
                    + EQConstants::ParamStringID::eq
                    + EQConstants::ParamStringID::filter
                    + (juce::String)filterID
                    + EQConstants::ParamStringID::gain;

                return paramString;
                };
            static inline juce::String filterType(int stageID, int filterID)
            {
                juce::String paramString = MainLayerConstants::ParamStringID::mainLayer
                    + MainLayerConstants::ParamStringID::stage
                    + (juce::String)stageID
                    + EQConstants::ParamStringID::eq
                    + EQConstants::ParamStringID::filter
                    + (juce::String)filterID
                    + EQConstants::ParamStringID::type;

                return paramString;
            };
            static inline juce::String filterOrder(int stageID, int filterID)
            {
                juce::String paramString = MainLayerConstants::ParamStringID::mainLayer
                    + MainLayerConstants::ParamStringID::stage
                    + (juce::String)stageID
                    + EQConstants::ParamStringID::eq
                    + EQConstants::ParamStringID::filter
                    + (juce::String)filterID
                    + EQConstants::ParamStringID::order;

                return paramString;
            };
            static inline juce::String filterIsBypassed(int stageID, int filterID)
            {
                juce::String paramString = MainLayerConstants::ParamStringID::mainLayer
                    + MainLayerConstants::ParamStringID::stage
                    + (juce::String)stageID
                    + EQConstants::ParamStringID::eq
                    + EQConstants::ParamStringID::filter
                    + (juce::String)filterID
                    + EQConstants::ParamStringID::isBypassed;

                return paramString;
            };
            static inline juce::String filterIsActive(int stageID, int filterID)
            {
                juce::String paramString = MainLayerConstants::ParamStringID::mainLayer
                    + MainLayerConstants::ParamStringID::stage
                    + (juce::String)stageID
                    + EQConstants::ParamStringID::eq
                    + EQConstants::ParamStringID::filter
                    + (juce::String)filterID
                    + EQConstants::ParamStringID::isActive;

                return paramString;
            };
        };
    };

    namespace AutomationString
    {
        static const juce::String eq{ "EQ " };
        static const juce::String filter{ "Filter " };
        static const juce::String cutoff{ "Cutoff" };
        static const juce::String q{ "Q" };
        static const juce::String gain{ "Gain" };
        static const juce::String type{ "Type" };
        static const juce::String order{ "Order" };
        static const juce::String onOff{ "On Off" };
        static const juce::String isActive{ "Is Active" };
        static const juce::String mix{ "Mix" };
        static const juce::String isBypassed{ "Is Bypassed" };
        static const juce::String deleteFilter{ "Delete" };

        class GetString
        {
        public:
            static inline juce::String mix(int stageID)
            {
                juce::String paramString = MainLayerConstants::AutomationString::mainLayer
                    + MainLayerConstants::AutomationString::stage
                    + (juce::String)stageID + " "
                    + EQConstants::AutomationString::eq
                    + EQConstants::AutomationString::mix;

                return paramString;
            };
            static inline juce::String isBypassed(int stageID)
            {
                juce::String paramString = MainLayerConstants::AutomationString::mainLayer
                    + MainLayerConstants::AutomationString::stage
                    + (juce::String)stageID + " "
                    + EQConstants::AutomationString::eq
                    + EQConstants::AutomationString::isBypassed;

                return paramString;
            };
            static inline juce::String filterCutoff(int stageID, int filterID)
            {
                juce::String paramString = MainLayerConstants::AutomationString::mainLayer
                    + MainLayerConstants::AutomationString::stage
                    + (juce::String)stageID + " "
                    + EQConstants::AutomationString::eq
                    + EQConstants::AutomationString::filter
                    + (juce::String)filterID + " "
                    + EQConstants::AutomationString::cutoff;

                return paramString;
            };
            static inline juce::String filterQ(int stageID, int filterID)
            {
                juce::String paramString = MainLayerConstants::AutomationString::mainLayer
                    + MainLayerConstants::AutomationString::stage
                    + (juce::String)stageID + " "
                    + EQConstants::AutomationString::eq
                    + EQConstants::AutomationString::filter
                    + (juce::String)filterID + " "
                    + EQConstants::AutomationString::q;

                return paramString;
            };
            static inline juce::String filterGain(int stageID, int filterID)
            {
                juce::String paramString = MainLayerConstants::AutomationString::mainLayer
                    + MainLayerConstants::AutomationString::stage
                    + (juce::String)stageID + " "
                    + EQConstants::AutomationString::eq
                    + EQConstants::AutomationString::filter
                    + (juce::String)filterID + " "
                    + EQConstants::AutomationString::gain;

                return paramString;
            };
            static inline juce::String filterType(int stageID, int filterID)
            {
                juce::String paramString = MainLayerConstants::AutomationString::mainLayer
                    + MainLayerConstants::AutomationString::stage
                    + (juce::String)stageID + " "
                    + EQConstants::AutomationString::eq
                    + EQConstants::AutomationString::filter
                    + (juce::String)filterID + " "
                    + EQConstants::AutomationString::type;

                return paramString;
            };
            static inline juce::String filterOrder(int stageID, int filterID)
            {
                juce::String paramString = MainLayerConstants::AutomationString::mainLayer
                    + MainLayerConstants::AutomationString::stage
                    + (juce::String)stageID + " "
                    + EQConstants::AutomationString::eq
                    + EQConstants::AutomationString::filter
                    + (juce::String)filterID + " "
                    + EQConstants::AutomationString::order;

                return paramString;
            };
            static inline juce::String filterIsBypassed(int stageID, int filterID)
            {
                juce::String paramString = MainLayerConstants::AutomationString::mainLayer
                    + MainLayerConstants::AutomationString::stage
                    + (juce::String)stageID + " "
                    + EQConstants::AutomationString::eq
                    + EQConstants::AutomationString::filter
                    + (juce::String)filterID + " "
                    + EQConstants::AutomationString::isActive;

                return paramString;
            };
            static inline juce::String filterIsActive(int stageID, int filterID)
            {
                juce::String paramString = MainLayerConstants::AutomationString::mainLayer
                    + MainLayerConstants::AutomationString::stage
                    + (juce::String)stageID + " "
                    + EQConstants::AutomationString::eq
                    + EQConstants::AutomationString::filter
                    + (juce::String)filterID + " "
                    + EQConstants::AutomationString::isActive;

                return paramString;
            };

        };
    };
};

namespace PhaserConstants
{
    template <typename SampleType>
    class Processor
    {
    public:
        class Parameters
        {
        public:
            //static const juce::String name = { "PHASER" };
            //static const juce::String automationName{ "Phaser " };
            
            class CentreFrequency
            {
            public:
                static constexpr SampleType limMin = static_cast<SampleType>(20.1);
                static constexpr SampleType limMax = static_cast<SampleType>(19999.9);
                static constexpr SampleType min = static_cast<SampleType>(120.0);
                static constexpr SampleType max = static_cast<SampleType>(12000.0);
                static constexpr SampleType start = static_cast<SampleType>(1000.0);
                //static const juce::String name = { "CENTREFREQUENCY" };
                //static const juce::String automationName{ "Centre Frequency" };

                //static inline juce::String paramStringID(int stageID)
                //{
                //    juce::String paramString = MainLayerConstants::ParamStringID::mainLayer
                //        + MainLayerConstants::ParamStringID::stage
                //        + (juce::String)stageID
                //        + PhaserConstants::ParamStringID::phaser
                //        + name;

                //    return paramString;
                //};

                //static inline juce::String automationString(int stageID)
                //{
                //    juce::String paramString = MainLayerConstants::AutomationString::mainLayer
                //        + MainLayerConstants::AutomationString::stage
                //        + (juce::String)stageID + " "
                //        + PhaserConstants::AutomationString::phaser
                //        + automationName;

                //    return paramString;
                //};
            };
        };

        static constexpr SampleType cfreqLimMin = static_cast<SampleType>(20.1);
        static constexpr SampleType cfreqLimMax = static_cast<SampleType>(19999.9);
        static constexpr SampleType cfreqMin = static_cast<SampleType>(120.0);
        static constexpr SampleType cfreqMax = static_cast<SampleType>(12000.0);
        static constexpr SampleType cfreqStartValue = static_cast<SampleType>(1000.0);
        static constexpr SampleType feedbackMin = static_cast<SampleType>(0.0);
        static constexpr SampleType feedbackMax = static_cast<SampleType>(0.9);
        static constexpr SampleType feedbackStartValue = static_cast<SampleType>(0.0);
        static constexpr SampleType rateMin = static_cast<SampleType>(0.01);
        static constexpr SampleType rateMax = static_cast<SampleType>(80.0);
        static constexpr SampleType rateStartValue = static_cast<SampleType>(1.0);
        static constexpr SampleType depthMin = static_cast<SampleType>(0.0);
        static constexpr SampleType depthMax = static_cast<SampleType>(1.0);
        static constexpr SampleType depthStartValue = static_cast<SampleType>(0.5);
        static constexpr SampleType mixMin = static_cast<SampleType>(0.0);
        static constexpr SampleType mixMax = static_cast<SampleType>(100.0);
        static constexpr SampleType mixStartValue = static_cast<SampleType>(30.0);
        static constexpr int nbOfStgStartValue = 1;
        static constexpr bool isBypassedStartValue = false;
    };

    class UI
    {
    public:
        static juce::StringArray nbOfStgStringArray;// ("2 Stages", "4 Stages", "6 Stages", "8 Namespace");
    };

    namespace ParamStringID
    {
        static const juce::String phaser = { "PHASER" };
        static const juce::String centerFrequency = { "CENTREFREQUENCY" };
        static const juce::String feedback = { "FEEDBACK" };
        static const juce::String rate = { "RATE" };
        static const juce::String depth = { "DEPTH" };
        static const juce::String mix = { "MIX" };
        static const juce::String nbOfStages = { "NUMBEROFSTAGES" };
        static const juce::String onOff = { "ONOFF" };
        static const juce::String isBypassed = { "ISBYPASSED" };

        class GetParamStringID
        {
        public:
            static inline juce::String centerFrequency(int stageID)
            {
                juce::String paramString = MainLayerConstants::ParamStringID::mainLayer
                    + MainLayerConstants::ParamStringID::stage
                    + (juce::String)stageID
                    + PhaserConstants::ParamStringID::phaser
                    + PhaserConstants::ParamStringID::centerFrequency;

                return paramString;
            };
            static inline juce::String feedback(int stageID)
            {
                juce::String paramString = MainLayerConstants::ParamStringID::mainLayer
                    + MainLayerConstants::ParamStringID::stage
                    + (juce::String)stageID
                    + PhaserConstants::ParamStringID::phaser
                    + PhaserConstants::ParamStringID::feedback;

                return paramString;
            };
            static inline juce::String rate(int stageID)
            {
                juce::String paramString = MainLayerConstants::ParamStringID::mainLayer
                    + MainLayerConstants::ParamStringID::stage
                    + (juce::String)stageID
                    + PhaserConstants::ParamStringID::phaser
                    + PhaserConstants::ParamStringID::rate;

                return paramString;
            };
            static inline juce::String depth(int stageID)
            {
                juce::String paramString = MainLayerConstants::ParamStringID::mainLayer
                    + MainLayerConstants::ParamStringID::stage
                    + (juce::String)stageID
                    + PhaserConstants::ParamStringID::phaser
                    + PhaserConstants::ParamStringID::depth;

                return paramString;
            };
            static inline juce::String nbOfStages(int stageID)
            {
                juce::String paramString = MainLayerConstants::ParamStringID::mainLayer
                    + MainLayerConstants::ParamStringID::stage
                    + (juce::String)stageID
                    + PhaserConstants::ParamStringID::phaser
                    + PhaserConstants::ParamStringID::nbOfStages;

                return paramString;
            };
            static inline juce::String mix(int stageID)
            {
                juce::String paramString = MainLayerConstants::ParamStringID::mainLayer
                    + MainLayerConstants::ParamStringID::stage
                    + (juce::String)stageID
                    + PhaserConstants::ParamStringID::phaser
                    + PhaserConstants::ParamStringID::mix;

                return paramString;
            };
            static inline juce::String isBypassed(int stageID)
            {
                juce::String paramString = MainLayerConstants::ParamStringID::mainLayer
                    + MainLayerConstants::ParamStringID::stage
                    + (juce::String)stageID
                    + PhaserConstants::ParamStringID::phaser
                    + PhaserConstants::ParamStringID::isBypassed;

                return paramString;
            };
        };
    };

    namespace AutomationString
    {
        static const juce::String phaser{ "Phaser " };
        static const juce::String centerFrequency{ "Centre Frequency" };
        static const juce::String feedback{ "Feedback" };
        static const juce::String rate{ "Rate" };
        static const juce::String depth{ "Depth" };
        static const juce::String mix{ "Mix" };
        static const juce::String nbOfStages{ "Number Of Stages" };
        static const juce::String isBypassed{ "Is Bypassed" };

        class GetString
        {
        public:
            static inline juce::String centerFrequency(int stageID)
            {
                juce::String paramString = MainLayerConstants::AutomationString::mainLayer
                    + MainLayerConstants::AutomationString::stage
                    + (juce::String)stageID + " "
                    + PhaserConstants::AutomationString::phaser
                    + PhaserConstants::AutomationString::centerFrequency;

                return paramString;
            };
            static inline juce::String feedback(int stageID)
            {
                juce::String paramString = MainLayerConstants::AutomationString::mainLayer
                    + MainLayerConstants::AutomationString::stage
                    + (juce::String)stageID + " "
                    + PhaserConstants::AutomationString::phaser
                    + PhaserConstants::AutomationString::feedback;

                return paramString;
            };
            static inline juce::String rate(int stageID)
            {
                juce::String paramString = MainLayerConstants::AutomationString::mainLayer
                    + MainLayerConstants::AutomationString::stage
                    + (juce::String)stageID + " "
                    + PhaserConstants::AutomationString::phaser
                    + PhaserConstants::AutomationString::rate;

                return paramString;
            };
            static inline juce::String depth(int stageID)
            {
                juce::String paramString = MainLayerConstants::AutomationString::mainLayer
                    + MainLayerConstants::AutomationString::stage
                    + (juce::String)stageID + " "
                    + PhaserConstants::AutomationString::phaser
                    + PhaserConstants::AutomationString::depth;

                return paramString;
            };
            static inline juce::String nbOfStages(int stageID)
            {
                juce::String paramString = MainLayerConstants::AutomationString::mainLayer
                    + MainLayerConstants::AutomationString::stage
                    + (juce::String)stageID + " "
                    + PhaserConstants::AutomationString::phaser
                    + PhaserConstants::AutomationString::nbOfStages;

                return paramString;
            };
            static inline juce::String mix(int stageID)
            {
                juce::String paramString = MainLayerConstants::AutomationString::mainLayer
                    + MainLayerConstants::AutomationString::stage
                    + (juce::String)stageID + " "
                    + PhaserConstants::AutomationString::phaser
                    + PhaserConstants::AutomationString::mix;

                return paramString;
            };
            static inline juce::String isBypassed(int stageID)
            {
                juce::String paramString = MainLayerConstants::AutomationString::mainLayer
                    + MainLayerConstants::AutomationString::stage
                    + (juce::String)stageID + " "
                    + PhaserConstants::AutomationString::phaser
                    + PhaserConstants::AutomationString::isBypassed;

                return paramString;
            };
        };
    }
};

namespace DistortionConstants
{
    template <typename SampleType>
    class Processor
    {
    public:
        static constexpr SampleType mixMin = static_cast<SampleType>(0.0);
        static constexpr SampleType mixMax = static_cast<SampleType>(100.0);
        static constexpr SampleType mixIncrement = static_cast<SampleType>(0.01);
        static constexpr SampleType mixStartValue = static_cast<SampleType>(100.0);
        static constexpr bool oversamplingStartValue = false;
        static constexpr bool isBypassedStartValue = false;
        static constexpr int nbOfDUMax = 2;
    };

    template <typename SampleType>
    class WaveShaper
    {
    public:
        static constexpr int nbOfPointMax = 100;
        static constexpr SampleType sampleLimitValue = static_cast<SampleType>(63095.734448); // juce::Decibels::decibelsToGain(96.0);
        static constexpr SampleType tensionMin = static_cast<SampleType>(0.1);
        static constexpr SampleType tensionMax = static_cast<SampleType>(10.0);
        static constexpr SampleType pointXMaxLin = static_cast<SampleType>(1.0);
        static constexpr SampleType pointXMinLin = static_cast<SampleType>(-1.0);
        static constexpr SampleType pointYMaxLin = static_cast<SampleType>(1.0);
        static constexpr SampleType pointYMinLin = static_cast<SampleType>(-1.0);
        static constexpr SampleType aSinHCurveCste = static_cast<SampleType>(2.61249613887); //static_cast<SampleType>(1.0 / std::log(1.0 + juce::MathConstants<double>::sqrt2));
        
        enum CurveTypes
        {
            polynomial,
            asinh,
            doubleCurve,
            square,
            triangle,
            stair,
            sin,
            maxCureType
        };
    };

    class UI
    {
    public:
        static constexpr int sliderHeight = 75;
        static constexpr int labelWidth = 75;
        static constexpr int labelHeight = 20;
        static constexpr int pixelSpace = 10;
        static constexpr float scopeMinLin_f = -1.125f;
        static constexpr float scopeMaxLin_f = +1.125f;

        static constexpr int nbOfPointsPerBin = 101;

        static juce::String distoUnitString;

        static juce::StringArray curveStringArray;// ("Polynomial", "ArcSinH", "Double Curve", "Square", "Triangle", "Stairs", "Sine");

        static juce::StringArray equationTypeStringArray;

        static juce::StringArray sigmoidEQAStringArray;// ("Tanh", "Parabolic", "Hyperbolic", "Asinh", "UnB Sat", "Fuzz", "Soft Clipper");

        static juce::StringArray symetricEQAStringArray;// ("Hard Clip", "Sin", "Sinh", "Sinh Alt 1", "Tape", "Foldover");

        static juce::StringArray asymetricEQAStringArray;// ("Tube Simulation", "Distortion Simulation", "Diode", "Diode 2");

        static juce::StringArray specialEQAStringArray;// ("Half Rect", "Full Rect");

        class Colour
        {
        public:
            static const juce::Colour background;
            static const juce::Colour innerGrid;//grey
            static constexpr float innerGridStrokeValue = 0.5f;
            static const juce::Colour outerGrid;//darkgrey
            static constexpr float outerGridStrokeValue = 0.5f;
            static const juce::Colour equation;
            static constexpr float equationPathStrokeValue = 2.0f;
            static const juce::Colour unipolar;//purple
            static constexpr float unipolarPathStrokeValue = 1.0f;
            static const juce::Colour bipolarOff; //darkgrey
            static constexpr float bipolarPathStrokeValue = 1.0f;
            static const juce::Colour selectedCurve; //darkorange
            static constexpr float selectedCurvePathStrokeValue = 1.0f;
            static const juce::Colour bipolarSquare; // black
            static constexpr float bipolarSquareColourOpacity = 0.5f; //0.3f darkgrey

            static const juce::Colour pointDragger;
            static const juce::Colour selectedPointDragger;
            static const juce::Colour tension;
            static const juce::Colour selectedTension;
        };
    };

    namespace ParamStringID
    {
        //static const juce::String distortion("DISTO");
        static const juce::String isBypassed("ISBYPASSED");
        //const juce::String firstDistoUnitOnOff{ "FIRSTWSONOFF" };
        //const juce::String secondDistoUnitOnOff{ "SECONDWSONOFF" };
        static const juce::String oversampling("OVERSAMPLING");
        static const juce::String mix{ "MIX" };
        static const juce::String onOff{ "ONOFF" };

        static const juce::Identifier distortion("DISTO");

        class GetParamStringID
        {
        public:
            static inline juce::String overSampling(int stageID)
            {
                juce::String paramString = MainLayerConstants::ParamStringID::mainLayer
                    + MainLayerConstants::ParamStringID::stage
                    + (juce::String)stageID
                    + DistortionConstants::ParamStringID::distortion
                    + DistortionConstants::ParamStringID::oversampling;

                return paramString;
            };

            static inline juce::String mix(int stageID)
            {
                juce::String paramString = MainLayerConstants::ParamStringID::mainLayer
                    + MainLayerConstants::ParamStringID::stage
                    + (juce::String)stageID
                    + DistortionConstants::ParamStringID::distortion
                    + DistortionConstants::ParamStringID::mix;

                return paramString;
            };

            static inline juce::String isBypassed(int stageID)
            {
                juce::String paramString = MainLayerConstants::ParamStringID::mainLayer
                    + MainLayerConstants::ParamStringID::stage
                    + (juce::String)stageID
                    + DistortionConstants::ParamStringID::distortion
                    + DistortionConstants::ParamStringID::isBypassed;

                return paramString;
            };

        };
    };

    namespace AutomationString
    {
        static const juce::String distortion("Distortion ");
        static const juce::String oversampling("OverSampling");
        static const juce::String mix{ "Mix" };
        static const juce::String isBypassed{ "Is Bypassed" };

        class GetString
        {
        public:
            static inline juce::String overSampling(int stageID)
            {
                juce::String paramString = MainLayerConstants::AutomationString::mainLayer
                    + MainLayerConstants::AutomationString::stage
                    + (juce::String)stageID + " "
                    + DistortionConstants::AutomationString::distortion
                    + DistortionConstants::AutomationString::oversampling;

                return paramString;
            };

            static inline juce::String mix(int stageID)
            {
                juce::String paramString = MainLayerConstants::AutomationString::mainLayer
                    + MainLayerConstants::AutomationString::stage
                    + (juce::String)stageID + " "
                    + DistortionConstants::AutomationString::distortion
                    + DistortionConstants::AutomationString::mix;

                return paramString;
            };

            static inline juce::String isBypassed(int stageID)
            {
                juce::String paramString = MainLayerConstants::AutomationString::mainLayer
                    + MainLayerConstants::AutomationString::stage
                    + (juce::String)stageID + " "
                    + DistortionConstants::AutomationString::distortion
                    + DistortionConstants::AutomationString::isBypassed;

                return paramString;
            }
        };
    };
};

namespace DistoUnitConstants
{
    template <typename SampleType>
    class Processor
    {
    public:
        static constexpr SampleType preGaindBMin = static_cast<SampleType>(-30.0);
        static constexpr SampleType preGaindBMax = static_cast<SampleType>(20.0);
        static constexpr SampleType preGaindBStartValue = static_cast<SampleType>(0.0);
        static constexpr SampleType driveMin = static_cast<SampleType>(0.0);
        static constexpr SampleType driveMax = static_cast<SampleType>(10.0);
        static constexpr SampleType driveStartValue = static_cast<SampleType>(2.0);
        static constexpr SampleType warpMin = static_cast<SampleType>(0.0);
        static constexpr SampleType warpMax = static_cast<SampleType>(1.0);
        static constexpr SampleType warpStartValue = static_cast<SampleType>(0.0);
        static constexpr SampleType postGaindBMin = static_cast<SampleType>(-40.0);
        static constexpr SampleType postGaindBMax = static_cast<SampleType>(10.0);
        static constexpr SampleType postGaindBStartValue = static_cast<SampleType>(0.0);
        static constexpr SampleType preGaindBIncrement = static_cast<SampleType>(0.1);
        static constexpr SampleType driveIncrement = static_cast<SampleType>(0.01);
        static constexpr SampleType warpIncrement = static_cast<SampleType>(0.01);
        static constexpr SampleType postGaindBIncrement = static_cast<SampleType>(0.1);
        static constexpr SampleType dcFilterCutoff = static_cast<SampleType>(20.0);
        static constexpr SampleType mixMin = static_cast<SampleType>(0.0);
        static constexpr SampleType mixMax = static_cast<SampleType>(100.0);
        static constexpr SampleType mixIncrement = static_cast<SampleType>(0.01);
        static constexpr SampleType mixStartValue = static_cast<SampleType>(100.0);
        static constexpr int equationStartType = 0;
        static constexpr bool routingStartValue = true;
        static constexpr bool isBipolarStartValue = false;
        static constexpr bool dcFilterIsBypassedStartValue = false;
        static constexpr bool isBypassedStartValue = false;
        static constexpr bool overSamplingStartValue = false;
    };

    namespace ParamStringID
    {
        //static const juce::String distoUnit("WS");
        static const juce::String isBypassed("ISBYPASSED");
        static const juce::String routing("ROUTING");
        static const juce::String eqa{ "EQA" };
        static const juce::String type{ "TYPE" };
        static const juce::String sigmoid{ "SIGM" };
        static const juce::String symetric{ "SYM" };
        static const juce::String asymetric{ "ASYM" };
        static const juce::String special{ "SPE" };
        //const juce::String eqNegativeValue{ "EQFORNEGVALUE" };
        static const juce::String isHybrid{ "ISHYBRID" };
        static const juce::String dcFilterIsBypassed{ "DCFILTERISBYPASSED" };
        static const juce::String preGain{ "PREGAIN" };
        static const juce::String drive{ "DRIVE" };
        static const juce::String warp{ "WARP" };
        static const juce::String postGain{ "POSTGAIN" };
        static const juce::String mix{ "MIX" };

        static const juce::Identifier distoUnit("DU");
        static const juce::Identifier selectedDistoUnitID("SelectedDistoUnitID");

        class GetParamStringID
        {
        public:
            static inline juce::String id(int distoUnitID)
            {
                juce::String paramString = DistoUnitConstants::ParamStringID::distoUnit + (juce::String)distoUnitID;

                return paramString;
            };

            static inline juce::String isBypassed(int stageID, int distortionUnitID)
            {
                juce::String paramString = MainLayerConstants::ParamStringID::mainLayer
                    + MainLayerConstants::ParamStringID::stage
                    + (juce::String)stageID
                    + DistortionConstants::ParamStringID::distortion
                    + DistoUnitConstants::ParamStringID::distoUnit
                    + (juce::String)distortionUnitID
                    + DistoUnitConstants::ParamStringID::isBypassed;

                return paramString;
            };

            static inline juce::String equationType(int stageID, int distortionUnitID)
            {
                juce::String paramString = MainLayerConstants::ParamStringID::mainLayer
                    + MainLayerConstants::ParamStringID::stage
                    + (juce::String)stageID
                    + DistortionConstants::ParamStringID::distortion
                    + DistoUnitConstants::ParamStringID::distoUnit
                    + (juce::String)distortionUnitID
                    + DistoUnitConstants::ParamStringID::eqa
                    + DistoUnitConstants::ParamStringID::type;

                return paramString;
            };

            static inline juce::String sigmoidEQA(int stageID, int distortionUnitID)
            {
                juce::String paramString = MainLayerConstants::ParamStringID::mainLayer
                    + MainLayerConstants::ParamStringID::stage
                    + (juce::String)stageID
                    + DistortionConstants::ParamStringID::distortion
                    + DistoUnitConstants::ParamStringID::distoUnit
                    + (juce::String)distortionUnitID
                    + DistoUnitConstants::ParamStringID::sigmoid
                    + DistoUnitConstants::ParamStringID::eqa;

                return paramString;
            };

            static inline juce::String symetricEQA(int stageID, int distortionUnitID)
            {
                juce::String paramString = MainLayerConstants::ParamStringID::mainLayer
                    + MainLayerConstants::ParamStringID::stage
                    + (juce::String)stageID
                    + DistortionConstants::ParamStringID::distortion
                    + DistoUnitConstants::ParamStringID::distoUnit
                    + (juce::String)distortionUnitID
                    + DistoUnitConstants::ParamStringID::symetric
                    + DistoUnitConstants::ParamStringID::eqa;

                return paramString;
            };

            static inline juce::String asymetricEQA(int stageID, int distortionUnitID)
            {
                juce::String paramString = MainLayerConstants::ParamStringID::mainLayer
                    + MainLayerConstants::ParamStringID::stage
                    + (juce::String)stageID
                    + DistortionConstants::ParamStringID::distortion
                    + DistoUnitConstants::ParamStringID::distoUnit
                    + (juce::String)distortionUnitID
                    + DistoUnitConstants::ParamStringID::asymetric
                    + DistoUnitConstants::ParamStringID::eqa;

                return paramString;
            };

            static inline juce::String specialEQA(int stageID, int distortionUnitID)
            {
                juce::String paramString = MainLayerConstants::ParamStringID::mainLayer
                    + MainLayerConstants::ParamStringID::stage
                    + (juce::String)stageID
                    + DistortionConstants::ParamStringID::distortion
                    + DistoUnitConstants::ParamStringID::distoUnit
                    + (juce::String)distortionUnitID
                    + DistoUnitConstants::ParamStringID::special
                    + DistoUnitConstants::ParamStringID::eqa;

                return paramString;
            };

            static inline juce::String routing(int stageID, int distortionUnitID)
            {
                juce::String paramString = MainLayerConstants::ParamStringID::mainLayer
                    + MainLayerConstants::ParamStringID::stage
                    + (juce::String)stageID
                    + DistortionConstants::ParamStringID::distortion
                    + DistoUnitConstants::ParamStringID::distoUnit
                    + (juce::String)distortionUnitID
                    + DistoUnitConstants::ParamStringID::routing;

                return paramString;
            };

            static inline juce::String preGain(int stageID, int distortionUnitID)
            {
                juce::String paramString = MainLayerConstants::ParamStringID::mainLayer
                    + MainLayerConstants::ParamStringID::stage
                    + (juce::String)stageID
                    + DistortionConstants::ParamStringID::distortion
                    + DistoUnitConstants::ParamStringID::distoUnit
                    + (juce::String)distortionUnitID
                    + DistoUnitConstants::ParamStringID::preGain;

                return paramString;
            };

            static inline juce::String drive(int stageID, int distortionUnitID)
            {
                juce::String paramString = MainLayerConstants::ParamStringID::mainLayer
                    + MainLayerConstants::ParamStringID::stage
                    + (juce::String)stageID
                    + DistortionConstants::ParamStringID::distortion
                    + DistoUnitConstants::ParamStringID::distoUnit
                    + (juce::String)distortionUnitID
                    + DistoUnitConstants::ParamStringID::drive;

                return paramString;
            };

            static inline juce::String warp(int stageID, int distortionUnitID)
            {
                juce::String paramString = MainLayerConstants::ParamStringID::mainLayer
                    + MainLayerConstants::ParamStringID::stage
                    + (juce::String)stageID
                    + DistortionConstants::ParamStringID::distortion
                    + DistoUnitConstants::ParamStringID::distoUnit
                    + (juce::String)distortionUnitID
                    + DistoUnitConstants::ParamStringID::warp;

                return paramString;
            };

            static inline juce::String postGain(int stageID, int distortionUnitID)
            {
                juce::String paramString = MainLayerConstants::ParamStringID::mainLayer
                    + MainLayerConstants::ParamStringID::stage
                    + (juce::String)stageID
                    + DistortionConstants::ParamStringID::distortion
                    + DistoUnitConstants::ParamStringID::distoUnit
                    + (juce::String)distortionUnitID
                    + DistoUnitConstants::ParamStringID::postGain;

                return paramString;
            };

            static inline juce::String isHybrid(int stageID, int distortionUnitID)
            {
                juce::String paramString = MainLayerConstants::ParamStringID::mainLayer
                    + MainLayerConstants::ParamStringID::stage
                    + (juce::String)stageID
                    + DistortionConstants::ParamStringID::distortion
                    + DistoUnitConstants::ParamStringID::distoUnit
                    + (juce::String)distortionUnitID
                    + DistoUnitConstants::ParamStringID::isHybrid;

                return paramString;
            };

            static inline juce::String dcFilterIsBypassed(int stageID, int distortionUnitID)
            {
                juce::String paramString = MainLayerConstants::ParamStringID::mainLayer
                    + MainLayerConstants::ParamStringID::stage
                    + (juce::String)stageID
                    + DistortionConstants::ParamStringID::distortion
                    + DistoUnitConstants::ParamStringID::distoUnit
                    + (juce::String)distortionUnitID
                    + DistoUnitConstants::ParamStringID::dcFilterIsBypassed;

                return paramString;
            };

            static inline juce::String mix(int stageID, int distortionUnitID)
            {
                juce::String paramString = MainLayerConstants::ParamStringID::mainLayer
                    + MainLayerConstants::ParamStringID::stage
                    + (juce::String)stageID
                    + DistortionConstants::ParamStringID::distortion
                    + DistoUnitConstants::ParamStringID::distoUnit
                    + (juce::String)distortionUnitID
                    + DistoUnitConstants::ParamStringID::mix;

                return paramString;
            };
        };
    };

    namespace AutomationString
    {
        static const juce::String distoUnit("Disto Unit ");
        static const juce::String isBypassed{ "Is Bypassed" };
        static const juce::String routing{ "Routing" };
        static const juce::String eqa{ "Equation" };
        static const juce::String type{ "Type" };
        static const juce::String sigmoid{ "Sigmoid " };
        static const juce::String symetric{ "Symetric " };
        static const juce::String asymetric{ "Asymetric " };
        static const juce::String special{ "Special " };
        //const juce::String eqPositiveValue{ "Equation For Positive Value" };
        //const juce::String eqNegativeValue{ "Equation For Negative Value" };
        static const juce::String isHybrid{ "Is Hybrid" };
        static const juce::String dcFilterIsBypassed{ "DC Filter Is Bypassed" };
        static const juce::String preGain{ "Pre Gain" };
        static const juce::String drive{ "Drive" };
        static const juce::String warp{ "Warp" };
        static const juce::String postGain{ "Post Gain" };
        static const juce::String mix{ "Mix" };

        class GetString
        {
        public:
            static inline juce::String isBypassed(int stageID, int distortionUnitID)
            {
                juce::String paramString = MainLayerConstants::AutomationString::mainLayer
                    + MainLayerConstants::AutomationString::stage
                    + (juce::String)stageID + " "
                    + DistortionConstants::AutomationString::distortion
                    + DistoUnitConstants::AutomationString::distoUnit
                    + (juce::String)distortionUnitID + " "
                    + DistoUnitConstants::AutomationString::isBypassed;

                return paramString;
            };

            static inline juce::String routing(int stageID, int distortionUnitID)
            {
                juce::String paramString = MainLayerConstants::AutomationString::mainLayer
                    + MainLayerConstants::AutomationString::stage
                    + (juce::String)stageID + " "
                    + DistortionConstants::AutomationString::distortion
                    + DistoUnitConstants::AutomationString::distoUnit
                    + (juce::String)distortionUnitID + " "
                    + DistoUnitConstants::AutomationString::routing;

                return paramString;
            };

            static inline juce::String equationType(int stageID, int distortionUnitID)
            {
                juce::String paramString = MainLayerConstants::AutomationString::mainLayer
                    + MainLayerConstants::AutomationString::stage
                    + (juce::String)stageID + " "
                    + DistortionConstants::AutomationString::distortion
                    + DistoUnitConstants::AutomationString::distoUnit
                    + (juce::String)distortionUnitID + " "
                    + DistoUnitConstants::AutomationString::eqa + " "
                    + DistoUnitConstants::AutomationString::type;

                return paramString;
            };

            static inline juce::String sigmoidEQA(int stageID, int distortionUnitID)
            {
                juce::String paramString = MainLayerConstants::AutomationString::mainLayer
                    + MainLayerConstants::AutomationString::stage
                    + (juce::String)stageID + " "
                    + DistortionConstants::AutomationString::distortion
                    + DistoUnitConstants::AutomationString::distoUnit
                    + (juce::String)distortionUnitID + " "
                    + DistoUnitConstants::AutomationString::sigmoid
                    + DistoUnitConstants::AutomationString::eqa;

                return paramString;
            };

            static inline juce::String symetricEQA(int stageID, int distortionUnitID)
            {
                juce::String paramString = MainLayerConstants::AutomationString::mainLayer
                    + MainLayerConstants::AutomationString::stage
                    + (juce::String)stageID + " "
                    + DistortionConstants::AutomationString::distortion
                    + DistoUnitConstants::AutomationString::distoUnit
                    + (juce::String)distortionUnitID + " "
                    + DistoUnitConstants::AutomationString::symetric
                    + DistoUnitConstants::AutomationString::eqa;

                return paramString;
            };

            static inline juce::String asymetricEQA(int stageID, int distortionUnitID)
            {
                juce::String paramString = MainLayerConstants::AutomationString::mainLayer
                    + MainLayerConstants::AutomationString::stage
                    + (juce::String)stageID + " "
                    + DistortionConstants::AutomationString::distortion
                    + DistoUnitConstants::AutomationString::distoUnit
                    + (juce::String)distortionUnitID + " "
                    + DistoUnitConstants::AutomationString::asymetric
                    + DistoUnitConstants::AutomationString::eqa;

                return paramString;
            };

            static inline juce::String specialEQA(int stageID, int distortionUnitID)
            {
                juce::String paramString = MainLayerConstants::AutomationString::mainLayer
                    + MainLayerConstants::AutomationString::stage
                    + (juce::String)stageID + " "
                    + DistortionConstants::AutomationString::distortion
                    + DistoUnitConstants::AutomationString::distoUnit
                    + (juce::String)distortionUnitID + " "
                    + DistoUnitConstants::AutomationString::special
                    + DistoUnitConstants::AutomationString::eqa;

                return paramString;
            };


            static inline juce::String preGain(int stageID, int distortionUnitID)
            {
                juce::String paramString = MainLayerConstants::AutomationString::mainLayer
                    + MainLayerConstants::AutomationString::stage
                    + (juce::String)stageID + " "
                    + DistortionConstants::AutomationString::distortion
                    + DistoUnitConstants::AutomationString::distoUnit
                    + (juce::String)distortionUnitID + " "
                    + DistoUnitConstants::AutomationString::preGain;

                return paramString;
            };

            static inline juce::String drive(int stageID, int distortionUnitID)
            {
                juce::String paramString = MainLayerConstants::AutomationString::mainLayer
                    + MainLayerConstants::AutomationString::stage
                    + (juce::String)stageID + " "
                    + DistortionConstants::AutomationString::distortion
                    + DistoUnitConstants::AutomationString::distoUnit
                    + (juce::String)distortionUnitID + " "
                    + DistoUnitConstants::AutomationString::drive;

                return paramString;
            };

            static inline juce::String warp(int stageID, int distortionUnitID)
            {
                juce::String paramString = MainLayerConstants::AutomationString::mainLayer
                    + MainLayerConstants::AutomationString::stage
                    + (juce::String)stageID + " "
                    + DistortionConstants::AutomationString::distortion
                    + DistoUnitConstants::AutomationString::distoUnit
                    + (juce::String)distortionUnitID + " "
                    + DistoUnitConstants::AutomationString::warp;

                return paramString;
            };

            static inline juce::String postGain(int stageID, int distortionUnitID)
            {
                juce::String paramString = MainLayerConstants::AutomationString::mainLayer
                    + MainLayerConstants::AutomationString::stage
                    + (juce::String)stageID + " "
                    + DistortionConstants::AutomationString::distortion
                    + DistoUnitConstants::AutomationString::distoUnit
                    + (juce::String)distortionUnitID + " "
                    + DistoUnitConstants::AutomationString::postGain;

                return paramString;
            };

            static inline juce::String isHybrid(int stageID, int distortionUnitID)
            {
                juce::String paramString = MainLayerConstants::AutomationString::mainLayer
                    + MainLayerConstants::AutomationString::stage
                    + (juce::String)stageID + " "
                    + DistortionConstants::AutomationString::distortion
                    + DistoUnitConstants::AutomationString::distoUnit
                    + (juce::String)distortionUnitID + " "
                    + DistoUnitConstants::AutomationString::isHybrid;

                return paramString;
            };

            static inline juce::String dcFilterIsBypassed(int stageID, int distortionUnitID)
            {
                juce::String paramString = MainLayerConstants::AutomationString::mainLayer
                    + MainLayerConstants::AutomationString::stage
                    + (juce::String)stageID + " "
                    + DistortionConstants::AutomationString::distortion
                    + DistoUnitConstants::AutomationString::distoUnit
                    + (juce::String)distortionUnitID + " "
                    + DistoUnitConstants::AutomationString::dcFilterIsBypassed;

                return paramString;
            };

            static inline juce::String mix(int stageID, int distortionUnitID)
            {
                juce::String paramString = MainLayerConstants::AutomationString::mainLayer
                    + MainLayerConstants::AutomationString::stage
                    + (juce::String)stageID + " "
                    + DistortionConstants::AutomationString::distortion
                    + DistoUnitConstants::AutomationString::distoUnit
                    + (juce::String)distortionUnitID + " "
                    + DistoUnitConstants::AutomationString::mix;

                return paramString;
            };
        };
    };
};

namespace SampleRemapperConstants
{
    namespace ParamStringID
    {
        static const juce::String sampleRemapper{ "SR" };

        static const juce::String selectedCurveID{ "SelectedCurveID" };
        static const juce::String selectedPointID{ "SelectedPointID" };
        static const juce::String selectedTensionID{ "SelectedTensionID" };
        static const juce::String srPoints{ "srPoints" };
        static const juce::String point{ "Point" };
        static const juce::String pointX{ "PointX" };
        static const juce::String pointY{ "PointY" };
        static const juce::String tension{ "Tension" };
        static const juce::String horizontalDragOn{ "HorDragOn" };
        static const juce::String curveType{ "CurveType" };
        static const juce::String isDeletable{ "IsDeletable" };
        static const juce::String wsPointSave{ "WSPointSave" };
        static const juce::String isBipolar{ "IsBipolar" };

        //class GetIdentifier
        //{
        //    static inline juce::Identifier sampleRemapper()
        //    {
        //        juce::Identifier id(SampleRemapper)
        //        return isBipolar;
        //    };
        //};
    };
};

namespace DistortionCircuitConstants
{
    template <typename SampleType>
    class Processor
    {
    public:
        enum class EquationType
        {
            sigmoid,
            symetric,
            asymetric,
            special,
            maxEQAType
        };

        enum class SigmoidEquationID
        {
            none,
            tanh,
            parabolic,
            hyperbolic,
            asinh,
            unbSat1,
            fuzz,
            softClipper,
            maxEQAID
        };

        enum class SymetricEquationID
        {
            none,
            hardclip,
            sin,
            sinh,
            sinhAlt,
            tape,
            foldover,
            maxSymEQAID
        };

        enum class AsymetricEquationID
        {
            none,
            tubeSimulation,
            distortionSimulation,
            diode,
            diode2,
            maxAsymEQAID
        };

        enum class SpecialEquationID
        {
            none,
            halfrect,
            fullrect,
            maxSpeEQAID
        };
    };

    namespace ParamsID
    {
        static const juce::Identifier distortionCircuit("DistoCircuit");
        static const juce::Identifier equationType("equationType");
        static const juce::Identifier equationID("equationID");

        class GetString
        {
        public:

        };
    };
};