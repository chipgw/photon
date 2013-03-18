#include "photon_gui.h"
#include "photon_level.h"
#include "photon_opengl.h"
#include "photon_texture.h"

namespace photon{

namespace gui{
photon_gui_game InitGameGUI(){
    photon_gui_game gui;

    gui.bar.top = 0.5f;
    gui.bar.bottom = 0.0f;
    gui.bar.left = -1.0f;
    gui.bar.right = 1.0f;

    gui.bar_texture = texture::Load("/textures/gui/bar.png");

    gui.current_item.top = 0.56f;
    gui.current_item.bottom = 0.06f;
    gui.current_item.left = -0.6f;
    gui.current_item.right = -0.2f;

    return gui;
}

void DrawBounds(photon_gui_bounds &bounds){
    glBegin(GL_QUADS);{
        glVertexAttrib2f(PHOTON_VERTEX_UV_ATTRIBUTE, 0.0f, 0.0f);
        glVertexAttrib2f(PHOTON_VERTEX_LOCATION_ATTRIBUTE, bounds.left, bounds.bottom);
        glVertexAttrib2f(PHOTON_VERTEX_UV_ATTRIBUTE, 0.0f, 1.0f);
        glVertexAttrib2f(PHOTON_VERTEX_LOCATION_ATTRIBUTE, bounds.left, bounds.top);
        glVertexAttrib2f(PHOTON_VERTEX_UV_ATTRIBUTE, 1.0f, 1.0f);
        glVertexAttrib2f(PHOTON_VERTEX_LOCATION_ATTRIBUTE, bounds.right, bounds.top);
        glVertexAttrib2f(PHOTON_VERTEX_UV_ATTRIBUTE, 1.0f, 0.0f);
        glVertexAttrib2f(PHOTON_VERTEX_LOCATION_ATTRIBUTE, bounds.right, bounds.bottom);
    }glEnd();
}

void DrawGameGUI(photon_gui_game &gui){
    opengl::SetColorGUI(glm::vec4(0.0f));
    opengl::SetCenterGUI(glm::vec2(0.0f,-1.0f));
    glBindTexture(GL_TEXTURE_2D, gui.bar_texture);
    DrawBounds(gui.bar);

    // TODO - get current item from inventory.
    glBindTexture(GL_TEXTURE_2D, blocks::GetBlockTexture(mirror));
    DrawBounds(gui.current_item);
}

bool InBounds(glm::vec2 coord, photon_gui_bounds &bounds){
    return coord.x > bounds.left && coord.x < bounds.right &&
            coord.y > bounds.bottom && coord.y < bounds.top;
}

}

}
