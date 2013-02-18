#include "GL/glew.h"

#include <stdlib.h>
#include <stdio.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/rotate_vector.hpp>

#include "photon_opengl.h"
#include "photon_sdl.h"
#include "photon_core.h"
#include "photon_texture.h"

#include <physfs.h>
#include <SOIL.h>

namespace photon{

namespace opengl{
//shaders for 3d pass (saves data to gBuffer)
photon_shader shader_main;

void InitOpenGL(photon_window &window){
    PrintToLog("INFO: Initializing OpenGL.");
    SDL_GL_MakeCurrent(window.window_SDL, window.context_SDL);

    GLuint glewstatus = glewInit();
    if(glewstatus != GLEW_OK){
        PrintToLog("GLEW ERROR: %s", glewGetErrorString(glewstatus));
        return;
    }

    glClearDepth(1.0f);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

    glDisable(GL_CULL_FACE);

    shader_main = LoadShaderXML("/shaders/main.xml");

    // load identety matrix to shader
    GLint model_matrix_uniform = glGetUniformLocation(shader_main.program, "model");
    glm::mat4 model(1.0f);
    glUniformMatrix4fv(model_matrix_uniform, 1, GL_FALSE, glm::value_ptr(model));

    if(!opengl::CheckOpenGLErrors()){
        PrintToLog("INFO: OpenGL succesfully initilized.");
    }
}

void GarbageCollect(photon_window &window){
    SDL_GL_MakeCurrent(window.window_SDL, window.context_SDL);

    texture::GarbageCollect();

    DeleteShader(shader_main);

    PrintToLog("INFO: OpenGL garbage collection complete.");
}

void OnResize(int width, int height, photon_window &window){
    SDL_GL_MakeCurrent(window.window_SDL, window.context_SDL);

    PrintToLog("INFO: Resizing window to %ix%i.", width, height);

    window.width = width;
    window.height = height;
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
}
}
