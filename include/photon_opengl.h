#ifndef _PHOTON_OPENGL_H_
#define _PHOTON_OPENGL_H_

#include <GL/glew.h>
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
void OnResize(int width, int height, photon_window &window);

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

void UpdateZoom(float zoom);
}
}
#endif
