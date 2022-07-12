//
//  Program.cpp
//  txtgencmd
//
//  Created by joerg piringer on 06.10.17.
//  Copyright © 2017 joerg piringer. All rights reserved.
//

#include "Program.hpp"

#include "Runner.hpp"

#include "Parser.hpp"

#include "utils.hpp"

#if TARGET_OS_MAC
#include <lua/lua.hpp>
#else
#include "lua.hpp"
#endif

#include <cstdlib>
#include <locale>

#if TARGET_OS_MAC
#include <CoreGraphics/CoreGraphics.h>
#include <CoreText/CoreText.h>

#include "VideoExporter.hpp"
#endif

#define MAX_WAIT_TIME 10.f

inline float randRange(float lower, float upper) {
	return ((((float)std::rand()) / (float)RAND_MAX)*(upper - lower) + lower);
}

TextWorld TextWorld::textWorld;
std::shared_ptr<Font> textDisplayFont;

LuaProgram *LuaProgram::theProgram = nullptr;
static std::string programOutput = "";

void TextNode::draw(Graphics &g) {
    const Graphics::ScopedSaveState state(g);
    Rectangle<int> r = g.getClipBounds();
    
    if (textDisplayFont == nullptr) {
        textDisplayFont = std::make_shared<Font>(Font::getDefaultMonospacedFontName(), 20, Font::plain);
    }
    textDisplayFont->setHeight(size);
    g.setFont(*textDisplayFont);
    //g.addTransform(AffineTransform::scale(scale));
    g.addTransform(AffineTransform::translation(r.getX()+(r.getWidth()*(getX()+1.f))/2.f, r.getY()+(r.getHeight()*(getY()+1.f))/2.f));
    
    g.addTransform(AffineTransform::rotation(getRotation()/180.f*M_PI));
    g.setColour(Colour((uint8)(getRed()*255.f), (uint8)(getGreen()*255.f), (uint8)(getBlue()*255.f), getAlpha()));
    g.drawSingleLineText(text, 0, 0, Justification::horizontallyCentred);
}

void TextNode::drawNative(void *cg) {
#if TARGET_OS_MAC
	CGContextRef context = (CGContextRef)cg;
    CGFloat width = CGBitmapContextGetWidth(context);
    CGFloat height = CGBitmapContextGetHeight(context);

    CGContextSaveGState(context);
    
    if (textDisplayFont == nullptr) {
        textDisplayFont = std::make_shared<Font>(Font::getDefaultMonospacedFontName(), 20, Font::plain);
    }
    
    CGContextTranslateCTM(context, (width*(getX()+1.f))/2.f, (height*(-getY()+1.f))/2.f);
    CGContextRotateCTM(context, getRotation()/180.f*M_PI);
    
    CGColorRef color = CGColorCreateGenericRGB(getRed(), getGreen(), getBlue(), getAlpha());
    CGContextSetStrokeColorWithColor(context, color);

    CFStringRef str = CFStringCreateWithCString(nullptr, text.c_str(), kCFStringEncodingUTF8);
    
    CFStringRef fontName = CFStringCreateWithCString(nullptr, textDisplayFont->getTypeface()->getName().getCharPointer(), kCFStringEncodingUTF8);
    CTFontRef font = CTFontCreateWithName(fontName, size, nullptr);
    
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
#endif
}

TextWorld::~TextWorld() {
}

void TextWorld::addNode(std::shared_ptr<Node> node) {
    std::lock_guard<std::mutex> guard(worldMutex);
    
    nodes.push_back(node); node->setID(nextID++);
}

void TextWorld::removeNode(int _id) {
    std::lock_guard<std::mutex> guard(worldMutex);

    for (auto n : nodes) {
        if (n->getID()) {
            nodes.remove(n);
            break;
        }
    }
}

void TextWorld::removeAllNodes() {
    std::lock_guard<std::mutex> guard(worldMutex);

    nodes.clear();
}

void TextWorld::update(float dt) {
    std::lock_guard<std::mutex> guard(worldMutex);
    std::vector<std::shared_ptr<Node>> nodesToDelete;
    
    for (auto n : nodes) {
        n->update(dt);
        if (n->isDead()) {
            nodesToDelete.push_back(n);
        }
    }
    
    for (auto n : nodesToDelete) {
        nodes.remove(n);
    }
}

void TextWorld::draw(Graphics &gc) {
    std::lock_guard<std::mutex> guard(worldMutex);
    float r, g, b, a;

    if (LuaProgram::sharedProgram() != nullptr) {
        LuaProgram::sharedProgram()->getBackground(r, g, b, a);
        gc.setColour(Colour((uint8)(r*255.f), (uint8)(g*255.f), (uint8)(b*255.f), a));
        Rectangle<int> rect = gc.getClipBounds();
        gc.fillRect(rect);
    }
    
    for (auto n : nodes) {
        n->draw(gc);
    }
}

