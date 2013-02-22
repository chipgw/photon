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

    glm::uvec2 last_trace_location = segment->start;

    while(segment != nullptr){
        glm::vec2 trace(1.0,0.0);
        trace = glm::rotate(trace, segment->angle);

        // laser should always be at anle that is a multiple of 45, so rounding each coordinate to 1 will keep it as such.
        trace.x = std::floor(trace.x + 0.5f);
        trace.y = std::floor(trace.y + 0.5f);

        glm::uvec2 trace_location = last_trace_location + glm::uvec2(trace);

        segment->end = trace_location;

        // check to se if the laser is within the bounds of the level.
        if(trace_location.x < 0 || trace_location.x >= level.grid.shape()[0] ||
                trace_location.y < 0 || trace_location.y >= level.grid.shape()[1]){
            segment = nullptr;
            break;
        }

        segment = blocks::OnLightInteract(segment, trace_location, level);

        last_trace_location = trace_location;
    }
}

photon_lasersegment *CreateChildBeam(photon_lasersegment *parent){
    parent->beam.segments.push_back(photon_lasersegment(parent->beam));

    parent->child = &parent->beam.segments.back();
    parent->child->parent = parent;

    // By default the beam inherits these values.
    parent->child->angle = parent->angle;
    parent->child->color = parent->color;
    parent->child->start = parent->end;

    return parent->child;
}

}
}
