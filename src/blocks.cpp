#include "photon_blocks.h"
#include "photon_opengl.h"
#include "photon_level.h"
#include "photon_laser.h"

namespace photon{

namespace blocks{

void DrawBox(glm::uvec2 location){
    // TODO - maybe add optional rotation? perhaps via UV coordinates?
    glBegin(GL_QUADS);{
        glVertexAttrib2f(PHOTON_VERTEX_UV_ATTRIBUTE, 1.0f, 1.0f);
        glVertexAttrib2f(PHOTON_VERTEX_LOCATION_ATTRIBUTE,location.x + 0.5, location.y + 0.5);

        glVertexAttrib2f(PHOTON_VERTEX_UV_ATTRIBUTE, 0.0f, 1.0f);
        glVertexAttrib2f(PHOTON_VERTEX_LOCATION_ATTRIBUTE,location.x - 0.5, location.y + 0.5);

        glVertexAttrib2f(PHOTON_VERTEX_UV_ATTRIBUTE, 0.0f, 0.0f);
        glVertexAttrib2f(PHOTON_VERTEX_LOCATION_ATTRIBUTE,location.x - 0.5, location.y - 0.5);

        glVertexAttrib2f(PHOTON_VERTEX_UV_ATTRIBUTE, 1.0f, 0.0f);
        glVertexAttrib2f(PHOTON_VERTEX_LOCATION_ATTRIBUTE,location.x + 0.5, location.y - 0.5);
    }glEnd();
}

void Draw(photon_block block, glm::uvec2 location){
    switch(block.type){
    case PHOTON_BLOCKS_AIR:
        break;
    case PHOTON_BLOCKS_PLAIN:
        // TODO - bind texture.
        DrawBox(location);
        break;
    }
}

photon_lasersegment *OnLightInteract(photon_lasersegment *segment, glm::uvec2 location, photon_level &level){
    unsigned char type = level.grid[location.x][location.y].type;
    switch(type){
    case PHOTON_BLOCKS_AIR:
        break;
    case PHOTON_BLOCKS_RECIEVER:
        // TODO - make trigger
    case PHOTON_BLOCKS_PLAIN:
    case PHOTON_BLOCKS_INDESTRUCTIBLE:
        // stops tracing the laser.
        return nullptr;
        break;
    }
    return segment;
}

}

}
