//
//  Program.cpp
//  txtgencmd
//
//  Created by joerg piringer on 06.10.17.
//  Copyright © 2017 joerg piringer. All rights reserved.
//

#include "Program.hpp"

#if TARGET_MACOS
#include <lua.hpp>
#else
#include "lua.hpp"
#endif

LuaProgram *LuaProgram::theProgram = nullptr;
static std::string programOutput = "";

std::wstring LuaProgram::convertToProgram(const std::wstring &str) {
    std::wstring ret =
LR"(-- copy this to Program mode to execute there
color("black")
background("white")
size(30)
center()
)";
    
    auto newline = [&ret]() {
        ret += L"show(\"";
    };
    auto endline = [&ret]() {
        ret += L"\", 0.3)\n";
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

static int printL(lua_State* L) {
    int nargs = lua_gettop(L);
    for (int i=1; i <= nargs; ++i) {
        programOutput += lua_tostring(L, i);
    }
    programOutput += "\n";
    
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
    COL(grey, 0.5, 0.5, 0.5) else
    COL(lightgrey, 0.75, 0.75, 0.75) else
    COL(darkgrey, 0.25, 0.25, 0.25) else {
        luaL_error(L, "color '%s' unknown", str);
    }
}

static int showL(lua_State *L) {
    checkDisplay();
    
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
        a = lua_tonumber(L, 4);
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
        a = lua_tonumber(L, 4);
    }
    
    LuaProgram::sharedProgram()->setBackground(r, g, b, a);
    
    return 0;}

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

static int moveL(lua_State *L) {
    luaL_error(L, "function not yet implemented: move");
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

#define CMD(c) {#c, c##L}
static const struct luaL_Reg printlib [] = {
    CMD(print),
    CMD(show),
    CMD(color),
    CMD(background),
    CMD(left),
    CMD(right),
    CMD(center),
    CMD(move),
    CMD(rotate),
    CMD(size),
    {nullptr, nullptr}
};

std::string LuaProgram::execute() {
    programOutput = "";
    theProgram = this;
    
    setSize(30);
    setColor(0,0,0,1);
    setBackground(1, 1, 1, 1);
    setJustification(jp::JustificationCenter);
    
    lua_State *L = luaL_newstate();
    luaL_openlibs(L);
    lua_getglobal(L, "_G");
    luaL_setfuncs(L, printlib, 0);
    lua_pop(L, 1);
    luaL_loadstring(L, code.c_str());
    if (lua_pcall(L, 0, LUA_MULTRET, 0) != LUA_OK) {
        _error(lua_tostring(L, -1));
    }
    lua_close(L);
    
    return programOutput;
}
