#ifndef _PHOTON_OPENGL_H_
#define _PHOTON_OPENGL_H_

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <vector>
#include <string>

namespace photon{
struct photon_window;

struct photon_shader{
    bool is_valid = false;

    std::vector<GLuint> shader_objects;

    GLuint program;
};

/*!
 * \brief contains OpenGL functions
 */
namespace opengl{

#define PHOTON_VERTEX_LOCATION_ATTRIBUTE       0
#define PHOTON_VERTEX_UV_ATTRIBUTE             1

/*!
 * \brief InitOpenGL
 */
void InitOpenGL(photon_window &window);

/*!
 * \brief GarbageCollect
 */
void GarbageCollect(photon_window &window);

/*!
 * \brief OnResize
 * \param width
 * \param height
 * \param window
 */
void OnResize(uint32_t width, uint32_t height, photon_window &window);

/*!
 * \brief CheckOpenGLErrors
 * \return GLenum returned by glGetError()
 */
GLenum CheckOpenGLErrors();

/*!
 * \brief Loads & compiles shader.
 * \param filename should be obvious...
 * \param shaderType the shader type as passed to glCreateShader()
 * \return the shader handle.
 */
GLuint LoadAndCompileShader(const char *filename, GLenum shader_type);

/*!
 * \brief Links a shader program and prints info to log.
 * \param shader the shader to link.
 * \return value of GL_LINK_STATUS
 */
int LinkShaderProgram(photon_shader &shader);

/*!
 * \brief Deletes a Shader object.
 * \param shader the shader to delete.
 */
void DeleteShader(photon_shader &shader);

/*!
 * \brief Loads a XML shader definition file.
 * \param filename path of XML file to load.
 * \return
 */
photon_shader LoadShaderXML(const std::string &filename);

/*!
 * \brief Updates the zoom uniforms in the shaders.
 * \param zoom
 */
void UpdateZoom(const float &zoom);

/*!
 * \brief Updates the center of the viewport in the shaders.
 * \param center
 */
void UpdateCenter(const glm::vec2 &center);

/*!
 * \brief switches to the background drawing mode. (clears scene buffer)
 * \param window
 */
void DrawModeScene(photon_window &window);

/*!
 * \brief switches to the laser drawing mode.
 * \param window
 */
void DrawModeLaser(photon_window &window);

/*!
 * \brief switches to the level drawing mode.
 * \param window
 */
void DrawModeLevel(photon_window &window);

/*!
 * \brief switches to the light pass mode.
 * \param window
 */
void DrawModeLight(photon_window &window);

/*!
 * \brief DrawPhoton
 * \param location
 */
void DrawPhoton(const glm::vec2 &location);

void DrawPhotonLight(const glm::vec2 &location);

void DrawModeFX(photon_window &window);

void DrawModeGUI(photon_window &window);

void SetColorGUI(const glm::vec4 &color);

void SetCenterGUI(const glm::vec2 &center);

void SetLaserColor(const glm::vec3 &color);

void SetModelMatrix(const glm::mat3 &matrix);

void SetFacFX(const float &fac);

}
}
#endif
