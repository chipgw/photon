#ifndef _PHOTON_LEVEL_H_
#define _PHOTON_LEVEL_H_

#include "boost/multi_array.hpp"
#include "photon_blocks.h"

namespace photon{
struct photon_level{
    boost::multi_array<photon_block,2> grid;

    photon_level(int w, int h) : grid(boost::extents[w][h]) {}
    photon_level() {}

    // really annoying that boost::multi_array requires this, but it does so here it is.
    photon_level& operator= (const photon_level &other){
        // Check for self-assignment
        if (this != &other){
            this->grid.resize(boost::extents[other.grid.shape()[0]][other.grid.shape()[1]]);
            this->grid = other.grid;
        }
        return *this;
    }
};

namespace level{

void Draw(photon_level &level);

void DrawFX(photon_level &level);

photon_level LoadLevelXML(const std::string &filename);

void AdvanceFrame(photon_level &level, float time);

}

}
#endif
