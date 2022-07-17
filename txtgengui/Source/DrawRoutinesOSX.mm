/*
  ==============================================================================

    DrawRoutinesOSX.mm.cpp
    Created: 17 Jul 2022 3:34:50pm
    Author:  joerg piringer

  ==============================================================================
*/

#include "DrawRoutines.h"

#include <CoreGraphics/CoreGraphics.h>
#include <CoreText/CoreText.h>

#include "Program.hpp"

void drawTextNode(void *cg, TextNode *textNode, const char *fontName) {
    CGContextRef context = (CGContextRef)cg;
    CGFloat width = CGBitmapContextGetWidth(context);
    CGFloat height = CGBitmapContextGetHeight(context);

    CGContextSaveGState(context);
    

    
    CGContextTranslateCTM(context, (width*(textNode->getX()+1.f))/2.f, (height*(-textNode->getY()+1.f))/2.f);
    CGContextRotateCTM(context, textNode->getRotation()/180.f*M_PI);
    
    CGColorRef color = CGColorCreateGenericRGB(textNode->getRed(), textNode->getGreen(), textNode->getBlue(), textNode->getAlpha());
    CGContextSetStrokeColorWithColor(context, color);

    CFStringRef str = CFStringCreateWithCString(nullptr, textNode->getText().c_str(), kCFStringEncodingUTF8);
    
    CFStringRef fontNameStr = CFStringCreateWithCString(nullptr, fontName, kCFStringEncodingUTF8);
    CTFontRef font = CTFontCreateWithName(fontNameStr, textNode->getSize(), nullptr);
    
    CFStringRef keys[] = { kCTFontAttributeName };
    CFTypeRef values[] = { font };
    CFDictionaryRef attrs = CFDictionaryCreate(nullptr, (const void**)&keys, (const void**)&values, sizeof(keys) / sizeof(keys[0]),
                                               &kCFTypeDictionaryKeyCallBacks, &kCFTypeDictionaryValueCallBacks);
    CFAttributedStringRef attrString = CFAttributedStringCreate(nullptr, str, attrs);
    CTLineRef line = CTLineCreateWithAttributedString(attrString);
    CGRect bounds = CTLineGetBoundsWithOptions(line, kCTLineBoundsUseOpticalBounds);

    CGContextSetTextMatrix(context, CGAffineTransformIdentity);
    CGContextSetTextPosition(context, -bounds.size.width/2, -bounds.size.height/2);
    CTLineDraw(line, context);
    
    CFRelease(line);
    CGColorRelease(color);
    CFRelease(str);
    CFRelease(attrString);
    CFRelease(attrs);
    
    CFRelease(fontName);
    CFRelease(font);
    
    CGContextRestoreGState(context);
}

void drawTextWorld(void *cg, TextWorld *textWorld) {
#if TARGET_OS_MAC
    CGContextRef context = (CGContextRef)cg;
    float r, g, b, a;
    
    if (LuaProgram::sharedProgram() != nullptr) {
        LuaProgram::sharedProgram()->getBackground(r, g, b, a);
        CGColorRef color = CGColorCreateGenericRGB(r, g, b, a);
        CGContextSetFillColorWithColor(context, color);
        CGContextFillRect(context, CGRectMake(0, 0, CGBitmapContextGetWidth(context), CGBitmapContextGetHeight(context)));
        CGColorRelease(color);
    }
    
    for (auto n : textWorld->getNodes()) {
        n->drawNative(cg);
    }
#endif
}
