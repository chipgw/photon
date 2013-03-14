#include "photon_player.h"

#include <glm/gtx/norm.hpp>

namespace photon{

namespace player{

glm::vec2 PointOnSegment(photon_lasersegment &segment, glm::vec2 location){
    glm::vec2 start(segment.start);
    glm::vec2 end(segment.end);

    const float segment_length_sqr = glm::distance2(start, end);

    if(segment_length_sqr == 0.0){
        return glm::vec2(segment.start);
    }

    const float t = glm::dot(location - start, end - start) / segment_length_sqr;

    if(t < 0.0){
        return start;
    }
    else if(t > 1.0){
        return end;
    }

    return start + t * (end - start);
}

glm::vec2 SnapToBeam(photon_laserbeam &beam, glm::vec2 location){
    float dist = INFINITY;
    glm::vec2 loc = location;

    for(photon_lasersegment &segment : beam.segments){
        glm::vec2 point = PointOnSegment(segment, location);
        float tdist = glm::distance(point, location);
        if(tdist < dist){
            dist = tdist;
            loc = point;
        }
    }
    return loc;
}

glm::vec2 SnapToBeams(std::vector<photon_laserbeam> &beams, glm::vec2 location){
    float dist = INFINITY;
    glm::vec2 loc = location;

    for(photon_laserbeam &beam : beams){
        for(photon_lasersegment &segment : beam.segments){
            glm::vec2 point = PointOnSegment(segment, location);
            float tdist = glm::distance(point, location);
            if(tdist < dist){
                dist = tdist;
                loc = point;
            }
        }
    }
    return loc;
}

}

}
