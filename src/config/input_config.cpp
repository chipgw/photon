#include "photon_core.h"

#include <physfs.h>
#include <libxml/parser.h>

namespace photon{

namespace input{

photon_input_state ParseInputSingle(xmlNode *node){
    photon_input_state state;
    xmlChar *type_str = xmlGetProp(node, (const xmlChar*)"type");

    if((xmlStrEqual(type_str, (const xmlChar*)"keyboard"))){
        state.type = photon_input_state::keyboard;

        xmlChar *key_str = xmlGetProp(node, (const xmlChar*)"key");
        xmlChar *mod_str = xmlGetProp(node, (const xmlChar*)"modifiers");

        state.key = SDL_GetScancodeFromName((char*)key_str);

        if(xmlStrstr(mod_str, (const xmlChar*)"ctrl")){
            state.modifiers |= KMOD_CTRL;
        }else if(xmlStrstr(mod_str, (const xmlChar*)"shift")){
            state.modifiers |= KMOD_SHIFT;
        }else if(xmlStrstr(mod_str, (const xmlChar*)"alt")){
            state.modifiers |= KMOD_ALT;
        }

        xmlFree(key_str);
        xmlFree(mod_str);
    }else if((xmlStrEqual(type_str, (const xmlChar*)"joystick_button"))){
        state.type = photon_input_state::joystick_button;

        xmlChar *index_str = xmlGetProp(node, (const xmlChar*)"index");
        state.joystick_input_index = atoi((char*)index_str);

        xmlFree(index_str);
    }else if((xmlStrEqual(type_str, (const xmlChar*)"joystick_axis"))){
        state.type = photon_input_state::joystick_axis;

        xmlChar *index_str = xmlGetProp(node, (const xmlChar*)"index");
        xmlChar *negate_str = xmlGetProp(node, (const xmlChar*)"negate");
        state.joystick_input_index = atoi((char*)index_str);
        state.axis_input_negate = xmlStrEqual(negate_str, (const xmlChar*)"true");

        xmlFree(negate_str);
        xmlFree(index_str);
    }else if((xmlStrEqual(type_str, (const xmlChar*)"controller_button"))){
        state.type = photon_input_state::gamecontroller_button;

        xmlChar *index_str = xmlGetProp(node, (const xmlChar*)"button");
        state.controller_button = SDL_GameControllerGetButtonFromString((char*)index_str);

        xmlFree(index_str);
    }else if((xmlStrEqual(type_str, (const xmlChar*)"controller_axis"))){
        state.type = photon_input_state::gamecontroller_axis;

        xmlChar *index_str = xmlGetProp(node, (const xmlChar*)"axis");
        xmlChar *negate_str = xmlGetProp(node, (const xmlChar*)"negate");
        state.controller_axis = SDL_GameControllerGetAxisFromString((char*)index_str);
        state.axis_input_negate = xmlStrEqual(negate_str, (const xmlChar*)"true");

        xmlFree(negate_str);
        xmlFree(index_str);
    }
    xmlFree(type_str);
    return state;
}

bool LoadConfig(const std::string &filename, photon_input &input){
    if(PHYSFS_exists(filename.c_str())){
        PHYSFS_File *file;
        long length;
        char *xml_buffer;

        file = PHYSFS_openRead(filename.c_str());
        if(!file){
            PrintToLog("ERROR: unable to open XML input config file \"%s\"", filename.c_str());
            return false;
        }

        length = PHYSFS_fileLength(file);
        xml_buffer = new char[length];

        PHYSFS_read(file, xml_buffer, 1, length);
        PHYSFS_close(file);

        xmlDocPtr doc = xmlParseMemory(xml_buffer, length);

        if(doc == nullptr) {
            PrintToLog("ERROR: Unable to load XML input config: Document not parsed successfully!");
            return false;
        }

        xmlNodePtr root = xmlDocGetRootElement(doc);
        if(root == nullptr) {
            PrintToLog("ERROR: Unable to load XML input config: empty document!");
            xmlFreeDoc(doc);
            return false;
        }
        if(xmlStrcmp(root->name, (const xmlChar*)"photon_input")) {
            PrintToLog("ERROR: Unable to load XML input config: root node not photon_input!");
            xmlFreeDoc(doc);
            return false;
        }
        input.is_valid = false;

        xmlChar *device_str = xmlGetProp(root, (const xmlChar*)"device");
        xmlChar *guid_str = xmlGetProp(root, (const xmlChar*)"device_guid");

        // TODO - use device name and GUID.

        xmlFree(device_str);
        xmlFree(guid_str);

        xmlChar *deadzone_str = xmlGetProp(root, (const xmlChar*)"deadzone");

        if(deadzone_str != nullptr){
            input.deadzone = atof((char*)deadzone_str);

            xmlFree(deadzone_str);
        }

        xmlNode *node = root->xmlChildrenNode;

        while(node != nullptr) {
            if((xmlStrEqual(node->name, (const xmlChar*)"interact"))){
                input.interact = ParseInputSingle(node);
            }else if((xmlStrEqual(node->name, (const xmlChar*)"next_item"))){
                input.next_item = ParseInputSingle(node);
            }else if((xmlStrEqual(node->name, (const xmlChar*)"previous_item"))){
                input.previous_item = ParseInputSingle(node);
            }else if((xmlStrEqual(node->name, (const xmlChar*)"open_inventory"))){
                input.open_inventory = ParseInputSingle(node);
            }else if((xmlStrEqual(node->name, (const xmlChar*)"rotate_clockwise"))){
                input.rotate_clockwise = ParseInputSingle(node);
            }else if((xmlStrEqual(node->name, (const xmlChar*)"rotate_counter_clockwise"))){
                input.rotate_counter_clockwise = ParseInputSingle(node);
            }else if((xmlStrEqual(node->name, (const xmlChar*)"zoom_in"))){
                input.zoom_in = ParseInputSingle(node);
            }else if((xmlStrEqual(node->name, (const xmlChar*)"zoom_out"))){
                input.zoom_out = ParseInputSingle(node);
            }else if((xmlStrEqual(node->name, (const xmlChar*)"move_right"))){
                input.move_right = ParseInputSingle(node);
            }else if((xmlStrEqual(node->name, (const xmlChar*)"move_left"))){
                input.move_left = ParseInputSingle(node);
            }else if((xmlStrEqual(node->name, (const xmlChar*)"move_up"))){
                input.move_up = ParseInputSingle(node);
            }else if((xmlStrEqual(node->name, (const xmlChar*)"move_down"))){
                input.move_down = ParseInputSingle(node);
            }else if((xmlStrEqual(node->name, (const xmlChar*)"camera_right"))){
                input.camera_right = ParseInputSingle(node);
            }else if((xmlStrEqual(node->name, (const xmlChar*)"camera_left"))){
                input.camera_left = ParseInputSingle(node);
            }else if((xmlStrEqual(node->name, (const xmlChar*)"camera_up"))){
                input.camera_up = ParseInputSingle(node);
            }else if((xmlStrEqual(node->name, (const xmlChar*)"camera_down"))){
                input.camera_down = ParseInputSingle(node);
            }else if((xmlStrEqual(node->name, (const xmlChar*)"pause"))){
                input.pause = ParseInputSingle(node);
            }else if((xmlStrEqual(node->name, (const xmlChar*)"gui_up"))){
                input.up = ParseInputSingle(node);
            }else if((xmlStrEqual(node->name, (const xmlChar*)"gui_down"))){
                input.down = ParseInputSingle(node);
            }else if((xmlStrEqual(node->name, (const xmlChar*)"gui_left"))){
                input.left = ParseInputSingle(node);
            }else if((xmlStrEqual(node->name, (const xmlChar*)"gui_right"))){
                input.right = ParseInputSingle(node);
            }else if((xmlStrEqual(node->name, (const xmlChar*)"gui_select"))){
                input.select = ParseInputSingle(node);
            }else if((xmlStrEqual(node->name, (const xmlChar*)"gui_back"))){
                input.back = ParseInputSingle(node);
            }
            node = node->next;
        }

        input.is_valid = true;

        delete[] xml_buffer;
        xmlFreeDoc(doc);

        return true;
    }else{
        PrintToLog("ERROR: Unable to load XML input config: \"%s\" does not exist!", filename.c_str());
    }
    return false;
}

}

}
