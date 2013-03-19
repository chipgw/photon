#ifndef _PHOTON_INPUT_H_
#define _PHOTON_INPUT_H_

#include "photon_window_managment.h"

namespace photon{

namespace input{
enum input_type{
    none,
    keyboard,
    joystick_button,
    joystick_axis,
    gamecontroller_button,
    gamecontroller_axis
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

    // the key scancode for keabord input types.
    SDL_Scancode key = (SDL_Scancode)0;
    // the modifiers for keyboard input types.
    SDL_Keymod modifiers = KMOD_NONE;

    // pointer to the game controller to use for gamecontroller_* types
    SDL_GameController *controller = nullptr;
    // the axis to use.
    SDL_GameControllerAxis controller_axis;
    // the button to use
    SDL_GameControllerButton controller_button;

    // whether or not to negate the axis input. (i.e mouse position, joystick axis and controller sticks)
    bool axis_input_negate = false;
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

    input_state open_inventory;
    input_state next_item;
    input_state previous_item;
};

/*!
 * \brief holds the inputs used by the GUI
 */
struct gui_input{
    input_state left;
    input_state right;
    input_state up;
    input_state down;

    input_state select;
    input_state back;
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

input_state CreateControllerAxisInput(SDL_GameController *controller, SDL_GameControllerAxis axis, bool negate = false);
input_state CreateControllerButtonInput(SDL_GameController *controller, SDL_GameControllerButton button);

input_state CreateKeyboardInput(SDL_Scancode key);

input_state CreateJoystickAxisInput(SDL_Joystick *joystick, int axis, bool negate = false);
input_state CreateJoystickButtonInput(SDL_Joystick *joystick, int button);
}

}

#endif
