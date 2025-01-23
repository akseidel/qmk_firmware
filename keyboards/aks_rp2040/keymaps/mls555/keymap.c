/* Copyright 2022 Jose Pablo Ramirez <jp.ramangulo@gmail.com>
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
#include "print.h" // in use only when trying to debug

enum layer_names { _PEDAL, _ALTERNATE, _LED_SETTINGS, _STANDBY };

enum custom_keycodes {
    PEDAL_L = SAFE_RANGE, // Left foot pedal simulation
    PEDAL_M,              // Middle foot pedal simulation
    PEDAL_R,              // Right foot pedal simulation
    ENMW_U,               // Encoder as mouse wheel up
    ENMW_D                // Encoder as mouse wheel dn
};

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [_PEDAL]        = LAYOUT(TO(_ALTERNATE),                                 // encoder press goto _ALTERNATE
                             KC_BRID, KC_BRIU, KC_NO,                        // computer screen brightness
                             MS_WHLU, KC_WH_U, KC_NO,
                             MS_WHLD, KC_WH_D, KC_NO,
                             PEDAL_L, PEDAL_M, PEDAL_R                       // pedal buttons in macros
                             ),
    [_ALTERNATE]    = LAYOUT(TO(_LED_SETTINGS),                              // encoder press goto _LED_SETTINGS
                             KC_NO,   KC_NO,   MS_ACL2,                      // computer screen brightness
                             MS_WHLU, KC_WH_U, MS_ACL1,
                             MS_WHLD, KC_WH_D, MS_ACL0,
                             PEDAL_L, PEDAL_M, PEDAL_R                       // pedal buttons in macros
                             ),
    [_LED_SETTINGS] = LAYOUT(TO(_STANDBY),                                   // encoder press goto _STANDBY
                             RM_NEXT, KC_NO, RM_TOGG,                        // Next RGB animation, nada, Toggle animations
                             KC_NO, KC_NO, KC_NO,
                             RM_VALU, RM_HUEU, RM_SPDU,                      // intensity up, color hue up, amim speed up
                             RM_VALD, RM_HUED, RM_SPDD                       // intensity dn, color hue dn, amim speed dn
                             ),
    [_STANDBY]      = LAYOUT(TO(_PEDAL),                                     // encoder press goto _PEDAL
                             KC_NO, KC_NO, KC_NO,
                             KC_NO, KC_NO, KC_NO,
                             KC_NO, KC_NO, KC_NO,
                             KC_NO, KC_NO, KC_NO
                             ),
};


static uint32_t en_turns = 0;    /* being used to record number of encoder turns */


#ifdef ENCODER_MAP_ENABLE
/*  This sets what the encoder turning does at each layer.
    There has to be an enum setting for each defined layer.
    Otherwise there will be a compile error.

    Keep in mind the transparency encoding applies.
*/
const uint16_t PROGMEM encoder_map[][NUM_ENCODERS][NUM_DIRECTIONS] = {
    [_PEDAL]        = {ENCODER_CCW_CW(ENMW_U, ENMW_D)},   // mouse wheel down and up
    [_ALTERNATE]    = {ENCODER_CCW_CW(KC_WH_U, KC_WH_D)}, // mouse wheel down and up
    [_LED_SETTINGS] = {ENCODER_CCW_CW(RM_VALD, RM_VALU)}, // LED brightness
    [_STANDBY]      = {ENCODER_CCW_CW(KC_NO, KC_NO)}      // do nothing
};
#endif

#ifdef OLED_ENABLE

// trying to render the logo only during startup
#    ifndef SHOW_LOGO
#        define SHOW_LOGO 1000
#    endif

// settings for showing logo only at startup
static bool     clear_logo      = true;
static uint32_t oled_logo_timer = 0;

