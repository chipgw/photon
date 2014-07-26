#ifndef _PHOTON_CORE_H_
#define _PHOTON_CORE_H_

#include "photon_player.h"
#include "photon_level.h"
#include "photon_input.h"
#include "photon_gui.h"

#include <list>
#include <chrono>
#include <algorithm>

namespace photon{

struct photon_settings{
    std::string data_path = "data";
    std::string save_path = "saves";

    bool fullscreen = true;

    bool vsync = true;
    bool doublebuffer = true;
    int multisamples = 16;

    bool screen_edges = true;

    std::string input_config;
};

struct photon_instance{
    photon_settings settings;

    photon_window window;

    photon_input input;

    photon_level level;

    photon_player player;

    photon_gui_container gui;
    bool paused = false;

    /*! \brief total frames rendered */
    uintmax_t total_frames = 0;
    std::chrono::high_resolution_clock::time_point creation_time = std::chrono::high_resolution_clock::now();

    /*! \brief is set to true when the main loop starts, set to false to end loop. */
    bool running = false;

    glm::vec3 camera_offset = glm::vec3(0.0f, 0.0f, 4.0f);
};

photon_instance& Init(int argc, char *argv[]);

void MainLoop(photon_instance &instance);

void GarbageCollect(photon_instance &instance);

void Close(photon_instance &instance);

/*!
 * \brief Prints formated string to log.
 * Prints to both stdout and log file (if open). syntax identical to printf().
 * \param format
 */
void PrintToLog(const char *format,...);

bool LoadEngineConfig(const std::string &filename, photon_instance &instance);

bool SetRootPhysFS(const std::string &dir, bool do_archives);

bool SetSavesDirPhysFS(const std::string &dir);

/*!
 * \brief holds version information.
 */
namespace build_info{
/*!
 * \brief holds the builds git sha1 value.
 */
extern const char* git_sha1;

/*!
 * \brief holds the build type (as defined in CMake by CMAKE_BUILD_TYPE)
 */
extern const char* build_type;

/*!
 * \brief version
 */
extern const char* version;
}

}

#endif
