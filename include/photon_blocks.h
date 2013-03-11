#ifndef _PHOTON_BLOCKS_H_
#define _PHOTON_BLOCKS_H_

#include <glm/glm.hpp>

// empty space.
#define PHOTON_BLOCKS_AIR               0x00

// normal movable & rotatable mirror.
#define PHOTON_BLOCKS_MIRROR            0x01

// mirror with locked position and rotation.
#define PHOTON_BLOCKS_MIRROR_LOCKED     0x02

// mirror with locked position.
#define PHOTON_BLOCKS_MIRROR_LOCKED_POS 0x03

// your basic everyday block.
#define PHOTON_BLOCKS_PLAIN             0x04

// you can't touch this!
#define PHOTON_BLOCKS_INDESTRUCTIBLE    0x05

// laser emitter, essensially the level start point.
// there should be one and only one of these per level.
#define PHOTON_BLOCKS_EMITTER           0x08

// laser reciever, essensially the level end point.
// there should be one and only one of these per level.
#define PHOTON_BLOCKS_RECIEVER          0x09

// KABOOM!!! blows up when the laser triggers it
// (by pointing at it for a couple seconds)
#define PHOTON_BLOCKS_TNT               0x0c

// draws the fireball after the TNT explodes.
#define PHOTON_BLOCKS_TNT_FIREBALL      0x0d

// Color filters
#define PHOTON_BLOCKS_FILTER_RED        0x12
#define PHOTON_BLOCKS_FILTER_GREEN      0x13
#define PHOTON_BLOCKS_FILTER_BLUE       0x14

// makes it easier to make new ones...
//#define PHOTON_BLOCKS_

namespace photon{
struct photon_lasersegment;
struct photon_level;

struct photon_block{
    // block type.
    unsigned char type = PHOTON_BLOCKS_AIR;

    // used for various things depending on the block type...
    float data = 0.0f;

    // used to tell if a laser intersected with it last frame.
    bool activated = false;
};

namespace blocks{

photon_lasersegment* OnLightInteract(photon_lasersegment* segment, glm::uvec2 location, photon_level &level, float time);

void OnPhotonInteract(glm::uvec2 location, photon_level &level);

void OnRotate(glm::uvec2 location, photon_level &level, bool counter_clockwise = false);

void Draw(photon_block block, glm::uvec2 location);

void DrawFX(photon_block block, glm::uvec2 location);

void LoadTextures();

void OnFrame(glm::uvec2 location, photon_level &level, float time);

}

}
#endif
