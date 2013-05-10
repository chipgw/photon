#include "photon_core.h"
#include "photon_texture.h"

#include <glm/gtc/type_ptr.hpp>

namespace photon{

namespace opengl{
photon_shader shader_scene;
photon_shader shader_laser;
photon_shader shader_light;
photon_shader shader_fx;
photon_shader shader_text;

GLuint photon_texture;
GLuint background;

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

    glGenTextures(1, &window.light_buffer_texture);
    glBindTexture(GL_TEXTURE_2D, window.light_buffer_texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // texture size is 1x1 because it will get resized properly later (on resized event from window creation)
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);

    glBindTexture(GL_TEXTURE_2D, 0);

    glGenFramebuffers(1, &window.light_buffer);
    glBindFramebuffer(GL_FRAMEBUFFER, window.light_buffer);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, window.light_buffer_texture, 0);

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

    glEnableVertexAttribArray(PHOTON_VERTEX_LOCATION_ATTRIBUTE);
    glEnableVertexAttribArray(PHOTON_VERTEX_UV_ATTRIBUTE);

    shader_scene = LoadShaderXML("/shaders/scene.xml");
    shader_laser = LoadShaderXML("/shaders/laser.xml");
    shader_light = LoadShaderXML("/shaders/light.xml");
    shader_fx = LoadShaderXML("/shaders/fx.xml");
    shader_text = LoadShaderXML("/shaders/text.xml");

    blocks::LoadTextures();

    photon_texture = texture::Load("/textures/photon.png");
    background = texture::Load("/textures/background.png");

    if(!opengl::CheckOpenGLErrors()){
        PrintToLog("INFO: OpenGL succesfully initilized.");
    }
}

void GarbageCollect(photon_window &window){
    SDL_GL_MakeCurrent(window.window_SDL, window.context_SDL);

    CheckOpenGLErrors();

    texture::GarbageCollect();

    DeleteShader(shader_scene);

    PrintToLog("INFO: OpenGL garbage collection complete.");
}

void OnResize(uint32_t width, uint32_t height, photon_window &window){
    SDL_GL_MakeCurrent(window.window_SDL, window.context_SDL);

    PrintToLog("INFO: Resizing window to %ix%i.", width, height);

    float aspect = (float)width/(float)height;

    glUseProgram(shader_scene.program);

    glUniform1f(glGetUniformLocation(shader_scene.program, "aspect"), aspect);
    glUseProgram(shader_laser.program);
    glUniform1f(glGetUniformLocation(shader_laser.program, "aspect"), aspect);
    glUseProgram(shader_light.program);
    glUniform1f(glGetUniformLocation(shader_light.program, "aspect"), aspect);
    glUseProgram(shader_fx.program);
    glUniform1f(glGetUniformLocation(shader_fx.program,   "aspect"), aspect);
    glUseProgram(shader_text.program);
    glUniform1f(glGetUniformLocation(shader_text.program, "aspect"), aspect);

    glViewport(0, 0, width, height);

    window.width = width;
    window.height = height;

    glBindTexture(GL_TEXTURE_2D, window.light_buffer_texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
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

void UpdateZoom(const float &zoom){
    glUseProgram(shader_scene.program);
    glUniform1f(glGetUniformLocation(shader_scene.program, "zoom"), 1.0f / zoom);
    glUseProgram(shader_laser.program);
    glUniform1f(glGetUniformLocation(shader_laser.program, "zoom"), 1.0f / zoom);
    glUseProgram(shader_light.program);
    glUniform1f(glGetUniformLocation(shader_light.program, "zoom"), 1.0f / zoom);
    glUseProgram(shader_fx.program);
    glUniform1f(glGetUniformLocation(shader_fx.program,    "zoom"), 1.0f / zoom);
}

void UpdateCenter(const glm::vec2 &center){
    glUseProgram(shader_scene.program);
    glUniform2fv(glGetUniformLocation(shader_scene.program, "center"), 1, glm::value_ptr(center));
    glUseProgram(shader_laser.program);
    glUniform2fv(glGetUniformLocation(shader_laser.program, "center"), 1, glm::value_ptr(center));
    glUseProgram(shader_light.program);
    glUniform2fv(glGetUniformLocation(shader_light.program, "center"), 1, glm::value_ptr(center));
    glUseProgram(shader_fx.program);
    glUniform2fv(glGetUniformLocation(shader_fx.program,    "center"), 1, glm::value_ptr(center));
}

void DrawModeScene(photon_window &window){
    SDL_GL_MakeCurrent(window.window_SDL, window.context_SDL);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glClear(GL_COLOR_BUFFER_BIT);

    glDisable(GL_BLEND);

    glActiveTexture(PHOTON_TEXTURE_UNIT_LIGHT);
    glBindTexture(GL_TEXTURE_2D, window.light_buffer_texture);
    glActiveTexture(PHOTON_TEXTURE_UNIT_COLOR);
}

void DrawModeLaser(photon_window &window){
    SDL_GL_MakeCurrent(window.window_SDL, window.context_SDL);

    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE);

    glUseProgram(shader_laser.program);
}

void DrawModeLevel(photon_window &window){
    SDL_GL_MakeCurrent(window.window_SDL, window.context_SDL);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

    glUseProgram(shader_scene.program);

    glActiveTexture(PHOTON_TEXTURE_UNIT_LIGHT);
    glBindTexture(GL_TEXTURE_2D, window.light_buffer_texture);
    glActiveTexture(PHOTON_TEXTURE_UNIT_COLOR);
}

void DrawModeLight(photon_window &window){
    SDL_GL_MakeCurrent(window.window_SDL, window.context_SDL);

    glBindFramebuffer(GL_FRAMEBUFFER, window.light_buffer);

    glClear(GL_COLOR_BUFFER_BIT);

    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE);

    glUseProgram(shader_light.program);

    glActiveTexture(PHOTON_TEXTURE_UNIT_LIGHT);
    glBindTexture(GL_TEXTURE_2D, 0);
    glActiveTexture(PHOTON_TEXTURE_UNIT_COLOR);
}

