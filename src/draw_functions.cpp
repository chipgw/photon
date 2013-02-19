#include "photon_opengl.h"
#include "photon_laser.h"

namespace photon{

namespace opengl{

void DrawLaserSegment(photon_lasersegment &segment){
    // TODO - code goes here
}

void DrawLaser(photon_laserbeam &beam){
    for(photon_lasersegment &segment : beam.segments){
        DrawLaserSegment(segment);
    }
}

}

}
