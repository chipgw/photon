#ifndef _PHOTON_PLAYER_H_
#define _PHOTON_PLAYER_H_

#include <glm/glm.hpp>
#include <photon_laser.h>
#include <vector>

namespace photon{

struct photon_player{
    glm::vec2 location;

    // TODO - keep a list of items at the player's disposal.
};

namespace player{

glm::vec2 SnapToBeam(photon_laserbeam &beam, glm::vec2 location);
glm::vec2 SnapToBeams(std::vector<photon_laserbeam> &beams, glm::vec2 location);

}

}

#endif
