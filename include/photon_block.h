#ifndef _PHOTON_BLOCK_H_
#define _PHOTON_BLOCK_H_

#include <glm/glm.hpp>

namespace photon{
struct photon_lasersegment;

class photon_block{
    unsigned char type = 0;

    photon_lasersegment *on_light_interact(photon_lasersegment *segment);
};

}
#endif
