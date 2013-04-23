#include "photon_core.h"

#include <stdlib.h>
#include <stdarg.h>
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

    PHYSFS_setWriteDir(PHYSFS_getBaseDir());
    if(PHYSFS_mkdir("saves")){
        if(!PHYSFS_setWriteDir("saves")){
            PrintToLog("ERROR: unable to set saves directory! %s", PHYSFS_getLastError());
        }else if(!PHYSFS_mount("saves", "/", 0)){ // makes the "saves" directory also work for custom configs and user modding sorts of things. (not that there is much to mod...)
            PrintToLog("ERROR: unable to mount saves directory! %s", PHYSFS_getLastError());
        }
    }else{
        PrintToLog("ERROR: unable to set saves directory! %s", PHYSFS_getLastError());
    }

    instance.window = window_managment::CreateSDLWindow();

    opengl::InitOpenGL(instance.window);

    gui::InitFreeType();
    instance.gui = gui::InitGUI();

    instance.input = input::InitInput();

    return instance;
}

void GarbageCollect(photon_instance &instance){
    PrintToLog("INFO: Doing garbage collection.");
    input::GarbageCollect(instance.input);

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

    std::chrono::high_resolution_clock::time_point current = std::chrono::high_resolution_clock::now();

    time_t tnow = std::chrono::high_resolution_clock::to_time_t(current);
    tm *date = std::localtime(&tnow);
    int microseconds = std::chrono::duration_cast<std::chrono::microseconds>(current.time_since_epoch()).count() % 1000000;

    snprintf(buffer, 18, "%02i:%02i:%02i.%06i: ", date->tm_hour, date->tm_min, date->tm_sec, microseconds);

    // index 17 is the right place to start after the time has been printed.
    vsnprintf(&buffer[17], 495, format, args);
    va_end(args);

    std::string out(buffer);

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
