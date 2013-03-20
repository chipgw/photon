#ifndef _PHOTON_GUI_H_
#define _PHOTON_GUI_H_

#include <glm/glm.hpp>
#include <string>
#include "photon_opengl.h"

namespace photon{
struct photon_level;
struct photon_gui_bounds{
    float top = 0.0f;
    float bottom = 0.0f;
    float left = 0.0f;
    float right = 0.0f;
};

struct photon_gui_game{
    photon_gui_bounds current_item;

    photon_gui_bounds bar;
    GLuint bar_texture;

    photon_gui_bounds time_display;
    photon_gui_bounds moves_display;
};

namespace gui{

photon_gui_game InitGameGUI();

void InitFreeType();

void RenderText(glm::vec2 position, glm::vec2 scale, glm::vec4 color, bool center, const char *text,...);

void DrawGameGUI(photon_gui_game &gui);

bool InBounds(glm::vec2 coord, photon_gui_bounds &bounds);

}

}

#endif
