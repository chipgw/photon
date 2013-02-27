#include <GL/glew.h>

#include "photon_core.h"
#include "photon_opengl.h"
#include "photon_texture.h"
#include "photon_level.h"
#include "photon_laser.h"
#include "photon_blocks.h"
#include "photon_input.h"
#include <SDL_timer.h>

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/rotate_vector.hpp>

namespace photon{
void MainLoop(photon_instance &instance){
    instance.running = true;
    photon_laserbeam beam;

    beam.origin = glm::uvec2(0,1);
    beam.color = glm::vec3(1,0,0);
    beam.origin_angle = 0;

    instance.level = level::LoadLevelXML("/level.xml");

    PrintToLog("INFO: Main loop started at: %f seconds.", SDL_GetTicks()*0.001);
    float start_time = SDL_GetTicks();
    float last_time = SDL_GetTicks();
    float frame_delta = 0;

    instance.player.location = glm::vec2(5.0f,1.0f);

    while(instance.running){
        frame_delta = SDL_GetTicks() - last_time;
        last_time = SDL_GetTicks();

        input::DoEvents(instance, frame_delta);

        opengl::UpdateCenter(instance.player.location);

        opengl::DrawModeScene(instance.window);

        // TODO - draw a background texture or something...

        opengl::DrawModeLaser(instance.window);

        tracer::TraceBeam(beam, instance.level);

        opengl::DrawLaser(beam);

        opengl::DrawModeLevel(instance.window);

        level::Draw(instance.level);

        opengl::DrawModeLight(instance.window);

        opengl::DrawLaserLight(beam);

        sdl::UpdateWindow(instance.window);

        instance.total_frames++;
    }

    // print total amount of seconds from SDL_Init()
    PrintToLog("INFO: Total Time: %f seconds.", SDL_GetTicks()*0.001);
    // print total frame count.
    PrintToLog("INFO: Total Frames: %i", instance.total_frames);

    // print average draw time by dividing the total amount of time from the start of the main loop by the total amount of frames.
    PrintToLog("INFO: Average Draw Time: %fms.",(SDL_GetTicks()-start_time)/((float) instance.total_frames));
    // print average framerate by inverting the total draw time.
    PrintToLog("INFO: Average Framerate: %f fps.", 1/(((SDL_GetTicks()-start_time)*0.001)/((float) instance.total_frames)));
}

}
