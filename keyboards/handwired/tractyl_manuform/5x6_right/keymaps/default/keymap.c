/* Copyright 2020 Christopher Courtney, aka Drashna Jael're  (@drashna) <drashna@live.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#define SCROLL_BUF_SIZE 40
#include QMK_KEYBOARD_H

// Define a type for as many tap dance states as you need
typedef enum {
    TD_NONE,
    TD_UNKNOWN,
    TD_SINGLE_TAP,
    TD_SINGLE_HOLD,
    TD_DOUBLE_TAP,
    TD_DOUBLE_HOLD
} td_state_t;

typedef struct {
    bool is_press_action;
    td_state_t state;
} td_tap_t;

enum {
    QUOT_LAYR, // Our custom tap dance key; add any other tap dance keys to this enum 
};

// Declare the functions to be used with your tap dance key(s)

// Function associated with all tap dances
td_state_t cur_dance(qk_tap_dance_state_t *state);

// Functions associated with individual tap dances
void ql_finished(qk_tap_dance_state_t *state, void *user_data);
void ql_reset(qk_tap_dance_state_t *state, void *user_data);

enum custom_layers {
    _QWERTY,
    _LOWER,
    _RAISE,
};

#define RAISE MO(_RAISE)
#define LOWER MO(_LOWER)

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
  [_QWERTY] = LAYOUT_5x6_right(
     KC_GRV , KC_1  , KC_2  , KC_3  , KC_4  , KC_5  ,                         KC_6  , KC_7  , KC_8  , KC_9  , KC_0  ,KC_MINUS,
     KC_TAB , KC_Q  , KC_W  , KC_E  , KC_R  , KC_T  ,                         KC_Y  , KC_U  , KC_I  , KC_O  , KC_P  ,KC_LBRC,
     KC_CAPS, KC_A  , KC_S  , KC_D  , KC_F  , KC_G  ,                         KC_H  , KC_J  , KC_K  , KC_L  ,KC_SCLN,KC_QUOT,
     KC_ESC, KC_Z  , KC_X  , KC_C  , KC_V  , KC_B  ,                         KC_N  , KC_M  ,KC_COMM,KC_DOT ,KC_SLSH,KC_BSLASH,
                      MO(1), KC_MS_BTN3,                                                       KC_EQL, KC_RBRC,
                                      KC_LSFT,KC_ENT,                                    KC_RSFT,
                                     KC_LCTL, KC_MS_BTN1,                                  KC_BSPC,
                                      TD(QUOT_LAYR), KC_MS_BTN2,                        KC_LGUI, KC_SPACE
  ),

  [_LOWER] = LAYOUT_5x6_right(
     KC_TILD,KC_EXLM, KC_AT ,KC_HASH,KC_DLR ,KC_PERC,                        KC_CIRC,KC_AMPR,KC_ASTR,KC_LPRN,KC_RPRN,KC_DEL,
     _______,_______,_______,_______,_______,KC_LBRC,                        KC_RBRC, KC_P7 , KC_P8 , KC_P9 ,_______,KC_PLUS,
     _______,KC_HOME,KC_PGUP,KC_PGDN,KC_END ,KC_LPRN,                        KC_RPRN, KC_P4 , KC_P5 , KC_P6 ,KC_MINS,KC_PIPE,
     _______,_______,_______,KC_PSCR,_______,_______,                        _______, KC_P1 , KC_P2 , KC_P3 ,KC_EQL ,KC_UNDS,
                     _______,KC_MS_BTN3,                                                        _______, KC_P0,
                                             _______,_______,                _______,
                                             _______,_______,                _______,
                                             KC_LALT,_______,        _______,_______

  ),

  [_RAISE] = LAYOUT_5x6_right(
       KC_F12 , KC_F1 , KC_F2 , KC_F3 , KC_F4 , KC_F5 ,                        KC_F6  , KC_F7 , KC_F8 , KC_F9 ,KC_F10 ,KC_F11 ,
       _______,_______,_______,_______,_______,KC_LBRC,                        KC_RBRC,_______,KC_NLCK,KC_INS ,KC_SLCK,KC_MUTE,
       KC_RPRN,KC_MPRV,KC_MPLY,KC_MNXT,_______,KC_VOLU,                        KC_LEFT,KC_DOWN,KC_UP,KC_RGHT,KC_LPRN,_______,
       _______,_______,_______,_______,_______,_______,                        _______,_______,_______,_______,_______,KC_VOLD,
                       _______,_______,                                                        KC_EQL ,_______,
                                               _______,_______,                _______,
                                               _______,_______,                _______,
                                               _______,_______,         _______,_______
  ),
};

typedef struct {
    int x_buf;
    int y_buf;
} trackball;

static bool scrolling_mode = false;
trackball achene_trackball = {.x_buf = 0, .y_buf = 0};


layer_state_t layer_state_set_user(layer_state_t state) {
    switch (get_highest_layer(state)) {
        case _RAISE:  // If we're on the _RAISE layer enable scrolling mode
            scrolling_mode = true;
            pointing_device_set_cpi(300);
            break;
        default:
            scrolling_mode = false;
            pointing_device_set_cpi(500);
            break;
    }
    return state;
}

report_mouse_t pointing_device_task_user(report_mouse_t mouse_report) {
    if (scrolling_mode) {
        achene_trackball.x_buf += mouse_report.x;
        achene_trackball.y_buf += mouse_report.y;
        if (abs(achene_trackball.y_buf) > SCROLL_BUF_SIZE) {
            mouse_report.v = achene_trackball.y_buf > 0 ? -1 : 1;
            achene_trackball.y_buf   = 0;
        }
        if (abs(achene_trackball.x_buf) > SCROLL_BUF_SIZE) {
            mouse_report.h = achene_trackball.x_buf > 1 ? 1 : -1;
            achene_trackball.x_buf   = 0;
        }
        mouse_report.x = 0;
        mouse_report.y = 0;
    }
    return mouse_report;
}





// Determine the current tap dance state
td_state_t cur_dance(qk_tap_dance_state_t *state) {
    if (state->count == 1) {
        return TD_SINGLE_HOLD;  // Убрал проверку на pressed - сразу HOLD
    } else if (state->count == 2) {
        if (!state->pressed) return TD_DOUBLE_TAP;
        else return TD_DOUBLE_HOLD;
    } else return TD_UNKNOWN;
}

// Initialize tap structure associated with example tap dance key
static td_tap_t ql_tap_state = {
    .is_press_action = true,
    .state = TD_NONE
};

void ql_finished(qk_tap_dance_state_t *state, void *user_data) {
    ql_tap_state.state = cur_dance(state);
    switch (ql_tap_state.state) {
        case TD_SINGLE_HOLD:
            layer_on(_RAISE);  // Сразу включаем слой при первом нажатии
            break;
        case TD_DOUBLE_HOLD:
            layer_on(_LOWER);  // Второе удерживание - слой LOWER
            break;
        default:
            break;
    }
}

void ql_reset(qk_tap_dance_state_t *state, void *user_data) {
    // Если клавиша удерживалась - выключаем соответствующий слой
    if (ql_tap_state.state == TD_SINGLE_HOLD) {
        layer_off(_RAISE);
    } else if (ql_tap_state.state == TD_DOUBLE_HOLD) {  // Добавил для DOUBLE_HOLD
        layer_off(_LOWER);
    }
    ql_tap_state.state = TD_NONE;
}

// Associate our tap dance key with its functionality
qk_tap_dance_action_t tap_dance_actions[] = {
    [QUOT_LAYR] = ACTION_TAP_DANCE_FN_ADVANCED(NULL, ql_finished, ql_reset)
};

// Set a long-ish tapping term for tap-dance keys
uint16_t get_tapping_term(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        case QK_TAP_DANCE ... QK_TAP_DANCE_MAX:
            return 0;
        default:
            return TAPPING_TERM;
    }
}
