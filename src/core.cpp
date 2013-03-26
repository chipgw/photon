#include <GL/glew.h>

#include "photon_core.h"
#include "photon_opengl.h"
#include "photon_gui.h"
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
    photon_instance instance;

    logfile = fopen("photon.log", "w");
    PrintToLog("INFO: Starting up Photon. Executable: \"%s\"", argv[0]);

    PrintToLog("INFO: Photon Git Version: %s", version::git_sha1);

    PrintToLog("INFO: Running on %s, Build Type: %s", SDL_GetPlatform(), version::build_type);

    PHYSFS_init(argv[0]);
    if(!PHYSFS_mount("data", "/", 0)){
        PrintToLog("WARNING: PhysFS was unable to mount data as '/', trying archive extensions...");

        const PHYSFS_ArchiveInfo **types = PHYSFS_supportedArchiveTypes();
        while((*types) != nullptr){
            std::string filename = "data.";
            if(PHYSFS_mount(filename.append((*types)->extension).c_str(), "/", 0)){
                PrintToLog("INFO: data.%s archive found!", (*types)->extension);
                break;
            }
            types++;
        }
    }

    instance.window = window_managment::CreateSDLWindow();

    opengl::InitOpenGL(instance.window);

    gui::InitFreeType();

    return instance;
}

void GarbageCollect(photon_instance &instance){
    PrintToLog("INFO: Doing garbage collection.");
    opengl::GarbageCollect(instance.window);
    window_managment::GarbageCollect(instance.window);

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
