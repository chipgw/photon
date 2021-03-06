#include "photon_core.h"

#include <glm/gtx/norm.hpp>

namespace photon{

namespace blocks{

photon_lasersegment *OnLightInteract(photon_lasersegment *segment, glm::uvec2 location, photon_level &level, float time){
    photon_level_coord coord(location.x, location.y);
    if(level.grid.count(coord)){
        photon_block &block = level.grid[coord];

        bool wasactivated = block.activated;
        block.activated = true;

        switch(block.type){
        case air:
        default:
            break;
        case receiver:
            block.power++;
            // stops tracing the laser.
            return nullptr;
            break;
        case receiver_red:
            if(segment->color.r > 0.8f){
                block.power++;
            }
            // stops tracing the laser.
            return nullptr;
            break;
        case receiver_green:
            if(segment->color.g > 0.8f){
                block.power++;
            }
            // stops tracing the laser.
            return nullptr;
            break;
        case receiver_blue:
            if(segment->color.b > 0.8f){
                block.power++;
            }
            // stops tracing the laser.
            return nullptr;
            break;
        case receiver_white:
            if(segment->color.r > 0.8f && segment->color.g > 0.8f && segment->color.b > 0.8f){
                block.power++;
            }
            // stops tracing the laser.
            return nullptr;
            break;
        case tnt:
            block.power += time;
        case emitter_white:
        case emitter_red:
        case emitter_green:
        case emitter_blue:
        case plain:
        case indestructible:
        case target:
            // stops tracing the laser.
            return nullptr;
            break;
        case mirror:
        case mirror_locked:{
            float angle = segment->angle - block.angle;
            if(fmod(angle, 180.0f) == 0.0f){
                return nullptr;
            }
            angle = fmod(angle + 180.0f, 360.0f) - 180.0f;
            segment = tracer::CreateChildBeam(segment);
            segment->angle = block.angle - angle;
            break;
        }
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
        case move:
            if(!wasactivated){
                block.angle = fmod(segment->angle + 360.0f, 360.0f);
                block.power += time;

                photon_level_coord newcoord(coord);

                if(block.angle == 0.0f){
                    newcoord.first++;
                }else if(block.angle == 90.0f){
                    newcoord.second++;
                }else if(block.angle == 180.0f){
                    newcoord.first--;
                }else if(block.angle == 270.0f){
                    newcoord.second--;
                }

                if(!level.grid.count(newcoord)){
                    break;
                }
            }
            block.angle = 0.0f;
            block.power = 0.0f;
            break;
        case move_reverse:
            if(!wasactivated){
                block.angle = fmod(segment->angle + 540.0f, 360.0f);
                block.power += time;

                photon_level_coord newcoord(coord);

                if(block.angle == 0.0f){
                    newcoord.first++;
                }else if(block.angle == 90.0f){
                    newcoord.second++;
                }else if(block.angle == 180.0f){
                    newcoord.first--;
                }else if(block.angle == 270.0f){
                    newcoord.second--;
                }

                if(!level.grid.count(newcoord)){
                    break;
                }
            }
            block.angle = 0.0f;
            block.power = 0.0f;
            break;
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
    case receiver:
    case receiver_red:
    case receiver_green:
    case receiver_blue:
    case receiver_white:
        if(level.victory_state > 0){
            // TODO - show victory screen.
            PrintToLog("INFO: VICTORY!");
            level = photon_level();
        }
        // TODO - show some indication of incompleteness.
        break;
    case move:
        if(!block.locked){
            block.type = move_reverse;
            block.power = -block.power;
            level.moves++;
        }
        break;
    case move_reverse:
        if(!block.locked){
            block.type = move;
            block.power = -block.power;
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
        case mirror:
            if(counter_clockwise){
                block.angle += 22.5f;
            }else{
                block.angle -= 22.5f;
            }
            level.moves++;
        default:
            break;
        }
    }
}

void OnRotate(glm::uvec2 location, photon_level &level, float to_angle){
    photon_level_coord coord(location.x, location.y);
    if(level.grid.count(coord)){
        photon_block &block = level.grid[coord];
        switch(block.type){
        case mirror:{
            float angle = round(to_angle / 22.5f) * 22.5f;
            if(block.angle != angle){
                block.angle = angle;
                level.moves++;
            }
        }
        default:
            break;
        }
    }
}

void CreateBeam(photon_level &level, glm::vec3 color, glm::uvec2 location, float angle){
    level.beams.push_back(photon_laserbeam());
    photon_laserbeam &beam = level.beams.back();
    beam.color = color;
    beam.origin = location;
    beam.origin_angle = angle;
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
            if(block.power > 1.0f){
                DamageAroundPoint(location, level, 4.0f);
                // TODO - KABOOM goes here...
                PrintToLog("INFO: KABOOM!");
                block.type = tnt_fireball;
                // cooldown of fireball
                block.power = 1.0f;
                break;
            }
            // if block was not activated last frame cool down timer.
            if(!block.activated){
                block.power -= time;
                block.power = std::max(block.power, 0.0f);
            }
            break;
        case tnt_fireball:
            block.power -= time * 3.0f;
            if(block.power > 0.4f){
                DamageAroundPoint(location, level, 4.0f * (1.2f - block.power));
            }else if(block.power < 0.0f){
#ifdef _MSC_VER
                /* TODO - this is just a temporary workaround because MSVC's STL doesn't let you erase from within a range based for loop. */
                block.type = air;
#else
                level.grid.erase(coord);
#endif
            }
            break;
        case emitter_white:{
            CreateBeam(level, glm::vec3(0.9f), location, block.angle);
            break;
        }
        case emitter_red:{
            CreateBeam(level, glm::vec3(0.9f,0.2f,0.1f), location, block.angle);
            break;
        }
        case emitter_green:{
            CreateBeam(level, glm::vec3(0.1f,0.9f,0.2f), location, block.angle);
            break;
        }
        case emitter_blue:{
            CreateBeam(level, glm::vec3(0.1f,0.2f,0.9f), location, block.angle);
            break;
        }
        case receiver:
        case receiver_red:
        case receiver_green:
        case receiver_blue:
        case receiver_white:
            block.power = 0.0f;
            break;
        case move:
        case move_reverse:
            if(!block.activated){
                block.power = 0.0f;
            }else if(block.power >= 0.8f){
                block.power--;

                photon_level_coord newcoord(coord);

                if(block.angle == 0.0f){
                    newcoord.first++;
                }else if(block.angle == 90.0f){
                    newcoord.second++;
                }else if(block.angle == 180.0f){
                    newcoord.first--;
                }else if(block.angle == 270.0f){
                    newcoord.second--;
                }else{
                    break;
                }
                level.grid[newcoord] = block;

#ifdef _MSC_VER
                /* TODO - this is just a temporary workaround because MSVC's STL doesn't let you erase from within a range based for loop. */
                block.type = air;
#else
                level.grid.erase(coord);
#endif
            }
            break;
        }
        block.activated = false;
    }
}

void OnDamage(glm::uvec2 location, photon_level &level, float damage){
    photon_level_coord coord(location.x, location.y);
    if(level.grid.count(coord)){
        photon_block &block = level.grid[coord];
        switch(block.type){
        case plain:
        case target:
            if(damage > 0.5f){
                level.grid.erase(coord);
            }
            break;
        case tnt:
            // will make explosions trigger nearby TNT...
            block.power += damage;
        default:
            break;
        }
    }
}

void DamageAroundPoint(glm::uvec2 location, photon_level &level, float strength){
    glm::uvec2 dist(std::ceil(strength));
    glm::uvec2 current = glm::uvec2(glm::max(glm::ivec2(location) - glm::ivec2(dist), glm::ivec2(0)));
    glm::uvec2 end = location + dist;

    unsigned int y = current.y;
    for(;current.x <= end.x; current.x++){
        for(current.y = y; current.y <= end.y; current.y++){
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
        CASESTR(receiver);
        CASESTR(receiver_white);
        CASESTR(receiver_red);
        CASESTR(receiver_green);
        CASESTR(receiver_blue);
        CASESTR(target);
        CASESTR(tnt);
        CASESTR(tnt_fireball);
        CASESTR(filter_red);
        CASESTR(filter_green);
        CASESTR(filter_blue);
        CASESTR(filter_yellow);
        CASESTR(filter_cyan);
        CASESTR(filter_magenta);
        CASESTR(move);
        CASESTR(move_reverse);
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
    else CASESTR(receiver)
    else CASESTR(receiver_white)
    else CASESTR(receiver_red)
    else CASESTR(receiver_green)
    else CASESTR(receiver_blue)
    else CASESTR(target)
    else CASESTR(tnt)
    else CASESTR(tnt_fireball)
    else CASESTR(filter_red)
    else CASESTR(filter_green)
    else CASESTR(filter_blue)
    else CASESTR(filter_yellow)
    else CASESTR(filter_cyan)
    else CASESTR(filter_magenta)
    else CASESTR(move)
    else CASESTR(move_reverse)
    else{
        return invalid_block;
    }
}

#undef CASESTR

}

}