void TextWorld::drawNative(void *cg) {
#if TARGET_OS_MAC
    CGContextRef context = (CGContextRef)cg;
    std::lock_guard<std::mutex> guard(worldMutex);
    float r, g, b, a;
    
    if (LuaProgram::sharedProgram() != nullptr) {
        LuaProgram::sharedProgram()->getBackground(r, g, b, a);
        CGColorRef color = CGColorCreateGenericRGB(r, g, b, a);
        CGContextSetFillColorWithColor(context, color);
        CGContextFillRect(context, CGRectMake(0, 0, CGBitmapContextGetWidth(context), CGBitmapContextGetHeight(context)));
        CGColorRelease(color);
    }
    
    for (auto n : nodes) {
        n->drawNative(cg);
    }
#endif
}

void LuaProgram::_error(const std::string &msg) {
    errors += msg + "\n";
    
    errorCount++;
    runContext->setErrors(errors);
}

std::wstring LuaProgram::convertToProgram(const std::wstring &code, const std::wstring &str, jp::RunnerType sourceType) {
    std::wstring ret =
LR"(-- copy this to Program mode to execute there
screen(1920, 1080)
color("black")
background("white")
size(30)
center()
)";
    
    switch (sourceType) {
        case jp::NamShub: {
            auto newline = [&ret]() {
                ret += L"show(\"";
            };
            auto endline = [&ret]() {
                ret += L"\", 1)\n";
            };
            
            newline();
            for (size_t i = 0; i < str.length(); i++) {
                if (str[i] == '\"') {
                    ret += L"\\\"";
                }
                if (str[i] == '\n') {
                    endline();
                    newline();
                }
                else {
                    ret += str[i];
                }
            }
            endline();
        }
        break;
        case jp::Grammar: {
            ret += L"local results = grammar [[\n";
            ret += code;
            ret += L"]]\n";
            ret += L"showAndSpeak(results, 5)";
        }
        break;
        case jp::LSystem: {
            ret += L"local results = LSystem([[\n";
            ret += code;
            ret += L"]], [[\n";
            ret += str;
            ret += L"]])\n";
            ret += L"showAndSpeak(results, 5)";
        }
        break;
        default:
            break;
    }
    
    return ret;
}

static int printL(lua_State *L) {
    int nargs = lua_gettop(L);
    for (int i=1; i <= nargs; ++i) {
        programOutput += lua_tostring(L, i);
    }
    programOutput += "\n";
    
    LuaProgram::sharedProgram()->changeOutput(programOutput);
    
    return 0;
}

static int screenL(lua_State *L) {
    lua_Number width = lua_tonumber(L, 1);
    lua_Number height = lua_tonumber(L, 2);
    
    LuaProgram::sharedProgram()->changeShowSize(width, height);
    
    return 0;
}

static void checkDisplay() {
    
}

#define COL(nm, _r, _g, _b) if (strcmp(str, #nm) == 0) {r = _r; g = _g; b = _b;}
static void interpretColor(lua_State *L, const char *str, float &r, float &g, float &b) {
    COL(white, 1, 1, 1) else
    COL(black, 0, 0, 0) else
    COL(red, 1, 0, 0) else
    COL(green, 0, 1, 0) else
    COL(blue, 0, 0, 1) else
    COL(yellow, 1, 1, 0) else
    COL(orange, 1, 153.f/255.f, 53.f/255.f) else
    COL(brown, 153.f/255.f, 76.f/255.f, 0) else
    COL(grey, 0.5, 0.5, 0.5) else
    COL(lightgrey, 0.75, 0.75, 0.75) else
    COL(darkgrey, 0.25, 0.25, 0.25) else {
        luaL_error(L, "color '%s' unknown", str);
    }
}

std::shared_ptr<Node> addText(const std::string &str) {
    float r, g, b, a;
    float x, y;

    auto textNode = std::make_shared<TextNode>(str);
    textNode->setJustification(LuaProgram::sharedProgram()->getJustification());
    textNode->setSize(LuaProgram::sharedProgram()->getSize());
    
    LuaProgram::sharedProgram()->getColor(r, g, b, a);
    textNode->setColor(r, g, b, a);
    
    LuaProgram::sharedProgram()->getPosition(x, y);
    textNode->setPosition(x, y);
    
    textNode->setRotation(LuaProgram::sharedProgram()->getRotation());
    
    TextWorld::sharedTextWorld().addNode(textNode);
    
    return textNode;
}

