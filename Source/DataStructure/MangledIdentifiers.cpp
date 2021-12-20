/*
  ==============================================================================

    Identifiers.cpp
    Created: 23 Sep 2021 3:17:48pm
    Author:  CookiMonstor

  ==============================================================================
*/

#include "MangledIdentifiers.h"

//Scope
juce::StringArray ScopeConstants::UI::thirtydB::gridLabelStringArray = { "30 dB", "+20 dB", "+10 dB", "+ 0 dB", "-10 dB", "-20 dB", "-30 dB" };
juce::StringArray ScopeConstants::UI::fifteendB::gridLabelStringArray = { "15 dB", "+10 dB", "+5 dB", "+ 0 dB", "-5 dB", "-10 dB", "-15 dB" };
juce::StringArray ScopeConstants::UI::ninedB::gridLabelStringArray = { "9 dB", "+6 dB", "+3 dB", "+ 0 dB", "-3 dB", "-6 dB", "-9 dB" };
juce::StringArray ScopeConstants::UI::sixdB::gridLabelStringArray = { "6 dB", "+4 dB", "+2 dB", "+ 0 dB", "-2 dB", "-4 dB", "-6 dB" };
juce::StringArray ScopeConstants::UI::threedB::gridLabelStringArray = { "3 dB", "+2 dB", "+1 dB", "+ 0 dB", "-1 dB", "-2 dB", "-3 dB" };

juce::StringArray ScopeConstants::UI::modeStringArray = { "FFT", "Temporal", "RMS", "Sound Field" };

//EQ
juce::StringArray EQConstants::UI::typeStringArray = { "Peak", "Low Pass", "High Pass", "Notch", "Band Pass", "LowShelf", "HighShelf" };
juce::StringArray EQConstants::UI::orderStringArray = {"12 dB/oct", "24 dB/oct", "36 dB/oct", "48 dB/oct"};

juce::Array<juce::Colour> EQConstants::UI::FilterColour::filterColour = { juce::Colours::cyan, juce::Colours::darkorange, juce::Colours::red, juce::Colours::blue, juce::Colours::green, juce::Colours::violet,
     juce::Colours::brown, juce::Colours::mistyrose,juce::Colours::darkorchid, juce::Colours::crimson,  juce::Colours::coral, juce::Colours::chocolate, juce::Colours::chartreuse, juce::Colours::yellow };

const juce::Colour EQConstants::UI::FilterColour::filterSumColour = juce::Colours::limegreen;

//Phaser
juce::StringArray PhaserConstants::UI::nbOfStgStringArray = juce::StringArray("2 Stages", "4 Stages", "6 Stages", "8 Stages");
//juce::String PhaserConstants::ParamIDString::phaser = juce::String("PHASER");
//juce::String PhaserConstants::ParamIDString::centerFrequency = juce::String("CENTREFREQUENCY");
//juce::String PhaserConstants::ParamIDString::feedback = juce::String("FEEDBACK");
//juce::String PhaserConstants::ParamIDString::rate = juce::String("RATE");
//juce::String PhaserConstants::ParamIDString::depth = juce::String("DEPTH");
//juce::String PhaserConstants::ParamIDString::mix = juce::String("MIX");
//juce::String PhaserConstants::ParamIDString::nbOfStages = juce::String("NUMBEROFSTAGES");
//juce::String PhaserConstants::ParamIDString::onOff = juce::String("ONOFF");
//juce::String PhaserConstants::ParamIDString::isBypassed = juce::String("ISBYPASSED");


//Distortion
juce::String DistortionConstants::UI::distoUnitString = juce::String("Unit ");
juce::StringArray DistortionConstants::UI::curveStringArray = { "Polynomial", "ArcSinH", "Double Curve", "Square", "Triangle", "Stairs", "Sine" };
juce::StringArray DistortionConstants::UI::sigmoidEQAStringArray = { "Tanh", "Parabolic", "Hyperbolic", "Asinh", "UnB Sat", "Fuzz", "Soft Clipper" };
juce::StringArray DistortionConstants::UI::symetricEQAStringArray = { "Hard Clip", "Sin", "Sinh", "Sinh Alt 1", "Tape", "Foldover" };
juce::StringArray DistortionConstants::UI::asymetricEQAStringArray = { "Tube Simulation", "Distortion Simulation", "Diode", "Diode 2" };
juce::StringArray DistortionConstants::UI::specialEQAStringArray = { "Half Rect", "Full Rect" };

