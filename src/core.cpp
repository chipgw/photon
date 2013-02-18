#include <GL/glew.h>

#include "photon_core.h"
#include "photon_opengl.h"
#include <SDL_timer.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <ctime>
#include <boost/date_time.hpp>
#include <physfs.h>

namespace photon{
FILE* logfile;

photon_instance Init(int argc, char *argv[], bool parseconfig){
    logfile = fopen("photon.log", "w");
    PrintToLog("INFO: Starting up Photon. Executable: \"%s\"", argv[0]);

    PrintToLog("INFO: Photon Git Version: %s", version::git_sha1);

    PrintToLog("INFO: Photon Build Type: %s", version::build_type);

    PHYSFS_init(argv[0]);
    PHYSFS_mount("data", "/", 0);

    photon_instance instance;

    instance.window = sdl::CreateSDLWindow();

    opengl::InitOpenGL(instance.window);

    return instance;
}

void GarbageCollect(photon_instance &instance){
    PrintToLog("INFO: Doing garbage collection.");
    opengl::GarbageCollect(instance.window);
    sdl::GarbageCollect(instance.window);

    PHYSFS_deinit();

    PrintToLog("INFO: Photon garbage collection complete (except log file, doing that now...)");
    fclose(logfile);
}

void Close(photon_instance &instance){
    PrintToLog("INFO: Request to shutdown Photon recieved.");
    instance.running = false;
}

void PrintToLog(const char *format,...){
    va_list args;
    va_start(args, format);

    char buffer[512];
    vsprintf(buffer, format, args);
    va_end(args);

    std::string out(buffer);

    auto t = boost::posix_time::microsec_clock::local_time();
    out.insert(0, boost::posix_time::to_simple_string(t).substr(12) + ": ");

    if(*out.rbegin() != '\n'){
        out.append("\n");
    }

    fprintf(stdout, out.c_str());
    fflush(stdout);

    if(logfile){
        fprintf(logfile, out.c_str());
        fflush(logfile);
    }
}
}
