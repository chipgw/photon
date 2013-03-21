#include "photon_blocks.h"
#include "photon_level.h"
#include "photon_laser.h"
#include "photon_core.h"

#include <glm/gtx/norm.hpp>

namespace photon{

namespace blocks{

photon_lasersegment *OnLightInteract(photon_lasersegment *segment, glm::uvec2 location, photon_level &level, float time){
    photon_block &block = level.grid[location.x][location.y];
    block.activated = true;

    switch(block.type){
    case air:
    default:
        break;
    case reciever:
        // TODO - make trigger
        // stops tracing the laser.
        return nullptr;
        break;
    case emitter_white:
    case emitter_red:
    case emitter_green:
    case emitter_blue:
    case plain:
    case indestructible:
        // stops tracing the laser.
        return nullptr;
        break;
    case mirror:
    case mirror_locked:
    case mirror_locked_pos:{
        float angle = segment->angle - block.data;
        if(fmod(angle, 180.0f) == 0.0f){
            return nullptr;
        }
        angle = fmod(angle + 180.0f, 360.0f) - 180.0f;
        segment = tracer::CreateChildBeam(segment);
        segment->angle = block.data - angle;
        break;
    }
    case tnt:
        block.data += time;
        // stops tracing the laser.
        return nullptr;
        break;
    case filter_red:{
        glm::vec3 color = segment->color;
        color.g = glm::min(color.g, 0.2f);
        color.b = glm::min(color.b, 0.1f);
        if(glm::length2(color) > 0.2f){
            segment = tracer::CreateChildBeam(segment);
            segment->color = color;
        }else{
            // stops tracing the laser.
            return nullptr;
        }
        break;
    }
    case filter_green:{
        glm::vec3 color = segment->color;
        color.r = glm::min(color.r, 0.1f);
        color.b = glm::min(color.b, 0.1f);
        if(glm::length2(color) > 0.2f){
            segment = tracer::CreateChildBeam(segment);
            segment->color = color;
        }else{
            // stops tracing the laser.
            return nullptr;
        }
        break;
    }
    case filter_blue:{
        glm::vec3 color = segment->color;
        color.r = glm::min(color.r, 0.1f);
        color.g = glm::min(color.g, 0.2f);
        if(glm::length2(color) > 0.2f){
            segment = tracer::CreateChildBeam(segment);
            segment->color = color;
        }else{
            // stops tracing the laser.
            return nullptr;
        }
        break;
    }
    case filter_yellow:{
        glm::vec3 color = segment->color;
        color.b = glm::min(color.b, 0.1f);
        if(glm::length2(color) > 0.2f){
            segment = tracer::CreateChildBeam(segment);
            segment->color = color;
        }else{
            // stops tracing the laser.
            return nullptr;
        }
        break;
    }
    case filter_cyan:{
        glm::vec3 color = segment->color;
        color.r = glm::min(color.r, 0.1f);
        if(glm::length2(color) > 0.2f){
            segment = tracer::CreateChildBeam(segment);
            segment->color = color;
        }else{
            // stops tracing the laser.
            return nullptr;
        }
        break;
    }
    case filter_magenta:{
        glm::vec3 color = segment->color;
        color.g = glm::min(color.g, 0.2f);
        if(glm::length2(color) > 0.2f){
            segment = tracer::CreateChildBeam(segment);
            segment->color = color;
        }else{
            // stops tracing the laser.
            return nullptr;
        }
        break;
    }
    }

    return segment;
}

void OnPhotonInteract(glm::uvec2 location, photon_level &level, photon_player &player){
    if(level.grid.shape()[0] > location.x && level.grid.shape()[1] > location.y){
        photon_block &block = level.grid[location.x][location.y];
        switch(block.type){
        case air:
            if(player.current_item != invalid_block){
                block.type = player.current_item;
                player::AddItemCurrent(player, -1);
            }
            break;
        default:
            break;
        case tnt:
        case mirror:
            player::AddItem(player, block.type);
            block.type = air;
            block.data = 0.0f;
            break;
        }
    }
}

void OnRotate(glm::uvec2 location, photon_level &level, bool counter_clockwise){
    if(level.grid.shape()[0] > location.x && level.grid.shape()[1] > location.y){
        photon_block &block = level.grid[location.x][location.y];
        switch(block.type){
        default:
            break;
        case mirror:
        case mirror_locked_pos:
            if(counter_clockwise){
                block.data += 22.5f;
            }else{
                block.data -= 22.5f;
            }
            break;
        }
    }
}

void OnFrame(glm::uvec2 location, photon_level &level, float time){
    photon_block &block = level.grid[location.x][location.y];
    switch(block.type){
    default:
        break;
    case tnt:
        // if block is triggered, explode.
        if(block.data > 1.0f){
            DamageAroundPoint(location, level, 4.0f);
            // TODO - KABOOM goes here...
            PrintToLog("INFO: KABOOM!");
            block.type = tnt_fireball;
            // cooldown of fireball
            block.data = 1.0f;
            break;
        }
        // if block was not activated last frame cool down timer.
        if(!block.activated){
            block.data -= time;
            block.data = std::max(block.data, 0.0f);
        }
        break;
    case tnt_fireball:
        block.data -= time * 3.0f;
        if(block.data < 0.0f){
            block.data = 0.0f;
            block.type = air;
        }
        break;
    case emitter_white:{
        level.beams.push_back(photon_laserbeam());
        photon_laserbeam &beam = level.beams.back();
        beam.color = glm::vec3(0.9f,0.9f,0.9f);
        beam.origin = location;
        beam.origin_angle = block.data;
        break;
    }
    case emitter_red:{
        level.beams.push_back(photon_laserbeam());
        photon_laserbeam &beam = level.beams.back();
        beam.color = glm::vec3(0.9f,0.2f,0.1f);
        beam.origin = location;
        beam.origin_angle = block.data;
        break;
    }
    case emitter_green:{
        level.beams.push_back(photon_laserbeam());
        photon_laserbeam &beam = level.beams.back();
        beam.color = glm::vec3(0.1f,0.9f,0.2f);
        beam.origin = location;
        beam.origin_angle = block.data;
        break;
    }
    case emitter_blue:{
        level.beams.push_back(photon_laserbeam());
        photon_laserbeam &beam = level.beams.back();
        beam.color = glm::vec3(0.1f,0.2f,0.9f);
        beam.origin = location;
        beam.origin_angle = block.data;
        break;
    }
    }
    block.activated = false;
}

void OnDamage(glm::uvec2 location, photon_level &level, float damage){
    photon_block &block = level.grid[location.x][location.y];
    switch(block.type){
    default:
        break;
    case plain:
        if(damage > 0.5f){
            block.type = air;
        }
        break;
    case tnt:
        // will make explosions trigger nearby TNT...
        block.data += damage * 2.0f;
        break;
    }
}

void DamageAroundPoint(glm::uvec2 location, photon_level &level, float strength){
    glm::uvec2 dist(std::ceil(strength));
    glm::uvec2 current = glm::uvec2(glm::max(glm::ivec2(location) - glm::ivec2(dist), glm::ivec2(0)));
    glm::uvec2 end = location + dist;

    if(end.x >= level.grid.shape()[0]){
        end.x = level.grid.shape()[0] - 1;
    }
    if(end.y >= level.grid.shape()[1]){
        end.y = level.grid.shape()[1] - 1;
    }

    unsigned int y = current.y;
    for(;current.x <= end.x;current.x++){
        for(current.y = y;current.y <= end.y;current.y++){
            OnDamage(current, level, std::max(2.0f - (glm::distance2(glm::vec2(current), glm::vec2(location)) / strength), 0.0f));
        }
    }
}

}

}
