#ifndef _PHOTON_LEVEL_H_
#define _PHOTON_LEVEL_H_

#include "boost/multi_array.hpp"
#include "photon_block.h"

namespace photon{
struct photon_level{
    boost::multi_array<photon_block,2> grid;
};

namespace level{

}

}
#endif
