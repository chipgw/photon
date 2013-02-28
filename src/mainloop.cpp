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

    instance.player.location = glm::vec2(1.0f,1.0f);

    instance.input_set.move_positive_x.key = SDL_SCANCODE_D;
    instance.input_set.move_positive_x.type = input::keyboard;
    instance.input_set.move_negative_x.key = SDL_SCANCODE_A;
    instance.input_set.move_negative_x.type = input::keyboard;

    instance.input_set.move_positive_y.key = SDL_SCANCODE_W;
    instance.input_set.move_positive_y.type = input::keyboard;
    instance.input_set.move_negative_y.key = SDL_SCANCODE_S;
    instance.input_set.move_negative_y.type = input::keyboard;

    instance.input_set.zoom_in.key = SDL_SCANCODE_KP_PLUS;
    instance.input_set.zoom_in.type = input::keyboard;
    instance.input_set.zoom_out.key = SDL_SCANCODE_KP_MINUS;
    instance.input_set.zoom_out.type = input::keyboard;

    SDL_Joystick *joy = SDL_JoystickOpen(0);
    if(joy != nullptr){
        instance.input_set.move_positive_x.joystick = joy;
        instance.input_set.move_positive_x.joystick_input_index = 1;
        instance.input_set.move_positive_x.type = input::joystick_axis;
        instance.input_set.move_negative_x.joystick = joy;
        instance.input_set.move_negative_x.joystick_input_index = -1;
        instance.input_set.move_negative_x.type = input::joystick_axis;

        instance.input_set.move_positive_y.joystick = joy;
        instance.input_set.move_positive_y.joystick_input_index = -2;
        instance.input_set.move_positive_y.type = input::joystick_axis;
        instance.input_set.move_negative_y.joystick = joy;
        instance.input_set.move_negative_y.joystick_input_index = 2;
        instance.input_set.move_negative_y.type = input::joystick_axis;


        instance.input_set.interact.type = input::joystick_button;
        instance.input_set.interact.joystick = joy;
        instance.input_set.interact.joystick_input_index = 0;

        instance.input_set.rotate_clockwise.type = input::joystick_button;
        instance.input_set.rotate_clockwise.joystick = joy;
        instance.input_set.rotate_clockwise.joystick_input_index = 4;

        instance.input_set.rotate_counter_clockwise.type = input::joystick_button;
        instance.input_set.rotate_counter_clockwise.joystick = joy;
        instance.input_set.rotate_counter_clockwise.joystick_input_index = 5;

        instance.input_set.zoom_in.joystick = joy;
        instance.input_set.zoom_in.joystick_input_index = -5;
        instance.input_set.zoom_in.type = input::joystick_axis;
        instance.input_set.zoom_out.joystick = joy;
        instance.input_set.zoom_out.joystick_input_index = 5;
        instance.input_set.zoom_out.type = input::joystick_axis;
    }

    while(instance.running){
        frame_delta = SDL_GetTicks() - last_time;
        last_time = SDL_GetTicks();

        input::DoEvents(instance, frame_delta);

        input::DoInput(instance, frame_delta);

        instance.zoom = std::max(0.01f,instance.zoom);
        opengl::UpdateZoom(instance.zoom);

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

        opengl::DrawPhoton(instance.player.location);

        window_managment::UpdateWindow(instance.window);

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
