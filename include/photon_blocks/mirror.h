#ifndef _PHOTON_MIRROR_H_
#define _PHOTON_MIRROR_H_

#include "photon_blocks/block_base.h"

namespace photon{
struct photon_lasersegment;

namespace blocks{

class photon_block{
public:
    const unsigned char type = 1;

    photon_lasersegment *on_light_interact(photon_lasersegment *segment);
};

}

}
#endif
