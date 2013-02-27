#ifndef _PHOTON_SDL_H_
#define _PHOTON_SDL_H_

#define SDL_MAIN_HANDLED
#include <SDL.h>

namespace photon{
/*!
 * \brief The window struct
 */
struct photon_window{
    SDL_Window *window_SDL = nullptr;
    SDL_GLContext context_SDL = nullptr;
    bool fullscreen = false;

    unsigned int width = 1;
    unsigned int height = 1;

    unsigned int scene_buffer = 0;
    unsigned int scene_buffer_texture = 0;
};
struct photon_instance;

/*!
 * \brief contains SDL window managment functions
 */
namespace window_managment{

/*!
 * \brief Creates SDL window
 *
 * \sa opengl::InitOpenGL()
 *
 * \return The window created.
*/
photon_window CreateSDLWindow();

/*!
 * \brief Swaps window buffers
 * \param window The window to swap buffers of.
*/
void UpdateWindow(photon_window &window);

/*!
 * \brief Do SDL garbage collection.
 *
 * \param window The window to destroy.
 * \param quitSDL if true call SDL_Quit().
*/
void GarbageCollect(photon_window &window, bool quitSDL = true);

/*!
 * \brief Toggle window fullscreen
 *
 * \param window The window to toggle.
*/
void ToggleFullscreen(photon_window &window);
}
}
#endif
