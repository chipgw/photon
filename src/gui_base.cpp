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

    return gui;
}

void DrawBounds(photon_gui_bounds &bounds){
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

void DrawGameGUI(photon_instance &instance, float time){
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
        glBindTexture(GL_TEXTURE_2D, 0);

        photon_gui_bounds tmp(-100.0f, 100.0f, -100.0f, 100.0f);
        opengl::SetColorGUI(glm::vec4(0.1f, 0.1f, 0.1f, -0.4f));
        DrawBounds(tmp);

        photon_gui_pause_menu &gui = instance.gui.pause_menu;
        opengl::SetColorGUI(glm::vec4(0.0f));

        glBindTexture(GL_TEXTURE_2D, instance.gui.text_button_texture);
        DrawBounds(gui.resume_button);
        DrawBounds(gui.load_button);
        DrawBounds(gui.save_button);
        DrawBounds(gui.exit_button);

        RenderText(glm::vec2(0.0f, gui.resume_button.bottom + 0.15f), glm::vec2(0.15f), glm::vec4(0.9f), true, "Resume");
        RenderText(glm::vec2(0.0f, gui.load_button.bottom + 0.15f), glm::vec2(0.15f), glm::vec4(0.9f), true, "Load");
        RenderText(glm::vec2(0.0f, gui.save_button.bottom + 0.15f), glm::vec2(0.15f), glm::vec4(0.9f), true, "Save");
        RenderText(glm::vec2(0.0f, gui.exit_button.bottom + 0.15f), glm::vec2(0.15f), glm::vec4(0.9f), true, "Exit");
    }
}

bool InBounds(glm::vec2 coord, photon_gui_bounds &bounds){
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
    if(instance.paused){
        if(InBounds(location, instance.gui.pause_menu.resume_button)){
            instance.paused = false;
        }
        if(InBounds(location, instance.gui.pause_menu.load_button)){
            // TODO - ask for confirmation & file name or slot.
            instance.level = level::LoadLevelXML("/save.xml", instance.player);
        }
        if(InBounds(location, instance.gui.pause_menu.save_button)){
            // TODO - ask for confirmation & file name or slot.
            level::SaveLevelXML("/save.xml", instance.level, instance.player);
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

}

}
