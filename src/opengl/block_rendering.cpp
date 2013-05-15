#include "photon_texture.h"
#include "photon_core.h"

#include <glm/gtx/rotate_vector.hpp>

GLuint texture_plain_block;
GLuint texture_indestructible_block;
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
GLuint texture_receiver;
GLuint texture_receiver_fx;
GLuint texture_receiver_fx_red;
GLuint texture_receiver_fx_green;
GLuint texture_receiver_fx_blue;
GLuint texture_target;

namespace photon{

namespace blocks{

void DrawBlock(glm::vec2 location, float size = 0.5f, float rotation = 0.0f){
    static const float verts[] = { 1.0f, 1.0f,
                                   1.0f,-1.0f,
                                  -1.0f,-1.0f,
                                  -1.0f, 1.0f};

    static const float uv[] = {1.0f, 1.0f,
                               1.0f, 0.0f,
                               0.0f, 0.0f,
                               0.0f, 1.0f};

    rotation = glm::radians(rotation);

    glm::mat3 matrix;
    matrix[0] = glm::vec3( glm::cos(rotation), glm::sin(rotation), 0.0f);
    matrix[1] = glm::vec3(-glm::sin(rotation), glm::cos(rotation), 0.0f);

    matrix *= size;
    matrix[2] = glm::vec3(location, 1.0f);

    opengl::SetModelMatrix(matrix);

    glVertexAttribPointer(PHOTON_VERTEX_LOCATION_ATTRIBUTE, 2, GL_FLOAT, GL_FALSE, 0, verts);
    glVertexAttribPointer(PHOTON_VERTEX_UV_ATTRIBUTE,       2, GL_FLOAT, GL_FALSE, 0, uv);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
}

void Draw(photon_block block, glm::vec2 location){
    switch(block.type){
    default:
        break;
    case plain:
        glBindTexture(GL_TEXTURE_2D, texture_plain_block);
        DrawBlock(location);
        break;
    case indestructible:
        glBindTexture(GL_TEXTURE_2D, texture_indestructible_block);
        DrawBlock(location);
        break;
    case mirror:
    case mirror_locked:
        glBindTexture(GL_TEXTURE_2D, texture_mirror);
        DrawBlock(location, 0.4f, block.angle);
        break;
    case target:
        glBindTexture(GL_TEXTURE_2D, texture_target);
        DrawBlock(location);
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
        DrawBlock(location, 0.5f, block.angle);
        break;
    case receiver:
    case receiver_white:
    case receiver_red:
    case receiver_green:
    case receiver_blue:
        glBindTexture(GL_TEXTURE_2D, texture_receiver);
        DrawBlock(location, 0.5f, block.angle);
        break;
    case move:{
        // TODO - make texture.
        glBindTexture(GL_TEXTURE_2D, texture_indestructible_block);
        glm::vec2 offset(location);
        offset += glm::vec2(cos(block.angle) * block.power, sin(block.angle) * block.power);
        DrawBlock(offset);
        break;
    }
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
        return texture_indestructible_block;
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
    case receiver:
    case receiver_white:
    case receiver_red:
    case receiver_green:
    case receiver_blue:
        return texture_receiver;
        break;
    }
}

void DrawFX(photon_block block, glm::vec2 location){
    switch(block.type){
    default:
        break;
    case tnt:
        glBindTexture(GL_TEXTURE_2D, texture_filter_red);
        opengl::SetFacFX(block.power);
        DrawBlock(location);
        break;
    case tnt_fireball:
        glBindTexture(GL_TEXTURE_2D, texture_explosion);
        opengl::SetFacFX(block.power);
        DrawBlock(location, 1.5);
        break;
    case receiver:
    case receiver_white:
        glBindTexture(GL_TEXTURE_2D, texture_receiver_fx);
        opengl::SetFacFX(block.power + 0.2f);
        DrawBlock(location, 0.5f, block.angle);
        break;
    case receiver_red:
        glBindTexture(GL_TEXTURE_2D, texture_receiver_fx_red);
        opengl::SetFacFX(block.power + 0.2f);
        DrawBlock(location, 0.5f, block.angle);
        break;
    case receiver_green:
        glBindTexture(GL_TEXTURE_2D, texture_receiver_fx_green);
        opengl::SetFacFX(block.power + 0.2f);
        DrawBlock(location, 0.5f, block.angle);
        break;
    case receiver_blue:
        glBindTexture(GL_TEXTURE_2D, texture_receiver_fx_blue);
        opengl::SetFacFX(block.power + 0.2f);
        DrawBlock(location, 0.5f, block.angle);
        break;
    }
}

void LoadTextures(){
    texture_plain_block = texture::Load("/textures/blocks/block.png");
    texture_indestructible_block = texture::Load("/textures/blocks/indestructible_block.png");
    texture_mirror = texture::Load("/textures/blocks/mirror.png");
    texture_tnt = texture::Load("/textures/blocks/tnt.png");
    texture_filter_red = texture::Load("/textures/blocks/filter_red.png");
    texture_filter_green = texture::Load("/textures/blocks/filter_green.png");;
    texture_filter_blue = texture::Load("/textures/blocks/filter_blue.png");;
    texture_filter_yellow = texture::Load("/textures/blocks/filter_yellow.png");
    texture_filter_cyan = texture::Load("/textures/blocks/filter_cyan.png");;
    texture_filter_magenta = texture::Load("/textures/blocks/filter_magenta.png");;
    texture_emitter = texture::Load("/textures/blocks/emitter.png");
    texture_receiver = texture::Load("/textures/blocks/receiver.png");
    texture_receiver_fx = texture::Load("/textures/blocks/receiver_fx.png");
    texture_receiver_fx_red = texture::Load("/textures/blocks/receiver_fx_red.png");
    texture_receiver_fx_green = texture::Load("/textures/blocks/receiver_fx_green.png");
    texture_receiver_fx_blue = texture::Load("/textures/blocks/receiver_fx_blue.png");
    texture_explosion = texture::Load("/textures/explosion.png");
    texture_target = texture::Load("/textures/blocks/target.png");
}


}

}
