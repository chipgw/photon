#include "photon_texture.h"
#include "photon_core.h"

#include <physfs.h>
#include <algorithm>

namespace photon{

namespace gui{


photon_gui_container InitGUI(){
    photon_gui_container gui;

    gui.base_color = glm::vec4(0.9f);
    gui.highlight_color = glm::vec4(0.9f, 0.8f, 0.4f, 0.9f);
    gui.background_color = glm::vec4(0.1f, 0.1f, 0.1f, -0.4f);
    gui.small_font = glm::vec2(0.05f);

    gui.game.bar_texture = texture::Load("/textures/gui/bar.png");
    gui.game.pause_menu_button_texture = texture::Load("/textures/gui/pause_button.png");
    gui.game.toggle_fullscreen_button_texture = texture::Load("/textures/gui/fullscreen_button.png");

    gui.text_button_texture = texture::Load("/textures/gui/button.png");

    gui.load_save_menu.file_list_background = texture::Load("/textures/gui/file_list.png");
    gui.load_save_menu.filename_box_background = texture::Load("/textures/gui/text_input_box.png");

    gui.pause_menu.buttons.push_back({"Resume",
                                      [](photon_instance &instance) {
                                          instance.paused = false;
                                      } });
    gui.pause_menu.buttons.push_back({"Load", StartLoadingGUI });
    gui.pause_menu.buttons.push_back({"Save", StartSavingGUI });
    gui.pause_menu.buttons.push_back({"Exit to Menu",
                                      [](photon_instance &instance) {
                                          instance.level = photon_level();
                                      } });
    gui.pause_menu.buttons.push_back({"Exit Game", Close });

    CalculateButtonListBounds(gui.pause_menu, { 0.15f,-0.15f, -0.6f, 0.6f}, 0.02f);
    gui.pause_menu.base_color = gui.base_color;
    gui.pause_menu.highlight_color = gui.highlight_color;

    gui.main_menu.buttons.push_back({"Play",
                                     [](photon_instance &instance) {
                                         instance.level = level::LoadLevelXML("/level.xml", instance.player);
                                         instance.paused = false;
                                     } });
    gui.main_menu.buttons.push_back({"Load", StartSavingGUI });
    gui.main_menu.buttons.push_back({"Exit Game", Close});
    CalculateButtonListBounds(gui.main_menu, { 0.15f,-0.15f, -0.9f, 0.3f}, 0.02f);
    gui.main_menu.base_color = gui.base_color;
    gui.main_menu.highlight_color = gui.highlight_color;

    return gui;
}

void DrawBounds(const photon_gui_bounds &bounds){
    static const float uv[] = {0.0f, 0.0f,
                               0.0f, 1.0f,
                               1.0f, 1.0f,
                               1.0f, 0.0f};

    float verts[] = {bounds.left, bounds.bottom,
                     bounds.left, bounds.top,
                     bounds.right, bounds.top,
                     bounds.right, bounds.bottom};

    opengl::SetCenterGUI(bounds.offset);

    glVertexAttribPointer(PHOTON_VERTEX_LOCATION_ATTRIBUTE, 2, GL_FLOAT, GL_FALSE, 0, verts);
    glVertexAttribPointer(PHOTON_VERTEX_UV_ATTRIBUTE, 2, GL_FLOAT, GL_FALSE, 0, uv);

    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
}

void DrawGUI(photon_instance &instance){
    static const photon_gui_bounds fill_bounds(-100.0f, 100.0f, -100.0f, 100.0f);
    static const glm::vec4 blank(0.0f);

    if(!instance.level.is_valid){
        opengl::SetColorGUI(blank);

        glBindTexture(GL_TEXTURE_2D, instance.gui.text_button_texture);

        DrawButtonList(instance.gui.main_menu);
    }else{
        photon_gui_game &gui = instance.gui.game;
        opengl::SetColorGUI(blank);

        glBindTexture(GL_TEXTURE_2D, gui.bar_texture);
        DrawBounds(gui.bar);

        glBindTexture(GL_TEXTURE_2D, gui.pause_menu_button_texture);
        DrawBounds(gui.pause_menu_button);
        glBindTexture(GL_TEXTURE_2D, gui.toggle_fullscreen_button_texture);
        DrawBounds(gui.toggle_fullscreen_button);

        GLuint tex = blocks::GetBlockTexture(player::CurrentItem(instance.player));
        if(tex){
            glBindTexture(GL_TEXTURE_2D, tex);
            DrawBounds(gui.current_item);

            int8_t count = player::GetItemCountCurrent(instance.player);
            if(count < 0){
                RenderText(glm::vec2(gui.current_item.left, gui.current_item.bottom), instance.gui.small_font, instance.gui.base_color, false, "infinite");
            }else{
                RenderText(glm::vec2(gui.current_item.left, gui.current_item.bottom), instance.gui.small_font, instance.gui.base_color, false, "%i", count);
            }
        }
        gui::RenderText(gui.moves_display_location, instance.gui.small_font, instance.gui.base_color, false, "Moves: %i", instance.level.moves);
        gui::RenderText(gui.time_display_location,  instance.gui.small_font, instance.gui.base_color, false, "Time: %i:%02i", int(instance.level.time) / 60, int(instance.level.time) % 60);

        if(!gui.message.empty() && gui.message_timeout - instance.level.time > 0.0f){
            float strength = std::min(gui.message_timeout - instance.level.time, 1.0f);
            glm::vec4 color = instance.gui.background_color;
            color.a += strength - 1.0f;
            // TODO - use a texture for this...
            glBindTexture(GL_TEXTURE_2D, 0);
            opengl::SetColorGUI(color);
            DrawBounds(gui.message_area);

            color = instance.gui.base_color;
            color.a += strength - 2.0f;
            RenderText(glm::vec2(gui.message_area.left + 0.04f, gui.message_area.top - instance.gui.small_font.y - 0.04f), instance.gui.small_font, color, false, gui.message);
        }

        if(instance.paused){
            glBindTexture(GL_TEXTURE_2D, 0);
            opengl::SetColorGUI(instance.gui.background_color);
            DrawBounds(fill_bounds);

            opengl::SetColorGUI(blank);

            glBindTexture(GL_TEXTURE_2D, instance.gui.text_button_texture);

            DrawButtonList(instance.gui.pause_menu);
        }
    }
    if(instance.gui.load_save_menu.loading || instance.gui.load_save_menu.saving){
        photon_gui_load_save_menu &gui = instance.gui.load_save_menu;

        glBindTexture(GL_TEXTURE_2D, 0);
        opengl::SetColorGUI(instance.gui.background_color);
        DrawBounds(fill_bounds);

        opengl::SetColorGUI(blank);
        glBindTexture(GL_TEXTURE_2D, gui.file_list_background);
        DrawBounds(gui.file_list_bounds);

        glBindTexture(GL_TEXTURE_2D, gui.filename_box_background);
        DrawBounds(gui.filename_box);

        glBindTexture(GL_TEXTURE_2D, instance.gui.text_button_texture);
        DrawBounds(gui.cancel_button.bounds);
        DrawBounds(gui.confirm_button.bounds);

        DrawButtonText(gui.cancel_button, false, instance.gui.base_color, instance.gui.highlight_color);
        DrawButtonText(gui.confirm_button, false, instance.gui.base_color, instance.gui.highlight_color);

        float text_width = gui.filename_box.right - gui.filename_box.left - 0.05f;
        if(GetTextWidth(gui.filename, instance.gui.small_font) > text_width){
            auto limits = GetTextLimits(gui.filename, text_width, instance.gui.small_font, gui.cursor + 4);

            RenderText(glm::vec2(gui.filename_box.left, gui.filename_box.bottom) + 0.025f, instance.gui.small_font, instance.gui.base_color, false, gui.filename.substr(limits.first, limits.second - limits.first));

            RenderText(glm::vec2(gui.filename_box.left + GetTextWidth(gui.filename, instance.gui.small_font, limits.first, gui.cursor), gui.filename_box.bottom) + 0.025f, instance.gui.small_font, instance.gui.base_color, false, "_");
        }else{
            RenderText(glm::vec2(gui.filename_box.left, gui.filename_box.bottom) + 0.025f, instance.gui.small_font, instance.gui.base_color, false, gui.filename);

            RenderText(glm::vec2(gui.filename_box.left + GetTextWidth(gui.filename, instance.gui.small_font, 0, gui.cursor), gui.filename_box.bottom) + 0.025f, instance.gui.small_font, instance.gui.base_color, false, "_");
        }
        uint16_t i = 0;
        glm::vec2 location(gui.file_list_bounds.left + 0.1f, gui.file_list_bounds.top - 0.15f);
        for(auto file : gui.file_list){
            if(i == gui.current_file_index){
                RenderText(location, instance.gui.small_font, instance.gui.highlight_color, false, file);
            }else{
                RenderText(location, instance.gui.small_font, instance.gui.base_color, false, file);
            }
            location.y -= 0.08f;
            i++;
        }
    }

    if(!instance.input.is_valid){
        glBindTexture(GL_TEXTURE_2D, 0);
        opengl::SetColorGUI(instance.gui.background_color);
        DrawBounds(fill_bounds);
        opengl::SetCenterGUI(glm::vec2(0.0f));
        RenderText(glm::vec2(0.0f), instance.gui.small_font, instance.gui.base_color, true, "Press a button on the device to use...");
    }
}

bool InBounds(glm::vec2 coord, const photon_gui_bounds &bounds){
    coord -= bounds.offset;
    return coord.x > bounds.left && coord.x < bounds.right &&
            coord.y > bounds.bottom && coord.y < bounds.top;
}

bool HandleMouseClick(photon_instance &instance, int x, int y){
    float widthfac = instance.window.width / 2.0f;
    float heightfac = instance.window.height / 2.0f;

    glm::vec2 location((x - widthfac), (heightfac - y));
    location /= std::min(widthfac, heightfac);

    // TODO - check other gui states. (you know, the ones that don't exist yet...)
    if(instance.gui.load_save_menu.loading || instance.gui.load_save_menu.saving){
        if(InBounds(location, instance.gui.load_save_menu.confirm_button.bounds)){
            ConfirmLoadSave(instance);
        }
        if(InBounds(location, instance.gui.load_save_menu.cancel_button.bounds)){
            instance.gui.load_save_menu.loading = false;
            instance.gui.load_save_menu.saving  = false;
            SDL_StopTextInput();
        }
        if(InBounds(location, instance.gui.load_save_menu.file_list_bounds)){
            int index = ((instance.gui.load_save_menu.file_list_bounds.top - 0.1f) - location.y) / 0.08f;
            if(index >= 0 && index < instance.gui.load_save_menu.file_list.size()){
                instance.gui.load_save_menu.current_file_index = index;
                instance.gui.load_save_menu.filename = instance.gui.load_save_menu.file_list[index];
            }
        }
        return true;
    }else if(!instance.level.is_valid){
        ActivateButton(instance, instance.gui.main_menu, location);
        // main menu absorbs all clicks.
        return true;
    }else if(instance.paused){
        ActivateButton(instance, instance.gui.pause_menu, location);
        // pause menu absorbs all clicks.
        return true;
    }else{
        if(InBounds(location, instance.gui.game.pause_menu_button)){
            instance.paused = true;
            return true;
        }
        if(InBounds(location, instance.gui.game.toggle_fullscreen_button)){
            window_managment::ToggleFullscreen(instance.window);
            return true;
        }
    }

    return false;
}

void ConfirmLoadSave(photon_instance &instance){
    if(instance.gui.load_save_menu.loading && !instance.gui.load_save_menu.saving){
        // TODO - make a popup box with an unable to load message if it failed.
        instance.level = level::LoadLevelXML(instance.gui.load_save_menu.filename, instance.player);
    }else if(instance.gui.load_save_menu.saving && !instance.gui.load_save_menu.loading){
        // TODO - some GUI feedback of whether or not it actually saved.
        level::SaveLevelXML(instance.gui.load_save_menu.filename, instance.level, instance.player);
    }else{
        PrintToLog("WARNING: Either both loading and saving were enabled at the same time or something is very wrong...");
    }
    instance.gui.load_save_menu.loading = false;
    instance.gui.load_save_menu.saving  = false;
    SDL_StopTextInput();
}

void CancelLoadSave(photon_instance &instance){
    instance.gui.load_save_menu.loading = false;
    instance.gui.load_save_menu.saving  = false;
    SDL_StopTextInput();
}

void FillFileList(photon_gui_load_save_menu &gui){
    gui.file_list.clear();

    char** files = PHYSFS_enumerateFiles("/");

    for(char** file = files; *file != nullptr; file++){
        gui.file_list.push_back(*file);
    }
    PHYSFS_freeList(files);

    for(auto file = gui.file_list.begin(); file != gui.file_list.end();){
        if(file->length() > 4){
            std::string ext = file->substr(file->length() - 4);
            std::transform(ext.begin(), ext.end(), ext.begin(), tolower);
            if(ext.compare(".xml")){
                file = gui.file_list.erase(file);
            }else{
                ++file;
            }
        }else{
            // filename is too short to have an extension.
            file = gui.file_list.erase(file);
        }
    }
}

void StartLoadingGUI(photon_instance &instance){
    instance.gui.load_save_menu.loading = true;
    instance.gui.load_save_menu.saving  = false;

    FillFileList(instance.gui.load_save_menu);

    SDL_StartTextInput();
}

void StartSavingGUI(photon_instance &instance){
    instance.gui.load_save_menu.loading = false;
    instance.gui.load_save_menu.saving  = true;

    FillFileList(instance.gui.load_save_menu);

    SDL_StartTextInput();
}

}

}