const juce::Colour DistortionConstants::UI::Colour::background = juce::Colours::black;
const juce::Colour DistortionConstants::UI::Colour::innerGrid = juce::Colours::white;
const juce::Colour DistortionConstants::UI::Colour::outerGrid = juce::Colours::white;
const juce::Colour DistortionConstants::UI::Colour::equation = juce::Colours::limegreen;
const juce::Colour DistortionConstants::UI::Colour::unipolar = juce::Colours::white;
const juce::Colour DistortionConstants::UI::Colour::bipolarOff = juce::Colours::white;
const juce::Colour DistortionConstants::UI::Colour::selectedCurve = juce::Colours::orange;
const juce::Colour DistortionConstants::UI::Colour::bipolarSquare = juce::Colours::white;
const juce::Colour DistortionConstants::UI::Colour::pointDragger = juce::Colours::cyan;
const juce::Colour DistortionConstants::UI::Colour::selectedPointDragger = juce::Colours::orange;
const juce::Colour DistortionConstants::UI::Colour::tension = DistortionConstants::UI::Colour::unipolar;
const juce::Colour DistortionConstants::UI::Colour::selectedTension = DistortionConstants::UI::Colour::selectedCurve;

//Distortion======================================================================================================
//juce::String getDistortionFirstDUOnOffParamString(int stageID)
//{
//    juce::String paramString = MainLayerConstants::ParamStringID::mainLayer
//        + MainLayerConstants::ParamStringID::stage
//        + (juce::String)stageID
//        + DistortionConstants::DistortionParamStringID::distortion
//        + DistortionConstants::DistortionParamStringID::firstDistoUnitOnOff;
//
//    return paramString;
//}
//
//juce::String getDistortionSecondDUOnOffParamString(int stageID)
//{
//    juce::String paramString = MainLayerConstants::ParamStringID::mainLayer
//        + MainLayerConstants::ParamStringID::stage
//        + (juce::String)stageID
//        + DistortionConstants::DistortionParamStringID::distortion
//        + DistortionConstants::DistortionParamStringID::secondDistoUnitOnOff;
//
//    return paramString;
//}

juce::String getDistortionOverSamplingParamString(int stageID)
{
    juce::String paramString = MainLayerConstants::ParamStringID::mainLayer
        + MainLayerConstants::ParamStringID::stage
        + (juce::String)stageID
        + DistortionConstants::DistortionParamStringID::distortion
        + DistortionConstants::DistortionParamStringID::oversampling;

    return paramString;
}

juce::String getDistortionMixParamString(int stageID)
{
    juce::String paramString = MainLayerConstants::ParamStringID::mainLayer
        + MainLayerConstants::ParamStringID::stage
        + (juce::String)stageID
        + DistortionConstants::DistortionParamStringID::distortion
        + DistortionConstants::DistortionParamStringID::mix;

    return paramString;
}

juce::String getDistortionIsBypassedParamString(int stageID)
{
    juce::String paramString = MainLayerConstants::ParamStringID::mainLayer
        + MainLayerConstants::ParamStringID::stage
        + (juce::String)stageID
        + DistortionConstants::DistortionParamStringID::distortion
        + DistortionConstants::DistortionParamStringID::isBypassed;

    return paramString;
}

juce::String getDistortionDUIsBypassedParamString(int stageID, int distortionUnitID)
{
    juce::String paramString = MainLayerConstants::ParamStringID::mainLayer
        + MainLayerConstants::ParamStringID::stage
        + (juce::String)stageID
        + DistortionConstants::DistortionParamStringID::distortion
        + DistortionConstants::DistoUnitParamStringID::distoUnit
        + (juce::String)distortionUnitID
        + DistortionConstants::DistoUnitParamStringID::isBypassed;

    return paramString;
}

juce::String getDistortionDURoutingParamString(int stageID, int distortionUnitID)
{
    juce::String paramString = MainLayerConstants::ParamStringID::mainLayer
        + MainLayerConstants::ParamStringID::stage
        + (juce::String)stageID
        + DistortionConstants::DistortionParamStringID::distortion
        + DistortionConstants::DistoUnitParamStringID::distoUnit
        + (juce::String) distortionUnitID
        + DistortionConstants::DistoUnitParamStringID::routing;

    return paramString;
}

juce::String getDistortionDUPreGainParamString(int stageID, int distortionUnitID)
{
    juce::String paramString = MainLayerConstants::ParamStringID::mainLayer
        + MainLayerConstants::ParamStringID::stage
        + (juce::String)stageID
        + DistortionConstants::DistortionParamStringID::distortion
        + DistortionConstants::DistoUnitParamStringID::distoUnit
        + (juce::String)distortionUnitID
        + DistortionConstants::DistoUnitParamStringID::preGain;

    return paramString;
}

