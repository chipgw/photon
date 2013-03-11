#include "photon_blocks.h"
#include "photon_opengl.h"
#include "photon_level.h"
#include "photon_laser.h"
#include "photon_texture.h"
#include "photon_core.h"

#include "glm/ext.hpp"

GLuint texture_plain_block;
GLuint texture_mirror;
GLuint texture_tnt;
GLuint texture_explosion;
GLuint texture_filter_red;
GLuint texture_filter_green;
GLuint texture_filter_blue;
GLuint texture_filter_yellow;
GLuint texture_filter_cyan;
GLuint texture_filter_magenta;

namespace photon{

namespace blocks{

void DrawBox(glm::uvec2 location, float size = 0.5f){
    // TODO - maybe add optional rotation? perhaps via UV coordinates?
    glBegin(GL_QUADS);{
        glVertexAttrib2f(PHOTON_VERTEX_UV_ATTRIBUTE, 1.0f, 1.0f);
        glVertexAttrib2f(PHOTON_VERTEX_LOCATION_ATTRIBUTE,location.x + size, location.y + size);

        glVertexAttrib2f(PHOTON_VERTEX_UV_ATTRIBUTE, 0.0f, 1.0f);
        glVertexAttrib2f(PHOTON_VERTEX_LOCATION_ATTRIBUTE,location.x - size, location.y + size);

        glVertexAttrib2f(PHOTON_VERTEX_UV_ATTRIBUTE, 0.0f, 0.0f);
        glVertexAttrib2f(PHOTON_VERTEX_LOCATION_ATTRIBUTE,location.x - size, location.y - size);

        glVertexAttrib2f(PHOTON_VERTEX_UV_ATTRIBUTE, 1.0f, 0.0f);
        glVertexAttrib2f(PHOTON_VERTEX_LOCATION_ATTRIBUTE,location.x + size, location.y - size);
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
    case PHOTON_BLOCKS_FILTER_RED:
        glBindTexture(GL_TEXTURE_2D, texture_filter_red);
        DrawBox(location, 0.2f);
        break;
    case PHOTON_BLOCKS_FILTER_GREEN:
        glBindTexture(GL_TEXTURE_2D, texture_filter_green);
        DrawBox(location, 0.2f);
        break;
    case PHOTON_BLOCKS_FILTER_BLUE:
        glBindTexture(GL_TEXTURE_2D, texture_filter_blue);
        DrawBox(location, 0.2f);
        break;
    case PHOTON_BLOCKS_FILTER_YELLOW:
        glBindTexture(GL_TEXTURE_2D, texture_filter_yellow);
        DrawBox(location, 0.2f);
        break;
    case PHOTON_BLOCKS_FILTER_CYAN:
        glBindTexture(GL_TEXTURE_2D, texture_filter_cyan);
        DrawBox(location, 0.2f);
        break;
    case PHOTON_BLOCKS_FILTER_MAGENTA:
        glBindTexture(GL_TEXTURE_2D, texture_filter_magenta);
        DrawBox(location, 0.2f);
        break;
    }
}

void DrawFX(photon_block block, glm::uvec2 location){
    switch(block.type){
    case PHOTON_BLOCKS_TNT:
        // TODO - draw some warmup thing...
        glBindTexture(GL_TEXTURE_2D, texture_explosion);
        opengl::SetFacFX(block.data * 0.5f);
        DrawBox(location);
        break;
    case PHOTON_BLOCKS_TNT_FIREBALL:
        glBindTexture(GL_TEXTURE_2D, texture_explosion);
        opengl::SetFacFX(block.data);
        DrawBox(location, 1.5);
        break;
    }
}

photon_lasersegment *OnLightInteract(photon_lasersegment *segment, glm::uvec2 location, photon_level &level, float time){
    photon_block &block = level.grid[location.x][location.y];
    block.activated = true;

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
            block.type = PHOTON_BLOCKS_TNT_FIREBALL;
            // cooldown of fireball
            block.data = 1.0f;
            break;
        }
        // stops tracing the laser.
        return nullptr;
        break;
    case PHOTON_BLOCKS_FILTER_RED:{
        glm::vec3 color = segment->color;
        color.g = glm::min(color.g, 0.2f);
        color.b = glm::min(color.b, 0.1f);
        if(glm::length2(color) > 0.2f){
            segment = tracer::CreateChildBeam(segment);
            segment->color = color;
        }else{
            // stops tracing the laser.
            return nullptr;
        }
        break;
    }
    case PHOTON_BLOCKS_FILTER_GREEN:{
        glm::vec3 color = segment->color;
        color.r = glm::min(color.r, 0.1f);
        color.b = glm::min(color.b, 0.1f);
        if(glm::length2(color) > 0.2f){
            segment = tracer::CreateChildBeam(segment);
            segment->color = color;
        }else{
            // stops tracing the laser.
            return nullptr;
        }
        break;
    }
    case PHOTON_BLOCKS_FILTER_BLUE:{
        glm::vec3 color = segment->color;
        color.r = glm::min(color.r, 0.1f);
        color.g = glm::min(color.g, 0.2f);
        if(glm::length2(color) > 0.2f){
            segment = tracer::CreateChildBeam(segment);
            segment->color = color;
        }else{
            // stops tracing the laser.
            return nullptr;
        }
        break;
    }
    case PHOTON_BLOCKS_FILTER_YELLOW:{
        glm::vec3 color = segment->color;
        color.b = glm::min(color.b, 0.1f);
        if(glm::length2(color) > 0.2f){
            segment = tracer::CreateChildBeam(segment);
            segment->color = color;
        }else{
            // stops tracing the laser.
            return nullptr;
        }
        break;
    }
    case PHOTON_BLOCKS_FILTER_CYAN:{
        glm::vec3 color = segment->color;
        color.r = glm::min(color.r, 0.1f);
        if(glm::length2(color) > 0.2f){
            segment = tracer::CreateChildBeam(segment);
            segment->color = color;
        }else{
            // stops tracing the laser.
            return nullptr;
        }
        break;
    }
    case PHOTON_BLOCKS_FILTER_MAGENTA:{
        glm::vec3 color = segment->color;
        color.g = glm::min(color.g, 0.2f);
        if(glm::length2(color) > 0.2f){
            segment = tracer::CreateChildBeam(segment);
            segment->color = color;
        }else{
            // stops tracing the laser.
            return nullptr;
        }
        break;
    }
    }

