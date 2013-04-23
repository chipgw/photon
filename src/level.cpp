#include "photon_core.h"

namespace photon{

namespace level{

void Draw(photon_level &level){
    for(auto &block : level.grid){
        blocks::Draw(block.second, glm::uvec2(block.first.first, block.first.second));
    }
}

void DrawBeams(photon_level &level){
    for(photon_laserbeam &beam : level.beams){
        opengl::DrawLaser(beam);
    }
}

void DrawBeamsLight(photon_level &level){
    for(photon_laserbeam &beam : level.beams){
        opengl::DrawLaserLight(beam);
    }
}

void DrawFX(photon_level &level){
    for(auto &block : level.grid){
        blocks::DrawFX(block.second, glm::uvec2(block.first.first, block.first.second));
    }
}

void AdvanceFrame(photon_level &level, photon_player &player, float time){
    level.beams.clear();
    for(auto &block : level.grid){
        blocks::OnFrame(glm::uvec2(block.first.first, block.first.second), level, time);
    }
    for(photon_laserbeam &beam : level.beams){
        tracer::TraceBeam(beam, level, time);
    }

    level.time += time;

    // if victory_state is 0 before checking but not after then defeat occured. (keeps it from repeating message over and over)
    if(level.victory_state == 0 && level::CheckVictory(level, player) < 0){
        PrintToLog("INFO: DEFEAT! (game is unwinnable)");
    }
}

int8_t SetVictoryState(photon_level &level, int8_t victory){
    level.end_time = level.time;
    level.victory_state = victory;
    return victory;
}

int8_t CheckVictory(photon_level &level, photon_player &player){
    if(level.victory_state == 0){
        switch(level.mode){
        case photon_level::none:
            return SetVictoryState(level, 1);
            break;
        case photon_level::power:{
            for(auto &block : level.grid){
                if(block.second.type == reciever){
                    // if any reciever is not powered return false.
                    if(block.second.power < 0.9f){
                        return 0;
                    }
                }
            }
            return SetVictoryState(level, 1);
            break;
        }
        case photon_level::targets:
            // TODO - implement.
            return SetVictoryState(level, 1);
            break;
        case photon_level::destruction:
            // TODO - implement.
            return SetVictoryState(level, 1);
            break;
        case photon_level::tnt_harvester:
            if(player::GetItemCount(player, tnt) >= level.goal){
                return SetVictoryState(level, 1);
            }else{
                int32_t tnt_count = 0;
                for(auto block : level.grid){
                    if(block.second.type == tnt){
                        tnt_count++;
                    }
                }
                if(player::GetItemCount(player, tnt) + tnt_count < level.goal){
                    // not enough tnt in inventory or level, game cannot be won.
                    return SetVictoryState(level, -1);
                }
            }

            break;
        }
    }
    return level.victory_state;
}

}

}
