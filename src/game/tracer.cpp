#include "photon_level.h"

#include <glm/gtx/rotate_vector.hpp>

namespace photon{

namespace tracer{

void TraceBeam(photon_laserbeam& beam, photon_level &level, float time){
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
        glm::vec2 trace = glm::rotate(glm::vec2(1.0f, 0.0f), glm::radians(segment->angle));

        // laser should always be at angle that is a multiple of 45, so rounding the coordinates to 1 will keep it as such.
        trace = glm::round(trace);

        glm::uvec2 trace_location = last_trace_location + glm::uvec2(trace);

        // check to se if the laser is within the bounds of the level.
        if(trace_location.x >= level.width || trace_location.y >= level.height){
            segment = nullptr;
            break;
        }

        segment->end = trace_location;

        segment = blocks::OnLightInteract(segment, trace_location, level, time);

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
