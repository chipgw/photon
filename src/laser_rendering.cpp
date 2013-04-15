#include "photon_opengl.h"
#include "photon_laser.h"

#include <glm/gtx/rotate_vector.hpp>

namespace photon{

namespace opengl{

void DrawLaserSegment(photon_lasersegment &segment){
    // TODO - ditch immediate mode here as well... this will be more difficult because of the parent_offset & child_offset.
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

    glBegin(GL_QUADS);{
        glVertexAttrib2f(PHOTON_VERTEX_UV_ATTRIBUTE, 1.0f,0.0f);
        glVertexAttrib2f(PHOTON_VERTEX_LOCATION_ATTRIBUTE, segment.start.x, segment.start.y);

        glVertexAttrib2f(PHOTON_VERTEX_UV_ATTRIBUTE, 0.0f,0.0f);
        glVertexAttrib2f(PHOTON_VERTEX_LOCATION_ATTRIBUTE, segment.start.x - tangent.x + parent_offset.x, segment.start.y - tangent.y + parent_offset.y);

        glVertexAttrib2f(PHOTON_VERTEX_UV_ATTRIBUTE, 0.0f,0.0f);
        glVertexAttrib2f(PHOTON_VERTEX_LOCATION_ATTRIBUTE,segment.end.x - tangent.x - child_offset.x, segment.end.y - tangent.y - child_offset.y);

        glVertexAttrib2f(PHOTON_VERTEX_UV_ATTRIBUTE, 1.0f,0.0f);
        glVertexAttrib2f(PHOTON_VERTEX_LOCATION_ATTRIBUTE,segment.end.x, segment.end.y);


        glVertexAttrib2f(PHOTON_VERTEX_UV_ATTRIBUTE, 1.0f,0.0f);
        glVertexAttrib2f(PHOTON_VERTEX_LOCATION_ATTRIBUTE, segment.start.x, segment.start.y);

        glVertexAttrib2f(PHOTON_VERTEX_UV_ATTRIBUTE, 0.0f,0.0f);
        glVertexAttrib2f(PHOTON_VERTEX_LOCATION_ATTRIBUTE, segment.start.x + tangent.x - parent_offset.x, segment.start.y + tangent.y - parent_offset.y);

        glVertexAttrib2f(PHOTON_VERTEX_UV_ATTRIBUTE, 0.0f,0.0f);
        glVertexAttrib2f(PHOTON_VERTEX_LOCATION_ATTRIBUTE,segment.end.x + tangent.x + child_offset.x, segment.end.y + tangent.y + child_offset.y);

        glVertexAttrib2f(PHOTON_VERTEX_UV_ATTRIBUTE, 1.0f,0.0f);
        glVertexAttrib2f(PHOTON_VERTEX_LOCATION_ATTRIBUTE,segment.end.x, segment.end.y);
    }glEnd();
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
