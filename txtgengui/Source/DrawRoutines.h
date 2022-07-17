/*
  ==============================================================================

    DrawRoutines.h
    Created: 17 Jul 2022 3:37:42pm
    Author:  joerg piringer

  ==============================================================================
*/

#pragma once

class TextNode;
class TextWorld;

void drawTextNode(void *cg, TextNode *textNode, const char *fontName);

void drawTextWorld(void *cg, TextWorld *textWorld);
