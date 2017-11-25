//
//  Program.cpp
//  txtgencmd
//
//  Created by joerg piringer on 06.10.17.
//  Copyright © 2017 joerg piringer. All rights reserved.
//

#include "Program.hpp"

#include "Runner.hpp"

#if TARGET_MACOS
#include <lua.hpp>
#else
#include "lua.hpp"
#endif

#define MAX_WAIT_TIME 10.f

TextWorld TextWorld::textWorld;
std::shared_ptr<Font> textDisplayFont;

LuaProgram *LuaProgram::theProgram = nullptr;
static std::string programOutput = "";

void TextNode::draw(Graphics &g) {
    const Graphics::ScopedSaveState state(g);
    Rectangle<int> r = g.getClipBounds();
    
    textDisplayFont->setHeight(size);
    g.setFont(*textDisplayFont);
    //g.addTransform(AffineTransform::scale(scale));
    g.addTransform(AffineTransform::translation(r.getX()+(r.getWidth()*(getX()+1.f))/2.f, r.getY()+(r.getHeight()*(getY()+1.f))/2.f));
    //g.addTransform(AffineTransform::rotation(angle));
    g.setColour(Colour((uint8)(getRed()*255.f), (uint8)(getGreen()*255.f), (uint8)(getBlue()*255.f), getAlpha()));
    g.drawSingleLineText(text, 0, 0, Justification::horizontallyCentred);
}

TextWorld::~TextWorld() {
}

void TextWorld::addNode(std::shared_ptr<Node> node) {
    std::lock_guard<std::mutex> guard(worldMutex);
    
    nodes.push_back(node); node->setID(nextID++);
}

void TextWorld::removeNode() {
    std::lock_guard<std::mutex> guard(worldMutex);

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


std::wstring LuaProgram::convertToProgram(const std::wstring &str) {
    std::wstring ret =
LR"(-- copy this to Program mode to execute there
screen(1920, 1080)
color("black")
background("white")
size(30)
center()
)";
    
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
    textNode->setPos(x, y);
    
    TextWorld::sharedTextWorld().addNode(textNode);
    
    return textNode;
}

static int showL(lua_State *L) {
    checkDisplay();
    
    auto node = addText(lua_tostring(L, 1));
    
    lua_Number lifetime = lua_tonumber(L, 2);
    node->setLifetime(lifetime);

    if (lifetime > MAX_WAIT_TIME) {
        luaL_error(L, "cannot wait for more than %f seconds (was: %f)", MAX_WAIT_TIME, (float)lifetime);
    }

    std::this_thread::sleep_for(std::chrono::milliseconds((long)(lifetime*1000.f)));
    
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
    
    luaL_error(L, "function not yet implemented: removeText");

    return 0;
}

static int clearL(lua_State *L) {
    checkDisplay();
    
    TextWorld::sharedTextWorld().removeAllNodes();

    return 0;
}

static int colorL(lua_State *L) {
    float r, g, b, a;
    
    if (lua_isstring(L, 1)) {
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
    
    if (lua_isstring(L, 1)) {
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

static int rotateL(lua_State *L) {
    luaL_error(L, "function not yet implemented: rotate");
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
    std::this_thread::sleep_for(std::chrono::milliseconds((long)(t*1000.f)));
    return 0;
}

#define CMD(c) {#c, c##L}
static const struct luaL_Reg printlib [] = {
    CMD(print),
    CMD(screen), // set the screen/window size
    CMD(show), // show a text for a certain time
    CMD(addText), // add a text node
    CMD(removeText), // remove a certain text
    CMD(clear), // clear all texts
    CMD(color), // set the text color
    CMD(background), // set the background color
    CMD(left), // left adjust
    CMD(right), // right adjust
    CMD(center), // center text
    CMD(position), // move next text to x, y
    CMD(rotate), // rotate next text
    CMD(size), // set the size
    CMD(wait), // wait for x seconds
    {nullptr, nullptr}
};

LuaProgram::~LuaProgram() {
    textDisplayFont = nullptr;
}

void LuaProgram::instructionHook(lua_State *L) {
    if (threadShouldExit()) {
        luaL_error(L, "User interrupt");
    }
}

void instructionHook(lua_State *L, lua_Debug *ar) {
    if(ar->event == LUA_HOOKCOUNT) {
        if (LuaProgram::sharedProgram() != nullptr) {
            LuaProgram::sharedProgram()->instructionHook(L);
        }
    }
}

void LuaProgram::changeOutput(const std::string &str) {
    runContext->changeOutput(str);
}

void LuaProgram::changeShowSize(float width, float height) {
    runContext->changeShowSize(width, height);
}

void LuaProgram::run() {
    lua_State *L = luaL_newstate();
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
}

std::string LuaProgram::execute(jp::RunContext *_runContext) {
    programOutput = "";
    theProgram = this;
    
    runContext = _runContext;
    
    setSize(30);
    setColor(0,0,0,1);
    setBackground(1, 1, 1, 1);
    setPosition(0, 0);
    setJustification(jp::JustificationCenter);

    textDisplayFont = std::make_shared<Font>(Font::getDefaultMonospacedFontName(), 20, Font::plain);

    TextWorld::sharedTextWorld().removeAllNodes();
    
    if (isThreadRunning()) {
        stopThread((MAX_WAIT_TIME+1.f)*1000);
    }
    else {
        startThread();
    }
    
    return programOutput;
}
