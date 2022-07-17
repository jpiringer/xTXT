/*
  ==============================================================================

    DrawRoutines.cpp
    Created: 17 Jul 2022 3:34:24pm
    Author:  joerg piringer

  ==============================================================================
*/

#include "DrawRoutines.h"

#include "../JuceLibraryCode/JuceHeader.h"

#if JUCE_MAC
#include "DrawRoutinesOSX.mm"
#endif

#if JUCE_WIN
#include "DrawRoutinesWin.cpp"
#endif
