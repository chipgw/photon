#ifndef _PHOTON_TEXTURE_H_
#define _PHOTON_TEXTURE_H_

#include "photon_opengl.h"
#include <string>

namespace photon{

/*!
 * \brief contains texture functions
 */
namespace texture{

#define PHOTON_TEXTURE_UNIT_COLOR          GL_TEXTURE0
#define PHOTON_TEXTURE_UNIT_LIGHT          GL_TEXTURE1

/*!
 * \brief Loads texture from file.
 * \param filename Filename for the texture.
 * \return the texture.
 */
GLuint Load(const std::string &filename);

/*!
 * \brief GarbageCollect
 */
void GarbageCollect();
}
}
#endif
