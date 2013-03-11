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

    instance.input_set.move_positive_x = input::CreateKeyboardInput(SDL_SCANCODE_D);
    instance.input_set.move_negative_x = input::CreateKeyboardInput(SDL_SCANCODE_A);

    instance.input_set.move_positive_y = input::CreateKeyboardInput(SDL_SCANCODE_W);
    instance.input_set.move_negative_y = input::CreateKeyboardInput(SDL_SCANCODE_S);

    instance.input_set.interact = input::CreateKeyboardInput(SDL_SCANCODE_SPACE);

    instance.input_set.rotate_clockwise = input::CreateKeyboardInput(SDL_SCANCODE_E);
    instance.input_set.rotate_counter_clockwise = input::CreateKeyboardInput(SDL_SCANCODE_Q);

    instance.input_set.zoom_in = input::CreateKeyboardInput(SDL_SCANCODE_KP_PLUS);
    instance.input_set.zoom_out = input::CreateKeyboardInput(SDL_SCANCODE_KP_MINUS);

    if(SDL_IsGameController(0)){
        SDL_GameController *controller = SDL_GameControllerOpen(0);

        if(controller != nullptr){
            instance.input_set.move_positive_x = input::CreateControllerAxisInput(controller, SDL_CONTROLLER_AXIS_LEFTX);
            instance.input_set.move_negative_x = input::CreateControllerAxisInput(controller, SDL_CONTROLLER_AXIS_LEFTX, true);

            instance.input_set.move_positive_y = input::CreateControllerAxisInput(controller, SDL_CONTROLLER_AXIS_LEFTY, true);
            instance.input_set.move_negative_y = input::CreateControllerAxisInput(controller, SDL_CONTROLLER_AXIS_LEFTY);

            instance.input_set.interact = input::CreateControllerButtonInput(controller, SDL_CONTROLLER_BUTTON_A);

            instance.input_set.rotate_clockwise = input::CreateControllerButtonInput(controller, SDL_CONTROLLER_BUTTON_LEFTSHOULDER);
            instance.input_set.rotate_counter_clockwise = input::CreateControllerButtonInput(controller, SDL_CONTROLLER_BUTTON_RIGHTSHOULDER);

            instance.input_set.zoom_in = input::CreateControllerAxisInput(controller, SDL_CONTROLLER_AXIS_RIGHTY, true);
            instance.input_set.zoom_out = input::CreateControllerAxisInput(controller, SDL_CONTROLLER_AXIS_RIGHTY);

            PrintToLog("INFO: Game controller found, using...");
        }
    }else{
        SDL_Joystick *joystick = SDL_JoystickOpen(0);
        if(joystick != nullptr){
            instance.input_set.move_positive_x = input::CreateJoystickAxisInput(joystick, 0);
            instance.input_set.move_negative_x = input::CreateJoystickAxisInput(joystick, 0, true);

            instance.input_set.move_positive_y = input::CreateJoystickAxisInput(joystick, 1, true);
            instance.input_set.move_negative_y = input::CreateJoystickAxisInput(joystick, 1);

            instance.input_set.interact = input::CreateJoystickButtonInput(joystick, 0);

            instance.input_set.rotate_clockwise = input::CreateJoystickButtonInput(joystick, 4);

            instance.input_set.rotate_counter_clockwise = input::CreateJoystickButtonInput(joystick, 5);

            instance.input_set.zoom_in = input::CreateJoystickAxisInput(joystick, 4, true);
            instance.input_set.zoom_out = input::CreateJoystickAxisInput(joystick, 4);

            SDL_JoystickGUID guid = SDL_JoystickGetGUID(joystick);
            char guid_str[33];
            SDL_JoystickGetGUIDString(guid, guid_str, 33);
            PrintToLog("INFO: Joystick found, using... (GUID: %s)", guid_str);
        }
    }

    photon_gui_element button;

    button.left = -0.4f;
    button.right = 0.4f;
    button.bottom = -0.9f;
    button.top = -0.7f;
    button.text_padding = 0.05f;
    button.text = "good!";
    button.text_color = glm::vec4(1.0f);
    button.texture = texture::Load("/textures/button.png");

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

        gui::DrawElement(button);
        gui::RenderText(glm::vec2(-1.0f), glm::vec2(0.1f), glm::vec4(0.8f,0.4f,0.1f,0.8f), false, "FPS: %f", 1.0f/frame_delta);

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
