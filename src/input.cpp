#include "photon_window_managment.h"
#include "photon_core.h"
#include "photon_opengl.h"

namespace photon{

namespace input{
void DoInput(photon_instance &instance, float time){

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
