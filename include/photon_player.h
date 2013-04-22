#ifndef _PHOTON_PLAYER_H_
#define _PHOTON_PLAYER_H_

#include <glm/glm.hpp>
#include <vector>
#include <map>
#include "photon_laser.h"
#include "photon_blocks.h"

namespace photon{

struct photon_player{
    glm::vec2 location = glm::vec2(0.0f);

    std::map<block_type, int8_t> items;
    block_type current_item = invalid_block;
};

namespace player{

glm::vec2 SnapToBeam(photon_laserbeam &beam, glm::vec2 location);
glm::vec2 SnapToBeams(std::vector<photon_laserbeam> &beams, glm::vec2 location);

int8_t AddItem(photon_player &player, block_type type, int8_t amount = 1);
int8_t AddItemCurrent(photon_player &player, int8_t amount = 1);
int8_t GetItemCount(photon_player &player, block_type type);
int8_t GetItemCountCurrent(photon_player &player);
block_type CurrentItem(photon_player &player);
block_type NextItem(photon_player &player);
block_type PreviousItem(photon_player &player);

void GiveInfiniteItems(photon_player &player, block_type type);
}

}

#endif
