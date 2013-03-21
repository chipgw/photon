#include "photon_blocks.h"
#include "photon_opengl.h"
#include "photon_level.h"
#include "photon_texture.h"
#include "photon_core.h"

#include <glm/gtx/rotate_vector.hpp>

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

GLuint GetBlockTexture(block_type type){
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

void LoadTextures(){
    texture_plain_block = texture::Load("/textures/blocks/block.png");
    texture_mirror = texture::Load("/textures/blocks/mirror.png");
    texture_tnt = texture::Load("/textures/blocks/tnt.png");
    texture_filter_red = texture::Load("/textures/blocks/filter_red.png");
    texture_filter_green = texture::Load("/textures/blocks/filter_green.png");;
    texture_filter_blue = texture::Load("/textures/blocks/filter_blue.png");;
    texture_filter_yellow = texture::Load("/textures/blocks/filter_yellow.png");
    texture_filter_cyan = texture::Load("/textures/blocks/filter_cyan.png");;
    texture_filter_magenta = texture::Load("/textures/blocks/filter_magenta.png");;
    texture_emitter = texture::Load("/textures/blocks/emitter.png");
    texture_explosion = texture::Load("/textures/explosion.png");
}


}

}
