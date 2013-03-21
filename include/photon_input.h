#ifndef _PHOTON_INPUT_H_
#define _PHOTON_INPUT_H_

#include "photon_window_managment.h"

namespace photon{

struct photon_input_state{
    // the current state value.
    float current_state = 0;
    // the value as of last frame.
    float last_state = 0;

    enum input_type{
        none,
        keyboard,
        joystick_button,
        joystick_axis,
        gamecontroller_button,
        gamecontroller_axis
    };

    input_type type = none;

    // the index of the button or axis of the joystick.
    int joystick_input_index = -1;

    // the key scancode for keabord input types.
    SDL_Scancode key = (SDL_Scancode)0;
    // the modifiers for keyboard input types.
    int modifiers = KMOD_NONE;

    // the axis to use.
    SDL_GameControllerAxis controller_axis;
    // the button to use
    SDL_GameControllerButton controller_button;

    // whether or not to negate the axis input. (i.e mouse position, joystick axis and controller sticks)
    bool axis_input_negate = false;
};

/*!
 * \brief holds the inputs used by the game & UI
 */
struct photon_input{
    // pointer to joystick to use if event is one of the joystick_* types
    SDL_Joystick* joystick = nullptr;
    // pointer to the game controller to use for gamecontroller_* types
    SDL_GameController *controller = nullptr;

    photon_input_state move_positive_x;
    photon_input_state move_negative_x;

    photon_input_state move_positive_y;
    photon_input_state move_negative_y;

    photon_input_state interact;

    photon_input_state rotate_clockwise;
    photon_input_state rotate_counter_clockwise;

    photon_input_state zoom_out;
    photon_input_state zoom_in;

    photon_input_state open_inventory;
    photon_input_state next_item;
    photon_input_state previous_item;

    photon_input_state left;
    photon_input_state right;
    photon_input_state up;
    photon_input_state down;

    photon_input_state select;
    photon_input_state back;
};

namespace input{

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

photon_input_state CreateControllerAxisInput(SDL_GameControllerAxis axis, bool negate = false);
photon_input_state CreateControllerButtonInput(SDL_GameControllerButton button);

photon_input_state CreateKeyboardInput(SDL_Scancode key);

photon_input_state CreateJoystickAxisInput(int axis, bool negate = false);
photon_input_state CreateJoystickButtonInput(int button);

photon_input LoadConfig(const std::string &filename);

}

}

#endif
