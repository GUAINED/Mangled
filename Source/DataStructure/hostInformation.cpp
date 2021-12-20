/*
  ==============================================================================

    hostInformation.cpp
    Created: 11 May 2021 1:31:00pm
    Author:  CookiMonstor

  ==============================================================================
*/

#include "hostInformation.h"
void hostInformation::setPlayHead(juce::AudioPlayHead* newPlayHead) 
{
    playHead = newPlayHead;

    if (playHead != nullptr)
    {
        playHead->getCurrentPosition(hostInfo);

        bpm = hostInfo.bpm;
    }
}