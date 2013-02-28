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
        if(state.joystick_input_index > 0){
            int index = state.joystick_input_index - 1;
            state.current_state = SDL_JoystickGetAxis(state.joystick, index) / 32768.0f;
            if(state.current_state < 0.0f){
                state.current_state = 0.0f;
            }
        }else if(state.joystick_input_index < 0){
            int index = abs(state.joystick_input_index) - 1;
            state.current_state = -SDL_JoystickGetAxis(state.joystick, index) / 32768.0f;
            if(state.current_state < 0.0f){
                state.current_state = 0.0f;
            }
        }
        // deadzone. TODO - make setting.
        if(fabs(state.current_state) < 0.16f){
            state.current_state = 0.0f;
        }
        break;
    }
    case joystick_button:{
        state.current_state = SDL_JoystickGetButton(state.joystick, state.joystick_input_index);
        break;
    }
    case none:
        break;
    }
}

void DoInput(photon_instance &instance, float time){
    DoInputSingle(instance.input_set.interact);
    DoInputSingle(instance.input_set.move_positive_x);
    DoInputSingle(instance.input_set.move_negative_x);
    DoInputSingle(instance.input_set.move_positive_y);
    DoInputSingle(instance.input_set.move_negative_y);
    DoInputSingle(instance.input_set.rotate_clockwise);
    DoInputSingle(instance.input_set.rotate_counter_clockwise);

    instance.player.location.x += (instance.input_set.move_positive_x.current_state - instance.input_set.move_negative_x.current_state) * time * 1.0e-3 * instance.zoom;
    instance.player.location.y += (instance.input_set.move_positive_y.current_state - instance.input_set.move_negative_y.current_state) * time * 1.0e-3 * instance.zoom;

    if(instance.input_set.interact.current_state > 0.9f && instance.input_set.interact.last_state < 0.9f){
        blocks::OnPhotonInteract(glm::uvec2(instance.player.location + glm::vec2(0.5f)), instance.level);
    }
    if(instance.input_set.rotate_clockwise.current_state > 0.9f && instance.input_set.rotate_clockwise.last_state < 0.9f){
        blocks::OnRotate(glm::uvec2(instance.player.location + glm::vec2(0.5f)), instance.level);
    }
    if(instance.input_set.rotate_counter_clockwise.current_state > 0.9f && instance.input_set.rotate_counter_clockwise.last_state < 0.9f){
        blocks::OnRotate(glm::uvec2(instance.player.location + glm::vec2(0.5f)), instance.level, true);
    }
}

void DoEvents(photon_instance &instance, float time){
    SDL_Event event;
    while(SDL_PollEvent(&event)){
        switch (event.type) {
        case SDL_KEYDOWN:
            PrintToLog("INFO: The %s key was pressed!", SDL_GetScancodeName(event.key.keysym.scancode));
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
            instance.zoom = std::max(0.01f,instance.zoom);
            opengl::UpdateZoom(instance.zoom);
            break;
        }
    }
}
}

}