static const char PROGMEM aks_mls_logo[] = {
    // 'mls_logo_layers_totop', 128x24px
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc0, 0x20, 0x10, 0x18, 0x08, 0xc4, 0xc4, 0x84, 0x02, 0x02, 0x02, 0x83, 0xc1, 0x01, 0x01, 0xc1, 0xc1, 0x01, 0x01, 0x01, 0x01, 0x01, 0x03, 0x82, 0xc2, 0x42, 0x44, 0x44, 0x84, 0x08, 0x18, 0x10, 0x20, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc0, 0xc0, 0x80, 0x00, 0x00, 0x00, 0x80, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xe0, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0x30, 0x40, 0x80, 0x80, 0x00, 0x3f, 0x01, 0x03, 0x07, 0x02, 0x01, 0x3f, 0x3f, 0x00, 0x00, 0x3f, 0x3f, 0x20, 0x20, 0x20, 0x20, 0x00, 0x00, 0x13, 0x27, 0x26, 0x26, 0x3e, 0x1c, 0x00, 0x80, 0x80, 0x40, 0x30, 0x0f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3f, 0x01, 0x03, 0x07, 0x02, 0x01, 0x3f, 0x3f, 0x00, 0x00, 0x18, 0x3d, 0x25, 0x25, 0x3f, 0x3e, 0x00, 0x00, 0x1e, 0x3f, 0x21, 0x21, 0x21, 0x00, 0x3f, 0x3f, 0x02, 0x03, 0x03, 0x00, 0x1e, 0x3f, 0x21, 0x21, 0x3f, 0x1e, 0x00, 0xff, 0xff, 0x21, 0x21, 0x3f, 0x1e, 0x00, 0x00, 0x18, 0x3d, 0x25, 0x25, 0x3f, 0x3e, 0x00, 0x1e, 0x3f, 0x21, 0x21, 0x3f, 0x3f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x02, 0x02, 0x06, 0x04, 0x04, 0x04, 0x0c, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x0c, 0x04, 0x04, 0x04, 0x06, 0x02, 0x02, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

void render_logo(void) {
    // set the oled display rotation
    // note: this oled only does 0 and 180 in its hardware
    // rotating 90 must be done in software
    bool oled_init(oled_rotation_t(OLED_ROTATION_0));
    // bool oled_init(oled_rotation_t(OLED_ROTATION_1));  // does not work 1/22/2025
    // send logo data to oled
    oled_write_raw_P(aks_mls_logo, sizeof(aks_mls_logo));
}

void clear_screenlogo(void) {
    // if logo not cleared yet
    if (clear_logo) {
        for (uint8_t i = 0; i < OLED_DISPLAY_HEIGHT; ++i) {
            for (uint8_t j = 0; j < OLED_DISPLAY_WIDTH; ++j) {
                oled_write_raw_byte(0x0, i * OLED_DISPLAY_WIDTH + j);
            }
        }
        clear_logo = false; // flag logo as cleared
    }
}

void init_logo_timer(void) {
    oled_logo_timer = timer_read32();
};

void keyboard_post_init_kb(void) {
    // This function executes once.
    init_logo_timer();
    keyboard_post_init_user();
}

void keyboard_post_init_user(void) {
    // This function executes once??
    // Customize these values to desired behaviour
    debug_enable   = true;
    debug_matrix   = true;
    debug_keyboard = true;
    debug_mouse    = true;
}

bool oled_task_kb(void) {
    /* This function executes at every matrix scan.
    Basically on a continuous basis. */
    if (!oled_task_user()) {
        // oled_task_user writes to oled according to layer
        // and returns false. So !oled_task_user means
        // it has just done that so
        // don't let it run after this oled_task_kb
        return false;
    }
    if ((timer_elapsed32(oled_logo_timer) < SHOW_LOGO)) {
        // show logo until logo timer times out
        render_logo();
        return false; // don't continue to put anything else oled
    } else {
        // clear_logo is a one shot
        clear_screenlogo();
        return true;
    }
    return false;
}

bool oled_task_user(void) {
    /* This function executes at every matrix scan as long
    as the oled_task_kb allows it. That happens when oled_task_kb
    returns true. Basically on a continuous basis depending on
    what oled_task_kb does. */
    switch (get_highest_layer(layer_state)) {
        case _PEDAL:
            oled_write_ln(PSTR("Pedal: Action On"), false);
            oled_write_ln(PSTR("Encoder: Mouse Wheel"), false);
            oled_advance_page(true);
            oled_advance_page(true);
            break;
        case _ALTERNATE:
            oled_write_ln(PSTR("Pedal Alt: Action On"), false);
            oled_write_ln(PSTR("Encoder: Mouse Wheel"), false);
            oled_advance_page(true);
            oled_advance_page(true);
            break;
        case _LED_SETTINGS:
            oled_write_ln(PSTR("LED Settings"), false);
            oled_write_ln(PSTR("Encoder: Brightness"), false);
            oled_write_ln(PSTR("Pedal: Action Off"), false);
            oled_advance_page(true);
            oled_advance_page(true);
            oled_advance_page(true);
            break;
        case _STANDBY:
            oled_write_ln(PSTR("Standby Mode"), false);
            oled_write_ln(PSTR("Pedal: Actions Off"), false);
            oled_write_ln(PSTR("Keys do nothing"), false);
            oled_write_ln(PSTR("Layer key re-engages"), false);

            break;
        default:
            oled_write_ln_P(PSTR("Undefined"), false);
            oled_advance_page(true);
            oled_advance_page(true);
    }
    return true;
}

#endif



/* oled_clean_write_ln() first cleans the desired ln_y oled line (frame)
before then writing the desired text at the desired ln_y position.
This gets around the problem of having residual text from previous
writes on that line. */
void oled_clean_write_ln(uint8_t ln_x, uint8_t ln_y, char wrds[]) {
    oled_set_cursor(0, ln_y);
    oled_write_ln(PSTR(""), false);
    oled_set_cursor(ln_x, ln_y);
    oled_write_ln(PSTR(wrds), false);
}

void rept_encoder_turns(void){
    char turn_num[5];
    int x = (20-strlen(turn_num))/2;
    itoa(en_turns, turn_num, 10);
    oled_clean_write_ln(x, 5, turn_num);
}


bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    /* This function executes at every matrix scan as long
        as the process_record__kb allows it. That happens when process_record_kb
        returns true. Basically on a continuous basis depending on
        what process_record_kb does. */
    switch (keycode) {
            /* The mechanical foot pedal actions seem to be on the release.
            Here we register the MS_BTNx code on the press down event while
            also reporting to the oled display. Then we unregister the MS_BTNx
            code on the release event and erase the reported event. The message
            reported to the oled stays displayed during that press down to release time
            period because the matrix scan sees those events on every scan and
            they are sensed in this switch(keycode) code block. */
        case PEDAL_L:
            if (record->event.pressed) {
                // when pressed
                register_code(MS_BTN1);
                oled_clean_write_ln(0, 7, "<= Pedal Left");
            } else {
                // when released
                unregister_code(MS_BTN1);
                oled_clean_write_ln(0, 7, "");
            }
            break;

        case PEDAL_M:
            if (record->event.pressed) {
                // when pressed
                register_code(MS_BTN2);
                oled_clean_write_ln(2, 7, ">> Pedal Middle <<");
            } else {
                // when released
                unregister_code(MS_BTN2);
                oled_clean_write_ln(2, 7, "");
            }
            break;

        case PEDAL_R:
            if (record->event.pressed) {
                // when pressed
                register_code(MS_BTN3);
                oled_clean_write_ln(7, 7, "Pedal Right =>");
            } else {
                // when released
                unregister_code(MS_BTN3);
                oled_clean_write_ln(7, 7, "");
            }
            break;

            /*  ENMW_U, Encoder as mouse wheel up
             ENMW_D   Encoder as mouse wheel dn
             KC_WH_D, KC_WH_U  mouse wheel down and up  keycodes
             Encoder events come a pressed/release pairs.*/

        case ENMW_U:
            if (record->event.pressed) {
                // when pressed
                register_code(KC_WH_U);
                en_turns++;
                oled_clean_write_ln(3, 4, "/// Up Scroll \\\\\\");
                // rept_encoder_turns();
                // char turn_num[5];
                // int x = (20-strlen(turn_num))/2;
                // itoa(en_turns, turn_num, 10);
                // oled_clean_write_ln(x, 5, turn_num);
            } else {
                // when released
                unregister_code(KC_WH_U);
                // oled_clean_write_ln(0, 4, "");
            }
            break;

        case ENMW_D:
            if (record->event.pressed) {
                // when pressed
                register_code(KC_WH_D);
                en_turns--;
                oled_clean_write_ln(3, 4, "\\\\\\ Dn Scroll ///");
                // rept_encoder_turns();
                // char turn_num[5];
                // int x = (20-strlen(turn_num))/2;
                // itoa(en_turns, turn_num, 10);
                // oled_clean_write_ln(x, 5, turn_num);

            } else {
                // when released
                unregister_code(KC_WH_D);
                // oled_clean_write_ln(0, 4, "");
            }
            break;
    }
    rept_encoder_turns();
    return true;
};
