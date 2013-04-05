#ifndef _PHOTON_GUI_H_
#define _PHOTON_GUI_H_

#include <glm/glm.hpp>
#include <string>
#include "photon_opengl.h"

namespace photon{
struct photon_level;
struct photon_instance;

struct photon_gui_bounds{
    float top = 0.0f;
    float bottom = 0.0f;
    float left = 0.0f;
    float right = 0.0f;

    glm::vec2 offset = glm::vec2(0.0f);

    photon_gui_bounds() = default;
    photon_gui_bounds(float t, float b, float l, float r, float offsetx = 0.0f, float offsety = 0.0f)
        : top(t), bottom(b), left(l), right(r), offset(offsetx, offsety) {}
};

struct photon_gui_game{
    photon_gui_bounds current_item = {0.44f, 0.04f, -0.6f, -0.2f, 0.0f, -1.0f};

    photon_gui_bounds bar = {0.5f, 0.0f, -1.0f, 1.0f, 0.0f, -1.0f};
    GLuint bar_texture = 0;

    photon_gui_bounds pause_menu_button = {0.24f, 0.14f, 0.84f, 0.94f, 0.0f, -1.0f};
    GLuint pause_menu_button_texture = 0;
    photon_gui_bounds toggle_fullscreen_button = {0.12f, 0.02f, 0.84f, 0.94f, 0.0f, -1.0f};
    GLuint toggle_fullscreen_button_texture = 0;

    glm::vec2 moves_display_location = glm::vec2(-0.1f, 0.18f);
    glm::vec2 time_display_location  = glm::vec2(-0.1f, 0.10f);
    glm::vec2 fps_display_location   = glm::vec2(-0.1f, 0.02f);
};

struct photon_gui_pause_menu{
    photon_gui_bounds resume_button     = { 0.85f, 0.55f, -0.6f, 0.6f};
    photon_gui_bounds load_button       = { 0.50f, 0.20f, -0.6f, 0.6f};
    photon_gui_bounds save_button       = { 0.15f,-0.15f, -0.6f, 0.6f};
    photon_gui_bounds main_menu_button  = {-0.20f,-0.50f, -0.6f, 0.6f};
    photon_gui_bounds exit_button       = {-0.55f,-0.85f, -0.6f, 0.6f};
};

struct photon_gui_main_menu{
    photon_gui_bounds play_button = { 0.50f, 0.20f,-0.9f, 0.3f};
    photon_gui_bounds load_button = { 0.15f,-0.15f,-0.9f, 0.3f};
    photon_gui_bounds exit_button = {-0.20f,-0.50f,-0.9f, 0.3f};
};

struct photon_gui_container{
    photon_gui_game game;
    photon_gui_pause_menu pause_menu;
    photon_gui_main_menu main_menu;

    // TODO - add other gui states.

    GLuint text_button_texture = 0;
};

namespace gui{

photon_gui_container InitGUI();

void InitFreeType();

void RenderText(glm::vec2 position, glm::vec2 scale, glm::vec4 color, bool center, const char *text,...);

void DrawGUI(photon_instance &instance, float time);

bool InBounds(glm::vec2 coord, const photon_gui_bounds &bounds);

bool HandleMouseClick(photon_instance &instance, int x, int y);

}

}

#endif
