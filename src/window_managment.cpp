#include "photon_core.h"
#include "photon_window_managment.h"
#include "photon_opengl.h"
#include <glm/gtx/vector_angle.hpp>

#include <SDL_opengl.h>
#include <SDL_gamecontroller.h>

namespace photon{

namespace window_managment{

photon_window CreateSDLWindow(){
    PrintToLog("INFO: Initializing SDL.");
    photon_window window;

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);

    if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK | SDL_INIT_GAMECONTROLLER) == -1) {
        PrintToLog("ERROR: Unable to init SDL! \"%s\"", SDL_GetError());
        throw;
    }

    SDL_GL_SetAttribute(SDL_GL_RED_SIZE,            8);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE,          8);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE,           8);
    SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE,          8);

    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE,          16);
    SDL_GL_SetAttribute(SDL_GL_BUFFER_SIZE,         32);

    // TODO - load a setting for this.
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS,  1);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES,  16);

    window.window_SDL = SDL_CreateWindow("Photon", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600,
                                         SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE | SDL_WINDOW_MAXIMIZED);

    if (!window.window_SDL){
        throw;
    }

    window.context_SDL = SDL_GL_CreateContext(window.window_SDL);

    // Don't cap framerate on debug builds.
#ifdef NDEBUG
    SDL_GL_SetSwapInterval(1);
#else
    SDL_GL_SetSwapInterval(0);
#endif

    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

    SDL_ShowCursor(SDL_DISABLE);

    return window;
}

void UpdateWindow(photon_window &window){
    SDL_GL_SwapWindow(window.window_SDL);
}

void GarbageCollect(photon_window &window, bool quitSDL){
    SDL_GL_DeleteContext(window.context_SDL);
    SDL_DestroyWindow(window.window_SDL);

    if(quitSDL)
        SDL_Quit();

    PrintToLog("INFO: SDL garbage collection complete.");
}

void ToggleFullscreen(photon_window &window){
    window.fullscreen = !window.fullscreen;

    if(window.fullscreen){
        SDL_DisplayMode mode;
        SDL_GetDesktopDisplayMode(0, &mode);
        SDL_SetWindowDisplayMode(window.window_SDL, &mode);
    }
    SDL_SetWindowFullscreen(window.window_SDL, SDL_bool(window.fullscreen));

    PrintToLog("INFO: Window toggled fullscreen.");
}

}
}
