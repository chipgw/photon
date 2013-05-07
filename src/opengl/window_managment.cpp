#include "photon_core.h"

#include <SDL_opengl.h>
#include <SDL_gamecontroller.h>
#include <SOIL.h>
#include <physfs.h>

#define PHOTON_WINDOW_FLAGS SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE | SDL_WINDOW_MAXIMIZED

namespace photon{

namespace window_managment{

photon_window CreateSDLWindow(const photon_settings &settings){
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

    if(settings.multisamples > 0){
        SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS,  1);
        SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES,  settings.multisamples);
    }

    window.window_SDL = SDL_CreateWindow("Photon", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, PHOTON_WINDOW_FLAGS | (settings.fullscreen * SDL_WINDOW_FULLSCREEN));

    if (!window.window_SDL){
        PrintToLog("ERROR: Unable to create window!");
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

    SDL_ShowCursor(SDL_ENABLE);

    SetWindowIcon(window, "/textures/gui/window_icon.png");

    return window;
}

void UpdateWindow(photon_window &window){
    SDL_GL_SwapWindow(window.window_SDL);
}

void GarbageCollect(photon_window &window, bool quitSDL){
    SDL_GL_DeleteContext(window.context_SDL);
    SDL_DestroyWindow(window.window_SDL);

    if(quitSDL){
        SDL_Quit();
    }

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

void SetWindowIcon(photon_window &window, const std::string &filename){
    if(PHYSFS_exists(filename.c_str())){
        auto fp = PHYSFS_openRead(filename.c_str());
        intmax_t length = PHYSFS_fileLength(fp);
        if(length > 0){
            uint8_t *buffer = (uint8_t*) malloc(length);

            PHYSFS_read(fp, buffer, 1, length);

            PHYSFS_close(fp);

            int width, height, channels;
            uint8_t *image = SOIL_load_image_from_memory(buffer, length, &width, &height, &channels, SOIL_LOAD_RGBA);

            if(image == nullptr){
                PrintToLog("ERROR: image loading failed!");
            }

#if SDL_BYTEORDER == SDL_BIG_ENDIAN
            const uint32_t rmask = 0xff000000;
            const uint32_t gmask = 0x00ff0000;
            const uint32_t bmask = 0x0000ff00;
            const uint32_t amask = 0x000000ff;
#else
            const uint32_t rmask = 0x000000ff;
            const uint32_t gmask = 0x0000ff00;
            const uint32_t bmask = 0x00ff0000;
            const uint32_t amask = 0xff000000;
#endif

            SDL_Surface *icon = SDL_CreateRGBSurfaceFrom(image, width, height, channels * 8, channels*width, rmask, gmask, bmask, amask);


            if(icon == nullptr){
                PrintToLog("ERROR: surface creation failed!");
            }

            SDL_SetWindowIcon(window.window_SDL, icon);

            SDL_FreeSurface(icon);
            free(image);
            free(buffer);
        }else{
            PrintToLog("ERROR: Unable to open image file \"%s\"!");
        }
    }else{
        PrintToLog("ERROR: Image file \"%s\" does not exist!", filename.c_str());
    }
}

}
}