static void wait(float time) {
    LuaProgram::sharedProgram()->wait(time);
}

static int showL(lua_State *L) {
    checkDisplay();
    
    auto node = addText(lua_tostring(L, 1));
    
    lua_Number lifetime = lua_tonumber(L, 2);
    node->setLifetime(lifetime);

    if (lifetime > MAX_WAIT_TIME) {
        luaL_error(L, "cannot wait for more than %f seconds (was: %f)", MAX_WAIT_TIME, (float)lifetime);
    }

    wait(lifetime);
    
    return 0;
}

static int showAndSpeakL(lua_State *L) {
    checkDisplay();
    std::string str = lua_tostring(L, 1);
    
    auto node = addText(str);
    
    lua_Number lifetime = lua_tonumber(L, 2);
    node->setLifetime(lifetime);

    if (lifetime > MAX_WAIT_TIME) {
        luaL_error(L, "cannot wait for more than %f seconds (was: %f)", MAX_WAIT_TIME, (float)lifetime);
    }
    
    LuaProgram::sharedProgram()->speak(str);

    wait(lifetime);
    
    return 0;
}

static int addTextL(lua_State *L) {
    checkDisplay();

    auto node = addText(lua_tostring(L, 1));
    lua_Integer _id = node->getID();
    
    lua_pushinteger(L, _id);
    return 1;
}

static int removeTextL(lua_State *L) {
    checkDisplay();
    
    lua_Integer _id = lua_tointeger(L, 1);

    TextWorld::sharedTextWorld().removeNode((int)_id);

    return 0;
}

static int clearL(lua_State *L) {
    checkDisplay();
    
    TextWorld::sharedTextWorld().removeAllNodes();

    return 0;
}

static int colorL(lua_State *L) {
    float r, g, b, a;
    
    if (!lua_isnumber(L, 1)) {
        a = 1;
        interpretColor(L, lua_tostring(L, 1), r, g, b);
    }
    else {
        r = lua_tonumber(L, 1);
        g = lua_tonumber(L, 2);
        b = lua_tonumber(L, 3);
        a = luaL_optnumber(L, 4, 1);
    }
    
    LuaProgram::sharedProgram()->setColor(r, g, b, a);
    
    return 0;
}

static int backgroundL(lua_State *L) {
    float r, g, b, a;
    
    if (!lua_isnumber(L, 1)) {
        a = 1;
        interpretColor(L, lua_tostring(L, 1), r, g, b);
    }
    else {
        r = lua_tonumber(L, 1);
        g = lua_tonumber(L, 2);
        b = lua_tonumber(L, 3);
        a = luaL_optnumber(L, 4, 1);
    }
    
    LuaProgram::sharedProgram()->setBackground(r, g, b, a);
    
    return 0;
}

static int leftL(lua_State *L) {
    LuaProgram::sharedProgram()->setJustification(jp::JustificationLeft);
    return 0;
}

static int rightL(lua_State *L) {
    LuaProgram::sharedProgram()->setJustification(jp::JustificationRight);
    return 0;
}

static int centerL(lua_State *L) {
    LuaProgram::sharedProgram()->setJustification(jp::JustificationCenter);
    return 0;
}

static int positionL(lua_State *L) {
    LuaProgram::sharedProgram()->setPosition(lua_tonumber(L, 1), lua_tonumber(L, 2));
    return 0;
}

static int randomPositionL(lua_State *L) {
    LuaProgram::sharedProgram()->setPosition(randRange(-1,1), randRange(-1,1));
    return 0;
}

static int rotationL(lua_State *L) {
    LuaProgram::sharedProgram()->setRotation(lua_tonumber(L,1));
    return 0;
}

static int randomRotationL(lua_State *L) {
    LuaProgram::sharedProgram()->setRotation(randRange(0,360));
    return 0;
}

static int sizeL(lua_State *L) {
    LuaProgram::sharedProgram()->setSize(lua_tonumber(L, 1));
    return 0;
}

static int waitL(lua_State *L) {
    lua_Number t = lua_tonumber(L, 1);
    if (t > MAX_WAIT_TIME) {
        luaL_error(L, "cannot wait for more than %f seconds (was: %f)", MAX_WAIT_TIME, (float)t);
    }
    wait(t);
    return 0;
}

static int waitForKeyL(lua_State *L) {
    LuaProgram::sharedProgram()->waitForKey();
    return 0;
}

static int foreverL(lua_State *L) {
    for (;;) {
        waitForKeyL(L);
        if (LuaProgram::sharedProgram()->threadShouldExit()) {
            luaL_error(L, "User interrupt");
        }
    }
    return 0;
}

