#ifndef _PHOTON_CORE_H_
#define _PHOTON_CORE_H_

#include "photon_window_managment.h"
#include "photon_player.h"
#include "photon_level.h"
#include "photon_input.h"
#include <list>

namespace photon{

struct photon_instance{
    photon_window window;

    input::game_input input_set;

    photon_level level;

    photon_player player;

    /*! \brief total frames rendered */
    unsigned long total_frames = 0;
    /*! \brief is set to true when the main loop starts, set to false to end loop. */
    bool running = false;

    float zoom = 1.0;
};

photon_instance Init(int argc, char *argv[], bool parseconfig = true);

void MainLoop(photon_instance &instance);

void GarbageCollect(photon_instance &instance);

void Close(photon_instance &instance);

/*!
 * \brief Prints formated string to log.
 * Prints to both stdout and log file (if open). syntax identical to printf().
 * \param format
 */
void PrintToLog(const char *format,...);


/*!
 * \brief holds version information.
 */
namespace version{
/*!
 * \brief holds the builds git sha1 value.
 */
extern const char* git_sha1;

/*!
 * \brief holds the build type (as defined in CMake by CMAKE_BUILD_TYPE)
 */
extern const char* build_type;
}

}

#endif
