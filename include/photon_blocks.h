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

// makes it easier to make new ones...
//#define PHOTON_BLOCKS_

namespace photon{
struct photon_lasersegment;
struct photon_level;

struct photon_block{
    unsigned char type;
    // used for various things depending on the block type...
    float data;
};

namespace blocks{

photon_lasersegment* OnLightInteract(photon_lasersegment* segment, glm::uvec2 location, photon_level &level, float time);

void OnPhotonInteract(glm::uvec2 location, photon_level &level);

void OnRotate(glm::uvec2 location, photon_level &level, bool counter_clockwise = false);

void Draw(photon_block block, glm::uvec2 location);

void LoadTextures();

}

}
#endif