static int speakL(lua_State *L) {
    std::string str = lua_tostring(L, 1);
    
    LuaProgram::sharedProgram()->speak(str);
    return 0;
}

int getKeyCode(const char *s) {
    if (strcmp(s, "space") == 0) {
        return 32;
    }
    if (strcmp(s, "enter") == 0) {
        return 13;
    }
    if (strcmp(s, "backspace") == 0) {
        return 127;
    }
    if (strcmp(s, "tab") == 0) {
        return 9;
    }
    if (strcmp(s, "esc") == 0) {
        return 27;
    }
    if (strcmp(s, "up") == 0) {
        return 63232;
    }
    if (strcmp(s, "down") == 0) {
        return 63233;
    }
    if (strcmp(s, "left") == 0) {
        return 63234;
    }
    if (strcmp(s, "right") == 0) {
        return 63235;
    }
    std::locale loc;

    return std::toupper(s[0], loc);
}

static int onKeyL(lua_State *L) {
    int keyCode = -1;
    
    if (lua_isstring(L, 1)) {
        const char *key = lua_tostring(L, 1);

        keyCode = getKeyCode(key);
    }
    else if (lua_isnumber(L, 1)) {
        keyCode = lua_tonumber(L, 1);
    }
    
    if (lua_type(L, 2) == LUA_TFUNCTION) {
        lua_pushvalue(L, 2);
        int ref = luaL_ref(L, LUA_REGISTRYINDEX);
        
        LuaProgram::sharedProgram()->registerKeyFunction(keyCode, ref);
    }
    else {
        luaL_error(L, "parameter 2 must be a function");
    }
    
    return 0;
}

static int grammarL(lua_State *L) {
    std::string grammar = lua_tostring(L, 1);
    auto converted = fromUTF8(grammar);
    
    jp::Parser parser(converted);
    parser.parse();
    std::string result = toUTF8(parser.generate());
    
    if (parser.getErrorCount() > 0) {
        std::string errors = toUTF8(parser.getErrors());
        luaL_error(L, "grammar error: \'%s\'", errors.c_str());
    }
    
    lua_pushstring(L, result.c_str());
    return 1;
}

static int LSystemL(lua_State *L) {
    std::string rules = lua_tostring(L, 1);
    std::string input = lua_tostring(L, 2);
    auto converted = fromUTF8(rules);
    
    auto lsystem = LSystem::parseLSystem(converted);
    
    lsystem->setState(fromUTF8(input));
    lsystem->iterate();
    std::string result = toUTF8(lsystem->getState());
    
    if (lsystem->getErrorCount() > 0) {
        std::string errors = toUTF8(lsystem->getErrors());
        luaL_error(L, "LSystem error: \'%s\'", errors.c_str());
    }
    
    lua_pushstring(L, result.c_str());
    return 1;
}


#define CMD(c) {#c, c##L}
static const struct luaL_Reg printlib [] = {
    CMD(print), // print replacement for capturing the output
    CMD(screen), // show the window & set the screen/window size
    CMD(show), // show a text for a certain time
    CMD(showAndSpeak), // show a text for a certain time
    CMD(addText), // add a text node
    CMD(removeText), // remove a certain text
    CMD(clear), // clear all texts
    CMD(color), // set the text color
    CMD(background), // set the background color
    CMD(left), // left adjust
    CMD(right), // right adjust
    CMD(center), // center text
    CMD(position), // move next text to x, y
    CMD(randomPosition), // move next text to a random position
    CMD(rotation), // rotate next text
    CMD(randomRotation), // rotate next text
    CMD(size), // set the size
    CMD(wait), // wait for x seconds
    CMD(speak), // speak string
    CMD(onKey), // react to key input
    CMD(waitForKey), // waits until a key was pressed
    CMD(forever), // waits forever (or until interrupt)
    CMD(grammar), // generate from a grammar
    CMD(LSystem), // generate from a grammar
    {nullptr, nullptr}
};

LuaProgram::~LuaProgram() {
    textDisplayFont = nullptr;
}

bool LuaProgram::processKeyPresses() {
    bool pressed = false;
    
    if (!keyPresses.empty()) {
        int keyCode = keyPresses.front();
        keyPresses.pop_front();
        
        pressed = true;
        if (keyMap.find(keyCode) != keyMap.end()) {
            int funcRef = keyMap[keyCode];
            
            lua_rawgeti(L, LUA_REGISTRYINDEX, funcRef);
            lua_pcall(L, 0, 0, 0);
            //std::cout << "key press processed: " << keyCode << std::endl;
        }
    }
    return pressed;
}

