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

static std::string programOutput = "";

static int l_my_print(lua_State* L) {
    int nargs = lua_gettop(L);
    for (int i=1; i <= nargs; ++i) {
        programOutput += lua_tostring(L, i);
    }
    programOutput += "\n";
    
    return 0;
}

static const struct luaL_Reg printlib [] = {
    {"print", l_my_print},
    {NULL, NULL} /* end of array */
};

std::string LuaProgram::execute() {
    programOutput = "";
    
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
