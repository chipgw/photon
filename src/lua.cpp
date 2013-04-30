#include "photon_lua.h"
#include "photon_core.h"
#include <lua.hpp>
#include <physfs.h>

namespace photon{

namespace lua{

lua_State *lua = nullptr;
photon_instance *instance = nullptr;

static int Print(lua_State *L) {
    int n = lua_gettop(L);  /* number of arguments */
    lua_getglobal(L, "tostring");
    std::string s = "LUA: ";
    for(int i = 1; i <= n; i++) {
        lua_pushvalue(L, -1);  /* function to be called */
        lua_pushvalue(L, i);   /* value to print */
        lua_call(L, 1, 1);
        if (i > 1){
            s.append("\t");
        }
        s.append(lua_tostring(L, -1));  /* get result */
        lua_pop(L, 1);  /* pop result */
    }
    PrintToLog(s.c_str());
    return 0;
}

namespace window_funcs{

static int ToggleFullscreen(lua_State *L) {
    if(instance != nullptr){
        window_managment::ToggleFullscreen(instance->window);
    }
    return 0;
}

static const luaL_Reg funcs[] = {
    {"toggle_fullscreen", ToggleFullscreen},
    {nullptr, nullptr}
};

}

namespace level_funcs{

static int LoadLevel(lua_State *L) {
    if(instance != nullptr){
        int n = lua_gettop(L);  /* number of arguments */
        std::string file;
        if(n == 1){
            lua_getglobal(L, "tostring");
            lua_pushvalue(L, -1);  /* function to be called */
            lua_pushvalue(L, 1);   /* value to print */
            lua_call(L, 1, 1);
            file = lua_tostring(L, -1);  /* get result */
            lua_pop(L, 1);  /* pop result */

            instance->level = level::LoadLevelXML(file, instance->player);

            PrintToLog("INFO: Lua loaded level file %s", file.c_str());
        }else{
            PrintToLog("LUA WARNING: load_level() called with the wrong number of arguments! expected 1 got %i!", n);
        }
    }
    return 0;
}

static int SaveLevel(lua_State *L) {
    if(instance != nullptr){
        int n = lua_gettop(L);  /* number of arguments */
        std::string file;
        if(n == 1){
            lua_getglobal(L, "tostring");
            lua_pushvalue(L, -1);  /* function to be called */
            lua_pushvalue(L, 1);   /* value to print */
            lua_call(L, 1, 1);
            file = lua_tostring(L, -1);  /* get result */
            lua_pop(L, 1);  /* pop result */

            level::SaveLevelXML(file, instance->level, instance->player);

            PrintToLog("INFO: Lua loaded level file %s", file.c_str());
        }else{
            PrintToLog("LUA WARNING: load_level() called with the wrong number of arguments! expected 1 got %i!", n);
        }
    }
    return 0;
}

static int CloseLevel(lua_State *L) {
    if(instance != nullptr){
        instance->level = photon_level();
    }
    return 0;
}

const luaL_Reg funcs[] = {
    {"load", LoadLevel},
    {"save", SaveLevel},
    {"close", CloseLevel},
    {nullptr, nullptr}
};
}

namespace gui_funcs{

static int SetMessage(lua_State *L) {
    if(instance != nullptr){
        int n = lua_gettop(L);  /* number of arguments */
        if(n == 1 || n == 2){
            if(n == 2){
                instance->gui.game.message_timeout = lua_tonumber(lua, 2);
            }else{
                instance->gui.game.message_timeout = INFINITY;
            }
            lua_getglobal(L, "tostring");
            lua_pushvalue(L, -1);  /* function to be called */
            lua_pushvalue(L, 1);   /* value to print */
            lua_call(L, 1, 1);
            instance->gui.game.message = lua_tostring(L, -1);  /* get result */
            lua_pop(L, 1);  /* pop result */

            PrintToLog("INFO: Lua set message to %s", instance->gui.game.message.c_str());
        }else{
            PrintToLog("LUA WARNING: set_message() called with the wrong number of arguments! expected 1 or 2 got %i!", n);
        }
    }
    return 0;
}

const luaL_Reg funcs[] = {
    {"set_message", SetMessage},
    {nullptr, nullptr}
};
}

namespace build_info_funcs{

static int IsDebug(lua_State *L) {
#ifdef NDEBUG
    lua_pushboolean(L, false);
#else
    lua_pushboolean(L, true);
#endif
    return 1;
}

static int GitSHA1(lua_State *L) {
    lua_pushstring(L, build_info::git_sha1);
    return 1;
}

static int Version(lua_State *L) {
    lua_pushstring(L, build_info::version);
    return 1;
}

const luaL_Reg funcs[] = {
    {"is_debug", IsDebug},
    {"version", Version},
    {"git_sha1", GitSHA1},
    {nullptr, nullptr}
};
}

#define PHOTON_API_ENTRY(F, N) luaL_newlib(lua, N::funcs); lua_setfield(lua, 1, F); lua_settop(lua, 1)

void InitLua(photon_instance &in, const std::string &initscript){
    PrintToLog("INFO: Initializing Lua.");
    lua = luaL_newstate();

    instance = &in;

    if(lua == nullptr){
        PrintToLog("ERROR: Unable to initilize Lua! luaL_newstate() returned null!");
        throw;
    }

    luaL_openlibs(lua);

    lua_register(lua, "print", Print);
    lua_register(lua, "io.write", Print);

    lua_newtable(lua);

    PHOTON_API_ENTRY("window", window_funcs);
    PHOTON_API_ENTRY("level", level_funcs);
    PHOTON_API_ENTRY("build", build_info_funcs);
    PHOTON_API_ENTRY("gui", gui_funcs);

    lua_setglobal(lua, "photon");

    DoFile(initscript);
}

void GarbageCollect(){
    lua_close(lua);

    PrintToLog("INFO: Lua garbage collection complete.");
}

int DoFile(const std::string &filename){
    if(PHYSFS_exists(filename.c_str()) && lua != nullptr){
        auto fp = PHYSFS_openRead(filename.c_str());
        intmax_t length = PHYSFS_fileLength(fp);
        if(length > 0){
            char *buffer = (char*)malloc(length + 1);

            // make it null terminated (fixes some Lua errors)
            buffer[length] = 0;

            PHYSFS_read(fp, buffer, 1, length);

            PHYSFS_close(fp);

            int state = luaL_dostring(lua, buffer);

            if(state!=0){
                PrintToLog("LUA ERROR: %s\n", lua_tostring(lua, -1));
                lua_pop(lua, 1);
            }else{
                return 0;
            }
        }
    }
    return 1;
}

}

}