juce::String getDistortionDUDriveParamString(int stageID, int distortionUnitID)
{
    juce::String paramString = MainLayerConstants::ParamStringID::mainLayer
        + MainLayerConstants::ParamStringID::stage
        + (juce::String)stageID
        + DistortionConstants::DistortionParamStringID::distortion
        + DistortionConstants::DistoUnitParamStringID::distoUnit
        + (juce::String)distortionUnitID
        + DistortionConstants::DistoUnitParamStringID::drive;

    return paramString;
}

juce::String getDistortionDUWarpParamString(int stageID, int distortionUnitID)
{
    juce::String paramString = MainLayerConstants::ParamStringID::mainLayer
        + MainLayerConstants::ParamStringID::stage
        + (juce::String)stageID
        + DistortionConstants::DistortionParamStringID::distortion
        + DistortionConstants::DistoUnitParamStringID::distoUnit
        + (juce::String)distortionUnitID
        + DistortionConstants::DistoUnitParamStringID::warp;

    return paramString;
}

juce::String getDistortionDUPostGainParamString(int stageID, int distortionUnitID)
{
    juce::String paramString = MainLayerConstants::ParamStringID::mainLayer
        + MainLayerConstants::ParamStringID::stage
        + (juce::String)stageID
        + DistortionConstants::DistortionParamStringID::distortion
        + DistortionConstants::DistoUnitParamStringID::distoUnit
        + (juce::String)distortionUnitID
        + DistortionConstants::DistoUnitParamStringID::postGain;

    return paramString;
}

juce::String getDistortionDUIsHybridParamString(int stageID, int distortionUnitID)
{
    juce::String paramString = MainLayerConstants::ParamStringID::mainLayer
        + MainLayerConstants::ParamStringID::stage
        + (juce::String)stageID
        + DistortionConstants::DistortionParamStringID::distortion
        + DistortionConstants::DistoUnitParamStringID::distoUnit
        + (juce::String)distortionUnitID
        + DistortionConstants::DistoUnitParamStringID::isHybrid;

    return paramString;
}

juce::String getDistortionDUDCFilterIsBypassedParamString(int stageID, int distortionUnitID)
{
    juce::String paramString = MainLayerConstants::ParamStringID::mainLayer
        + MainLayerConstants::ParamStringID::stage
        + (juce::String)stageID
        + DistortionConstants::DistortionParamStringID::distortion
        + DistortionConstants::DistoUnitParamStringID::distoUnit
        + (juce::String)distortionUnitID
        + DistortionConstants::DistoUnitParamStringID::dcFilterIsBypassed;

    return paramString;
}

juce::String getDistortionDUMixParamString(int stageID, int distortionUnitID)
{
    juce::String paramString = MainLayerConstants::ParamStringID::mainLayer
        + MainLayerConstants::ParamStringID::stage
        + (juce::String)stageID
        + DistortionConstants::DistortionParamStringID::distortion
        + DistortionConstants::DistoUnitParamStringID::distoUnit
        + (juce::String)distortionUnitID
        + DistortionConstants::DistoUnitParamStringID::mix;

    return paramString;
}

//Distortion Param String get Function
juce::String getDistortionOverSamplingParamAutomationString(int stageID)
{
    juce::String paramString = MainLayerConstants::AutomationString::mainLayer
        + MainLayerConstants::AutomationString::stage
        + (juce::String)stageID + " "
        + DistortionConstants::AutomationString::distortion
        + DistortionConstants::AutomationString::oversampling;

    return paramString;
}

juce::String getDistortionMixParamAutomationString(int stageID)
{
    juce::String paramString = MainLayerConstants::AutomationString::mainLayer
        + MainLayerConstants::AutomationString::stage
        + (juce::String)stageID + " "
        + DistortionConstants::AutomationString::distortion
        + DistortionConstants::AutomationString::mix;

    return paramString;
}

juce::String getDistortionIsBypassedParamAutomationString(int stageID)
{
    juce::String paramString = MainLayerConstants::AutomationString::mainLayer
        + MainLayerConstants::AutomationString::stage
        + (juce::String)stageID + " "
        + DistortionConstants::AutomationString::distortion
        + DistortionConstants::AutomationString::isBypassed;

    return paramString;
}

