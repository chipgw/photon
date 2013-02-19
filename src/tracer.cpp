#include "photon_laser.h"
#include "photon_level.h"

#include <glm/ext.hpp>

namespace photon{

namespace tracer{

void TraceBeam(photon_laserbeam& beam, photon_level &level){
    photon_lasersegment *segment;

    // delete old data.
    beam.segments.clear();

    beam.segments.push_back(photon_lasersegment(beam));

    segment = &beam.segments.back();

    segment->color = beam.color;
    segment->start = beam.origin;
    segment->angle = beam.origin_angle;

    while(segment != nullptr){
        glm::vec2 trace(1.0,0.0);
        glm::rotate(trace, segment->angle);

        // TODO - code goes here.
    }
}

}
}
