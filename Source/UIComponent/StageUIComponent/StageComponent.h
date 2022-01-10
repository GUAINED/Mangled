/*
  ==============================================================================

    StageComponent.h
    Created: 2 Jun 2021 4:45:09pm
    Author:  CookiMonstor

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

#include "../../UIComponent/ScopeUIComponent/ScopeComponent.h"
#include "../../UIComponent/EQUIComponent/EQComponent.h"
#include "../../UIComponent/PhaserUIComponent/PhaserComponent.h"
#include "../../UIComponent/DistortionUIComponent/DistortionComponent.h"
#include "../../UIComponent/Controls/MangledDualStateButton.h"
#include "RMSMeter.h"
#include "StereoRMSMeterComponent.h"
#include "../../Processor/LayerProcessor/MainLayerProcessor.h"

//==============================================================================
/*
*/
class StageComponent : public juce::Component
                     , public juce::Button::Listener
                     , public juce::ComboBox::Listener
{
public:
    StageComponent(MainLayerDataStruct& mlDataStruct);
    ~StageComponent() override;

    enum RadioButtonIds
    {
        ProcessorButtons = 1001
    };


    void mouseDown(const juce::MouseEvent& e)
    {
        int stageID = dataStruct.getSelectedStageID();

        juce::MouseEvent c = e.getEventRelativeTo(scope.getScopeDisplay());

        if (e.getNumberOfClicks() == 1)
        {
            int selectedFilterID = scope.getScopeDisplay()->findFilterDragger(c);

            if (selectedFilterID != -1)
            {
                getEQComponent()->switchToEQSliderMenu(selectedFilterID);
                shouldDrag = true;
            }

            dataStruct.setSelectedFilterID(stageID, selectedFilterID);
        }
        else if (e.getNumberOfClicks() == 2)
        {
            int selectedFilterID = scope.getScopeDisplay()->findFilterDragger(c);

            if (selectedFilterID == -1)
            {
                int filterID = dataStruct.findNextAvailableFilterIDEQ(stageID);

                if (filterID != -1)
                {
                    juce::Rectangle<float> scopeBounds = scope.getScopeDisplay()->getBounds().toFloat();
                    float scopeWidth = static_cast<float>(scopeBounds.getWidth());
                    float scopeHeight = static_cast<float>(scopeBounds.getHeight());
                    dataStruct.setMouseAddFilterEQ(e.getEventRelativeTo(scope.getScopeDisplay()), filterID, scopeWidth, scopeHeight);

                    //getEQComponent()->addFilter();
                    getEQComponent()->switchToEQSliderMenu(filterID);
                    //getEQComponent()->getFilterIdUpEQTextButton()->setEnabled(true);
                    //eqComponent.getFilterIdEQLabel()->setVisible(true);
                    getEQComponent()->getFilterIdUpEQTextButton()->setVisible(true);
                    //getEQComponent()->getFilterIdDownEQTextButton()->setEnabled(true);
                    getEQComponent()->getFilterIdDownEQTextButton()->setVisible(true);
                }

            }
        }
    }

    void mouseDrag(const juce::MouseEvent& e)
    {
        float scopeHeight = (float)scope.getScopeDisplay()->getHeight();
        float scopeWidth = (float)scope.getScopeDisplay()->getWidth();

        float scopeHeight2 = (float)scope.getHeight();
        float scopeWidth2 = (float)scope.getWidth();
        float scopeHeight3 = (float)getHeight();
        float scopeWidth3 = (float)getWidth();
        if (shouldDrag)
        {
            dataStruct.setMouseDragEventEQ(e.getEventRelativeTo(scope.getScopeDisplay()), scopeWidth, scopeHeight);
        }
        
    }

    void mouseUp(const juce::MouseEvent& e)
    {
        if (shouldDrag)
        {
            shouldDrag = false;
        }
    }

    void mouseWheelMove(const juce::MouseEvent& event, const juce::MouseWheelDetails& wheel)
    {
        ScopeDisplay* pScopeDisplay = scope.getScopeDisplay();
        juce::MouseEvent mouseEventRelativeEQScope = event.getEventRelativeTo(pScopeDisplay);

        int filterID = pScopeDisplay->findFilterDragger(mouseEventRelativeEQScope);
        if (filterID == -1)
        {
            return;
        }

        dataStruct.setMouseWheelEventEQ(mouseEventRelativeEQScope, wheel, filterID);

    }

    void paint (juce::Graphics&) override;
    void resized() override;

    //Button::Listener
    void buttonClicked(juce::Button* button) override;

    //ComboBox Listener
    void comboBoxChanged(juce::ComboBox* comboBoxThatHasChanged) override;

    void switchEQToWaveShaper();
    void switchWaveShaperToEQ();

    juce::ComboBox* getDataToDisplayComboBox() { return &dataToDisplayComboBox; };
    MangledDualStateButton* getDisplayEQButton() { return &displayEQButton; };
    MangledDualStateButton* getDisplayDistoButton() { return &displayDistoButton; };
    MangledDualStateButton* getEnvButton() { return &displayEnvButton; };

    ScopeComponent*      getScope()               { return &scope; };
    EQComponent*         getEQComponent()         { return &eqComponent; };
    PhaserComponent*     getPhaserComponent()     { return &phaserComponent; };
    DistortionComponent* getDistortionComponent() { return &distortionComponent; };

    StereoRMSMeterComponent* getInputRMSMeter() {return &inputRMSMeter; };
    StereoRMSMeterComponent* getOutputRMSMeter() { return &outputRMSMeter; };
private:
    juce::Label dataLabel;
    juce::ComboBox dataToDisplayComboBox;
    MangledDualStateButton displayEQButton;
    MangledDualStateButton displayDistoButton;
    MangledDualStateButton displayEnvButton;

    ScopeComponent scope;

    EQComponent eqComponent;

    PhaserComponent phaserComponent;

    DistortionComponent distortionComponent;

    StereoRMSMeterComponent inputRMSMeter;
    StereoRMSMeterComponent outputRMSMeter;

    bool shouldDrag = false;

    MainLayerDataStruct& dataStruct;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (StageComponent)
};