void DrawPhoton(const glm::vec2 &location){
    static const float verts[] = { 0.2f, 0.2f,
                                  -0.2f, 0.2f,
                                  -0.2f,-0.2f,
                                   0.2f,-0.2f};

    static const float uv[] = {1.0f, 1.0f,
                               0.0f, 1.0f,
                               0.0f, 0.0f,
                               1.0f, 0.0f};

    glBindTexture(GL_TEXTURE_2D, photon_texture);

    SetFacFX(1.0f);

    opengl::SetModelMatrix(glm::mat3(1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, location.x, location.y, 1.0f));

    glVertexAttribPointer(PHOTON_VERTEX_LOCATION_ATTRIBUTE, 2, GL_FLOAT, GL_FALSE, 0, verts);
    glVertexAttribPointer(PHOTON_VERTEX_UV_ATTRIBUTE,       2, GL_FLOAT, GL_FALSE, 0, uv);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
}

void DrawPhotonLight(const glm::vec2 &location){
    static const float verts[] = { 16.0f, 16.0f,
                                  -16.0f, 16.0f,
                                  -16.0f,-16.0f,
                                   16.0f,-16.0f};

    static const float uv[] = { 1.0f, 1.0f,
                               -1.0f, 1.0f,
                               -1.0f,-1.0f,
                                1.0f,-1.0f};

    opengl::SetModelMatrix(glm::mat3(1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, location.x, location.y, 1.0f));

    glVertexAttribPointer(PHOTON_VERTEX_LOCATION_ATTRIBUTE, 2, GL_FLOAT, GL_FALSE, 0, verts);
    glVertexAttribPointer(PHOTON_VERTEX_UV_ATTRIBUTE,       2, GL_FLOAT, GL_FALSE, 0, uv);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
}

void DrawModeGUI(photon_window &window){
    SDL_GL_MakeCurrent(window.window_SDL, window.context_SDL);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glUseProgram(shader_text.program);
}

void SetColorGUI(const glm::vec4 &color){
    glUseProgram(shader_text.program);
    glUniform4fv(glGetUniformLocation(shader_text.program, "color"), 1, glm::value_ptr(color));
}

void SetCenterGUI(const glm::vec2 &center){
    glUseProgram(shader_text.program);
    glUniform2fv(glGetUniformLocation(shader_text.program, "center"), 1, glm::value_ptr(center));
}

void SetFacFX(const float &fac){
    glUniform1f(glGetUniformLocation(shader_fx.program, "fac"), fac);
}

void SetLaserColor(const glm::vec3 &color){
    glUniform3fv(glGetUniformLocation(shader_laser.program, "color"), 1, glm::value_ptr(color));
    glUniform3fv(glGetUniformLocation(shader_light.program, "color"), 1, glm::value_ptr(color));
}

void SetModelMatrix(const glm::mat3 &matrix){
    GLint program = 0;
    glGetIntegerv(GL_CURRENT_PROGRAM, &program);
    GLint uniform = glGetUniformLocation(program, "model");
    if(uniform > -1){
        glUniformMatrix3fv(uniform, 1, GL_FALSE, glm::value_ptr(matrix));
    }
}

void DrawModeFX(photon_window &window){
    SDL_GL_MakeCurrent(window.window_SDL, window.context_SDL);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE);

    glUseProgram(shader_fx.program);

    glActiveTexture(PHOTON_TEXTURE_UNIT_LIGHT);
    glBindTexture(GL_TEXTURE_2D, 0);
    glActiveTexture(PHOTON_TEXTURE_UNIT_COLOR);
}

void DrawBackground(photon_instance &instance){
    static const float verts[] = { 1.0f, 1.0f,
                                   1.0f,-1.0f,
                                  -1.0f,-1.0f,
                                  -1.0f, 1.0f};

    static const float uv[] = {1.0f, 1.0f,
                               1.0f, 0.0f,
                               0.0f, 0.0f,
                               0.0f, 1.0f};

    DrawModeFX(instance.window);

    glBindTexture(GL_TEXTURE_2D, background);

    glm::mat3 matrix(instance.camera_offset.z);

    float aspect = float(instance.window.width) / float(instance.window.height);
    if(aspect > 1.0f){
        matrix *= aspect;
    }else if(aspect < 1.0f){
        matrix /= aspect;
    }
    matrix[2] = glm::vec3(instance.player.location + glm::vec2(instance.camera_offset), 1.0f);

    SetFacFX(0.5f);
    SetModelMatrix(matrix);

    glVertexAttribPointer(PHOTON_VERTEX_LOCATION_ATTRIBUTE, 2, GL_FLOAT, GL_FALSE, 0, verts);
    glVertexAttribPointer(PHOTON_VERTEX_UV_ATTRIBUTE,       2, GL_FLOAT, GL_FALSE, 0, uv);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
}

}
}
