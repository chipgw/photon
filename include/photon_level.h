#ifndef _PHOTON_LEVEL_H_
#define _PHOTON_LEVEL_H_

#include "photon_blocks.h"
#include "photon_laser.h"

#include <map>

namespace photon{
struct photon_player;

typedef std::pair<uint8_t, uint8_t>  photon_level_coord;

struct photon_level{
    enum game_mode{
        none,           // nothin.
        power,          // power up all the receivers to win.
        targets,        // destroy all the targets to win.
        destruction,    // destroy all blocks to win.
        tnt_harvester,  // gather <goal> TNT blocks to win.
        script          // uses a script to do the objectives.
    };

    std::map<photon_level_coord, photon_block> grid;
    std::vector<photon_laserbeam> beams;

    uint8_t width = 0;
    uint8_t height = 0;

    // the time spent on the level.
    float time = 0.0f;
    // the time of either victory or defeat.
    float end_time = INFINITY;
    int8_t victory_state = 0;

    uint32_t moves = 0;
    bool is_valid = false;

    game_mode mode = none;

    int16_t blocks_destroyed = 0;
    int16_t goal = 0;

    // the lua reference to the victory checking function for script game modes.
    int lua_checkvictory = -2;
};

namespace level{

void Draw(photon_level &level);

void DrawBeams(photon_level &level);

void DrawBeamsLight(photon_level &level);

void DrawFX(photon_level &level);

bool LoadLevelXML(const std::string &filename, photon_instance &instance);

void SaveLevelXML(const std::string &filename, const photon_level &level, const photon_player &player);

void AdvanceFrame(photon_level &level, photon_player &player, float time);

int8_t CheckVictory(photon_level &level, photon_player &player);

}

}
#endif
