/*
  ==============================================================================

    VideoExporter.cpp
    Created: 17 Jul 2022 9:49:23am
    Author:  joerg piringer

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"

#if JUCE_MAC
#include "VideoExporterOSX.mm"
#endif

#if JUCE_WIN
#include "VideoExporterWin.cpp"
#endif
