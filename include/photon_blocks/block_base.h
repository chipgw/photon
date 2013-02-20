#ifndef _PHOTON_BLOCK_BASE_H_
#define _PHOTON_BLOCK_BASE_H_

#include <glm/glm.hpp>

namespace photon{
struct photon_lasersegment;

class photon_block{
public:
    const unsigned char type = 0;

    photon_lasersegment *on_light_interact(photon_lasersegment *segment);
    void draw();
};

}
#endif
