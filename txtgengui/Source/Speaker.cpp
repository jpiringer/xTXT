/*
  ==============================================================================

    Speaker.cpp
    Created: 17 Jul 2022 9:46:38am
    Author:  joerg piringer

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"

#if JUCE_MAC
#include "SpeakerOSX.mm"
#endif

#if JUCE_WIN
#include "SpeakerWin.cpp"
#endif
