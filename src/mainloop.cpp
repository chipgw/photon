#include <GL/glew.h>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/rotate_vector.hpp>

#include "photon_core.h"
#include "photon_opengl.h"
#include "photon_texture.h"
#include "photon_level.h"
#include "photon_laser.h"
#include "photon_blocks.h"
#include "photon_input.h"
#include "photon_gui.h"

namespace photon{

void MainLoop(photon_instance &instance){
    instance.running = true;

    instance.level = level::LoadLevelXML("/level.xml", instance.player);

    instance.player.location = glm::vec2(1.0f, 1.0f);

    instance.input = input::LoadConfig("/config/keyboard.xml");

    for(int i = 0; i < SDL_NumJoysticks(); i++){
        if(SDL_IsGameController(i)){
            SDL_GameController *controller = SDL_GameControllerOpen(0);

            if(controller != nullptr){
                instance.input = input::LoadConfig("/config/controller.xml");
                instance.input.controller = controller;

                PrintToLog("INFO: Game controller found, using...");
                break;
            }
        }
    }

    // TODO - find a proper place to put this... probably some structure that holds all GUI states and put that in the instance...
    photon_gui_game game_gui = gui::InitGameGUI();

    std::chrono::high_resolution_clock::time_point start_time = std::chrono::high_resolution_clock::now();
    std::chrono::high_resolution_clock::time_point last_time = std::chrono::high_resolution_clock::now();
    float frame_delta = 0;

    PrintToLog("INFO: Main loop started at: %f seconds.", (start_time - instance.creation_time));

    while(instance.running){
        std::chrono::high_resolution_clock::time_point current = std::chrono::high_resolution_clock::now();
        frame_delta = std::chrono::duration_cast<std::chrono::microseconds>(current - last_time).count() * 1.0e-6f;
        last_time = current;

        input::DoEvents(instance, frame_delta);

        input::DoInput(instance, frame_delta);

        level::AdvanceFrame(instance.level, frame_delta);

        instance.zoom = std::max(0.01f, instance.zoom);
        opengl::UpdateZoom(instance.zoom);

        instance.player.location = player::SnapToBeams(instance.level.beams, instance.player.location);

        opengl::UpdateCenter(instance.player.location);

        opengl::DrawModeLight(instance.window);

        level::DrawBeamsLight(instance.level);

        opengl::SetLaserColor(glm::vec3(1.0f));

        opengl::DrawPhotonLight(instance.player.location);

        opengl::DrawModeScene(instance.window);

        // TODO - draw a background texture or something...

        opengl::DrawModeLaser(instance.window);

        level::DrawBeams(instance.level);

        opengl::DrawModeLevel(instance.window);

        level::Draw(instance.level);

        opengl::DrawModeFX(instance.window);

        opengl::DrawPhoton(instance.player.location);

        level::DrawFX(instance.level);

        opengl::DrawModeGUI(instance.window);

        gui::DrawGameGUI(game_gui, instance);

        opengl::SetCenterGUI(glm::vec2(0.0f,-1.0f));
        gui::RenderText(glm::vec2(0.0f), glm::vec2(0.05f), glm::vec4(0.8f, 0.4f, 0.1f, 0.8f), false, "FPS: %f", 1.0f / frame_delta);

        window_managment::UpdateWindow(instance.window);

        instance.total_frames++;
    }

    // print total amount of time since instance was created.
    std::chrono::high_resolution_clock::time_point current = std::chrono::high_resolution_clock::now();
    PrintToLog("INFO: Total Time: %f seconds.", (std::chrono::duration_cast<std::chrono::microseconds>(current - instance.creation_time).count() * 1.0e-6f));
    // print total frame count.
    PrintToLog("INFO: Total Frames: %i", instance.total_frames);

    // print average draw time by dividing the total amount of time from the start of the main loop by the total amount of frames.
    PrintToLog("INFO: Average Draw Time: %fms.", (std::chrono::duration_cast<std::chrono::microseconds>(current - start_time).count() * 1.0e-3f) / (float)instance.total_frames);
    // print average framerate by inverting the total draw time.
    PrintToLog("INFO: Average Framerate: %f fps.", (1.0f / (std::chrono::duration_cast<std::chrono::microseconds>(current - start_time).count() / (float)instance.total_frames)) * 1.0e6f);
}

}
