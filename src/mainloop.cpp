#include <GL/glew.h>

#include "photon_core.h"
#include "photon_opengl.h"
#include "photon_texture.h"
#include "photon_level.h"
#include "photon_laser.h"
#include "photon_blocks.h"
#include "photon_input.h"
#include "photon_gui.h"
#include <SDL_timer.h>

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/rotate_vector.hpp>

namespace photon{
void MainLoop(photon_instance &instance){
    instance.running = true;

    instance.level = level::LoadLevelXML("/level.xml");

    instance.player.location = glm::vec2(1.0f,1.0f);

    instance.input_game.move_positive_x = input::CreateKeyboardInput(SDL_SCANCODE_D);
    instance.input_game.move_negative_x = input::CreateKeyboardInput(SDL_SCANCODE_A);

    instance.input_game.move_positive_y = input::CreateKeyboardInput(SDL_SCANCODE_W);
    instance.input_game.move_negative_y = input::CreateKeyboardInput(SDL_SCANCODE_S);

    instance.input_game.interact = input::CreateKeyboardInput(SDL_SCANCODE_SPACE);

    instance.input_game.rotate_clockwise = input::CreateKeyboardInput(SDL_SCANCODE_E);
    instance.input_game.rotate_counter_clockwise = input::CreateKeyboardInput(SDL_SCANCODE_Q);

    instance.input_game.zoom_in = input::CreateKeyboardInput(SDL_SCANCODE_KP_PLUS);
    instance.input_game.zoom_out = input::CreateKeyboardInput(SDL_SCANCODE_KP_MINUS);

    if(SDL_IsGameController(0)){
        SDL_GameController *controller = SDL_GameControllerOpen(0);

        if(controller != nullptr){
            instance.input_game.move_positive_x = input::CreateControllerAxisInput(controller, SDL_CONTROLLER_AXIS_LEFTX);
            instance.input_game.move_negative_x = input::CreateControllerAxisInput(controller, SDL_CONTROLLER_AXIS_LEFTX, true);

            instance.input_game.move_positive_y = input::CreateControllerAxisInput(controller, SDL_CONTROLLER_AXIS_LEFTY, true);
            instance.input_game.move_negative_y = input::CreateControllerAxisInput(controller, SDL_CONTROLLER_AXIS_LEFTY);

            instance.input_game.interact = input::CreateControllerButtonInput(controller, SDL_CONTROLLER_BUTTON_A);

            instance.input_game.rotate_clockwise = input::CreateControllerButtonInput(controller, SDL_CONTROLLER_BUTTON_LEFTSHOULDER);
            instance.input_game.rotate_counter_clockwise = input::CreateControllerButtonInput(controller, SDL_CONTROLLER_BUTTON_RIGHTSHOULDER);

            instance.input_game.zoom_in = input::CreateControllerAxisInput(controller, SDL_CONTROLLER_AXIS_RIGHTY, true);
            instance.input_game.zoom_out = input::CreateControllerAxisInput(controller, SDL_CONTROLLER_AXIS_RIGHTY);

            instance.input_game.open_inventory = input::CreateControllerButtonInput(controller, SDL_CONTROLLER_BUTTON_Y);
            instance.input_game.next_item = input::CreateControllerButtonInput(controller, SDL_CONTROLLER_BUTTON_DPAD_RIGHT);
            instance.input_game.previous_item = input::CreateControllerButtonInput(controller, SDL_CONTROLLER_BUTTON_DPAD_LEFT);

            instance.input_gui.left = input::CreateControllerButtonInput(controller, SDL_CONTROLLER_BUTTON_DPAD_LEFT);
            instance.input_gui.right = input::CreateControllerButtonInput(controller, SDL_CONTROLLER_BUTTON_DPAD_RIGHT);
            instance.input_gui.up = input::CreateControllerButtonInput(controller, SDL_CONTROLLER_BUTTON_DPAD_UP);
            instance.input_gui.down = input::CreateControllerButtonInput(controller, SDL_CONTROLLER_BUTTON_DPAD_DOWN);
            instance.input_gui.select = input::CreateControllerButtonInput(controller, SDL_CONTROLLER_BUTTON_A);
            instance.input_gui.back = input::CreateControllerButtonInput(controller, SDL_CONTROLLER_BUTTON_B);

            PrintToLog("INFO: Game controller found, using...");
        }
    }

    // TODO - find a proper place to put this... probably some structure that holds all GUI states and put that in the instance...
    photon_gui_game game_gui = gui::InitGameGUI();

    PrintToLog("INFO: Main loop started at: %f seconds.", SDL_GetTicks()*0.001);
    float start_time = SDL_GetTicks();
    float last_time = SDL_GetTicks();
    float frame_delta = 0;

    while(instance.running){
        frame_delta = (SDL_GetTicks() - last_time) * 0.001f;
        last_time = SDL_GetTicks();

        input::DoEvents(instance, frame_delta);

        input::DoInput(instance, frame_delta);

        level::AdvanceFrame(instance.level, frame_delta);

        instance.zoom = std::max(0.01f,instance.zoom);
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

        gui::DrawGameGUI(game_gui);

        opengl::SetCenterGUI(glm::vec2(0.0f,-1.0f));
        gui::RenderText(glm::vec2(-0.9f,0.0f), glm::vec2(0.1f), glm::vec4(0.8f,0.4f,0.1f,0.8f), false, "FPS: %f", 1.0f/frame_delta);

        window_managment::UpdateWindow(instance.window);

        instance.total_frames++;
    }

    // print total amount of seconds from SDL_Init()
    PrintToLog("INFO: Total Time: %f seconds.", SDL_GetTicks()*0.001f);
    // print total frame count.
    PrintToLog("INFO: Total Frames: %i", instance.total_frames);

    // print average draw time by dividing the total amount of time from the start of the main loop by the total amount of frames.
    PrintToLog("INFO: Average Draw Time: %fms.",(SDL_GetTicks()-start_time)/((float) instance.total_frames));
    // print average framerate by inverting the total draw time.
    PrintToLog("INFO: Average Framerate: %f fps.", (1.0f/((SDL_GetTicks()-start_time)/((float) instance.total_frames)))*1000.0f);
}

}
