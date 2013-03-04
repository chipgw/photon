#ifndef _PHOTON_GUI_H_
#define _PHOTON_GUI_H_

#include <glm/glm.hpp>

namespace photon{

namespace gui{

void InitFreeType();

void RenderText(glm::vec2 position, glm::vec2 scale, glm::vec4 color,const char *text,...);

}

}

#endif
