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

void DrawBeams(photon_level &level){
    for(photon_laserbeam &beam : level.beams){
        opengl::DrawLaser(beam);
    }
}

void DrawBeamsLight(photon_level &level){
    for(photon_laserbeam &beam : level.beams){
        opengl::DrawLaserLight(beam);
    }
}

void DrawFX(photon_level &level){
    glm::uvec2 current(0);
    for(current.x = 0; current.x < level.grid.shape()[0];current.x++){
        for(current.y = 0; current.y < level.grid.shape()[1];current.y++){
            blocks::DrawFX(level.grid[current.x][current.y], current);
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

        // fill the borders with indestructible blocks.
        for(int x = 0;x<width;x++){
            level.grid[x][0].type = indestructible;
            level.grid[x][height-1].type = indestructible;
        }
        for(int y = 0;y<height;y++){
            level.grid[0][y].type = indestructible;
            level.grid[width-1][y].type = indestructible;
        }

        xmlNode *node = root->xmlChildrenNode;

        while(node != nullptr) {
            if((!xmlStrcmp(node->name, (const xmlChar *)"data"))){
                xmlNode *row = node->xmlChildrenNode;
                while(row != nullptr) {
                    if((!xmlStrcmp(row->name, (const xmlChar *)"row"))){
                        xmlChar *y_str = xmlGetProp(row, (const xmlChar *)"y");
                        unsigned int y = atoi((char*)y_str);
                        xmlFree(y_str);

                        xmlNode *block_xml = row->xmlChildrenNode;
                        while(block_xml != nullptr) {
                            if((!xmlStrcmp(block_xml->name, (const xmlChar *)"block"))){
                                xmlChar *x_str = xmlGetProp(block_xml, (const xmlChar *)"x");
                                unsigned int x = atoi((char*)x_str);
                                xmlFree(x_str);

                                xmlChar *type_str = xmlGetProp(block_xml, (const xmlChar *)"type");

                                photon_block &block = level.grid[x][y];

                                if((!xmlStrcmp(type_str, (const xmlChar *)"plain"))){
                                    block.type = plain;
                                }else if((!xmlStrcmp(type_str, (const xmlChar *)"mirror"))){
                                    block.type = mirror;

                                    xmlChar *angle_str = xmlGetProp(block_xml, (const xmlChar *)"angle");

                                    block.data = atof((char*)angle_str);

                                    xmlFree(angle_str);
                                }else if((!xmlStrcmp(type_str, (const xmlChar *)"mirror_locked"))){
                                    block.type = mirror_locked;

                                    xmlChar *angle_str = xmlGetProp(block_xml, (const xmlChar *)"angle");

                                    block.data = atof((char*)angle_str);

                                    xmlFree(angle_str);
                                }else if((!xmlStrcmp(type_str, (const xmlChar *)"mirror_locked_pos"))){
                                    block.type = mirror_locked_pos;

                                    xmlChar *angle_str = xmlGetProp(block_xml, (const xmlChar *)"angle");

                                    block.data = atof((char*)angle_str);

                                    xmlFree(angle_str);
                                }else if((!xmlStrcmp(type_str, (const xmlChar *)"tnt"))){
                                    block.type = tnt;
                                }else if((!xmlStrcmp(type_str, (const xmlChar *)"filter_red"))){
                                    block.type = filter_red;
                                }else if((!xmlStrcmp(type_str, (const xmlChar *)"filter_green"))){
                                    block.type = filter_green;
                                }else if((!xmlStrcmp(type_str, (const xmlChar *)"filter_blue"))){
                                    block.type = filter_blue;
                                }else if((!xmlStrcmp(type_str, (const xmlChar *)"filter_yellow"))){
                                    block.type = filter_yellow;
                                }else if((!xmlStrcmp(type_str, (const xmlChar *)"filter_cyan"))){
                                    block.type = filter_cyan;
                                }else if((!xmlStrcmp(type_str, (const xmlChar *)"filter_magenta"))){
                                    block.type = filter_magenta;
                                }else if((!xmlStrcmp(type_str, (const xmlChar *)"emitter_white"))){
                                    block.type = emitter_white;

                                    xmlChar *angle_str = xmlGetProp(block_xml, (const xmlChar *)"angle");

                                    block.data = atof((char*)angle_str);

                                    xmlFree(angle_str);
                                }else if((!xmlStrcmp(type_str, (const xmlChar *)"emitter_red"))){
                                    block.type = emitter_red;

                                    xmlChar *angle_str = xmlGetProp(block_xml, (const xmlChar *)"angle");

                                    block.data = atof((char*)angle_str);

                                    xmlFree(angle_str);
                                }else if((!xmlStrcmp(type_str, (const xmlChar *)"emitter_green"))){
                                    block.type = emitter_green;

                                    xmlChar *angle_str = xmlGetProp(block_xml, (const xmlChar *)"angle");

                                    block.data = atof((char*)angle_str);

                                    xmlFree(angle_str);
                                }else if((!xmlStrcmp(type_str, (const xmlChar *)"emitter_blue"))){
                                    block.type = emitter_blue;

                                    xmlChar *angle_str = xmlGetProp(block_xml, (const xmlChar *)"angle");

                                    block.data = atof((char*)angle_str);

                                    xmlFree(angle_str);
                                }
                                // TODO - load other block types.
                                xmlFree(type_str);
                            }
                            block_xml = block_xml->next;
                        }
                    }

                    row = row->next;
                }
            }
            // TODO - load game mode & victory condition.
            node = node->next;
        }

        free(xml_buffer);
        xmlFreeDoc(doc);
    }else{
        PrintToLog("ERROR: Unable to load XML Level: \"%s\" does not exist!", filename.c_str());
    }

    return level;
}

void AdvanceFrame(photon_level &level, float time){
    level.beams.clear();
    glm::uvec2 location(0);

    for(location.x = 0; location.x < level.grid.shape()[0];location.x++){
        for(location.y = 0; location.y < level.grid.shape()[1];location.y++){
            blocks::OnFrame(location, level, time);
        }
    }
    for(photon_laserbeam &beam : level.beams){
        tracer::TraceBeam(beam, level, time);
    }
}

}

}
