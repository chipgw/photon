#include "photon_texture.h"
#include "photon_core.h"

#include <physfs.h>
#include <algorithm>

namespace photon{

namespace gui{

void DrawButtonList(photon_gui_button_list &list){
    for(auto button : list.buttons){
        DrawBounds(button.bounds);
    }

    for(uint8_t i = 0; i < list.buttons.size(); i++){
        DrawButtonText(list.buttons[i], i == list.highlighted, list.base_color, list.highlight_color);
    }
}

void ActivateButton(photon_instance &instance, photon_gui_button_list &list, glm::vec2 coordinate){
    for(int i = 0; i < list.buttons.size(); i++){
        if(InBounds(coordinate, list.buttons[i].bounds)){
            ActivateButton(instance, list, i);
        }
    }
}

void ActivateButton(photon_instance &instance, photon_gui_button_list &list, int8_t button){
    if(button >= 0 && button < list.buttons.size() && list.buttons[button].action != nullptr){
        list.buttons[button].action(instance);

        list.highlighted = -1;
    }
}

void CalculateButtonListBounds(photon_gui_button_list &list, photon_gui_bounds base, float padding){
    float button_height = base.top - base.bottom + padding;
    float v = (list.buttons.size() - 1) - (list.buttons.size() - 1) / 2.0f;

    for(uint8_t i = 0; i < list.buttons.size(); i++){
        float y = button_height * (v - i);
        list.buttons[i].bounds = base;
        list.buttons[i].bounds.top += y;
        list.buttons[i].bounds.bottom += y;
    }
}

}

}
