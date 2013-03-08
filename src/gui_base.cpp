#include "photon_gui.h"
#include "photon_opengl.h"
#include "photon_texture.h"

namespace photon{

namespace gui{

void DrawElement(photon_gui_element &element){
    RenderText(glm::vec2(element.left, element.bottom), glm::vec2(element.top - element.bottom), element.text_color, element.text.c_str());
}

bool InBounds(glm::vec2 coord, photon_gui_element &element){
    return coord.x > element.left && coord.x < element.right &&
            coord.y > element.bottom && coord.y < element.top;
}

}

}
