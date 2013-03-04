#include "GL/glew.h"

#include <stdlib.h>
#include <stdio.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/rotate_vector.hpp>

#include "photon_opengl.h"
#include "photon_window_managment.h"
#include "photon_core.h"
#include "photon_blocks.h"
#include "photon_texture.h"

#include <physfs.h>
#include <SOIL.h>

namespace photon{

namespace opengl{
photon_shader shader_scene;
photon_shader shader_laser;
photon_shader shader_light;
photon_shader shader_text;

GLuint photon_texture;

void InitOpenGL(photon_window &window){
    PrintToLog("INFO: Initializing OpenGL.");
    SDL_GL_MakeCurrent(window.window_SDL, window.context_SDL);

    GLuint glewstatus = glewInit();
    if(glewstatus != GLEW_OK){
        PrintToLog("GLEW ERROR: %s", glewGetErrorString(glewstatus));
        return;
    }
    if(GLEW_VERSION_2_0){
        PrintToLog("INFO: OpenGL 2.0 support detected, things should work fine.");
    }else{
        PrintToLog("WARNING: OpenGL 2.0 support NOT detected, things probably won't work.");
    }

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

    glGenTextures(1,&window.scene_buffer_texture);
    glBindTexture(GL_TEXTURE_2D, window.scene_buffer_texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // texture size is 1x1 because it will get resized properly later (on resized event from window creation)
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, 1, 1, 0, GL_RGBA, GL_FLOAT, 0);


    glBindTexture(GL_TEXTURE_2D, 0);

    glGenFramebuffers(1, &window.scene_buffer);
    glBindFramebuffer(GL_FRAMEBUFFER, window.scene_buffer);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, window.scene_buffer_texture, 0);

    GLenum buffer = GL_COLOR_ATTACHMENT0;
    glDrawBuffers(1, &buffer);

    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE){
        PrintToLog("ERROR: Frambuffer creation failed!");
        // TODO - proper error handling.
        throw;
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glDisable(GL_CULL_FACE);
    glEnable(GL_MULTISAMPLE);

    shader_scene = LoadShaderXML("/shaders/scene.xml");
    glUniform1f(glGetUniformLocation(shader_scene.program, "zoom"), 1.0f);

    shader_laser = LoadShaderXML("/shaders/laser.xml");
    glUniform1f(glGetUniformLocation(shader_laser.program, "zoom"), 1.0f);

    shader_light = LoadShaderXML("/shaders/light.xml");
    glUniform1f(glGetUniformLocation(shader_light.program, "zoom"), 1.0f);

    shader_text = LoadShaderXML("/shaders/text.xml");

    blocks::LoadTextures();

    photon_texture = texture::Load("/textures/photon.tga");

    if(!opengl::CheckOpenGLErrors()){
        PrintToLog("INFO: OpenGL succesfully initilized.");
    }
}

void GarbageCollect(photon_window &window){
    SDL_GL_MakeCurrent(window.window_SDL, window.context_SDL);

    texture::GarbageCollect();

    DeleteShader(shader_scene);

    PrintToLog("INFO: OpenGL garbage collection complete.");
}

void OnResize(int width, int height, photon_window &window){
    SDL_GL_MakeCurrent(window.window_SDL, window.context_SDL);

    PrintToLog("INFO: Resizing window to %ix%i.", width, height);

    float aspect = (float)width/(float)height;

    glUseProgram(shader_scene.program);

    glUniform1f(glGetUniformLocation(shader_scene.program, "aspect"), aspect);
    glUseProgram(shader_laser.program);
    glUniform1f(glGetUniformLocation(shader_laser.program, "aspect"), aspect);
    glUseProgram(shader_light.program);
    glUniform1f(glGetUniformLocation(shader_light.program, "aspect"), aspect);
    glUseProgram(shader_text.program);
    glUniform1f(glGetUniformLocation(shader_text.program, "aspect"), aspect);

    glViewport(0,0,width,height);

    window.width = width;
    window.height = height;

    glBindTexture(GL_TEXTURE_2D, window.scene_buffer_texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, 0);
}


GLenum CheckOpenGLErrors(){
    GLenum err = glGetError();

    switch(err){
    case GL_INVALID_ENUM:
        PrintToLog("OPENGL ERROR: Invalid Enum.");
        break;
    case GL_INVALID_VALUE:
        PrintToLog("OPENGL ERROR: Invalid Value.");
        break;
    case GL_INVALID_OPERATION:
        PrintToLog("OPENGL ERROR: Invalid Operation.");
        break;
    case GL_INVALID_FRAMEBUFFER_OPERATION:
        PrintToLog("OPENGL ERROR: Invalid Framebuffer Operation.");
        break;
    case GL_OUT_OF_MEMORY:
        PrintToLog("OPENGL ERROR: Out Of Memory.");
        break;
    case GL_NO_ERROR:
        break;
    }

    return err;
}

void UpdateZoom(float zoom){
    glUseProgram(shader_scene.program);
    glUniform1f(glGetUniformLocation(shader_scene.program, "zoom"), 1.0f/zoom);
    glUseProgram(shader_laser.program);
    glUniform1f(glGetUniformLocation(shader_laser.program, "zoom"), 1.0f/zoom);
    glUseProgram(shader_light.program);
    glUniform1f(glGetUniformLocation(shader_light.program, "zoom"), 1.0f/zoom);
}

void UpdateCenter(glm::vec2 center){
    glUseProgram(shader_scene.program);
    glUniform2fv(glGetUniformLocation(shader_scene.program, "center"), 1, glm::value_ptr(center));
    glUseProgram(shader_laser.program);
    glUniform2fv(glGetUniformLocation(shader_laser.program, "center"), 1, glm::value_ptr(center));
    glUseProgram(shader_light.program);
    glUniform2fv(glGetUniformLocation(shader_light.program, "center"), 1, glm::value_ptr(center));
}

void DrawModeScene(photon_window &window){
    SDL_GL_MakeCurrent(window.window_SDL, window.context_SDL);

    glBindFramebuffer(GL_FRAMEBUFFER, window.scene_buffer);

    glClear(GL_COLOR_BUFFER_BIT);

    glDisable(GL_BLEND);

    glUseProgram(shader_scene.program);
}

void DrawModeLaser(photon_window &window){
    SDL_GL_MakeCurrent(window.window_SDL, window.context_SDL);

    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE,GL_ONE);


