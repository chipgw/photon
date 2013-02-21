#include "photon_opengl.h"
#include "photon_laser.h"

#include <glm/ext.hpp>

namespace photon{

namespace opengl{

void DrawLaserSegment(photon_lasersegment &segment){
    glm::vec2 tangent = 0.5f * glm::normalize(glm::rotate(glm::vec2(segment.start) - glm::vec2(segment.end), 90.0f));

    glm::vec2 child_offset(0.0f);
    glm::vec2 parent_offset(0.0f);

    if(segment.child != nullptr){
        float child_angle = 0.5 * glm::acos(glm::normalizeDot(glm::vec2(segment.end) - glm::vec2(segment.start), glm::vec2(segment.child->start) - glm::vec2(segment.child->end)));

        float tan = glm::tan(child_angle);

        child_offset = glm::rotate(tangent * tan, 90.0f);
    }
    if(segment.parent != nullptr){
        float parent_angle = 0.5 * glm::acos(glm::normalizeDot(glm::vec2(segment.parent->end) - glm::vec2(segment.parent->start), glm::vec2(segment.start) - glm::vec2(segment.end)));

        float tan = glm::tan(parent_angle);

        parent_offset = glm::rotate(tangent * tan, 90.0f);
    }

    glBegin(GL_QUADS);{
        glVertexAttrib2f(PHOTON_VERTEX_UV_ATTRIBUTE, 1.0f,0.0f);
        glVertexAttrib2f(PHOTON_VERTEX_LOCATION_ATTRIBUTE, segment.start.x - tangent.x + parent_offset.x, segment.start.y - tangent.y + parent_offset.y);

        glVertexAttrib2f(PHOTON_VERTEX_UV_ATTRIBUTE,-1.0f,0.0f);
        glVertexAttrib2f(PHOTON_VERTEX_LOCATION_ATTRIBUTE, segment.start.x + tangent.x - parent_offset.x, segment.start.y + tangent.y - parent_offset.y);

        glVertexAttrib2f(PHOTON_VERTEX_UV_ATTRIBUTE,-1.0f,0.0f);
        glVertexAttrib2f(PHOTON_VERTEX_LOCATION_ATTRIBUTE,segment.end.x + tangent.x + child_offset.x, segment.end.y + tangent.y + child_offset.y);

        glVertexAttrib2f(PHOTON_VERTEX_UV_ATTRIBUTE, 1.0f,0.0f);
        glVertexAttrib2f(PHOTON_VERTEX_LOCATION_ATTRIBUTE,segment.end.x - tangent.x - child_offset.x, segment.end.y - tangent.y - child_offset.y);
    }glEnd();
}

void DrawLaser(photon_laserbeam &beam){
    for(photon_lasersegment &segment : beam.segments){
        DrawLaserSegment(segment);
    }
}

void DrawLaserSegmentLight(photon_lasersegment &segment){
    glm::vec2 beam_direction = 4.0f * glm::normalize(glm::vec2(segment.start) - glm::vec2(segment.end));
    glm::vec2 tangent = glm::rotate(beam_direction, 90.0f);

    glBegin(GL_QUADS);{
        glVertexAttrib2f(PHOTON_VERTEX_UV_ATTRIBUTE, 1.0f,1.0f);
        glVertexAttrib2f(PHOTON_VERTEX_LOCATION_ATTRIBUTE, segment.start.x - tangent.x, segment.start.y - tangent.y);

        glVertexAttrib2f(PHOTON_VERTEX_UV_ATTRIBUTE,-1.0f,1.0f);
        glVertexAttrib2f(PHOTON_VERTEX_LOCATION_ATTRIBUTE, segment.start.x + tangent.x, segment.start.y + tangent.y);

        glVertexAttrib2f(PHOTON_VERTEX_UV_ATTRIBUTE,-1.0f,1.0f);
        glVertexAttrib2f(PHOTON_VERTEX_LOCATION_ATTRIBUTE,segment.end.x + tangent.x, segment.end.y + tangent.y);

        glVertexAttrib2f(PHOTON_VERTEX_UV_ATTRIBUTE, 1.0f,1.0f);
        glVertexAttrib2f(PHOTON_VERTEX_LOCATION_ATTRIBUTE,segment.end.x - tangent.x, segment.end.y - tangent.y);


        glVertexAttrib2f(PHOTON_VERTEX_UV_ATTRIBUTE,-1.0f,1.0f);
        glVertexAttrib2f(PHOTON_VERTEX_LOCATION_ATTRIBUTE, segment.start.x + tangent.x, segment.start.y + tangent.y);

        glVertexAttrib2f(PHOTON_VERTEX_UV_ATTRIBUTE, 1.0f,1.0f);
        glVertexAttrib2f(PHOTON_VERTEX_LOCATION_ATTRIBUTE, segment.start.x - tangent.x, segment.start.y - tangent.y);

        glVertexAttrib2f(PHOTON_VERTEX_UV_ATTRIBUTE, 1.0f,0.0f);
        glVertexAttrib2f(PHOTON_VERTEX_LOCATION_ATTRIBUTE, segment.start.x - tangent.x + beam_direction.x, segment.start.y - tangent.y + beam_direction.y);

        glVertexAttrib2f(PHOTON_VERTEX_UV_ATTRIBUTE,-1.0f,0.0f);
        glVertexAttrib2f(PHOTON_VERTEX_LOCATION_ATTRIBUTE, segment.start.x + tangent.x + beam_direction.x, segment.start.y + tangent.y + beam_direction.y);


        glVertexAttrib2f(PHOTON_VERTEX_UV_ATTRIBUTE,-1.0f,1.0f);
        glVertexAttrib2f(PHOTON_VERTEX_LOCATION_ATTRIBUTE,segment.end.x + tangent.x, segment.end.y + tangent.y);

        glVertexAttrib2f(PHOTON_VERTEX_UV_ATTRIBUTE, 1.0f,1.0f);
        glVertexAttrib2f(PHOTON_VERTEX_LOCATION_ATTRIBUTE,segment.end.x - tangent.x, segment.end.y - tangent.y);

        glVertexAttrib2f(PHOTON_VERTEX_UV_ATTRIBUTE, 1.0f,0.0f);
        glVertexAttrib2f(PHOTON_VERTEX_LOCATION_ATTRIBUTE,segment.end.x - tangent.x - beam_direction.x, segment.end.y - tangent.y - beam_direction.y);

        glVertexAttrib2f(PHOTON_VERTEX_UV_ATTRIBUTE,-1.0f,0.0f);
        glVertexAttrib2f(PHOTON_VERTEX_LOCATION_ATTRIBUTE,segment.end.x + tangent.x - beam_direction.x, segment.end.y + tangent.y - beam_direction.y);
    }glEnd();
}

void DrawLaserLight(photon_laserbeam &beam){
    for(photon_lasersegment &segment : beam.segments){
        DrawLaserSegmentLight(segment);
    }
}

}

}