    return segment;
}

void LoadTextures(){
    texture_plain_block = texture::Load("/textures/block.png");
    texture_mirror = texture::Load("/textures/mirror.png");
    texture_tnt = texture::Load("/textures/tnt.png");
    texture_explosion = texture::Load("/textures/explosion.png");
    texture_filter_red = texture::Load("/textures/filter_red.png");
    texture_filter_green = texture::Load("/textures/filter_green.png");;
    texture_filter_blue = texture::Load("/textures/filter_blue.png");;
    texture_filter_yellow = texture::Load("/textures/filter_yellow.png");
    texture_filter_cyan = texture::Load("/textures/filter_cyan.png");;
    texture_filter_magenta = texture::Load("/textures/filter_magenta.png");;
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

void OnFrame(glm::uvec2 location, photon_level &level, float time){
    photon_block &block = level.grid[location.x][location.y];
    switch(block.type){
    case PHOTON_BLOCKS_AIR:
    default:
        break;
    case PHOTON_BLOCKS_RECIEVER:
        break;
    case PHOTON_BLOCKS_PLAIN:
    case PHOTON_BLOCKS_INDESTRUCTIBLE:
        break;
    case PHOTON_BLOCKS_MIRROR:
    case PHOTON_BLOCKS_MIRROR_LOCKED:
    case PHOTON_BLOCKS_MIRROR_LOCKED_POS:
        break;
    case PHOTON_BLOCKS_TNT:
        // if block was not activated last frame cool down timer.
        if(!block.activated){
            block.data -= time;
            block.data = std::max(block.data, 0.0f);
        }
        break;
    case PHOTON_BLOCKS_TNT_FIREBALL:
        block.data -= time * 3.0f;
        if(block.data < 0.0f){
            block.data = 0.0f;
            block.type = PHOTON_BLOCKS_AIR;
        }
        break;
    }
    block.activated = false;
}

}

}
