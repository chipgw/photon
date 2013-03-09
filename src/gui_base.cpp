#include "photon_gui.h"
#include "photon_opengl.h"
#include "photon_texture.h"

namespace photon{

namespace gui{

void DrawElement(photon_gui_element &element){
    opengl::SetColorGUI(glm::vec4(0.0f));
    glBindTexture(GL_TEXTURE_2D, element.texture);

    glBegin(GL_QUADS);{
        glVertexAttrib2f(PHOTON_VERTEX_UV_ATTRIBUTE, 0.0f, 0.0f);
        glVertexAttrib2f(PHOTON_VERTEX_LOCATION_ATTRIBUTE, element.left, element.bottom);
        glVertexAttrib2f(PHOTON_VERTEX_UV_ATTRIBUTE, 0.0f, 1.0f);
        glVertexAttrib2f(PHOTON_VERTEX_LOCATION_ATTRIBUTE, element.left, element.top);
        glVertexAttrib2f(PHOTON_VERTEX_UV_ATTRIBUTE, 1.0f, 1.0f);
        glVertexAttrib2f(PHOTON_VERTEX_LOCATION_ATTRIBUTE, element.right, element.top);
        glVertexAttrib2f(PHOTON_VERTEX_UV_ATTRIBUTE, 1.0f, 0.0f);
        glVertexAttrib2f(PHOTON_VERTEX_LOCATION_ATTRIBUTE, element.right, element.bottom);
    }glEnd();

    glm::vec2 text_position(element.left, element.bottom + element.text_padding);
    float scale = (element.top - element.text_padding) - (element.bottom + element.text_padding);

    if(element.center_text){
        text_position.x += element.right;
        text_position.x /= 2;
    }

    RenderText(text_position, glm::vec2(scale), element.text_color, element.center_text, element.text.c_str());
}

bool InBounds(glm::vec2 coord, photon_gui_element &element){
    return coord.x > element.left && coord.x < element.right &&
            coord.y > element.bottom && coord.y < element.top;
}

}

}
