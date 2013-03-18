#include "photon_blocks.h"
#include "photon_opengl.h"
#include "photon_level.h"
#include "photon_laser.h"
#include "photon_texture.h"
#include "photon_core.h"

#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/norm.hpp>

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
GLuint texture_emitter;

namespace photon{

namespace blocks{

void DrawBlock(glm::uvec2 location, float size = 0.5f, float rotation = 0.0f){
    glm::vec2 point1(size);
    glm::vec2 point2(size);
    point1 = glm::rotate(point1, rotation);
    point2 = glm::rotate(point2, rotation - 90.0f);

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
    default:
        break;
    case plain:
        glBindTexture(GL_TEXTURE_2D, texture_plain_block);

        DrawBlock(location);
        break;
    case indestructible:
        // TODO - make different texture.
        glBindTexture(GL_TEXTURE_2D, texture_plain_block);
        DrawBlock(location);
        break;
    case mirror:
    case mirror_locked:
    case mirror_locked_pos:
        glBindTexture(GL_TEXTURE_2D, texture_mirror);
        DrawBlock(location, 0.4f, block.data);
        break;
    case tnt:
        glBindTexture(GL_TEXTURE_2D, texture_tnt);
        DrawBlock(location);
        break;
    case filter_red:
        glBindTexture(GL_TEXTURE_2D, texture_filter_red);
        DrawBlock(location, 0.2f);
        break;
    case filter_green:
        glBindTexture(GL_TEXTURE_2D, texture_filter_green);
        DrawBlock(location, 0.2f);
        break;
    case filter_blue:
        glBindTexture(GL_TEXTURE_2D, texture_filter_blue);
        DrawBlock(location, 0.2f);
        break;
    case filter_yellow:
        glBindTexture(GL_TEXTURE_2D, texture_filter_yellow);
        DrawBlock(location, 0.2f);
        break;
    case filter_cyan:
        glBindTexture(GL_TEXTURE_2D, texture_filter_cyan);
        DrawBlock(location, 0.2f);
        break;
    case filter_magenta:
        glBindTexture(GL_TEXTURE_2D, texture_filter_magenta);
        DrawBlock(location, 0.2f);
        break;
    case emitter_white:
    case emitter_red:
    case emitter_green:
    case emitter_blue:
        glBindTexture(GL_TEXTURE_2D, texture_emitter);
        DrawBlock(location, 0.5f, block.data);
        break;
    }
}

unsigned int GetBlockTexture(block_type type){
    switch(type){
    default:
        return 0;
        break;
    case plain:
        return texture_plain_block;
        break;
    case indestructible:
        // TODO - make different texture.
        return texture_plain_block;
        break;
    case mirror:
    case mirror_locked:
    case mirror_locked_pos:
        return texture_mirror;
        break;
    case tnt:
        return texture_tnt;
        break;
    case filter_red:
        return texture_filter_red;
        break;
    case filter_green:
        return texture_filter_green;
        break;
    case filter_blue:
        return texture_filter_blue;
        break;
    case filter_yellow:
        return texture_filter_yellow;
        break;
    case filter_cyan:
        return texture_filter_cyan;
        break;
    case filter_magenta:
        return texture_filter_magenta;
        break;
    case emitter_white:
    case emitter_red:
    case emitter_green:
    case emitter_blue:
        return texture_emitter;
        break;
    }
}

void DrawFX(photon_block block, glm::uvec2 location){
    switch(block.type){
    default:
        break;
    case tnt:
        // TODO - draw some warmup thing...
        glBindTexture(GL_TEXTURE_2D, texture_explosion);
        opengl::SetFacFX(block.data * 0.5f);
        DrawBlock(location);
        break;
    case tnt_fireball:
        glBindTexture(GL_TEXTURE_2D, texture_explosion);
        opengl::SetFacFX(block.data);
        DrawBlock(location, 1.5);
        break;
    }
}

photon_lasersegment *OnLightInteract(photon_lasersegment *segment, glm::uvec2 location, photon_level &level, float time){
    photon_block &block = level.grid[location.x][location.y];
    block.activated = true;

    switch(block.type){
    case air:
    default:
        break;
    case reciever:
        // TODO - make trigger
        // stops tracing the laser.
        return nullptr;
        break;
    case plain:
    case indestructible:
        // stops tracing the laser.
        return nullptr;
        break;
    case mirror:
    case mirror_locked:
    case mirror_locked_pos:{
        float angle = segment->angle - block.data;
        if(fmod(angle, 180.0f) == 0.0f){
            return nullptr;
        }
        angle = fmod(angle + 180.0f, 360.0f) - 180.0f;
        segment = tracer::CreateChildBeam(segment);
        segment->angle = block.data - angle;
        break;
    }
    case tnt:
        block.data += time;
        if(block.data > 1.0f){
            // TODO - KABOOM goes here...
            PrintToLog("INFO: KABOOM!");
            block.type = tnt_fireball;
            // cooldown of fireball
            block.data = 1.0f;
            break;
        }
        // stops tracing the laser.
        return nullptr;
        break;
    case filter_red:{
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
    case filter_green:{
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
    case filter_blue:{
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
    case filter_yellow:{
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
    case filter_cyan:{
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
    case filter_magenta:{
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
    texture_emitter = texture::Load("/textures/emitter.png");;
}

void OnPhotonInteract(glm::uvec2 location, photon_level &level){
    if(level.grid.shape()[0] > location.x && level.grid.shape()[1] > location.y){
        photon_block &block = level.grid[location.x][location.y];
        switch(block.type){
        case air:
            // TODO - place currently selected item in inventory.
            block.type = mirror;
            break;
        default:
            break;
        case mirror:
            // TODO - store mirror in inventory.
            block.type = air;
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
        case mirror:
        case mirror_locked_pos:
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
    default:
        break;
    case tnt:
        // if block was not activated last frame cool down timer.
        if(!block.activated){
            block.data -= time;
            block.data = std::max(block.data, 0.0f);
        }
        break;
    case tnt_fireball:
        block.data -= time * 3.0f;
        if(block.data < 0.0f){
            block.data = 0.0f;
            block.type = air;
        }
        break;
    case emitter_white:{
        level.beams.push_back(photon_laserbeam());
        photon_laserbeam &beam = level.beams.back();
        beam.color = glm::vec3(0.9f,0.9f,0.9f);
        beam.origin = location;
        beam.origin_angle = block.data;
        break;
    }
    case emitter_red:{
        level.beams.push_back(photon_laserbeam());
        photon_laserbeam &beam = level.beams.back();
        beam.color = glm::vec3(0.9f,0.2f,0.1f);
        beam.origin = location;
        beam.origin_angle = block.data;
        break;
    }
    case emitter_green:{
        level.beams.push_back(photon_laserbeam());
        photon_laserbeam &beam = level.beams.back();
        beam.color = glm::vec3(0.1f,0.9f,0.2f);
        beam.origin = location;
        beam.origin_angle = block.data;
        break;
    }
    case emitter_blue:{
        level.beams.push_back(photon_laserbeam());
        photon_laserbeam &beam = level.beams.back();
        beam.color = glm::vec3(0.1f,0.2f,0.9f);
        beam.origin = location;
        beam.origin_angle = block.data;
        break;
    }
    }
    block.activated = false;
}

}

}
