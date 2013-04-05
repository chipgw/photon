#include <physfs.h>
#include <algorithm>
#include "photon_gui.h"
#include "photon_level.h"
#include "photon_opengl.h"
#include "photon_texture.h"
#include "photon_core.h"

namespace photon{

namespace gui{

photon_gui_container InitGUI(){
    photon_gui_container gui;

    gui.game.bar_texture = texture::Load("/textures/gui/bar.png");
    gui.game.pause_menu_button_texture = texture::Load("/textures/gui/pause_button.png");
    gui.game.toggle_fullscreen_button_texture = texture::Load("/textures/gui/fullscreen_button.png");

    gui.text_button_texture = texture::Load("/textures/gui/button.png");

    gui.load_save_menu.file_list_background = texture::Load("/textures/gui/file_list.png");
    gui.load_save_menu.filename_box_background = texture::Load("/textures/gui/text_input_box.png");

    return gui;
}

void DrawBounds(const photon_gui_bounds &bounds){
    opengl::SetCenterGUI(bounds.offset);
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

void DrawGUI(photon_instance &instance, float time){
    static const photon_gui_bounds fill_bounds(-100.0f, 100.0f, -100.0f, 100.0f);

    if(!instance.level.is_valid){
        // TODO - draw background.
        photon_gui_main_menu &gui = instance.gui.main_menu;
        opengl::SetColorGUI(glm::vec4(0.0f));

        glBindTexture(GL_TEXTURE_2D, instance.gui.text_button_texture);
        DrawBounds(gui.play_button);
        DrawBounds(gui.load_button);
        DrawBounds(gui.exit_button);

        RenderText(glm::vec2((gui.play_button.right + gui.play_button.left) / 2.0f, gui.play_button.bottom + 0.1f), glm::vec2(0.15f), glm::vec4(0.9f), true, "Play");
        RenderText(glm::vec2((gui.load_button.right + gui.load_button.left) / 2.0f, gui.load_button.bottom + 0.1f), glm::vec2(0.15f), glm::vec4(0.9f), true, "Load");
        RenderText(glm::vec2((gui.exit_button.right + gui.exit_button.left) / 2.0f, gui.exit_button.bottom + 0.1f), glm::vec2(0.15f), glm::vec4(0.9f), true, "Exit");
    }else{
        photon_gui_game &gui = instance.gui.game;
        opengl::SetColorGUI(glm::vec4(0.0f));

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
                RenderText(glm::vec2(gui.current_item.left, gui.current_item.bottom), glm::vec2(0.05f), glm::vec4(1.0f), false, "infinite");
            }else{
                RenderText(glm::vec2(gui.current_item.left, gui.current_item.bottom), glm::vec2(0.05f), glm::vec4(1.0f), false, "%i", count);
            }
        }
        gui::RenderText(gui.moves_display_location, glm::vec2(0.05f), glm::vec4(0.8f), false, "Moves: %i", instance.level.moves);
        gui::RenderText(gui.time_display_location, glm::vec2(0.05f), glm::vec4(0.8f), false, "Time: %f", instance.level.time);
        gui::RenderText(gui.fps_display_location, glm::vec2(0.05f), glm::vec4(0.8f), false, "FPS: %f", 1.0f / time);

        if(instance.paused){
            photon_gui_pause_menu &gui = instance.gui.pause_menu;

            glBindTexture(GL_TEXTURE_2D, 0);
            opengl::SetColorGUI(glm::vec4(0.1f, 0.1f, 0.1f, -0.4f));
            DrawBounds(fill_bounds);

            opengl::SetColorGUI(glm::vec4(0.0f));

            glBindTexture(GL_TEXTURE_2D, instance.gui.text_button_texture);
            DrawBounds(gui.resume_button);
            DrawBounds(gui.load_button);
            DrawBounds(gui.save_button);
            DrawBounds(gui.main_menu_button);
            DrawBounds(gui.exit_button);

            RenderText(glm::vec2((gui.resume_button.right    + gui.resume_button.left)    / 2.0f, gui.resume_button.bottom     + 0.1f), glm::vec2(0.15f), glm::vec4(0.9f), true, "Resume");
            RenderText(glm::vec2((gui.load_button.right      + gui.load_button.left)      / 2.0f, gui.load_button.bottom       + 0.1f), glm::vec2(0.15f), glm::vec4(0.9f), true, "Load");
            RenderText(glm::vec2((gui.save_button.right      + gui.save_button.left)      / 2.0f, gui.save_button.bottom       + 0.1f), glm::vec2(0.15f), glm::vec4(0.9f), true, "Save");
            RenderText(glm::vec2((gui.main_menu_button.right + gui.main_menu_button.left) / 2.0f, gui.main_menu_button.bottom  + 0.1f), glm::vec2(0.15f), glm::vec4(0.9f), true, "Exit to Menu");
            RenderText(glm::vec2((gui.exit_button.right      + gui.exit_button.left)      / 2.0f, gui.exit_button.bottom       + 0.1f), glm::vec2(0.15f), glm::vec4(0.9f), true, "Exit");
        }
    }
    if(instance.gui.load_save_menu.loading || instance.gui.load_save_menu.saving){
        photon_gui_load_save_menu &gui = instance.gui.load_save_menu;

        glBindTexture(GL_TEXTURE_2D, 0);
        opengl::SetColorGUI(glm::vec4(0.1f, 0.1f, 0.1f, -0.4f));
        DrawBounds(fill_bounds);

        opengl::SetColorGUI(glm::vec4(0.0f));
        glBindTexture(GL_TEXTURE_2D, gui.file_list_background);
        DrawBounds(gui.file_list_bounds);

        glBindTexture(GL_TEXTURE_2D, gui.filename_box_background);
        DrawBounds(gui.filename_box);

        glBindTexture(GL_TEXTURE_2D, instance.gui.text_button_texture);
        DrawBounds(gui.cancel_button);
        DrawBounds(gui.confirm_button);

        RenderText(glm::vec2((gui.cancel_button.right  + gui.cancel_button.left)  / 2.0f, gui.cancel_button.bottom  + 0.05f), glm::vec2(0.05f), glm::vec4(0.9f), true, "Cancel");
        RenderText(glm::vec2((gui.confirm_button.right + gui.confirm_button.left) / 2.0f, gui.confirm_button.bottom + 0.05f), glm::vec2(0.05f), glm::vec4(0.9f), true, "Confirm");

        RenderText(glm::vec2(gui.filename_box.left + 0.025f, gui.filename_box.bottom + 0.025f), glm::vec2(0.05f), glm::vec4(0.9f), false, gui.filename.c_str());

        std::string underscore = gui.filename;
        underscore.replace(gui.cursor, 1, "_");
        underscore.erase(gui.cursor + 1);
        RenderText(glm::vec2(gui.filename_box.left + 0.025f, gui.filename_box.bottom + 0.025f), glm::vec2(0.05f), glm::vec4(0.9f), false, underscore.c_str());

        int i = 0;
        glm::vec2 location(gui.file_list_bounds.left + 0.1f, gui.file_list_bounds.top - 0.15f);
        for(auto file : gui.file_list){
            if(i == gui.current_file_index){
                RenderText(location, glm::vec2(0.05f), glm::vec4(0.9f), false, file.c_str());
            }else{
                RenderText(location, glm::vec2(0.05f), glm::vec4(0.9f, 0.8f, 0.4f, 0.0f), false, file.c_str());
            }
            location.y -= 0.08f;
            i++;
        }
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
        if(InBounds(location, instance.gui.load_save_menu.confirm_button)){
            ConfirmLoadSave(instance);
        }
        if(InBounds(location, instance.gui.load_save_menu.cancel_button)){
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
        if(InBounds(location, instance.gui.main_menu.play_button)){
            instance.level = level::LoadLevelXML("/level.xml", instance.player);
            instance.paused = false;
        }
        if(InBounds(location, instance.gui.main_menu.load_button)){
            StartLoadingGUI(instance.gui.load_save_menu);
        }
        if(InBounds(location, instance.gui.main_menu.exit_button)){
            // TODO - ask for confirmation.
            Close(instance);
        }
        // main menu absorbs all clicks.
        return true;
    }else if(instance.paused){
        if(InBounds(location, instance.gui.pause_menu.resume_button)){
            instance.paused = false;
        }
        if(InBounds(location, instance.gui.pause_menu.load_button)){
            StartLoadingGUI(instance.gui.load_save_menu);
        }
        if(InBounds(location, instance.gui.pause_menu.save_button)){
            StartSavingGUI(instance.gui.load_save_menu);
        }
        if(InBounds(location, instance.gui.pause_menu.main_menu_button)){
            // TODO - ask for confirmation.
            instance.level = photon_level();
        }
        if(InBounds(location, instance.gui.pause_menu.exit_button)){
            // TODO - ask for confirmation.
            Close(instance);
        }
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

void StartLoadingGUI(photon_gui_load_save_menu &gui){
    gui.loading = true;
    gui.saving  = false;

    FillFileList(gui);

    SDL_StartTextInput();
}

void StartSavingGUI(photon_gui_load_save_menu &gui){
    gui.loading = false;
    gui.saving  = true;

    FillFileList(gui);

    SDL_StartTextInput();
}

}

}
