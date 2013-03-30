#include "photon_blocks.h"
#include "photon_level.h"
#include "photon_laser.h"
#include "photon_core.h"

#include <glm/gtx/norm.hpp>

namespace photon{

namespace blocks{

photon_lasersegment *OnLightInteract(photon_lasersegment *segment, glm::uvec2 location, photon_level &level, float time){
    photon_level_coord coord(location.x, location.y);
    if(level.grid.count(coord)){
        photon_block &block = level.grid[coord];
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
        case mirror_locked:{
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
    }
    return segment;
}

void OnPhotonInteract(glm::uvec2 location, photon_level &level, photon_player &player){
    photon_level_coord coord(location.x, location.y);
    photon_block &block = level.grid[coord];
    switch(block.type){
    case air:
        if(player.current_item != invalid_block){
            block.type = player.current_item;
            player::AddItemCurrent(player, -1);
            level.moves++;
        }
        break;
    default:
        break;
    case tnt:
    case filter_red:
    case filter_green:
    case filter_blue:
    case filter_yellow:
    case filter_cyan:
    case filter_magenta:
    case mirror:
        if(!block.locked){
            player::AddItem(player, block.type);
            level.grid.erase(coord);
            level.moves++;
        }
        break;
    }
    // if for some reason nothing happened to this block after it was added...
    if(block.type == air){
        level.grid.erase(coord);
    }
}

void OnRotate(glm::uvec2 location, photon_level &level, bool counter_clockwise){
    photon_level_coord coord(location.x, location.y);
    if(level.grid.count(coord)){
        photon_block &block = level.grid[coord];
        switch(block.type){
        default:
            break;
        case mirror:
            if(counter_clockwise){
                block.data += 22.5f;
            }else{
                block.data -= 22.5f;
            }
            level.moves++;
            break;
        }
    }
}

void OnFrame(glm::uvec2 location, photon_level &level, float time){
    photon_level_coord coord(location.x, location.y);
    if(level.grid.count(coord)){
        photon_block &block = level.grid[coord];
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
                level.grid.erase(coord);
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
}

void OnDamage(glm::uvec2 location, photon_level &level, float damage){
    photon_level_coord coord(location.x, location.y);
    if(level.grid.count(coord)){
        photon_block &block = level.grid[coord];
        switch(block.type){
        default:
            break;
        case plain:
            if(damage > 0.5f){
                level.grid.erase(coord);
            }
            break;
        case tnt:
            // will make explosions trigger nearby TNT...
            block.data += damage * 2.0f;
            break;
        }
    }
}

void DamageAroundPoint(glm::uvec2 location, photon_level &level, float strength){
    glm::uvec2 dist(std::ceil(strength));
    glm::uvec2 current = glm::uvec2(glm::max(glm::ivec2(location) - glm::ivec2(dist), glm::ivec2(0)));
    glm::uvec2 end = location + dist;

    unsigned int y = current.y;
    for(;current.x <= end.x;current.x++){
        for(current.y = y;current.y <= end.y;current.y++){
            OnDamage(current, level, std::max(2.0f - (glm::distance2(glm::vec2(current), glm::vec2(location)) / strength), 0.0f));
        }
    }
}
#define CASESTR(X) case X: return #X; break;

const char* GetBlockName(block_type type){
    switch(type){
    default:
        CASESTR(invalid_block);
        CASESTR(air);
        CASESTR(mirror);
        CASESTR(mirror_locked);
        CASESTR(plain);
        CASESTR(indestructible);
        CASESTR(emitter_white);
        CASESTR(emitter_red);
        CASESTR(emitter_green);
        CASESTR(emitter_blue);
        CASESTR(reciever);
        CASESTR(reciever_white);
        CASESTR(reciever_red);
        CASESTR(reciever_green);
        CASESTR(reciever_blue);
        CASESTR(tnt);
        CASESTR(tnt_fireball);
        CASESTR(filter_red);
        CASESTR(filter_green);
        CASESTR(filter_blue);
        CASESTR(filter_yellow);
        CASESTR(filter_cyan);
        CASESTR(filter_magenta);
    }
}
#undef CASESTR
#define CASESTR(X) if(!strcmp(name, #X)){ return X; }

block_type GetBlockFromName(const char* name){
    CASESTR(invalid_block)
    else CASESTR(air)
    else CASESTR(mirror)
    else CASESTR(mirror_locked)
    else CASESTR(plain)
    else CASESTR(indestructible)
    else CASESTR(emitter_white)
    else CASESTR(emitter_red)
    else CASESTR(emitter_green)
    else CASESTR(emitter_blue)
    else CASESTR(reciever)
    else CASESTR(reciever_white)
    else CASESTR(reciever_red)
    else CASESTR(reciever_green)
    else CASESTR(reciever_blue)
    else CASESTR(tnt)
    else CASESTR(tnt_fireball)
    else CASESTR(filter_red)
    else CASESTR(filter_green)
    else CASESTR(filter_blue)
    else CASESTR(filter_yellow)
    else CASESTR(filter_cyan)
    else CASESTR(filter_magenta)
    else{
        return invalid_block;
    }
}

#undef CASESTR

}

}
