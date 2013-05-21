#include "photon_lua.h"
#include "photon_core.h"

#include <lua.hpp>
#include <physfs.h>

namespace photon{

namespace lua{

struct timer{
    int lua_call_ref = LUA_NOREF;
    float timeout = INFINITY;
};

std::vector<timer> timers;

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

namespace generic_funcs{

static int After(lua_State *L){
    if(instance != nullptr){
        if(lua_gettop(L) == 2 && lua_isfunction(L, 1) && lua_isnumber(L, 2)){
            timer t;
            t.timeout = instance->level.time + lua_tonumber(L, 2);
            lua_pop(L, 1);

            int f = luaL_ref(L, LUA_REGISTRYINDEX);

            if(f != LUA_REFNIL && f != LUA_NOREF){
                t.lua_call_ref = f;

                timers.push_back(t);

                lua_pushboolean(L, true);
            }else{
                PrintToLog("WARNING: Unable to create timer: error getting function reference!");
                lua_pushboolean(L, false);
            }
        }else{
            PrintToLog("WARNING: Unable to create timer: invalid arguments!");
            lua_pushboolean(L, false);
        }
        return 1;
    }
    return 0;
}

static const luaL_Reg funcs[] = {
    {"after", After},
    {nullptr, nullptr}
};

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

            level::LoadLevelXML(file, *instance);

