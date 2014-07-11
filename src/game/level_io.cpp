#include "photon_core.h"
#include "photon_lua.h"

#include <physfs.h>
#include <libxml/parser.h>

namespace photon{

namespace level{

bool LoadLevelXML(const std::string &filename, photon_instance &instance){
    photon_level &level = instance.level;
    photon_player &player = instance.player;
    if(PHYSFS_exists(filename.c_str())){
        PHYSFS_File *file;
        long length;
        char *xml_buffer;

        file = PHYSFS_openRead(filename.c_str());
        if(!file){
            PrintToLog("ERROR: unable to open XML Level \"%s\"", filename.c_str());
            return false;
        }

        length = PHYSFS_fileLength(file);
        xml_buffer = new char[length];

        PHYSFS_read(file, xml_buffer, 1, length);
        PHYSFS_close(file);

        xmlDocPtr doc = xmlParseMemory(xml_buffer, length);

        if(doc == nullptr) {
            PrintToLog("ERROR: Unable to load XML Level: Document not parsed successfully!");
            return false;
        }

        xmlNodePtr root = xmlDocGetRootElement(doc);
        if(root == nullptr) {
            PrintToLog("ERROR: Unable to load XML Level: empty document!");
            xmlFreeDoc(doc);
            return false;
        }
        if(xmlStrcmp(root->name, (const xmlChar*)"photon_level")) {
            PrintToLog("ERROR: Unable to load XML Level: root node not photon_level!");
            xmlFreeDoc(doc);
            return false;
        }

        level = photon_level();
        lua::Reset();
        instance.gui.game.message.clear();

        xmlChar *width_str = xmlGetProp(root, (const xmlChar*)"width");
        xmlChar *height_str = xmlGetProp(root, (const xmlChar*)"height");

        int w = atoi((char*)width_str);
        int h = atoi((char*)height_str);

        if(w <= 0 || h <= 0){
            PrintToLog("WARNING: level \"%s\" dimensions are less than 1x1!", filename.c_str());
        }
        if(w > 250 || h > 250){
            PrintToLog("WARNING: level \"%s\" dimensions exceed 250x250! capping...", filename.c_str());
        }
        level.width = std::min(std::max(w, 1), 250);
        level.height = std::min(std::max(h, 1), 250);

        xmlFree(width_str);
        xmlFree(height_str);

        PrintToLog("INFO: Level size %i x %i", level.width, level.height);

        //because we fill the edges with indestructible blocks.
        level.width  += 2;
        level.height += 2;

        // fill the borders with indestructible blocks.
        for(int x = 0; x < level.width; x++){
            level.grid[photon_level_coord(x, 0               )].type = indestructible;
            level.grid[photon_level_coord(x, level.height - 1)].type = indestructible;
        }
        for(int y = 0; y < level.height; y++){
            level.grid[photon_level_coord(0,               y)].type = indestructible;
            level.grid[photon_level_coord(level.width - 1, y)].type = indestructible;
        }

        xmlChar *playerx_str = xmlGetProp(root, (const xmlChar*)"playerx");
        xmlChar *playery_str = xmlGetProp(root, (const xmlChar*)"playery");

        if(playerx_str && playery_str){
            player.location.x = atof((char*)playerx_str);
            player.location.y = atof((char*)playery_str);

            xmlFree(playerx_str);
            xmlFree(playery_str);
        }

        xmlNode *node = root->xmlChildrenNode;

        while(node != nullptr) {
            if((xmlStrEqual(node->name, (const xmlChar*)"data"))){
                xmlNode *row = node->xmlChildrenNode;
                while(row != nullptr){
                    if((xmlStrEqual(row->name, (const xmlChar*)"row"))){
                        xmlChar *y_str = xmlGetProp(row, (const xmlChar*)"y");
                        uint8_t y = atoi((char*)y_str);
                        xmlFree(y_str);

                        if(y >= level.height || y < 0){
                            PrintToLog("WARNING: Row location not within level bounds!");
                            row = row->next;
                            continue;
                        }

                        xmlNode *block_xml = row->xmlChildrenNode;
                        while(block_xml != nullptr) {
                            if((xmlStrEqual(block_xml->name, (const xmlChar*)"block"))){
                                xmlChar *x_str = xmlGetProp(block_xml, (const xmlChar*)"x");
                                uint8_t x = atoi((char*)x_str);
                                xmlFree(x_str);

                                if(x >= level.width || x < 0){
                                    PrintToLog("WARNING: Block location not within level bounds!");
                                    block_xml = block_xml->next;
                                    continue;
                                }

                                xmlChar *type_str = xmlGetProp(block_xml, (const xmlChar*)"type");

                                photon_block &block = level.grid[photon_level_coord(x,y)];

                                block.type = blocks::GetBlockFromName((char*)type_str);

                                if(block.type == mirror || block.type == mirror_locked ||
                                        block.type == emitter_white || block.type == emitter_red ||
                                        block.type == emitter_green || block.type == emitter_blue ||
                                        block.type == receiver_white || block.type == receiver_red ||
                                        block.type == receiver_green || block.type == receiver_blue ||
                                        block.type == receiver){

                                    xmlChar *angle_str = xmlGetProp(block_xml, (const xmlChar*)"angle");

                                    block.angle = atof((char*)angle_str);

                                    xmlFree(angle_str);
                                }

                                xmlFree(type_str);
                            }
                            block_xml = block_xml->next;
                        }
                    }

                    row = row->next;
                }
            }else if((xmlStrEqual(node->name, (const xmlChar*)"inventory"))){
                player.items.clear();
                xmlNode *item = node->xmlChildrenNode;
                while(item != nullptr) {
                    if((xmlStrEqual(item->name, (const xmlChar*)"item"))){
                        xmlChar *type_str = xmlGetProp(item, (const xmlChar*)"type");
                        block_type type = blocks::GetBlockFromName((char*)type_str);
                        if(type != invalid_block){
                            xmlChar *amount_str = xmlGetProp(item, (const xmlChar*)"amount");

                            if((xmlStrEqual(amount_str, (const xmlChar*)"infinite"))){
                                player::GiveInfiniteItems(player, type);
                            }else{
                                player::AddItem(player, type, atoi((char*)amount_str));
                            }

                            xmlFree(amount_str);
                        }

                        xmlFree(type_str);
                    }
                    item = item->next;
                }
            }
            node = node->next;
        }

        xmlChar *mode_str = xmlGetProp(root, (const xmlChar*)"mode");

        if(xmlStrEqual(mode_str, (const xmlChar*)"power")){
            level.mode = photon_level::power;
        }else if(xmlStrEqual(mode_str, (const xmlChar*)"targets")){
            level.mode = photon_level::targets;
        }else if(xmlStrEqual(mode_str, (const xmlChar*)"destruction")){
            level.mode = photon_level::destruction;
        }else if(xmlStrEqual(mode_str, (const xmlChar*)"tnt_harvester")){
            level.mode = photon_level::tnt_harvester;
        }else if(xmlStrEqual(mode_str, (const xmlChar*)"script")){
            xmlChar *script_str = xmlGetProp(root, (const xmlChar*)"script");

            if(script_str != nullptr){
                if(!lua::DoFile((char*)script_str)){
                    // only set the mode as script if it worked.
                    level.mode = photon_level::script;
                }

                xmlFree(script_str);
            }
        }else{
            level.mode = photon_level::none;
        }

        xmlFree(mode_str);

        xmlChar *goal_str = xmlGetProp(root, (const xmlChar*)"goal");

        if(goal_str != nullptr){
            level.goal = atoi((char*)goal_str);

            xmlFree(goal_str);
        }

        level.is_valid = true;

        delete[] xml_buffer;
        xmlFreeDoc(doc);
    }else{
        PrintToLog("ERROR: Unable to load XML Level: \"%s\" does not exist!", filename.c_str());
    }

    return level.is_valid;
}

void SaveLevelXML(const std::string &filename, const photon_level &level, const photon_player &player){
    xmlChar *xmlbuff;
    int buffersize;

    xmlDoc *doc = xmlNewDoc((const xmlChar*)"1.0");
    xmlNode *root = xmlNewNode(nullptr, (const xmlChar*)"photon_level");
    xmlDocSetRootElement(doc, root);

    xmlSetProp(root, (const xmlChar*)"width",  (const xmlChar*)std::to_string(level.width  - 2).c_str());
    xmlSetProp(root, (const xmlChar*)"height", (const xmlChar*)std::to_string(level.height - 2).c_str());

    xmlSetProp(root, (const xmlChar*)"playerx", (const xmlChar*)std::to_string(player.location.x).c_str());
    xmlSetProp(root, (const xmlChar*)"playery", (const xmlChar*)std::to_string(player.location.y).c_str());

    xmlNode *inventory = xmlNewNode(nullptr, (const xmlChar*)"inventory");
    xmlAddChild(root, inventory);

    for(auto item : player.items){
        xmlNode* item_xml = xmlNewNode(nullptr, (const xmlChar*)"item");
        xmlAddChild(inventory, item_xml);
        xmlSetProp(item_xml, (const xmlChar*)"type", (const xmlChar*)blocks::GetBlockName(item.first));

        if(item.second > 0){
            xmlSetProp(item_xml, (const xmlChar*)"amount", (const xmlChar*)std::to_string(item.second).c_str());
        }else{
            xmlSetProp(item_xml, (const xmlChar*)"amount", (const xmlChar*)"infinite");
        }
    }

    xmlNode *data = xmlNewNode(nullptr, (const xmlChar*)"data");
    xmlAddChild(root, data);

    std::map<uint8_t, xmlNode*> rows;

    for(auto block : level.grid){
        xmlNode* block_xml = xmlNewNode(nullptr, (const xmlChar*)"block");
        xmlSetProp(block_xml, (const xmlChar*)"x", (const xmlChar*)std::to_string(block.first.first).c_str());

        switch(block.second.type){
        case mirror:
        case mirror_locked:
        case emitter_white:
        case emitter_red:
        case emitter_green:
        case emitter_blue:
            xmlSetProp(block_xml, (const xmlChar*)"angle", (const xmlChar*)std::to_string(block.second.angle).c_str());
        case tnt:
            // TODO - store TNT warmup.
            break;
        case indestructible:
            if(block.first.first == 0 || block.first.first == level.width - 1 ||
                    block.first.second == 0 || block.first.second == level.height - 1){
                // block is a border block, no need to store.
                xmlFreeNode(block_xml);
                continue;
            }
            break;
        default:
            break;
        }

        xmlSetProp(block_xml, (const xmlChar*)"type", (const xmlChar*)blocks::GetBlockName(block.second.type));
        if(rows.count(block.first.second)){
            xmlAddChild(rows[block.first.second], block_xml);
        }else{
            xmlNode* row_xml = xmlNewNode(nullptr, (const xmlChar*)"row");
            xmlSetProp(row_xml, (const xmlChar*)"y", (const xmlChar*)std::to_string(block.first.second).c_str());
            xmlAddChild(data, row_xml);
            rows[block.first.second] = row_xml;
            xmlAddChild(row_xml, block_xml);
        }
    }

    // TODO - perhaps store what level originally loaded so people can come back to puzzles and still have it count? that will probably enable cheating though...

    xmlDocDumpFormatMemory(doc, &xmlbuff, &buffersize, 1);

    auto fp = PHYSFS_openWrite(filename.c_str());

    if(fp != nullptr){
        PHYSFS_write(fp, xmlbuff, 1, buffersize);

        PHYSFS_close(fp);
    }

    xmlFree(xmlbuff);
    xmlFreeDoc(doc);
}

}

}
