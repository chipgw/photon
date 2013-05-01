#include "photon_texture.h"
#include "photon_core.h"

#include <physfs.h>
#include <algorithm>

namespace photon{

namespace gui{

void DrawButtonText(photon_gui_button &button, bool highlighted, const glm::vec4 &base_color, const glm::vec4 &highlight_color){
    float padding = (button.bounds.top - button.bounds.bottom) / 3.0f;
    glm::vec2 size(button.bounds.top - button.bounds.bottom - padding * 1.5f);

    if(highlighted){
        RenderText(glm::vec2((button.bounds.right + button.bounds.left) / 2.0f, button.bounds.bottom + padding), size, highlight_color, true, button.text);
    }else{
        RenderText(glm::vec2((button.bounds.right + button.bounds.left) / 2.0f, button.bounds.bottom + padding), size, base_color, true, button.text);
    }
}

}

}
