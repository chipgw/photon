#include "photon_window_managment.h"
#include "photon_core.h"
#include "photon_opengl.h"
#include "photon_input.h"

namespace photon{

namespace input{
void DoInputSingle(input_state &state){
    state.last_state = state.current_state;
    switch(state.type){
    case keyboard:{
        Uint8* keyboard = SDL_GetKeyboardState(nullptr);
        SDL_Keymod modifiers = SDL_GetModState();
        state.current_state = keyboard[state.key] && (modifiers == state.modifiers);
        break;
    }
    case mouse_button:

        break;
    case mouse_position:
        break;
    case joystick_axis:{
        if(state.joystick_input_index > -1){
            state.current_state = SDL_JoystickGetAxis(state.joystick, state.joystick_input_index) / 32768.0f;
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
    case joystick_button:{
        state.current_state = SDL_JoystickGetButton(state.joystick, state.joystick_input_index);
        break;
    }
    case gamecontroller_axis:{
        state.current_state = SDL_GameControllerGetAxis(state.controller, state.controller_axis) / 32768.0f;
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
    case gamecontroller_button:{
        state.current_state = SDL_GameControllerGetButton(state.controller, state.controller_button);
        break;
    }
    case none:
        break;
    }
}

void DoInput(photon_instance &instance, float time){
    if(false){ // TODO - make it check if GUI is active.
        gui_input &gui = instance.input_gui;
        DoInputSingle(gui.left);
        DoInputSingle(gui.right);
        DoInputSingle(gui.up);
        DoInputSingle(gui.down);
        DoInputSingle(gui.select);
        DoInputSingle(gui.back);

        // TODO - code goes here...
    }else{
        game_input &game = instance.input_game;
        DoInputSingle(game.interact);
        DoInputSingle(game.move_positive_x);
        DoInputSingle(game.move_negative_x);
        DoInputSingle(game.move_positive_y);
        DoInputSingle(game.move_negative_y);
        DoInputSingle(game.rotate_clockwise);
        DoInputSingle(game.rotate_counter_clockwise);
        DoInputSingle(game.zoom_in);
        DoInputSingle(game.zoom_out);

        instance.player.location.x += (game.move_positive_x.current_state - game.move_negative_x.current_state) * time * instance.zoom;
        instance.player.location.y += (game.move_positive_y.current_state - game.move_negative_y.current_state) * time * instance.zoom;

        if(game.interact.current_state > 0.9f && game.interact.last_state < 0.9f){
            blocks::OnPhotonInteract(glm::uvec2(instance.player.location + glm::vec2(0.5f)), instance.level);
        }
        if(game.rotate_clockwise.current_state > 0.9f && game.rotate_clockwise.last_state < 0.9f){
            blocks::OnRotate(glm::uvec2(instance.player.location + glm::vec2(0.5f)), instance.level);
        }
        if(game.rotate_counter_clockwise.current_state > 0.9f && game.rotate_counter_clockwise.last_state < 0.9f){
            blocks::OnRotate(glm::uvec2(instance.player.location + glm::vec2(0.5f)), instance.level, true);
        }

        instance.zoom -= (game.zoom_in.current_state - game.zoom_out.current_state) * time;
    }
}

void DoEvents(photon_instance &instance, float time){
    SDL_Event event;
    while(SDL_PollEvent(&event)){
        switch (event.type) {
        case SDL_KEYDOWN:
            if(event.key.keysym.sym == SDLK_f && event.key.keysym.mod & KMOD_CTRL){
                window_managment::ToggleFullscreen(instance.window);
            }else if(event.key.keysym.sym == SDLK_ESCAPE){
                Close(instance);
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
            instance.zoom -= event.wheel.y * 0.1f;
            break;
        }
    }
}


input_state CreateControllerAxisInput(SDL_GameController *controller, SDL_GameControllerAxis axis, bool negate){
    input_state state;

    state.type = gamecontroller_axis;
    state.controller = controller;
    state.controller_axis = axis;
    state.axis_input_negate = negate;

    return state;
}

input_state CreateControllerButtonInput(SDL_GameController *controller, SDL_GameControllerButton button){
    input_state state;

    state.type = gamecontroller_button;
    state.controller = controller;
    state.controller_button = button;

    return state;
}

input_state CreateKeyboardInput(SDL_Scancode key){
    input_state state;

    state.type = keyboard;
    state.key = key;

    return state;
}

input_state CreateJoystickAxisInput(SDL_Joystick *joystick, int axis, bool negate){
    input_state state;

    state.type = joystick_axis;
    state.joystick = joystick;
    state.joystick_input_index = axis;
    state.axis_input_negate = negate;

    return state;
}

input_state CreateJoystickButtonInput(SDL_Joystick *joystick, int button){
    input_state state;

    state.type = joystick_button;
    state.joystick = joystick;
    state.joystick_input_index = button;

    return state;
}
}

}
