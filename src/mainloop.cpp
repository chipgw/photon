#include "photon_core.h"
#include "photon_texture.h"

#include <GL/glew.h>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/rotate_vector.hpp>

namespace photon{

void MainLoop(photon_instance &instance){
#ifndef NDEBUG
    instance.level = level::LoadLevelXML("/level.xml", instance.player);
#endif

    instance.running = true;

    std::chrono::high_resolution_clock::time_point start_time = std::chrono::high_resolution_clock::now();
    std::chrono::high_resolution_clock::time_point last_time = std::chrono::high_resolution_clock::now();
    float frame_delta = 0;

    PrintToLog("INFO: Main loop started at: %f seconds.", (start_time - instance.creation_time));

    while(instance.running){
        std::chrono::high_resolution_clock::time_point current = std::chrono::high_resolution_clock::now();
        frame_delta = std::chrono::duration_cast<std::chrono::microseconds>(current - last_time).count() * 1.0e-6f;
        last_time = current;

        input::DoEvents(instance);

        input::DoInput(instance, frame_delta);

        if(instance.level.is_valid){
            if(!instance.paused){
                level::AdvanceFrame(instance.level, instance.player, frame_delta);
            }

            instance.zoom = std::max(0.01f, instance.zoom);
            opengl::UpdateZoom(instance.zoom);

            instance.player.location = player::SnapToBeams(instance.level.beams, instance.player.location);

            opengl::UpdateCenter(instance.player.location);

            opengl::DrawModeLight(instance.window);

            level::DrawBeamsLight(instance.level);

            opengl::SetLaserColor(glm::vec3(1.0f));

            opengl::DrawPhotonLight(instance.player.location);

            opengl::DrawModeScene(instance.window);

            opengl::DrawBackground(instance);

            opengl::DrawModeLaser(instance.window);

            level::DrawBeams(instance.level);

            opengl::DrawModeLevel(instance.window);

            level::Draw(instance.level);

            opengl::DrawModeFX(instance.window);

            opengl::DrawPhoton(instance.player.location);

            level::DrawFX(instance.level);
        }else{
            // this is so that the screen gets cleared.
            opengl::DrawModeScene(instance.window);

            // TODO - use a seperate background for the main menu.
            opengl::DrawBackground(instance);
        }

        opengl::DrawModeGUI(instance.window);

        gui::DrawGUI(instance);

        window_managment::UpdateWindow(instance.window);

        instance.total_frames++;
    }

    // print total amount of time since instance was created.
    std::chrono::high_resolution_clock::time_point current = std::chrono::high_resolution_clock::now();
    PrintToLog("INFO: Total Time: %f seconds.", (std::chrono::duration_cast<std::chrono::microseconds>(current - instance.creation_time).count() * 1.0e-6f));
    // print total frame count.
    PrintToLog("INFO: Total Frames: %i", instance.total_frames);

    // print average draw time by dividing the total amount of time from the start of the main loop by the total amount of frames.
    PrintToLog("INFO: Average Draw Time: %fms.", (std::chrono::duration_cast<std::chrono::microseconds>(current - start_time).count() * 1.0e-3f) / float(instance.total_frames));
    // print average framerate by inverting the total draw time.
    PrintToLog("INFO: Average Framerate: %f fps.", (1.0f / (std::chrono::duration_cast<std::chrono::microseconds>(current - start_time).count() / float(instance.total_frames))) * 1.0e6f);
}

}
