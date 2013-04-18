#include "photon_opengl.h"
#include "photon_laser.h"
#include <glm/gtx/rotate_vector.hpp>

// TODO - maybe this would be better elswhere?
#ifndef M_PI
#define M_PI 3.1415926535897932384626433832795
#endif

namespace photon{

namespace opengl{

void DrawLaserSegment(photon_lasersegment &segment){
    // TODO - this function is really messy, I should probably clean it up...
    static const float uv[] = { 1.0f, 0.0f,
                                0.0f, 0.0f,
                                0.0f, 0.0f,
                                1.0f, 0.0f,
                                0.0f, 0.0f,
                                0.0f, 0.0f};

    opengl::SetLaserColor(segment.color);
    glm::vec2 tangent = 0.2f * glm::normalize(glm::rotate(glm::vec2(segment.start) - glm::vec2(segment.end), 90.0f));

    glm::vec2 child_offset(0.0f);
    glm::vec2 parent_offset(0.0f);

    if(segment.child != nullptr && segment.angle != segment.child->angle){
        float child_angle = segment.angle - segment.child->angle;
        child_angle = 0.5 * -fmod(child_angle + 180.0f, 360.0f);

        child_offset = glm::rotate(tangent * glm::tan(glm::radians(child_angle)), 90.0f);
    }
    if(segment.parent != nullptr && segment.angle != segment.parent->angle){
        float parent_angle = segment.parent->angle - segment.angle;
        parent_angle = 0.5 * -fmod(parent_angle + 180.0f, 360.0f);

        parent_offset = glm::rotate(tangent * glm::tan(glm::radians(parent_angle)), 90.0f);
    }

    glm::vec2 fstart = glm::vec2(segment.start);
    glm::vec2 fend = glm::vec2(segment.end);

    glm::vec2 verts[] = {fstart,
                         fstart - tangent + parent_offset,
                         fend - tangent - child_offset,
                         fend,
                         fend + tangent + child_offset,
                         fstart + tangent - parent_offset};

    glVertexAttribPointer(PHOTON_VERTEX_LOCATION_ATTRIBUTE, 2, GL_FLOAT, GL_FALSE, 0, verts);
    glVertexAttribPointer(PHOTON_VERTEX_UV_ATTRIBUTE, 2, GL_FLOAT, GL_FALSE, 0, uv);

    glDrawArrays(GL_TRIANGLE_FAN, 0, 6);
}

void DrawLaser(photon_laserbeam &beam){
    opengl::SetModelMatrix(glm::mat3(1.0f));
    for(photon_lasersegment &segment : beam.segments){
        DrawLaserSegment(segment);
    }
}

void DrawLaserSegmentLight(photon_lasersegment &segment){
    static const float verts[] = { 0.0f, 0.0f,
                                   1.0f, 0.0f,
                                   1.0f, 1.0f,
                                   0.0f, 1.0f,
                                  -1.0f, 1.0f,
                                  -1.0f, 0.0f};

    static const float uv[] = { 0.0f, 0.0f,
                                1.0f, 0.0f,
                                1.0f, 0.0f,
                                0.0f, 0.0f,
                                1.0f, 0.0f,
                                1.0f, 0.0f};

    static const float size = 8.0f;

    opengl::SetLaserColor(segment.color);

    float radians = glm::radians(segment.angle - 90);
    float dist = glm::distance(glm::vec2(segment.start), glm::vec2(segment.end));

    glm::mat3 matrix = glm::mat3( glm::cos(radians) * size, glm::sin(radians) * size, 0.0f,
                                 -glm::sin(radians) * dist, glm::cos(radians) * dist, 0.0f,
                                  segment.start.x, segment.start.y, 1.0f);

    opengl::SetModelMatrix(matrix);

    glVertexAttribPointer(PHOTON_VERTEX_LOCATION_ATTRIBUTE, 2, GL_FLOAT, GL_FALSE, 0, verts);
    glVertexAttribPointer(PHOTON_VERTEX_UV_ATTRIBUTE,       2, GL_FLOAT, GL_FALSE, 0, uv);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 6);

    matrix = glm::mat3( glm::cos(radians) * size, glm::sin(radians) * size, 0.0f,
                       -glm::sin(radians) * size, glm::cos(radians) * size, 0.0f,
                        segment.end.x, segment.end.y, 1.0f);

    opengl::SetModelMatrix(matrix);

    glVertexAttribPointer(PHOTON_VERTEX_UV_ATTRIBUTE, 2, GL_FLOAT, GL_FALSE, 0, verts);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 6);

    radians += M_PI;

    matrix = glm::mat3( glm::cos(radians) * size, glm::sin(radians) * size, 0.0f,
                       -glm::sin(radians) * size, glm::cos(radians) * size, 0.0f,
                        segment.start.x, segment.start.y, 1.0f);

    opengl::SetModelMatrix(matrix);

    glDrawArrays(GL_TRIANGLE_FAN, 0, 6);
}

void DrawLaserLight(photon_laserbeam &beam){
    for(photon_lasersegment &segment : beam.segments){
        DrawLaserSegmentLight(segment);
    }
}

}

}