            PrintToLog("INFO: Lua loaded level file %s", file.c_str());
        }else{
            PrintToLog("LUA WARNING: level.load() called with the wrong number of arguments! expected 1 got %i!", n);
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

            PrintToLog("INFO: Lua saved level file %s", file.c_str());
        }else{
            PrintToLog("LUA WARNING: level.save() called with the wrong number of arguments! expected 1 got %i!", n);
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

static int SetCheckVictory(lua_State *L){
    if(instance != nullptr && lua_gettop(L) == 1 && lua_isfunction(L, -1)){
        int f = luaL_ref(L, LUA_REGISTRYINDEX);

        if(f != LUA_REFNIL && f != LUA_NOREF){
            instance->level.mode = photon_level::script;
            instance->level.lua_checkvictory = f;
            PrintToLog("INFO: Lua set victory condition.");
        }else{
            PrintToLog("WARNING: Unable to set lua victory condition!");
        }
    }else{
        PrintToLog("WARNING: Unable to set lua victory condition!");
        lua_pushboolean(L, false);
    }
    return 1;
}

static int GetItemCount(lua_State *L) {
    if(instance != nullptr){
        int n = lua_gettop(L);  /* number of arguments */
        if(n == 1){
            lua_getglobal(L, "tostring");
            lua_pushvalue(L, -1);  /* function to be called */
            lua_pushvalue(L, 1);   /* value to print */
            lua_call(L, 1, 1);
            std::string type_str = lua_tostring(L, -1);  /* get result */
            lua_settop(L, 0);

            int count = 0;
            block_type type = blocks::GetBlockFromName(type_str.c_str());

            for(auto block : instance->level.grid){
                if(block.second.type == type){
                    count++;
                }
            }

            lua_pushinteger(L, count);
            return 1;
        }else if(n == 0){
            lua_pushinteger(L, instance->level.grid.size());
            return 1;
        }else{
            PrintToLog("LUA WARNING: level.get_item_count() called with the wrong number of arguments! expected 0 or 1 got %i!", n);
        }
    }
    return 0;
}

const luaL_Reg funcs[] = {
    {"load", LoadLevel},
    {"save", SaveLevel},
    {"close", CloseLevel},
    {"set_victory_condition", SetCheckVictory},
    {"get_item_count", GetItemCount},
    {nullptr, nullptr}
};
}

namespace player_funcs{

static int GetItemCount(lua_State *L) {
    if(instance != nullptr){
        int n = lua_gettop(L);  /* number of arguments */
        if(n == 1){
            lua_getglobal(L, "tostring");
            lua_pushvalue(L, -1);  /* function to be called */
            lua_pushvalue(L, 1);   /* value to print */
            lua_call(L, 1, 1);
            std::string type = lua_tostring(L, -1);  /* get result */
            lua_pop(L, 1);  /* pop result */

            lua_pushinteger(L, player::GetItemCount(instance->player, blocks::GetBlockFromName(type.c_str())));
            return 1;
        }else if(n == 0){
            lua_pushinteger(L, player::GetItemCountCurrent(instance->player));
            return 1;
        }else{
            PrintToLog("LUA WARNING: player.get_item_count() called with the wrong number of arguments! expected 0 or 1 got %i!", n);
        }
    }
    return 0;
}

static int SetLocation(lua_State *L) {
    if(instance != nullptr){
        int n = lua_gettop(L);  /* number of arguments */
        if(n == 2){
            instance->player.location.x = lua_tonumber(L, 1);
            instance->player.location.y = lua_tonumber(L, 2);
            return 0;
        }else{
            PrintToLog("LUA WARNING: player.set_location() called with the wrong number of arguments! expected 2 got %i!", n);
        }
    }
    return 0;
}

static int SnapToBeam(lua_State *L){
    if(instance != nullptr){
        int n = lua_gettop(L);
        if(n == 1){
            // 1 argument = set snap to beam.
            instance->player.snap_to_beam = lua_toboolean(L, -1);
        }else if(n == 0){
            // no arguments = get snap to beam.
            lua_pushboolean(L, instance->player.snap_to_beam);
            return 1;
        }else{
            PrintToLog("LUA WARNING: player.snap_to_beam() called with the wrong number of arguments! expected 0 or 1 got %i!", n);
        }
    }
    return 0;
}

const luaL_Reg funcs[] = {
    {"get_item_count", GetItemCount},
    {"set_location", SetLocation},
    {"snap_to_beam", SnapToBeam},
    {nullptr, nullptr}
};
}

namespace gui_funcs{

static int SetMessage(lua_State *L) {
    if(instance != nullptr){
        int n = lua_gettop(L);  /* number of arguments */
        if(n == 1 || n == 2){
            if(n == 2){
                instance->gui.game.message_timeout = instance->level.time + lua_tonumber(lua, 2);
            }else{
                instance->gui.game.message_timeout = INFINITY;
            }
            lua_getglobal(L, "tostring");
            lua_pushvalue(L, -1);  /* function to be called */
            lua_pushvalue(L, 1);   /* value to print */
            lua_call(L, 1, 1);
            instance->gui.game.message = lua_tostring(L, -1);  /* get result */
            lua_pop(L, 1);  /* pop result */

            PrintToLog("INFO: Lua set message to \"%s\"", instance->gui.game.message.c_str());
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
        // TODO - error handling.
        abort();
    }

    luaL_openlibs(lua);

    lua_register(lua, "print", Print);
    lua_register(lua, "io.write", Print);

    lua_newtable(lua);

    PHOTON_API_ENTRY("window", window_funcs);
    PHOTON_API_ENTRY("level", level_funcs);
    PHOTON_API_ENTRY("player", player_funcs);
    PHOTON_API_ENTRY("gui", gui_funcs);
    PHOTON_API_ENTRY("build", build_info_funcs);

    luaL_setfuncs(lua, generic_funcs::funcs, 0);

    lua_setglobal(lua, "photon");

    DoFile(initscript);
}

void AdvanceFrame(){
    if(instance != nullptr){
        for(auto i = timers.begin(); i != timers.end();){
            timer &t = *i;
            if(t.timeout < instance->level.time && t.lua_call_ref != LUA_NOREF && t.lua_call_ref != LUA_REFNIL){
                lua_rawgeti(lua, LUA_REGISTRYINDEX, t.lua_call_ref);
                if(!lua_isfunction(lua, -1) || lua_pcall(lua, 0, -1, 0) != 0){
                    PrintToLog("WARNING: calling timer function failed!");
                }
                luaL_unref(lua, LUA_REGISTRYINDEX, t.lua_call_ref);
                i = timers.erase(i);
            }else{
                ++i;
            }
        }
    }
}

void Reset(){
    for(timer &t : timers){
        luaL_unref(lua, LUA_REGISTRYINDEX, t.lua_call_ref);
    }
    timers.clear();

    // TODO - reset any script created globals (i.e. anything other than "photon")
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

            if(state != 0){
                PrintToLog("LUA ERROR: %s\n", lua_tostring(lua, -1));
                lua_pop(lua, 1);
            }else{
                return 0;
            }
        }
    }
    return 1;
}

int8_t CheckLuaVictory(int victory_function_ref){
    if(victory_function_ref != LUA_NOREF && victory_function_ref != LUA_REFNIL){
        lua_rawgeti(lua, LUA_REGISTRYINDEX, victory_function_ref);
        if(lua_isfunction(lua, -1)){
            if(lua_pcall(lua, 0, -1, 0) == 0){
                int8_t v = lua_tointeger(lua, -1);
                lua_settop(lua, 0);
                return v;
            }
        }else{
            PrintToLog("WARNING: CheckLuaVictory called with a lua reference that is not a function!");
        }
    }else{
        PrintToLog("WARNING: CheckLuaVictory called with an invlid reference!");
    }
    lua_settop(lua, 0);
    return -1;
}

}

}
