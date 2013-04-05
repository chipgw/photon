#include <physfs.h>
#include <libxml/parser.h>
#include "photon_window_managment.h"
#include "photon_core.h"
#include "photon_opengl.h"
#include "photon_input.h"

namespace photon{

namespace input{

void DoInputSingle(photon_input_state &state, photon_input &input){
    state.last_state = state.current_state;
    switch(state.type){
    case photon_input_state::keyboard:{
        Uint8* keyboard = SDL_GetKeyboardState(nullptr);
        SDL_Keymod modifiers = SDL_GetModState();
        state.current_state = keyboard[state.key] && (modifiers == state.modifiers || modifiers & state.modifiers);
        break;
    }
    case photon_input_state::joystick_axis:{
        if(state.joystick_input_index > -1){
            state.current_state = SDL_JoystickGetAxis(input.joystick, state.joystick_input_index) / 32768.0f;
            if(state.axis_input_negate){
                state.current_state = -state.current_state;
            }
        }
        // deadzone. TODO - make setting.
        if(fabs(state.current_state) < 0.16f){
            state.current_state = 0.0f;
        }
        if(state.current_state < 0.0f){
            state.current_state = 0.0f;
        }
        break;
    }
    case photon_input_state::joystick_button:{
        state.current_state = SDL_JoystickGetButton(input.joystick, state.joystick_input_index);
        break;
    }
    case photon_input_state::gamecontroller_axis:{
        state.current_state = SDL_GameControllerGetAxis(input.controller, state.controller_axis) / 32768.0f;
        if(state.axis_input_negate){
            state.current_state = -state.current_state;
        }
        // deadzone. TODO - make setting.
        if(fabs(state.current_state) < 0.16f){
            state.current_state = 0.0f;
        }
        if(state.current_state < 0.0f){
            state.current_state = 0.0f;
        }
        break;
    }
    case photon_input_state::gamecontroller_button:{
        state.current_state = SDL_GameControllerGetButton(input.controller, state.controller_button);
        break;
    }
    case photon_input_state::none:
        break;
    }
}

bool IsActivated(photon_input_state &state){
    return state.current_state > 0.9f && state.last_state < 0.9f;
}

void DoInput(photon_instance &instance, float time){
    photon_input &input = instance.input;

    DoInputSingle(input.left, input);
    DoInputSingle(input.right, input);
    DoInputSingle(input.up, input);
    DoInputSingle(input.down, input);
    DoInputSingle(input.select, input);
    DoInputSingle(input.back, input);

    DoInputSingle(input.pause, input);

    if(instance.gui.load_save_menu.loading || instance.gui.load_save_menu.saving){
        photon_gui_load_save_menu &load_save_menu = instance.gui.load_save_menu;
        if(IsActivated(input.select)){
            gui::ConfirmLoadSave(instance);
        }
        if(IsActivated(input.back)){
            gui::CancelLoadSave(instance);
        }
        if(IsActivated(input.left)){
            if(--load_save_menu.cursor < 0){
                load_save_menu.cursor = 0;
            }
        }
        if(IsActivated(input.right)){
            if(++load_save_menu.cursor > load_save_menu.filename.length()){
                load_save_menu.cursor = load_save_menu.filename.length();
            }
        }
        if(IsActivated(input.up)){
            if(--load_save_menu.current_file_index < 0){
                load_save_menu.current_file_index = 0;
            }
            load_save_menu.filename = load_save_menu.file_list[load_save_menu.current_file_index];
        }
        if(IsActivated(input.down)){
            if(++load_save_menu.current_file_index >= load_save_menu.file_list.size()){
                load_save_menu.current_file_index = load_save_menu.file_list.size() - 1;
            }
            load_save_menu.filename = load_save_menu.file_list[load_save_menu.current_file_index];
        }

    }else if(!instance.level.is_valid){
        photon_gui_main_menu &menu = instance.gui.main_menu;
        if(IsActivated(input.select)){
            gui::ActivateButtonMainMenu(instance, menu.highlighted);
        }
        if(IsActivated(input.up)){
            if(--menu.highlighted < 0){
                menu.highlighted = 0;
            }
        }
        if(IsActivated(input.down)){
            if(++menu.highlighted >= menu.buttons.size()){
                menu.highlighted = menu.buttons.size() - 1;
            }
        }
    }else if(instance.paused){
        photon_gui_pause_menu &menu = instance.gui.pause_menu;
        if(IsActivated(input.select)){
            gui::ActivateButtonPauseMenu(instance, menu.highlighted);
        }
        if(IsActivated(input.back) || IsActivated(input.pause)){
            instance.paused = false;
        }
        if(IsActivated(input.up)){
            if(--menu.highlighted < 0){
                menu.highlighted = 0;
            }
        }
        if(IsActivated(input.down)){
            if(++menu.highlighted >= menu.buttons.size()){
                menu.highlighted = menu.buttons.size() - 1;
            }
        }
    }else{
        DoInputSingle(input.interact, input);
        DoInputSingle(input.move_positive_x, input);
        DoInputSingle(input.move_negative_x, input);
        DoInputSingle(input.move_positive_y, input);
        DoInputSingle(input.move_negative_y, input);
        DoInputSingle(input.rotate_clockwise, input);
        DoInputSingle(input.rotate_counter_clockwise, input);
        DoInputSingle(input.zoom_in, input);
        DoInputSingle(input.zoom_out, input);
        DoInputSingle(input.next_item, input);
        DoInputSingle(input.previous_item, input);

        instance.player.location.x += (input.move_positive_x.current_state - input.move_negative_x.current_state) * time * instance.zoom;
        instance.player.location.y += (input.move_positive_y.current_state - input.move_negative_y.current_state) * time * instance.zoom;

        if(IsActivated(input.interact)){
            blocks::OnPhotonInteract(glm::uvec2(instance.player.location + glm::vec2(0.5f)), instance.level, instance.player);
        }
        if(IsActivated(input.rotate_clockwise)){
            blocks::OnRotate(glm::uvec2(instance.player.location + glm::vec2(0.5f)), instance.level);
        }
        if(IsActivated(input.rotate_counter_clockwise)){
            blocks::OnRotate(glm::uvec2(instance.player.location + glm::vec2(0.5f)), instance.level, true);
        }
        if(IsActivated(input.next_item)){
            player::NextItem(instance.player);
        }
        if(IsActivated(input.previous_item)){
            player::PreviousItem(instance.player);
        }

        instance.zoom -= (input.zoom_in.current_state - input.zoom_out.current_state) * instance.zoom * 0.5f * time;

        if(IsActivated(input.pause)){
            instance.paused = true;
        }
    }
}

void DoEvents(photon_instance &instance){
    SDL_Event event;
    while(SDL_PollEvent(&event)){
        switch (event.type) {
        case SDL_TEXTINPUT:
            if(instance.gui.load_save_menu.loading || instance.gui.load_save_menu.saving){

                instance.gui.load_save_menu.filename.insert(instance.gui.load_save_menu.cursor, event.text.text);

                for(char* c = event.text.text; *c != '\0'; c++){
                    instance.gui.load_save_menu.cursor++;
                }
                instance.gui.load_save_menu.current_file_index = -1;
            }else{
                // if we don't need text input anymore...
                SDL_StopTextInput();
            }
            break;
        case SDL_TEXTEDITING:
            if(instance.gui.load_save_menu.loading || instance.gui.load_save_menu.saving){
                instance.gui.load_save_menu.filename.insert(event.edit.start, event.edit.text);
            }else{
                // if we don't need text input anymore...
                SDL_StopTextInput();
            }
            break;
        case SDL_KEYDOWN:
            if(!instance.input.is_valid){
                input::LoadConfig("/config/keyboard.xml", instance.input);

                PrintToLog("INFO: Using keyboard input.");
            }
            if(event.key.keysym.sym == SDLK_f && event.key.keysym.mod & KMOD_CTRL){
                window_managment::ToggleFullscreen(instance.window);
            }else if((instance.gui.load_save_menu.loading || instance.gui.load_save_menu.saving)){
                photon_gui_load_save_menu &load_save_menu = instance.gui.load_save_menu;
                if(event.key.keysym.sym == SDLK_BACKSPACE){
                    if(load_save_menu.cursor > 0 && load_save_menu.cursor <= load_save_menu.filename.length()){
                        load_save_menu.filename.erase(--load_save_menu.cursor, 1);
                        instance.gui.load_save_menu.current_file_index = -1;
                    }
                }else if(event.key.keysym.sym == SDLK_DELETE){
                    if(load_save_menu.cursor >= 0 && load_save_menu.cursor < load_save_menu.filename.length()){
                        load_save_menu.filename.erase(load_save_menu.cursor, 1);
                        instance.gui.load_save_menu.current_file_index = -1;
                    }
                }else if(event.key.keysym.sym == SDLK_LEFT && instance.input.left.type != photon_input_state::keyboard){
                    if(--load_save_menu.cursor < 0){
                        load_save_menu.cursor = 0;
                    }
                }else if(event.key.keysym.sym == SDLK_RIGHT && instance.input.right.type != photon_input_state::keyboard){
                    if(++load_save_menu.cursor > load_save_menu.filename.length()){
                        load_save_menu.cursor = load_save_menu.filename.length();
                    }
                }else if(event.key.keysym.sym == SDLK_ESCAPE){
                    gui::CancelLoadSave(instance);
                }
            }else if(event.key.keysym.sym == SDLK_ESCAPE){
                instance.paused = !instance.paused;
            }else if(event.key.keysym.sym == SDLK_s && event.key.keysym.mod & KMOD_CTRL){
                instance.paused = true;
                gui::StartSavingGUI(instance.gui.load_save_menu);
            }
            break;
        case SDL_QUIT:
            Close(instance);
            break;
        case SDL_WINDOWEVENT:
            switch(event.window.event){
            case SDL_WINDOWEVENT_RESIZED:
//            case SDL_WINDOWEVENT_SIZE_CHANGED:
                if(event.window.data1 < 1){
                    // prevent window dissapearing if some weirdo decides to resize the window to a width of 0...
                    SDL_SetWindowSize(instance.window.window_SDL, 1, event.window.data2);
                    break;
                }
                if(event.window.data2 < 1){
                    // prevent window dissapearing if some weirdo decides to resize the window to a height of 0...
                    SDL_SetWindowSize(instance.window.window_SDL, event.window.data1, 1);
                    break;
                }
                opengl::OnResize(event.window.data1,event.window.data2, instance.window);
                break;
            }
            break;
        case SDL_MOUSEWHEEL:
            if(!instance.paused){
                instance.zoom -= event.wheel.y * instance.zoom * 0.02f;
            }
            break;
        case SDL_MOUSEBUTTONUP:
            if(event.button.button == SDL_BUTTON_LEFT){
                gui::HandleMouseClick(instance, event.button.x, event.button.y);
            }
            break;
        case SDL_CONTROLLERBUTTONUP:
            if(!instance.input.is_valid){
                SDL_GameController *controller = SDL_GameControllerOpen(event.cbutton.which);

                if(controller != nullptr){
                    input::LoadConfig("/config/controller.xml", instance.input);
                    instance.input.controller = controller;

                    PrintToLog("INFO: Using Game Controller (device index %i)", event.cbutton.which);
                }
            }
            break;
        case SDL_CONTROLLERAXISMOTION:
            if(!instance.input.is_valid){
                SDL_GameController *controller = SDL_GameControllerOpen(event.caxis.which);

                if(controller != nullptr){
                    input::LoadConfig("/config/controller.xml", instance.input);
                    instance.input.controller = controller;

                    PrintToLog("INFO: Using Game Controller (device index %i)", event.caxis.which);
                }
            }
            break;
        }
    }
}

photon_input_state CreateControllerAxisInput(SDL_GameControllerAxis axis, bool negate){
    photon_input_state state;

    state.type = photon_input_state::gamecontroller_axis;
    state.controller_axis = axis;
    state.axis_input_negate = negate;

    return state;
}

photon_input_state CreateControllerButtonInput(SDL_GameControllerButton button){
    photon_input_state state;

    state.type = photon_input_state::gamecontroller_button;
    state.controller_button = button;

    return state;
}

photon_input_state CreateKeyboardInput(SDL_Scancode key){
    photon_input_state state;

    state.type = photon_input_state::keyboard;
    state.key = key;

    return state;
}

photon_input_state CreateJoystickAxisInput(int axis, bool negate){
    photon_input_state state;

    state.type = photon_input_state::joystick_axis;
    state.joystick_input_index = axis;
    state.axis_input_negate = negate;

    return state;
}

photon_input_state CreateJoystickButtonInput(int button){
    photon_input_state state;

    state.type = photon_input_state::joystick_button;
    state.joystick_input_index = button;

    return state;
}

const xmlChar* operator "" _xml(const char* str, size_t /*length*/){
    return (const xmlChar*)str;
}

photon_input_state LoadInputSingle(xmlNode *node){
    photon_input_state state;
    xmlChar *type_str = xmlGetProp(node, "type"_xml);

    if((xmlStrEqual(type_str, "keyboard"_xml))){
        state.type = photon_input_state::keyboard;

        xmlChar *key_str = xmlGetProp(node, "key"_xml);
        xmlChar *mod_str = xmlGetProp(node, "modifiers"_xml);

        state.key = SDL_GetScancodeFromName((char*)key_str);

        if(xmlStrstr(mod_str, "ctrl"_xml)){
            state.modifiers |= KMOD_CTRL;
        }else if(xmlStrstr(mod_str, "shift"_xml)){
            state.modifiers |= KMOD_SHIFT;
        }else if(xmlStrstr(mod_str, "alt"_xml)){
            state.modifiers |= KMOD_ALT;
        }

        xmlFree(key_str);
        xmlFree(mod_str);
    }else if((xmlStrEqual(type_str, "joystick_button"_xml))){
        state.type = photon_input_state::joystick_button;

        xmlChar *index_str = xmlGetProp(node, "index"_xml);
        state.joystick_input_index = atoi((char*)index_str);

        xmlFree(index_str);
    }else if((xmlStrEqual(type_str, "joystick_axis"_xml))){
        state.type = photon_input_state::joystick_axis;

        xmlChar *index_str = xmlGetProp(node, "index"_xml);
        xmlChar *negate_str = xmlGetProp(node, "negate"_xml);
        state.joystick_input_index = atoi((char*)index_str);
        state.axis_input_negate = xmlStrEqual(negate_str, "true"_xml);

        xmlFree(negate_str);
        xmlFree(index_str);
    }else if((xmlStrEqual(type_str, "controller_button"_xml))){
        state.type = photon_input_state::gamecontroller_button;

        xmlChar *index_str = xmlGetProp(node, "button"_xml);
        state.controller_button = SDL_GameControllerGetButtonFromString((char*)index_str);

        xmlFree(index_str);
    }else if((xmlStrEqual(type_str, "controller_axis"_xml))){
        state.type = photon_input_state::gamecontroller_axis;

        xmlChar *index_str = xmlGetProp(node, "axis"_xml);
        xmlChar *negate_str = xmlGetProp(node, "negate"_xml);
        state.controller_axis = SDL_GameControllerGetAxisFromString((char*)index_str);
        state.axis_input_negate = xmlStrEqual(negate_str, "true"_xml);

        xmlFree(negate_str);
        xmlFree(index_str);
    }
    return state;
}

bool LoadConfig(const std::string &filename, photon_input &input){
    if(PHYSFS_exists(filename.c_str())){
        PHYSFS_File *file;
        long length;
        char *xml_buffer;

        file = PHYSFS_openRead(filename.c_str());
        if(!file){
            PrintToLog("ERROR: unable to open XML input config file \"%s\"", filename.c_str());
            return false;
        }

        length = PHYSFS_fileLength(file);
        xml_buffer = (char*)malloc(length);

        PHYSFS_read(file, xml_buffer, 1, length);
        PHYSFS_close(file);

        xmlDocPtr doc = xmlParseMemory(xml_buffer, length);

        if(doc == nullptr) {
            PrintToLog("ERROR: Unable to load XML input config: Document not parsed successfully!");
            return false;
        }

        xmlNodePtr root = xmlDocGetRootElement(doc);
        if(root == nullptr) {
            PrintToLog("ERROR: Unable to load XML input config: empty document!");
            xmlFreeDoc(doc);
            return false;
        }
        if(xmlStrcmp(root->name, "photon_input"_xml)) {
            PrintToLog("ERROR: Unable to load XML input config: root node not photon_input!");
            xmlFreeDoc(doc);
            return false;
        }
        input.is_valid = false;

        xmlChar *device_str = xmlGetProp(root, "device"_xml);
        xmlChar *guid_str = xmlGetProp(root, "device_guid"_xml);

        xmlFree(device_str);
        xmlFree(guid_str);

        xmlNode *node = root->xmlChildrenNode;

        while(node != nullptr) {
            if((xmlStrEqual(node->name, "interact"_xml))){
                input.interact = LoadInputSingle(node);
            }else if((xmlStrEqual(node->name, "next_item"_xml))){
                input.next_item = LoadInputSingle(node);
            }else if((xmlStrEqual(node->name, "previous_item"_xml))){
                input.previous_item = LoadInputSingle(node);
            }else if((xmlStrEqual(node->name, "open_inventory"_xml))){
                input.open_inventory = LoadInputSingle(node);
            }else if((xmlStrEqual(node->name, "rotate_clockwise"_xml))){
                input.rotate_clockwise = LoadInputSingle(node);
            }else if((xmlStrEqual(node->name, "rotate_counter_clockwise"_xml))){
                input.rotate_counter_clockwise = LoadInputSingle(node);
            }else if((xmlStrEqual(node->name, "zoom_in"_xml))){
                input.zoom_in = LoadInputSingle(node);
            }else if((xmlStrEqual(node->name, "zoom_out"_xml))){
                input.zoom_out = LoadInputSingle(node);
            }else if((xmlStrEqual(node->name, "move_right"_xml))){
                input.move_positive_x = LoadInputSingle(node);
            }else if((xmlStrEqual(node->name, "move_left"_xml))){
                input.move_negative_x = LoadInputSingle(node);
            }else if((xmlStrEqual(node->name, "move_up"_xml))){
                input.move_positive_y = LoadInputSingle(node);
            }else if((xmlStrEqual(node->name, "move_down"_xml))){
                input.move_negative_y = LoadInputSingle(node);
            }else if((xmlStrEqual(node->name, "pause"_xml))){
                input.pause = LoadInputSingle(node);
            }else if((xmlStrEqual(node->name, "gui_up"_xml))){
                input.up = LoadInputSingle(node);
            }else if((xmlStrEqual(node->name, "gui_down"_xml))){
                input.down = LoadInputSingle(node);
            }else if((xmlStrEqual(node->name, "gui_left"_xml))){
                input.left = LoadInputSingle(node);
            }else if((xmlStrEqual(node->name, "gui_right"_xml))){
                input.right = LoadInputSingle(node);
            }else if((xmlStrEqual(node->name, "gui_select"_xml))){
                input.select = LoadInputSingle(node);
            }else if((xmlStrEqual(node->name, "gui_back"_xml))){
                input.back = LoadInputSingle(node);
            }
            node = node->next;
        }

        input.is_valid = true;

        free(xml_buffer);
        xmlFreeDoc(doc);

        return true;
    }else{
        PrintToLog("ERROR: Unable to load XML input config: \"%s\" does not exist!", filename.c_str());
    }
    return false;
}

photon_input InitInput(){
    PrintToLog("INFO: Initializing Input System.");
    photon_input input;

    SDL_GameControllerEventState(SDL_ENABLE);
    SDL_JoystickEventState(SDL_ENABLE);

    for(int i = 0; i < SDL_NumJoysticks();i++){
        if(SDL_IsGameController(i)){
            input.open_controllers.push_back(SDL_GameControllerOpen(i));
        }else{
            input.open_joysticks.push_back(SDL_JoystickOpen(i));
        }
    }
    PrintToLog("INFO: Listening for input, press a button on the device you want to use.");

    return input;
}

void GarbageCollect(photon_input &input){
    for(auto controller : input.open_controllers){
        if(controller == input.controller){
            input.controller = nullptr;
        }
        SDL_GameControllerClose(controller);
    }
    for(auto joystick : input.open_joysticks){
        if(joystick == input.joystick){
            input.joystick = nullptr;
        }
        SDL_JoystickClose(joystick);
    }
    PrintToLog("INFO: Input System garbage collection complete.");
}

}

}
