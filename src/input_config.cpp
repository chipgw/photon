#include <physfs.h>
#include <libxml/parser.h>
#include "photon_core.h"
#include "photon_input.h"

namespace photon{

namespace input{

const xmlChar* operator "" _xml(const char* str, size_t /*length*/){
    return (const xmlChar*)str;
}

photon_input_state LoadInputSingle(xmlNode *node){
    photon_input_state state;
    xmlChar *type_str = xmlGetProp(node, "type"_xml);

    if((xmlStrEqual(type_str, "keyboard"_xml))){
        state.type = photon_input_state::keyboard;

        xmlChar *key_str = xmlGetProp(node, "key"_xml);
        xmlChar *mod_str = xmlGetProp(node, "modifiers"_xml);

        state.key = SDL_GetScancodeFromName((char*)key_str);

        if(xmlStrstr(mod_str, "ctrl"_xml)){
            state.modifiers |= KMOD_CTRL;
        }else if(xmlStrstr(mod_str, "shift"_xml)){
            state.modifiers |= KMOD_SHIFT;
        }else if(xmlStrstr(mod_str, "alt"_xml)){
            state.modifiers |= KMOD_ALT;
        }

        xmlFree(key_str);
        xmlFree(mod_str);
    }else if((xmlStrEqual(type_str, "joystick_button"_xml))){
        state.type = photon_input_state::joystick_button;

        xmlChar *index_str = xmlGetProp(node, "index"_xml);
        state.joystick_input_index = atoi((char*)index_str);

        xmlFree(index_str);
    }else if((xmlStrEqual(type_str, "joystick_axis"_xml))){
        state.type = photon_input_state::joystick_axis;

        xmlChar *index_str = xmlGetProp(node, "index"_xml);
        xmlChar *negate_str = xmlGetProp(node, "negate"_xml);
        state.joystick_input_index = atoi((char*)index_str);
        state.axis_input_negate = xmlStrEqual(negate_str, "true"_xml);

        xmlFree(negate_str);
        xmlFree(index_str);
    }else if((xmlStrEqual(type_str, "controller_button"_xml))){
        state.type = photon_input_state::gamecontroller_button;

        xmlChar *index_str = xmlGetProp(node, "button"_xml);
        state.controller_button = SDL_GameControllerGetButtonFromString((char*)index_str);

        xmlFree(index_str);
    }else if((xmlStrEqual(type_str, "controller_axis"_xml))){
        state.type = photon_input_state::gamecontroller_axis;

        xmlChar *index_str = xmlGetProp(node, "axis"_xml);
        xmlChar *negate_str = xmlGetProp(node, "negate"_xml);
        state.controller_axis = SDL_GameControllerGetAxisFromString((char*)index_str);
        state.axis_input_negate = xmlStrEqual(negate_str, "true"_xml);

        xmlFree(negate_str);
        xmlFree(index_str);
    }
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
        xml_buffer = (char*)malloc(length);

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
        if(xmlStrcmp(root->name, "photon_input"_xml)) {
            PrintToLog("ERROR: Unable to load XML input config: root node not photon_input!");
            xmlFreeDoc(doc);
            return false;
        }
        input.is_valid = false;

        xmlChar *device_str = xmlGetProp(root, "device"_xml);
        xmlChar *guid_str = xmlGetProp(root, "device_guid"_xml);

        xmlFree(device_str);
        xmlFree(guid_str);

        xmlNode *node = root->xmlChildrenNode;

        while(node != nullptr) {
            if((xmlStrEqual(node->name, "interact"_xml))){
                input.interact = LoadInputSingle(node);
            }else if((xmlStrEqual(node->name, "next_item"_xml))){
                input.next_item = LoadInputSingle(node);
            }else if((xmlStrEqual(node->name, "previous_item"_xml))){
                input.previous_item = LoadInputSingle(node);
            }else if((xmlStrEqual(node->name, "open_inventory"_xml))){
                input.open_inventory = LoadInputSingle(node);
            }else if((xmlStrEqual(node->name, "rotate_clockwise"_xml))){
                input.rotate_clockwise = LoadInputSingle(node);
            }else if((xmlStrEqual(node->name, "rotate_counter_clockwise"_xml))){
                input.rotate_counter_clockwise = LoadInputSingle(node);
            }else if((xmlStrEqual(node->name, "zoom_in"_xml))){
                input.zoom_in = LoadInputSingle(node);
            }else if((xmlStrEqual(node->name, "zoom_out"_xml))){
                input.zoom_out = LoadInputSingle(node);
            }else if((xmlStrEqual(node->name, "move_right"_xml))){
                input.move_right = LoadInputSingle(node);
            }else if((xmlStrEqual(node->name, "move_left"_xml))){
                input.move_left = LoadInputSingle(node);
            }else if((xmlStrEqual(node->name, "move_up"_xml))){
                input.move_up = LoadInputSingle(node);
            }else if((xmlStrEqual(node->name, "move_down"_xml))){
                input.move_down = LoadInputSingle(node);
            }else if((xmlStrEqual(node->name, "camera_right"_xml))){
                input.camera_right = LoadInputSingle(node);
            }else if((xmlStrEqual(node->name, "camera_left"_xml))){
                input.camera_left = LoadInputSingle(node);
            }else if((xmlStrEqual(node->name, "camera_up"_xml))){
                input.camera_up = LoadInputSingle(node);
            }else if((xmlStrEqual(node->name, "camera_down"_xml))){
                input.camera_down = LoadInputSingle(node);
            }else if((xmlStrEqual(node->name, "pause"_xml))){
                input.pause = LoadInputSingle(node);
            }else if((xmlStrEqual(node->name, "gui_up"_xml))){
                input.up = LoadInputSingle(node);
            }else if((xmlStrEqual(node->name, "gui_down"_xml))){
                input.down = LoadInputSingle(node);
            }else if((xmlStrEqual(node->name, "gui_left"_xml))){
                input.left = LoadInputSingle(node);
            }else if((xmlStrEqual(node->name, "gui_right"_xml))){
                input.right = LoadInputSingle(node);
            }else if((xmlStrEqual(node->name, "gui_select"_xml))){
                input.select = LoadInputSingle(node);
            }else if((xmlStrEqual(node->name, "gui_back"_xml))){
                input.back = LoadInputSingle(node);
            }
            node = node->next;
        }

        input.is_valid = true;

        free(xml_buffer);
        xmlFreeDoc(doc);

        return true;
    }else{
        PrintToLog("ERROR: Unable to load XML input config: \"%s\" does not exist!", filename.c_str());
    }
    return false;
}

}

}
