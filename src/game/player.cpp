#include "photon_core.h"

#include <glm/gtx/norm.hpp>

namespace photon{

namespace player{

glm::vec2 PointOnSegment(photon_lasersegment &segment, glm::vec2 location){
    glm::vec2 start(segment.start);
    glm::vec2 end(segment.end);

    const float segment_length_sqr = glm::distance2(start, end);

    if(segment_length_sqr == 0.0){
        return glm::vec2(segment.start);
    }

    const float t = glm::dot(location - start, end - start) / segment_length_sqr;

    if(t < 0.0){
        return start;
    }
    else if(t > 1.0){
        return end;
    }

    return start + t * (end - start);
}

glm::vec2 SnapToBeam(photon_laserbeam &beam, glm::vec2 location){
    float dist = INFINITY;
    glm::vec2 loc = location;

    for(photon_lasersegment &segment : beam.segments){
        glm::vec2 point = PointOnSegment(segment, location);
        float tdist = glm::distance(point, location);
        if(tdist < dist){
            dist = tdist;
            loc = point;
        }
    }
    return loc;
}

glm::vec2 SnapToBeams(std::vector<photon_laserbeam> &beams, glm::vec2 location){
    float dist = INFINITY;
    glm::vec2 loc = location;

    for(photon_laserbeam &beam : beams){
        for(photon_lasersegment &segment : beam.segments){
            glm::vec2 point = PointOnSegment(segment, location);
            float tdist = glm::distance(point, location);
            if(tdist < dist){
                dist = tdist;
                loc = point;
            }
        }
    }
    return loc;
}

int8_t AddItem(photon_player &player, block_type type, int8_t amount){
    // if it exists and is <= 0 it is infinite
    if(player.items.count(type) && player.items[type] <= 0){
        return -1;
    }
    player.items[type] += amount;
    // if it is <= 0 it was not so at first so it is empty, not infinite.
    if(player.items[type] <= 0){
        player.items.erase(type);
        return 0;
    }
    return player.items[type];
}

int8_t AddItemCurrent(photon_player &player, int8_t amount){
    if(player.items.count(player.current_item)){
        int8_t &value = player.items[player.current_item];
        if(value <= 0){
            return -1;
        }
        value += amount;
        // if it is <= 0 it was not so at first so it is empty, not infinite.
        if(value <= 0){
            player.items.erase(player.current_item);
            NextItem(player);
            return 0;
        }
        return value;
    }
    return 0;
}

int8_t GetItemCount(photon_player &player, block_type type){
    if(player.items.count(type)){
        if(player.items[type] <= 0){
            return -1;
        }
        return player.items[type];
    }
    return 0;
}

int8_t GetItemCountCurrent(photon_player &player){
    if(player.items.size() > 0){
        if(!player.items.count(player.current_item)){
            NextItem(player);
        }
        if(player.items[player.current_item] <= 0){
            return -1;
        }
        return player.items[player.current_item];
    }
    return 0;
}

block_type CurrentItem(photon_player &player){
    if(player.items.size() > 0){
        if(!player.items.count(player.current_item)){
            return NextItem(player);
        }
        return player.current_item;
    }else{
        player.current_item =  invalid_block;
        return invalid_block;
    }
}

block_type NextItem(photon_player &player){
    if(player.items.size() > 0){
        auto current = player.items.find(player.current_item);
        if(current == player.items.end()){
            // temporarily create the item in the list so we can retrieve what is after it.
            block_type tmp = player.current_item;
            player.items[tmp];
            NextItem(player);
            player.items.erase(tmp);

            return player.current_item;
        }

        ++current;

        if(current == player.items.end()){
            current = player.items.begin();
        }
        player.current_item = current->first;

#ifndef NDEBUG
        PrintToLog("DEBUG: NextItem() switched current item to: %i (%s)", player.current_item, blocks::GetBlockName(player.current_item));
#endif
        return player.current_item;
    }else{
        player.current_item =  invalid_block;
        return invalid_block;
    }
}

block_type PreviousItem(photon_player &player){
    if(player.items.size() > 0){
        auto current = player.items.find(player.current_item);
        if(current == player.items.end()){
            // temporarily create the item in the list so we can retrieve what is before it.
            block_type tmp = player.current_item;
            player.items[tmp];
            PreviousItem(player);
            player.items.erase(tmp);

            return player.current_item;
        }

        if(current == player.items.begin()){
            current = player.items.end();
        }
        --current;
        player.current_item = current->first;

#ifndef NDEBUG
        PrintToLog("DEBUG: PreviousItem() switched current item to: %i (%s)", player.current_item, blocks::GetBlockName(player.current_item));
#endif
        return player.current_item;

    }else{
        player.current_item =  invalid_block;
        return invalid_block;
    }
}

void GiveInfiniteItems(photon_player &player, block_type type){
    if(type != invalid_block){
        player.items[type] = -1;
    }
}

}

}
