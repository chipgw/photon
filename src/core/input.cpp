#include "photon_core.h"

namespace photon{

namespace input{

glm::vec2 WindowToWorldCoord(photon_instance &instance, int x, int y){
    float widthfac = instance.window.width / 2.0f;
    float heightfac = instance.window.height / 2.0f;

    glm::vec2 location((x - widthfac), (heightfac - y));
    location /= std::min(widthfac, heightfac);
    location *= instance.camera_offset.z;
    location.x += instance.player.location.x + instance.camera_offset.x;
    location.y += instance.player.location.y + instance.camera_offset.y;

    return location;
}

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
        if(state.current_state < input.deadzone){
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
        if(state.current_state < input.deadzone){
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

    DoInputSingle(input.interact, input);
    DoInputSingle(input.move_right, input);
    DoInputSingle(input.move_left, input);
    DoInputSingle(input.move_up, input);
    DoInputSingle(input.move_down, input);
    DoInputSingle(input.camera_right, input);
    DoInputSingle(input.camera_left, input);
    DoInputSingle(input.camera_up, input);
    DoInputSingle(input.camera_down, input);
    DoInputSingle(input.rotate_clockwise, input);
    DoInputSingle(input.rotate_counter_clockwise, input);
    DoInputSingle(input.zoom_in, input);
    DoInputSingle(input.zoom_out, input);
    DoInputSingle(input.next_item, input);
    DoInputSingle(input.previous_item, input);

    if(instance.gui.load_save_menu.loading || instance.gui.load_save_menu.saving){
        photon_gui_load_save_menu &load_save_menu = instance.gui.load_save_menu;
        if(IsActivated(input.select)){
            gui::ConfirmLoadSave(instance);
        }
        if(IsActivated(input.back)){
            gui::CancelLoadSave(instance);
        }
        if(IsActivated(input.left)){
            if(load_save_menu.cursor > 0){
                load_save_menu.cursor--;
            }
        }
        if(IsActivated(input.right)){
            if(++load_save_menu.cursor > load_save_menu.filename.length()){
                load_save_menu.cursor = load_save_menu.filename.length();
            }
        }
        if(IsActivated(input.up)){
            if(--load_save_menu.current_file_index < 0){
                load_save_menu.current_file_index = load_save_menu.file_list.size() - 1;
            }
            load_save_menu.filename = load_save_menu.file_list[load_save_menu.current_file_index];
        }
        if(IsActivated(input.down)){
            if(++load_save_menu.current_file_index >= (int)load_save_menu.file_list.size()){
                load_save_menu.current_file_index = 0;
            }
            load_save_menu.filename = load_save_menu.file_list[load_save_menu.current_file_index];
        }

    }else if(!instance.level.is_valid){
        photon_gui_button_list &menu = instance.gui.main_menu;
        if(IsActivated(input.select)){
            gui::ActivateButton(instance, menu, menu.highlighted);
        }
        if(IsActivated(input.up)){
            if(--menu.highlighted < 0){
                menu.highlighted = menu.buttons.size() - 1;
            }
        }
        if(IsActivated(input.down)){
            if(++menu.highlighted >= (int8_t)menu.buttons.size()){
                menu.highlighted = 0;
            }
        }
    }else if(instance.paused){
        photon_gui_button_list &menu = instance.gui.pause_menu;
        if(IsActivated(input.select)){
            gui::ActivateButton(instance, menu, menu.highlighted);
        }
        if(IsActivated(input.back) || IsActivated(input.pause)){
            instance.paused = false;
        }
        if(IsActivated(input.up)){
            if(--menu.highlighted < 0){
                menu.highlighted = menu.buttons.size() - 1;
            }
        }
        if(IsActivated(input.down)){
            if(++menu.highlighted >= (int8_t)menu.buttons.size()){
                menu.highlighted = 0;
            }
        }
    }else{
        instance.player.location.x += (input.move_right.current_state - input.move_left.current_state) * time * instance.camera_offset.z;
        instance.player.location.y += (input.move_up.current_state - input.move_down.current_state) * time * instance.camera_offset.z;

        glm::vec2 cam_offset(input.camera_right.current_state - input.camera_left.current_state,
                             input.camera_up.current_state - input.camera_down.current_state);

        // TODO - make a setting that enables/disables screen edges.
        // if only SDL_GetMouseState worked when the cursor is outside the window... oh well...
        if(SDL_GetMouseFocus() == instance.window.window_SDL){
            glm::ivec2 mouse;
            uint32_t buttons = SDL_GetMouseState(&mouse.x, &mouse.y);

            // if the mouse position is (0,0) chances are it hasn't recieved the first event yet... I should try to find a better way to fix this, but this will do for now...
            if(instance.settings.screen_edges && !(mouse == glm::ivec2(0))){
                cam_offset.x += (std::max(80 + mouse.x - int(instance.window.width),  0) - std::max(80 - mouse.x, 0)) / 80.0f;
                cam_offset.y += (std::max(80 - mouse.y, 0) - std::max(80 + mouse.y - int(instance.window.height), 0)) / 80.0f;
            }

            if(input.enable_mouse){
                glm::vec2 delta = WindowToWorldCoord(instance, mouse.x, mouse.y) - instance.player.location;
                if(buttons & SDL_BUTTON_LMASK){
                    if(glm::length(delta) > 0.5f){
                        instance.player.location += delta * time;
                    }
                }
                if(buttons & SDL_BUTTON_RMASK){
                    float angle = glm::degrees(glm::atan(delta.y, delta.x));

                    blocks::OnRotate(glm::uvec2(instance.player.location + 0.5f), instance.level, angle);
                }
            }
        }

        cam_offset *= instance.camera_offset.z * 0.5f * time * 2.0f;

        instance.camera_offset.x += cam_offset.x;
        instance.camera_offset.y += cam_offset.y;
        instance.camera_offset.x /= 1.0f + (time * 2.0f);
        instance.camera_offset.y /= 1.0f + (time * 2.0f);

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

        instance.camera_offset.z -= (input.zoom_in.current_state - input.zoom_out.current_state) * instance.camera_offset.z * 0.5f * time;

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
            }else if(event.key.keysym.sym == SDLK_i && event.key.keysym.mod & KMOD_CTRL){
                // re-detect input. by doing a garbage collect and an init over again it will detect newly connected controllers.
                PrintToLog("INFO: Reinitilizing input system to redetect available devices...");
                GarbageCollect(instance.input);
                instance.input = InitInput();
            }else if((instance.gui.load_save_menu.loading || instance.gui.load_save_menu.saving)){
                photon_gui_load_save_menu &load_save_menu = instance.gui.load_save_menu;
                if(event.key.keysym.sym == SDLK_BACKSPACE){
                    if(load_save_menu.cursor > 0 && load_save_menu.cursor <= load_save_menu.filename.length()){
                        load_save_menu.filename.erase(--load_save_menu.cursor, 1);
                        load_save_menu.current_file_index = -1;
                    }
                }else if(event.key.keysym.sym == SDLK_DELETE){
                    if(load_save_menu.cursor >= 0 && load_save_menu.cursor < load_save_menu.filename.length()){
                        load_save_menu.filename.erase(load_save_menu.cursor, 1);
                        load_save_menu.current_file_index = -1;
                    }
                }else if(event.key.keysym.sym == SDLK_LEFT && instance.input.left.type != photon_input_state::keyboard){
                    if(--load_save_menu.cursor < 0){
                        load_save_menu.cursor = 0;
                    }
                }else if(event.key.keysym.sym == SDLK_RIGHT && instance.input.right.type != photon_input_state::keyboard){
                    if(++load_save_menu.cursor > load_save_menu.filename.length()){
                        load_save_menu.cursor = load_save_menu.filename.length();
                    }
                }else if(event.key.keysym.sym == SDLK_HOME){
                    if(load_save_menu.current_file_index == -1){
                        load_save_menu.cursor = 0;
                    }else{
                        load_save_menu.current_file_index = 0;
                        load_save_menu.filename = load_save_menu.file_list[load_save_menu.current_file_index];
                    }
                }else if(event.key.keysym.sym == SDLK_END){
                    if(load_save_menu.current_file_index == -1){
                        load_save_menu.cursor = load_save_menu.filename.length();
                    }else{
                        load_save_menu.current_file_index = load_save_menu.file_list.size() - 1;
                        load_save_menu.filename = load_save_menu.file_list[load_save_menu.current_file_index];
                    }
                }else if(event.key.keysym.sym == SDLK_ESCAPE){
                    gui::CancelLoadSave(instance);
                }
            }else if(event.key.keysym.sym == SDLK_ESCAPE){
                instance.paused = !instance.paused;
            }else if(event.key.keysym.sym == SDLK_s && event.key.keysym.mod & KMOD_CTRL){
                instance.paused = true;
                gui::StartSavingGUI(instance);
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
                instance.camera_offset.z -= event.wheel.y * instance.camera_offset.z * 0.02f;
            }
            break;
        case SDL_MOUSEBUTTONUP:
            if(!instance.input.is_valid){
                input::LoadConfig("/config/keyboard.xml", instance.input);

                PrintToLog("INFO: Using mouse & keyboard input.");
            }else if(event.button.button == SDL_BUTTON_LEFT){
                if(!gui::HandleMouseClick(instance, event.button.x, event.button.y)){
                    if(glm::length(instance.player.location - WindowToWorldCoord(instance, event.button.x, event.button.y)) < 0.5f){
                        blocks::OnPhotonInteract(glm::uvec2(instance.player.location + 0.5f), instance.level, instance.player);
                    }
                }
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

photon_input InitInput(){
    PrintToLog("INFO: Initializing Input System.");
    photon_input input;

    SDL_GameControllerEventState(SDL_ENABLE);
    SDL_JoystickEventState(SDL_ENABLE);

    for(int i = 0; i < SDL_NumJoysticks(); i++){
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