    glUseProgram(shader_laser.program);
}

void DrawModeLevel(photon_window &window){
    SDL_GL_MakeCurrent(window.window_SDL, window.context_SDL);

    glBindFramebuffer(GL_FRAMEBUFFER, window.scene_buffer);

    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE,GL_ONE_MINUS_SRC_ALPHA);

    glUseProgram(shader_scene.program);
}

void DrawModeLight(photon_window &window){
    SDL_GL_MakeCurrent(window.window_SDL, window.context_SDL);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glClear(GL_COLOR_BUFFER_BIT);

    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE,GL_ONE);

    glUseProgram(shader_light.program);

    glBindTexture(GL_TEXTURE_2D, window.scene_buffer_texture);
}

void DrawPhoton(glm::vec2 location){
    glBegin(GL_TRIANGLE_FAN);{
        glVertexAttrib2f(PHOTON_VERTEX_UV_ATTRIBUTE, 0.0f, 1.0f);
        glVertexAttrib2f(PHOTON_VERTEX_LOCATION_ATTRIBUTE,location.x, location.y);

        glVertexAttrib2f(PHOTON_VERTEX_UV_ATTRIBUTE, 1.0f, 0.0f);
        glVertexAttrib2f(PHOTON_VERTEX_LOCATION_ATTRIBUTE,location.x + 8.0, location.y);

        glVertexAttrib2f(PHOTON_VERTEX_UV_ATTRIBUTE, 1.0f, 1.0f);
        glVertexAttrib2f(PHOTON_VERTEX_LOCATION_ATTRIBUTE,location.x , location.y + 8.0);

        glVertexAttrib2f(PHOTON_VERTEX_UV_ATTRIBUTE, 1.0f, 0.0f);
        glVertexAttrib2f(PHOTON_VERTEX_LOCATION_ATTRIBUTE,location.x - 8.0, location.y);

        glVertexAttrib2f(PHOTON_VERTEX_UV_ATTRIBUTE, 1.0f, 1.0f);
        glVertexAttrib2f(PHOTON_VERTEX_LOCATION_ATTRIBUTE,location.x, location.y - 8.0);

        glVertexAttrib2f(PHOTON_VERTEX_UV_ATTRIBUTE, 1.0f, 0.0f);
        glVertexAttrib2f(PHOTON_VERTEX_LOCATION_ATTRIBUTE,location.x + 8.0, location.y);
    }glEnd();

    glUseProgram(shader_scene.program);
    glBindTexture(GL_TEXTURE_2D, photon_texture);

    glBegin(GL_QUADS);{
        glVertexAttrib2f(PHOTON_VERTEX_UV_ATTRIBUTE, 1.0f, 1.0f);
        glVertexAttrib2f(PHOTON_VERTEX_LOCATION_ATTRIBUTE,location.x + 0.2, location.y + 0.2);

        glVertexAttrib2f(PHOTON_VERTEX_UV_ATTRIBUTE, 0.0f, 1.0f);
        glVertexAttrib2f(PHOTON_VERTEX_LOCATION_ATTRIBUTE,location.x - 0.2, location.y + 0.2);

        glVertexAttrib2f(PHOTON_VERTEX_UV_ATTRIBUTE, 0.0f, 0.0f);
        glVertexAttrib2f(PHOTON_VERTEX_LOCATION_ATTRIBUTE,location.x - 0.2, location.y - 0.2);

        glVertexAttrib2f(PHOTON_VERTEX_UV_ATTRIBUTE, 1.0f, 0.0f);
        glVertexAttrib2f(PHOTON_VERTEX_LOCATION_ATTRIBUTE,location.x + 0.2, location.y - 0.2);
    }glEnd();
}

void DrawModeGUI(photon_window &window){
    SDL_GL_MakeCurrent(window.window_SDL, window.context_SDL);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

    glUseProgram(shader_text.program);
}

void SetColorGUI(glm::vec4 color){
    glUseProgram(shader_text.program);
    glUniform4fv(glGetUniformLocation(shader_text.program, "color"), 1, glm::value_ptr(color));
}

}
}