juce::String getDistortionDUIsBypassedParamAutomationString(int stageID, int distortionUnitID)
{
    juce::String paramString = MainLayerConstants::AutomationString::mainLayer
        + MainLayerConstants::AutomationString::stage
        + (juce::String)stageID + " "
        + DistortionConstants::AutomationString::distortion
        + DistortionConstants::AutomationString::DistoUnitString::distoUnit
        + (juce::String)distortionUnitID + " "
        + DistortionConstants::AutomationString::DistoUnitString::isBypassed;

    return paramString;
}

juce::String getDistortionDURoutingParamAutomationString(int stageID, int distortionUnitID)
{
    juce::String paramString = MainLayerConstants::AutomationString::mainLayer
        + MainLayerConstants::AutomationString::stage
        + (juce::String)stageID + " "
        + DistortionConstants::AutomationString::distortion
        + DistortionConstants::AutomationString::DistoUnitString::distoUnit
        + (juce::String)distortionUnitID + " "
        + DistortionConstants::AutomationString::DistoUnitString::routing;

    return paramString;
}

juce::String getDistortionDUPreGainParamAutomationString(int stageID, int distortionUnitID)
{
    juce::String paramString = MainLayerConstants::AutomationString::mainLayer
        + MainLayerConstants::AutomationString::stage
        + (juce::String)stageID + " "
        + DistortionConstants::AutomationString::distortion
        + DistortionConstants::AutomationString::DistoUnitString::distoUnit
        + (juce::String)distortionUnitID + " "
        + DistortionConstants::AutomationString::DistoUnitString::preGain;

    return paramString;
}

juce::String getDistortionDUDriveParamAutomationString(int stageID, int distortionUnitID)
{
    juce::String paramString = MainLayerConstants::AutomationString::mainLayer
        + MainLayerConstants::AutomationString::stage
        + (juce::String)stageID + " "
        + DistortionConstants::AutomationString::distortion
        + DistortionConstants::AutomationString::DistoUnitString::distoUnit
        + (juce::String)distortionUnitID + " "
        + DistortionConstants::AutomationString::DistoUnitString::drive;

    return paramString;
}

juce::String getDistortionDUWarpParamAutomationString(int stageID, int distortionUnitID)
{
    juce::String paramString = MainLayerConstants::AutomationString::mainLayer
        + MainLayerConstants::AutomationString::stage
        + (juce::String)stageID + " "
        + DistortionConstants::AutomationString::distortion
        + DistortionConstants::AutomationString::DistoUnitString::distoUnit
        + (juce::String)distortionUnitID + " "
        + DistortionConstants::AutomationString::DistoUnitString::warp;

    return paramString;
}

juce::String getDistortionDUPostGainParamAutomationString(int stageID, int distortionUnitID)
{
    juce::String paramString = MainLayerConstants::AutomationString::mainLayer
        + MainLayerConstants::AutomationString::stage
        + (juce::String)stageID + " "
        + DistortionConstants::AutomationString::distortion
        + DistortionConstants::AutomationString::DistoUnitString::distoUnit
        + (juce::String)distortionUnitID + " "
        + DistortionConstants::AutomationString::DistoUnitString::postGain;

    return paramString;
}

juce::String getDistortionDUIsHybridParamAutomationString(int stageID, int distortionUnitID)
{
    juce::String paramString = MainLayerConstants::AutomationString::mainLayer
        + MainLayerConstants::AutomationString::stage
        + (juce::String)stageID + " "
        + DistortionConstants::AutomationString::distortion
        + DistortionConstants::AutomationString::DistoUnitString::distoUnit
        + (juce::String)distortionUnitID + " "
        + DistortionConstants::AutomationString::DistoUnitString::isHybrid;

    return paramString;
}

juce::String getDistortionDUDCFilterIsBypassedParamAutomationString(int stageID, int distortionUnitID)
{
    juce::String paramString = MainLayerConstants::AutomationString::mainLayer
        + MainLayerConstants::AutomationString::stage
        + (juce::String)stageID + " "
        + DistortionConstants::AutomationString::distortion
        + DistortionConstants::AutomationString::DistoUnitString::distoUnit
        + (juce::String)distortionUnitID + " "
        + DistortionConstants::AutomationString::DistoUnitString::dcFilterIsBypassed;

    return paramString;
}

juce::String getDistortionDUMixParamAutomationString(int stageID, int distortionUnitID)
{
    juce::String paramString = MainLayerConstants::AutomationString::mainLayer
        + MainLayerConstants::AutomationString::stage
        + (juce::String)stageID + " "
        + DistortionConstants::AutomationString::distortion
        + DistortionConstants::AutomationString::DistoUnitString::distoUnit
        + (juce::String)distortionUnitID + " "
        + DistortionConstants::AutomationString::DistoUnitString::mix;

    return paramString;
}
