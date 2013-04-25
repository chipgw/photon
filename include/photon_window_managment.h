#ifndef _PHOTON_WINDOW_MANAGMENT_H_
#define _PHOTON_WINDOW_MANAGMENT_H_

#include "photon_opengl.h"

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

    uint32_t width = 1;
    uint32_t height = 1;

    GLuint light_buffer = 0;
    GLuint light_buffer_texture = 0;
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

/*!
 * \brief loads and sets the window icon
 * \param filename
 */
void SetWindowIcon(photon_window &window, const std::string &filename);

}

}
#endif
