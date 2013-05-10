#ifndef _PHOTON_GUI_H_
#define _PHOTON_GUI_H_

#include "photon_opengl.h"

#include <functional>

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

struct photon_gui_button{
    photon_gui_bounds bounds;
    std::string text;

    std::function<void(photon_instance&)> action;

    photon_gui_button(std::string t) : text(t) {}
    photon_gui_button(std::string t, photon_gui_bounds b) : bounds(b), text(t) {}
    photon_gui_button(std::string t, std::function<void(photon_instance&)> a) : text(t), action(a) {}
};

struct photon_gui_textbox{
    photon_gui_bounds bounds;
    GLuint bounds_texture = 0;
    std::string text;
    int16_t cursor = 0;
};

struct photon_gui_button_list{
    std::vector<photon_gui_button> buttons;
    int8_t highlighted = -1;

    glm::vec4 base_color;
    glm::vec4 highlight_color;
};

struct photon_gui_game{
    photon_gui_bounds current_item = {0.44f, 0.04f, -0.8f, -0.4f, 0.0f, -1.0f};

    photon_gui_bounds bar = {0.5f, 0.0f, -1.0f, 1.0f, 0.0f, -1.0f};
    GLuint bar_texture = 0;

    photon_gui_bounds pause_menu_button = {0.24f, 0.14f, 0.84f, 0.94f, 0.0f, -1.0f};
    GLuint pause_menu_button_texture = 0;
    photon_gui_bounds toggle_fullscreen_button = {0.12f, 0.02f, 0.84f, 0.94f, 0.0f, -1.0f};
    GLuint toggle_fullscreen_button_texture = 0;

    std::string message;
    float message_timeout = INFINITY;
    photon_gui_bounds message_area = {0.7f, 0.4f, -0.3f, 1.0f, 0.0f, -1.0f};
    GLuint message_background_texture = 0;

    glm::vec2 moves_display_location     = glm::vec2(-0.3f, 0.16f);
    glm::vec2 time_display_location      = glm::vec2(-0.3f, 0.06f);
};

struct photon_gui_load_save_menu{
    photon_gui_bounds file_list_bounds = { 0.95f,-0.85f,-0.9f, 0.9f};
    photon_gui_bounds filename_box     = {-0.65f,-0.75f,-0.4f, 0.4f};
    GLuint file_list_background = 0;
    GLuint filename_box_background = 0;

    photon_gui_button cancel_button  = {"Cancel",  {-0.80f,-0.95f,-0.9f,-0.3f}};
    photon_gui_button confirm_button = {"Confirm", {-0.80f,-0.95f, 0.3f, 0.9f}};

    std::vector<std::string> file_list;
    int current_file_index = -1;
    std::string filename;
    int cursor = 0;

    bool loading = false;
    bool saving  = false;
};

struct photon_gui_container{
    photon_gui_game game;
    photon_gui_button_list pause_menu;
    photon_gui_button_list main_menu;
    photon_gui_load_save_menu load_save_menu;

    // TODO - add other gui states.

    glm::vec4 base_color;
    glm::vec4 highlight_color;
    glm::vec4 background_color;
    glm::vec2 small_font;

    GLuint text_button_texture = 0;
};

namespace gui{

photon_gui_container InitGUI();

void InitFreeType();

void RenderText(glm::vec2 position, glm::vec2 scale, glm::vec4 color, bool center, const char *text,...);

void RenderText(glm::vec2 position, glm::vec2 scale, glm::vec4 color, bool center, const std::string &text);

float GetTextWidth(const std::string &text, glm::vec2 scale, uint32_t start_pos = 0, int32_t end_pos = -1);

std::pair<int32_t, int32_t> GetTextLimits(const std::string &text, float desired_width, glm::vec2 scale, int32_t end_pos);

void DrawGUI(photon_instance &instance);

bool InBounds(glm::vec2 coord, const photon_gui_bounds &bounds);

bool HandleMouseClick(photon_instance &instance, int x, int y);

void ConfirmLoadSave(photon_instance &instance);

void CancelLoadSave(photon_instance &instance);

void StartLoadingGUI(photon_instance &instance);

void StartSavingGUI(photon_instance &instance);

void ActivateButton(photon_instance &instance, photon_gui_button_list &list, glm::vec2 coordinate);

void ActivateButton(photon_instance &instance, photon_gui_button_list &list, int8_t button);

void DrawBounds(const photon_gui_bounds &bounds);

void DrawButtonText(photon_gui_button &button, bool highlighted, const glm::vec4 &base_color, const glm::vec4 &highlight_color);

void DrawButtonList(photon_gui_button_list &list);

void CalculateButtonListBounds(photon_gui_button_list &list, photon_gui_bounds base, float padding);

}

}

#endif
