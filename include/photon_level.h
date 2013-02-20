#ifndef _PHOTON_LEVEL_H_
#define _PHOTON_LEVEL_H_

#include "boost/multi_array.hpp"
#include "photon_blocks.h"

namespace photon{
struct photon_level{
    boost::multi_array<photon_block,2> grid;

    photon_level(int w, int h) : grid(boost::extents[w][h]) {}
};

namespace level{

void Draw(photon_level &level);

}

}
#endif
