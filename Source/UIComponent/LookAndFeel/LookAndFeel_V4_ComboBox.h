/*
  ==============================================================================

    LookAndFeel_V4_ComboBox.h
    Created: 19 Nov 2021 2:27:38am
    Author:  CookiMonstor

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "../../DataStructure/MangledIdentifiers.h"
class LookAndFeel_V4_ComboBox : public juce::LookAndFeel_V4
{
public:
    void drawComboBox(juce::Graphics& g, int width, int height, bool,
        int, int, int, int, juce::ComboBox& box)
    {
        auto cornerSize = box.findParentComponentOfClass<juce::ChoicePropertyComponent>() != nullptr ? 0.0f : 3.0f;
        juce::Rectangle<int> boxBounds(0, 0, width, height);

        //g.setColour(box.findColour(juce::ComboBox::backgroundColourId));
        g.setColour(AudioEngineConstants::UI::darkestNeonGreen); //box.findColour(juce::ComboBox::backgroundColourId)
        g.fillRoundedRectangle(boxBounds.toFloat(), cornerSize);

        //g.setColour(box.findColour(juce::ComboBox::outlineColourId));
        g.setColour(AudioEngineConstants::UI::neonGreen.withAlpha(0.6f));//box.findColour(juce::ComboBox::outlineColourId)
        g.drawRoundedRectangle(boxBounds.toFloat(), cornerSize, 1.0f);//.reduced(0.5f, 0.5f)

        //Rectangle<int> arrowZone(width - 30, 0, 20, height);
        //Path path;
        //path.startNewSubPath((float)arrowZone.getX() + 3.0f, (float)arrowZone.getCentreY() - 2.0f);
        //path.lineTo((float)arrowZone.getCentreX(), (float)arrowZone.getCentreY() + 3.0f);
        //path.lineTo((float)arrowZone.getRight() - 3.0f, (float)arrowZone.getCentreY() - 2.0f);

        //g.setColour(box.findColour(ComboBox::arrowColourId).withAlpha((box.isEnabled() ? 0.9f : 0.2f)));
        //g.strokePath(path, PathStrokeType(2.0f));
    }

    void positionComboBoxText(juce::ComboBox& box, juce::Label& label) override
    {
        label.setBounds(1, 1,
            box.getWidth(),
            box.getHeight() - 2);

        label.setFont(getComboBoxFont(box));
    }
};