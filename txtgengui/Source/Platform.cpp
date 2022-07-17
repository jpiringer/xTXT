/*
  ==============================================================================

    Platform.cpp
    Created: 17 Jul 2022 9:49:14am
    Author:  joerg piringer

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"

#if JUCE_MAC
#include "PlatformOSX.mm"
#endif

#if JUCE_WIN
#include "PlatformWin.cpp"
#endif
