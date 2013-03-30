#ifndef _PHOTON_BLOCKS_H_
#define _PHOTON_BLOCKS_H_

#include <glm/glm.hpp>
#include "photon_opengl.h"

namespace photon{
struct photon_lasersegment;
struct photon_level;
struct photon_player;

enum block_type{
    invalid_block = -1,// if there is an error of some kind.

    air, // empty space.

    mirror, // normal movable & rotatable mirror.
    mirror_locked, // mirror with locked position and rotation.

    plain, // your basic everyday block.
    indestructible, // you can't touch this!

    // laser emitter.
    emitter_white,
    emitter_red,
    emitter_green,
    emitter_blue,

    // laser reciever.
    reciever,
    reciever_white,
    reciever_red,
    reciever_green,
    reciever_blue,

    tnt, // KABOOM!!! blows up when the laser triggers it (by pointing at it for a couple seconds)
    tnt_fireball, // draws the fireball after the TNT explodes.

    // Color filters
    filter_red,
    filter_green,
    filter_blue,
    filter_yellow,
    filter_cyan,
    filter_magenta
};


struct photon_block{
    // block type.
    block_type type = air;

    // used for various things depending on the block type...
    float data = 0.0f;

    // used to tell if a laser intersected with it last frame.
    bool activated = false;

    // if locked you cannot pick it up. (some you cannot pick up at all)
    bool locked = false;
};

namespace blocks{

photon_lasersegment* OnLightInteract(photon_lasersegment* segment, glm::uvec2 location, photon_level &level, float time);

void OnPhotonInteract(glm::uvec2 location, photon_level &level, photon_player &player);

void OnRotate(glm::uvec2 location, photon_level &level, bool counter_clockwise = false);

void OnFrame(glm::uvec2 location, photon_level &level, float time);

void OnDamage(glm::uvec2 location, photon_level &level, float damage);

void DamageAroundPoint(glm::uvec2 location, photon_level &level, float strength);

// Drawing functions

void Draw(photon_block block, glm::uvec2 location);

void DrawFX(photon_block block, glm::uvec2 location);

void LoadTextures();

GLuint GetBlockTexture(block_type type);

const char* GetBlockName(block_type type);

block_type GetBlockFromName(const char* name);

}

}
#endif
