#ifndef _PHOTON_INPUT_H_
#define _PHOTON_INPUT_H_

#include "photon_window_managment.h"

namespace photon{

namespace input{
enum input_type{
    keyboard,
    mouse_button,
    mouse_position,
    joystick_button,
    joystick_axis
};

struct state{
    // the current state value.
    float current_state;
    // the value as of last frame.
    float last_state;

    input_type type;

    // pointer to joystick to use if event is one of the joystick_* types
    SDL_Joystick* joystick;
    // the index of the button or axis of the joystick.
    int joystick_input_index;

    // the mouse button for that type of input.
    unsigned int mouse_button;

    // the key scancode for keabord input types.
    SDL_Scancode key;
    // the modifiers for keyboard input types.
    SDL_Keymod modifiers;
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
