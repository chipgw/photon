#ifndef _PHOTON_LASER_H_
#define _PHOTON_LASER_H_

#include "photon_level.h"
#include <glm/glm.hpp>
#include <list>

namespace photon{
struct photon_lasersegment;

// TODO - maybe make this a block?
struct photon_laserbeam{
    glm::uvec2 origin;
    float origin_angle;

    std::list<photon_lasersegment> segments;

    glm::vec3 color;
};

struct photon_lasersegment{
    photon_laserbeam &beam;
    photon_lasersegment *parent = nullptr;
    photon_lasersegment *child = nullptr;

    glm::uvec2 start;
    glm::uvec2 end;
    float angle;

    // seperate from the root color to allow for filters.
    glm::vec3 color;

    photon_lasersegment(photon_laserbeam &Beam) : beam(Beam) {}
};

namespace tracer{

void TraceBeam(photon_laserbeam& beam, photon_level &level);

photon_lasersegment *CreateChildBeam(photon_lasersegment *parent);

}

namespace opengl{

void DrawLaser(photon_laserbeam &beam);
void DrawLaserSegment(photon_laserbeam &beam);

}

}
#endif
