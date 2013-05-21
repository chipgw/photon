#include "photon_core.h"

#include <physfs.h>
#include <libxml/parser.h>

namespace photon{

bool SetRootPhysFS(const std::string &dir, bool do_archives){
    if(PHYSFS_mount(dir.c_str(), "/", 0)){
        PrintToLog("INFO: Succesfully mounted \"%s\" as data directory.", dir.c_str());
        return true;
    }

    if(do_archives){
        PrintToLog("WARNING: PhysFS was unable to mount \"%s\" as data directory, trying archive extensions...", dir.c_str());

        const PHYSFS_ArchiveInfo **types = PHYSFS_supportedArchiveTypes();
        while((*types) != nullptr){
            std::string filename = dir;
            filename.append(".").append((*types)->extension);
            if(PHYSFS_mount(filename.c_str(), "/", 0)){
                PrintToLog("INFO: %s archive found!", filename.c_str());
                return true;
            }
            types++;
        }
    }else{
        PrintToLog("WARNING: PhysFS was unable to mount \"%s\" as data directory!", dir.c_str());
    }

    return false;
}

bool SetSavesDirPhysFS(const std::string &dir){
    if(PHYSFS_mkdir(dir.c_str())){
        if(!PHYSFS_setWriteDir(dir.c_str())){
            PrintToLog("WARNING: unable to set \"%s\" as saves directory! %s", dir.c_str(), PHYSFS_getLastError());
            return false;
        }else if(!PHYSFS_mount(dir.c_str(), "/", 0)){ // makes the "saves" directory also work for custom configs and user modding sorts of things. (not that there is much to mod...)
            PrintToLog("WARNING: unable to mount \"%s\" as saves directory! %s", dir.c_str(), PHYSFS_getLastError());
            return false;
        }

        PrintToLog("INFO: Succesfully mounted \"%s\" as save directory.", dir.c_str());
        return true;
    }
    return false;
}

bool LoadEngineConfig(const std::string &filename, photon_instance &instance){
    xmlDocPtr doc = xmlParseFile(filename.c_str());

    if(doc == nullptr) {
        PrintToLog("ERROR: Unable to load XML config: Document not loaded/parsed successfully!");
        return false;
    }

    xmlNodePtr root = xmlDocGetRootElement(doc);
    if(root == nullptr) {
        PrintToLog("ERROR: Unable to load XML config: empty document!");
        xmlFreeDoc(doc);
        return false;
    }
    if(xmlStrcmp(root->name, (const xmlChar*)"photon_config")) {
        PrintToLog("ERROR: Unable to load XML config: root node not photon_input!");
        xmlFreeDoc(doc);
        return false;
    }

    xmlChar *fullscreen_str = xmlGetProp(root, (const xmlChar*)"fullscreen");

    if(fullscreen_str != nullptr){
        if(xmlStrEqual(fullscreen_str, (const xmlChar*)"true")){
            instance.settings.fullscreen = true;
        }else if(xmlStrEqual(fullscreen_str, (const xmlChar*)"false")){
            instance.settings.fullscreen = false;
        }

        xmlFree(fullscreen_str);
    }

    xmlChar *doublebuffer_str = xmlGetProp(root, (const xmlChar*)"doublebuffer");

    if(doublebuffer_str != nullptr){
        if(xmlStrEqual(doublebuffer_str, (const xmlChar*)"true")){
            instance.settings.doublebuffer = true;
        }else if(xmlStrEqual(doublebuffer_str, (const xmlChar*)"false")){
            instance.settings.doublebuffer = false;
        }

        xmlFree(doublebuffer_str);
    }

    xmlChar *vsync_str = xmlGetProp(root, (const xmlChar*)"vsync");

    if(vsync_str != nullptr){
        if(xmlStrEqual(vsync_str, (const xmlChar*)"true")){
            instance.settings.vsync = true;
        }else if(xmlStrEqual(vsync_str, (const xmlChar*)"false")){
            instance.settings.vsync = false;
        }

        xmlFree(vsync_str);
    }

    xmlChar *input_config = xmlGetProp(root, (const xmlChar*)"input");

    if(input_config != nullptr){
        instance.settings.input_config = (char*)input_config;

        xmlFree(input_config);
    }

    xmlChar *screenedges_str = xmlGetProp(root, (const xmlChar*)"screen_edges");

    if(screenedges_str != nullptr){
        if(xmlStrEqual(screenedges_str, (const xmlChar*)"true")){
            instance.settings.screen_edges = true;
        }else if(xmlStrEqual(screenedges_str, (const xmlChar*)"false")){
            instance.settings.screen_edges = false;
        }

        xmlFree(screenedges_str);
    }

    xmlChar *data_path = xmlGetProp(root, (const xmlChar*)"data_path");

    if(data_path != nullptr){
        instance.settings.data_path = (char*)data_path;

        xmlFree(data_path);
    }

    xmlChar *save_path = xmlGetProp(root, (const xmlChar*)"save_path");

    if(save_path != nullptr){
        instance.settings.save_path = (char*)save_path;

        xmlFree(save_path);
    }

    xmlChar *multisample_str = xmlGetProp(root, (const xmlChar*)"multisamples");

    if(multisample_str != nullptr){
        instance.settings.multisamples = atoi((char*)multisample_str);

        xmlFree(multisample_str);
    }

    xmlFreeDoc(doc);

    return true;
}

}
