#include "photon_core.h"
#include "photon_lua.h"

#include <stdlib.h>
#include <ctime>
#include <cstdio>
#include <stdarg.h>
#include <physfs.h>

namespace photon{
FILE* logfile;

photon_instance Init(int argc, char *argv[]){
    photon_instance instance;

    logfile = fopen("photon.log", "w");
    PrintToLog("INFO: Starting up Photon. Executable: \"%s\"", argv[0]);

    PrintToLog("INFO: Photon %s, git sha1: %s", build_info::version, build_info::git_sha1);

    PrintToLog("INFO: Running on %s, Build Type: %s", SDL_GetPlatform(), build_info::build_type);

    LoadEngineConfig("photon.xml", instance);

    PHYSFS_init(argv[0]);
    if(!SetRootPhysFS(instance.settings.data_path.c_str(), true)){
        SetRootPhysFS("data", true);
    }

    PHYSFS_setWriteDir(PHYSFS_getBaseDir());
    if(!SetSavesDirPhysFS(instance.settings.save_path) && !SetSavesDirPhysFS("saves")){
        PrintToLog("ERROR: unable to set saves directory! %s", PHYSFS_getLastError());
    }

    instance.window = window_managment::CreateSDLWindow(instance.settings);

    opengl::InitOpenGL(instance.window);

    gui::InitFreeType();
    instance.gui = gui::InitGUI();

    instance.input = input::InitInput();

    if(!instance.settings.input_config.empty()){
        input::LoadConfig(instance.settings.input_config, instance.input);
    }

    lua::InitLua(instance, "/init.lua");

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

#ifdef _MSC_VER
#define snprintf _snprintf
#endif

void PrintToLog(const char *format,...){
    va_list args, args2;
    va_start(args, format);
    va_copy(args2, args);

    /* Create the timestamp string */
    std::chrono::high_resolution_clock::time_point current = std::chrono::high_resolution_clock::now();

    time_t tnow = std::chrono::high_resolution_clock::to_time_t(current);
    tm *date = std::localtime(&tnow);
    int microseconds = std::chrono::duration_cast<std::chrono::microseconds>(current.time_since_epoch()).count() % 1000000;

    /* The size of this is constant, so it might as well not be on the heap.
     * It saves us from some confusing sizing & position calculation by keeping it seperate from the main output string. */
    char timestamp[18];
    snprintf(timestamp, 18, "%02i:%02i:%02i.%06i: ", date->tm_hour, date->tm_min, date->tm_sec, microseconds);
    timestamp[17] = '\0';

    /* Create the main output string */
    int len = vsnprintf(nullptr, 0, format, args) + 1;

    char* buffer = new char[len + 1];

    vsnprintf(buffer, len, format, args2);

    va_end(args);
    va_end(args2);

    buffer[len - 1] = '\n';
    buffer[len] = '\0';

    /* Write the strings to stdout and log file. */
    fprintf(stdout, timestamp);
    fprintf(stdout, buffer);
    fflush(stdout);

    if(logfile){
        fprintf(logfile, timestamp);
        fprintf(logfile, buffer);
        fflush(logfile);
    }

    delete[] buffer;
}
}