void LuaProgram::instructionHook(lua_State *L) {
    if (threadShouldExit()) {
        luaL_error(L, "User interrupt");
    }
    processKeyPresses();
}

void instructionHook(lua_State *L, lua_Debug *ar) {
    if(ar->event == LUA_HOOKCOUNT) {
        if (LuaProgram::sharedProgram() != nullptr) {
            LuaProgram::sharedProgram()->instructionHook(L);
        }
    }
}

void LuaProgram::changeOutput(const std::string &str) {
    if (isExporting()) {
        checkExportStarted();
    }
    else {
        runContext->changeOutput(str);
    }
}

void LuaProgram::changeShowSize(float width, float height) {
    if (isExporting()) {
        if (exportingStarted) { // only one changeShowSize allowed
            luaL_error(L, "command 'screen' can only be used once");
        }
        else {
            exportingStarted = true;
#if TARGET_OS_MAC
            exporter->start(filenameExport, width, height);
#else
			luaL_error(L, "exporting video doesn't work on Windows yet");
#endif
        }
    }
    else {
        runContext->changeShowSize(width, height);
    }
}

void LuaProgram::speak(const std::string &str) {
    if (isExporting()) {
        checkExportStarted();
    }
    else {
        runContext->speak(str);
    }
}

void LuaProgram::wait(float time) {
    if (isExporting()) { // then write frames
        checkExportStarted();
#if TARGET_OS_MAC
        residualWaitTime = exporter->encodeFrames(time+residualWaitTime, [this](float dt, void *context) {
            TextWorld::sharedTextWorld().update(dt);
            TextWorld::sharedTextWorld().drawNative(context);
        });
#endif
    }
    else {
        const float steps = 10;
        float milliseconds = time * 1000;
        while (milliseconds > 0) {
            if (threadShouldExit()) {
                luaL_error(L, "User interrupt");
            }
            LuaProgram::sharedProgram()->processKeyPresses();
            std::this_thread::sleep_for(std::chrono::milliseconds((long)(steps)));
            milliseconds -= steps;
        }
    }
}

void LuaProgram::waitForKey() {
    if (isExporting()) { // do nothing
    }
    else {
        while (!LuaProgram::sharedProgram()->processKeyPresses()) {
            if (threadShouldExit()) {
                luaL_error(L, "User interrupt");
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    }
}

void LuaProgram::checkExportStarted() {
    if (isExporting()) {
        if (!exportingStarted) {
            luaL_error(L, "Export must be started first with 'screen'");
        }
    }
}

void LuaProgram::run() {
    L = luaL_newstate();
    
    luaL_openlibs(L);
    lua_sethook(L, ::instructionHook, LUA_MASKCOUNT, 1);
    lua_getglobal(L, "_G");
    luaL_setfuncs(L, printlib, 0);
    lua_pop(L, 1);
    luaL_loadstring(L, code.c_str());
    if (lua_pcall(L, 0, LUA_MULTRET, 0) != LUA_OK) {
        _error(lua_tostring(L, -1));
    }
    lua_close(L);
    L = nullptr;
    
    if (isExporting()) {
        checkExportStarted();
#if TARGET_OS_MAC
        exporter->stop();
#endif
    }
}

void LuaProgram::initState() {
    programOutput = "";
    theProgram = this;
    residualWaitTime = 0;
    exportingStarted = false;
    
    setSize(30);
    setColor(0,0,0,1);
    setBackground(1, 1, 1, 1);
    setPosition(0, 0);
    setRotation(0);
    setJustification(jp::JustificationCenter);
    
    TextWorld::sharedTextWorld().removeAllNodes();
}

std::string LuaProgram::execute(jp::RunContext *_runContext) {
    exporting = false;

    runContext = _runContext;
    errorCount = 0;
    errors = "";
    
    keyMap.clear();
    keyPresses.clear();
    
    initState();
    
    if (isThreadRunning()) {
        stopThread((MAX_WAIT_TIME+1.f)*1000);
    }
    else {
        startThread();
    }
    
    return programOutput;
}

void LuaProgram::exportVideo(const std::string &filename) {
    exporting = true;
    
    runContext = nullptr;
#if TARGET_OS_MAC
    exporter = std::make_shared<jp::VideoExporter>();
#endif
    filenameExport = filename;

    initState();
    
    if (isThreadRunning()) {
        stopThread((MAX_WAIT_TIME+1.f)*1000);
    }
    else {
        runThread();
    }
}

void LuaProgram::keyPressed(int keyCode) {
    //std::cout << "key pressed: " << keyCode << std::endl;
    keyPresses.push_back(keyCode);
}
