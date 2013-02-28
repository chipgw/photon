#ifndef _PHOTON_INPUT_H_
#define _PHOTON_INPUT_H_

#include "photon_window_managment.h"

namespace photon{

namespace input{
enum input_type{
    none,
    keyboard,
    mouse_button,
    mouse_position,
    joystick_button,
    joystick_axis
};

struct input_state{
    // the current state value.
    float current_state = 0;
    // the value as of last frame.
    float last_state = 0;

    input_type type = none;

    // pointer to joystick to use if event is one of the joystick_* types
    SDL_Joystick* joystick = nullptr;
    // the index of the button or axis of the joystick.
    int joystick_input_index = -1;

    // the mouse button for that type of input.
    unsigned int mouse_button = 0;

    // the key scancode for keabord input types.
    SDL_Scancode key = (SDL_Scancode)0;
    // the modifiers for keyboard input types.
    SDL_Keymod modifiers = KMOD_NONE;
};

/*!
 * \brief holds the inputs used by the game (not UI)
 */
struct game_input{
    input_state move_positive_x;
    input_state move_negative_x;

    input_state move_positive_y;
    input_state move_negative_y;

    input_state interact;

    input_state rotate_clockwise;
    input_state rotate_counter_clockwise;

    input_state zoom_out;
    input_state zoom_in;
};

/*!
 * \brief Poll and parse SDL events.
 * \param instance
 * \param time
*/
void DoEvents(photon_instance &instance, float time);

/*!
 * \brief run the input handling system.
 * \param instance
 * \param time
 */
void DoInput(photon_instance &instance, float time);

}

}

#endif
