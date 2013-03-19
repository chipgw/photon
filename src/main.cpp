#include "photon_core.h"

int main(int argc, char *argv[]){
    photon::photon_instance instance = photon::Init(argc, argv);

    photon::MainLoop(instance);

    photon::GarbageCollect(instance);

    return 0;
}

