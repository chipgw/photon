#include "photon_level.h"
#include "photon_blocks.h"
#include "photon_core.h"
#include <physfs.h>
#include <libxml/parser.h>

namespace photon{

namespace level{

void Draw(photon_level &level){
    glm::uvec2 current(0);
    for(current.x = 0; current.x < level.grid.shape()[0];current.x++){
        for(current.y = 0; current.y < level.grid.shape()[1];current.y++){
            blocks::Draw(level.grid[current.x][current.y], current);
        }
    }
}

photon_level LoadLevelXML(const std::string &filename){
    photon_level level;

    if(PHYSFS_exists(filename.c_str())){
        PHYSFS_File *file;
        long length;
        char *xml_buffer;

        file = PHYSFS_openRead(filename.c_str());
        if(!file){
            PrintToLog("ERROR: unable to open XML Level \"%s\"", filename.c_str());
            return level;
        }

        length = PHYSFS_fileLength(file);
        xml_buffer = (char*)malloc(length);

        PHYSFS_read(file, xml_buffer, 1, length);
        PHYSFS_close(file);

        xmlDocPtr doc = xmlParseMemory(xml_buffer, length);

        if(doc == nullptr) {
            PrintToLog("ERROR: Unable to load XML Level: Document not parsed successfully!");
            return level;
        }

        xmlNodePtr root = xmlDocGetRootElement(doc);
        if(root == nullptr) {
            PrintToLog("ERROR: Unable to load XML Level: empty document!");
            xmlFreeDoc(doc);
            return level;
        }
        if(xmlStrcmp(root->name, (const xmlChar *) "photon_level")) {
            PrintToLog("ERROR: Unable to load XML Level: root node not photon_level!");
            xmlFreeDoc(doc);
            return level;
        }

        xmlChar *width_str = xmlGetProp(root, (const xmlChar *)"width");
        xmlChar *height_str = xmlGetProp(root, (const xmlChar *)"height");

        int width = atoi((char*)width_str);
        int height = atoi((char*)height_str);

        xmlFree(width_str);
        xmlFree(height_str);

        PrintToLog("INFO: Level size %i x %i", width, height);

        //because we fill the edges with indestructible blocks.
        width += 2;
        height += 2;

        level.grid.resize(boost::extents[width][height]);

        xmlNode *node = root->xmlChildrenNode;
        while(node != nullptr) {
            if((!xmlStrcmp(node->name, (const xmlChar *)"data"))){
                // TODO - code goes here.
            }
            node = node->next;
        }
    }else{
        PrintToLog("ERROR: Unable to load XML Level: \"%s\" does not exist!", filename.c_str());
    }

    return level;
}

}

}
