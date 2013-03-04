#include "photon_blocks.h"
#include "photon_opengl.h"
#include "photon_level.h"
#include "photon_laser.h"
#include "photon_texture.h"
#include "photon_core.h"

#include "glm/ext.hpp"

GLuint texture_plain_block, texture_mirror, texture_tnt;

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

void DrawMirror(glm::uvec2 location, float angle){
    glm::vec2 point1(0.4f, 0.05f);
    glm::vec2 point2(0.05f, 0.4f);
    point1 = glm::rotate(point1, angle);
    point2 = glm::rotate(point2, angle + 90.0f);

    glBegin(GL_QUADS);{
        glVertexAttrib2f(PHOTON_VERTEX_UV_ATTRIBUTE, 1.0f, 1.0f);
        glVertexAttrib2f(PHOTON_VERTEX_LOCATION_ATTRIBUTE,location.x + point1.x, location.y + point1.y);

        glVertexAttrib2f(PHOTON_VERTEX_UV_ATTRIBUTE, 0.0f, 1.0f);
        glVertexAttrib2f(PHOTON_VERTEX_LOCATION_ATTRIBUTE,location.x - point2.x, location.y - point2.y);

        glVertexAttrib2f(PHOTON_VERTEX_UV_ATTRIBUTE, 0.0f, 0.0f);
        glVertexAttrib2f(PHOTON_VERTEX_LOCATION_ATTRIBUTE,location.x - point1.x, location.y - point1.y);

        glVertexAttrib2f(PHOTON_VERTEX_UV_ATTRIBUTE, 1.0f, 0.0f);
        glVertexAttrib2f(PHOTON_VERTEX_LOCATION_ATTRIBUTE,location.x + point2.x, location.y + point2.y);
    }glEnd();
}

void Draw(photon_block block, glm::uvec2 location){
    switch(block.type){
    case PHOTON_BLOCKS_AIR:
        break;
    case PHOTON_BLOCKS_PLAIN:
        glBindTexture(GL_TEXTURE_2D, texture_plain_block);

        DrawBox(location);
        break;
    case PHOTON_BLOCKS_INDESTRUCTIBLE:
        // TODO - make different texture.
        glBindTexture(GL_TEXTURE_2D, texture_plain_block);
        DrawBox(location);
        break;
    case PHOTON_BLOCKS_MIRROR:
    case PHOTON_BLOCKS_MIRROR_LOCKED:
    case PHOTON_BLOCKS_MIRROR_LOCKED_POS:
        glBindTexture(GL_TEXTURE_2D, texture_mirror);
        DrawMirror(location, block.data);
        break;
    case PHOTON_BLOCKS_TNT:
        glBindTexture(GL_TEXTURE_2D, texture_tnt);
        DrawBox(location);
        break;
    }
}

photon_lasersegment *OnLightInteract(photon_lasersegment *segment, glm::uvec2 location, photon_level &level, float time){
    photon_block &block = level.grid[location.x][location.y];
    switch(block.type){
    case PHOTON_BLOCKS_AIR:
    default:
        break;
    case PHOTON_BLOCKS_RECIEVER:
        // TODO - make trigger
        // stops tracing the laser.
        return nullptr;
        break;
    case PHOTON_BLOCKS_PLAIN:
    case PHOTON_BLOCKS_INDESTRUCTIBLE:
        // stops tracing the laser.
        return nullptr;
        break;
    case PHOTON_BLOCKS_MIRROR:
    case PHOTON_BLOCKS_MIRROR_LOCKED:
    case PHOTON_BLOCKS_MIRROR_LOCKED_POS:{
        float angle = segment->angle - block.data;
        if(fmod(angle, 180.0f) == 0.0f){
            return nullptr;
        }
        angle = fmod(angle + 180.0f, 360.0f) - 180.0f;
        segment = tracer::CreateChildBeam(segment);
        segment->angle = block.data - angle;
        break;
    }
    case PHOTON_BLOCKS_TNT:
        block.data += time;
        if(block.data > 1.0f){
            // TODO - KABOOM goes here...
            PrintToLog("INFO: KABOOM!");
            block.data = 0.0f;
            block.type = PHOTON_BLOCKS_AIR;
            break;
        }
        // stops tracing the laser.
        return nullptr;
        break;
    }

    return segment;
}

void LoadTextures(){
    texture_plain_block = texture::Load("/textures/block.png");
    texture_mirror = texture::Load("/textures/mirror.png");
    texture_tnt = texture::Load("/textures/tnt.png");
}

void OnPhotonInteract(glm::uvec2 location, photon_level &level){
    if(level.grid.shape()[0] > location.x && level.grid.shape()[1] > location.y){
        photon_block &block = level.grid[location.x][location.y];
        switch(block.type){
        case PHOTON_BLOCKS_AIR:
            // TODO - place currently selected item in inventory.
            block.type = PHOTON_BLOCKS_MIRROR;
            break;
        default:
            break;
        case PHOTON_BLOCKS_MIRROR:
            // TODO - store mirror in inventory.
            block.type = PHOTON_BLOCKS_AIR;
            break;
        }
    }
}

void OnRotate(glm::uvec2 location, photon_level &level, bool counter_clockwise){
    if(level.grid.shape()[0] > location.x && level.grid.shape()[1] > location.y){
        photon_block &block = level.grid[location.x][location.y];
        switch(block.type){
        default:
            break;
        case PHOTON_BLOCKS_MIRROR:
        case PHOTON_BLOCKS_MIRROR_LOCKED_POS:
            if(counter_clockwise){
                block.data += 22.5f;
            }else{
                block.data -= 22.5f;
            }
            break;
        }
    }
}

}

}
