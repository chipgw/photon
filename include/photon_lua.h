#ifndef _PHOTON_LUA_H_
#define _PHOTON_LUA_H_

#include <string>

namespace photon{

struct photon_instance;

namespace lua{

void InitLua(photon_instance &in, const std::string &initscript);

void AdvanceFrame();

int DoFile(const std::string &filename);

int8_t CheckLuaVictory(int victory_function_ref);

}

}

#endif
