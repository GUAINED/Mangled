/*
  ==============================================================================

    Identifiers.cpp
    Created: 23 Sep 2021 3:17:48pm
    Author:  CookiMonstor

  ==============================================================================
*/

#include "MangledIdentifiers.h"

//Audio Engine
juce::StringArray AudioEngineConstants::UI::stateString = { "A", "B", "C", "D" };
const juce::Colour AudioEngineConstants::UI::neonGreen = juce::Colour(57, 255, 20);
const juce::Colour AudioEngineConstants::UI::darkestNeonGreen = juce::Colour(57, 255, 20).darker(10.0f);
const juce::Colour AudioEngineConstants::UI::darkNeonGreen = juce::Colour(57, 255, 20).darker(4.0f);
const juce::Colour AudioEngineConstants::UI::darkerNeonGreen = juce::Colour(57, 255, 20).darker(2.0f);

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

const juce::Colour EQConstants::UI::FilterColour::filterSumColour = AudioEngineConstants::UI::neonGreen;//juce::Colours::limegreen;

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
juce::StringArray DistortionConstants::UI::equationTypeStringArray = {"Sigmoid", "Symetric", "Asymetric", "Special"};
juce::StringArray DistortionConstants::UI::sigmoidEQAStringArray = { "None", "Tanh", "Parabolic", "Hyperbolic", "Asinh", "UnB Sat", "Fuzz", "Soft Clipper" };
juce::StringArray DistortionConstants::UI::symetricEQAStringArray = { "None", "Hard Clip", "Sin", "Sinh", "Sinh Alt 1", "Tape", "Foldover" };
juce::StringArray DistortionConstants::UI::asymetricEQAStringArray = { "None", "Tube Simulation", "Distortion Simulation", "Diode", "Diode 2" };
juce::StringArray DistortionConstants::UI::specialEQAStringArray = { "None", "Half Rect", "Full Rect" };

const juce::Colour DistortionConstants::UI::Colour::background = juce::Colours::black;
const juce::Colour DistortionConstants::UI::Colour::innerGrid = juce::Colours::white;
const juce::Colour DistortionConstants::UI::Colour::outerGrid = juce::Colours::white;
const juce::Colour DistortionConstants::UI::Colour::equation = AudioEngineConstants::UI::neonGreen;
const juce::Colour DistortionConstants::UI::Colour::unipolar = juce::Colours::orange;
const juce::Colour DistortionConstants::UI::Colour::bipolarOff = juce::Colours::white;
const juce::Colour DistortionConstants::UI::Colour::selectedCurve = juce::Colours::red;
const juce::Colour DistortionConstants::UI::Colour::bipolarSquare = juce::Colours::white;
const juce::Colour DistortionConstants::UI::Colour::pointDragger = juce::Colours::orange;
const juce::Colour DistortionConstants::UI::Colour::selectedPointDragger = juce::Colours::red;
const juce::Colour DistortionConstants::UI::Colour::tension = DistortionConstants::UI::Colour::unipolar;
const juce::Colour DistortionConstants::UI::Colour::selectedTension = DistortionConstants::UI::Colour::selectedCurve;
