#ifndef _PHOTON_GUI_H_
#define _PHOTON_GUI_H_

#include <glm/glm.hpp>
#include <string>

namespace photon{
struct photon_gui_element{
    float top = 0.0f;
    float bottom = 0.0f;
    float left = 0.0f;
    float right = 0.0f;

    unsigned int texture = 0;
    unsigned int texture_pressed = 0;

    glm::vec4 text_color;
    glm::vec4 text_color_pressed;

    float text_padding = 0.025f;
    bool center_text = true;

    std::string text;
};


namespace gui{

void InitFreeType();

void RenderText(glm::vec2 position, glm::vec2 scale, glm::vec4 color, bool center, const char *text,...);

void DrawElement(photon_gui_element &element);

bool InBounds(glm::vec2 coord, photon_gui_element &element);

}

}

#endif
