#ifndef _PHOTON_LEVEL_H_
#define _PHOTON_LEVEL_H_

#include "photon_blocks.h"
#include "photon_laser.h"
#include <map>

namespace photon{
struct photon_player;

typedef std::pair<uint8_t, uint8_t>  photon_level_coord;

struct photon_level{
    std::map<photon_level_coord, photon_block> grid;
    std::vector<photon_laserbeam> beams;

    uint8_t width = 0;
    uint8_t height = 0;

    float time = 0.0f;
    uint32_t moves = 0;
    bool is_valid = false;
};

namespace level{

void Draw(photon_level &level);

void DrawBeams(photon_level &level);

void DrawBeamsLight(photon_level &level);

void DrawFX(photon_level &level);

photon_level LoadLevelXML(const std::string &filename, photon_player &player);

void SaveLevelXML(const std::string &filename, const photon_level &level, const photon_player &player);

void AdvanceFrame(photon_level &level, float time);

}

}
#endif
