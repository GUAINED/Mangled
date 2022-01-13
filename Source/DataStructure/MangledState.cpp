/*
  ==============================================================================

    MangledAPVTS.cpp
    Created: 13 Jan 2022 8:39:04am
    Author:  CookiMonstor

  ==============================================================================
*/

#include "MangledState.h"

template<typename SampleType>
void MangledState<SampleType>::valueTreePropertyChanged(juce::ValueTree& treeWhosePropertyHasChanged, const juce::Identifier& property)
{
    //juce::ValueTree parent = treeWhosePropertyHasChanged.getParent();
    shouldUpdate = true;
}

template<typename SampleType>
void MangledState<SampleType>::valueTreeChildAdded(juce::ValueTree& parentTree, juce::ValueTree& childWhichHasBeenAdded)
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

template<typename SampleType>
void MangledState<SampleType>::valueTreeChildRemoved(juce::ValueTree& parentTree, juce::ValueTree& childWhichHasBeenRemoved, int indexFromWhichChildWasRemoved)
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

template<typename SampleType>
void MangledState<SampleType>::valueTreeChildOrderChanged(juce::ValueTree& parentTreeWhoseChildrenHaveMoved, int oldIndex, int newIndex)
{

}

template<typename SampleType>
void MangledState<SampleType>::valueTreeParentChanged(juce::ValueTree& treeWhoseParentHasChanged)
{

}

template<typename SampleType>
void MangledState<SampleType>::valueTreeRedirected(juce::ValueTree& treeWhichHasBeenChanged)
{

}
