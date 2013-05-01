#include "photon_texture.h"
#include "photon_core.h"

#include <physfs.h>
#include <algorithm>

namespace photon{

namespace gui{

static const glm::vec4 base_color(0.9f);
static const glm::vec4 highlight_color(0.9f, 0.8f, 0.4f, 0.9f);

void DrawButtonList(photon_gui_button_list &list){
    for(auto button : list.buttons){
        DrawBounds(button.bounds);
    }

    for(uint8_t i = 0; i < list.buttons.size(); i++){
        DrawButtonText(list.buttons[i], i == list.highlighted);
    }
}

void ActivateButtonMainMenu(photon_instance &instance, int8_t button){
    // TODO - make enum or macro or something to make this more human readable...
    switch(button){
    case 0:
        instance.level = level::LoadLevelXML("/level.xml", instance.player);
        instance.paused = false;
        break;
    case 1:
        StartLoadingGUI(instance.gui.load_save_menu);
        break;
    case 2:
        // TODO - ask for confirmation.
        Close(instance);
        break;
    }
    instance.gui.main_menu.highlighted = -1;
}

void ActivateButtonPauseMenu(photon_instance &instance, int8_t button){
    // TODO - make enum or macro or something to make this more human readable...
    switch(button){
    case 0:
        instance.paused = false;
        break;
    case 1:
        StartLoadingGUI(instance.gui.load_save_menu);
        break;
    case 2:
        StartSavingGUI(instance.gui.load_save_menu);
        break;
    case 3:
        // TODO - ask for confirmation.
        instance.level = photon_level();
        break;
    case 4:
        // TODO - ask for confirmation.
        Close(instance);
        break;
    }
    instance.gui.pause_menu.highlighted = -1;
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
