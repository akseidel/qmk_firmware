/* MLS_AKS
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

#include QMK_KEYBOARD_H


enum custom_keycodes {
    PEDAL_L = SAFE_RANGE, // Left foot pedal simulation
    PEDAL_M,              // Middle foot pedal simulation
    PEDAL_R              // Right foot pedal simulation
};

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [0]        = LAYOUT(              KC_NO,
                        KC_NO, KC_NO, KC_NO,
                        KC_NO, KC_NO, KC_NO,
                        PB_1, PB_2, PB_4,
                        PEDAL_L,  PEDAL_M,  PEDAL_R
                        )
};


bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        case PEDAL_L:
            if (record->event.pressed) {
                // when pressed
                programmable_button_register(0);

            } else {
                // when released
                programmable_button_unregister(0);
            }
            break;

        case PEDAL_M:
            if (record->event.pressed) {
                // when pressed
                programmable_button_register(1);

            } else {
                // when released
                programmable_button_unregister(1);
            }
            break;

        case PEDAL_R:
            if (record->event.pressed) {
                // when pressed
                programmable_button_register(2);

            } else {
                // when released
                programmable_button_unregister(2);
            }
            break;
        default:
            break;
    }
    return true;
};
