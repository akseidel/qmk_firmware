/* Copyright 2025 Miles Seidel <mlseidel@gmail.com>
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
// #include "print.h" // in use only when trying to debug

enum layer_names { _BASE,
                _ALTERNATE1,
                _ALTERNATE2,
                _LED_SETTINGS,
                _STANDBY
                };

enum custom_keycodes {
    ENC_U = SAFE_RANGE,   // Encoder as mouse wheel up
    ENC_D,               // Encoder as mouse wheel dn
    TURN_0,               // Zero the encoder counts
    ENC_STRIDE_INC,              // Stride increase
    ENC_STRIDE_DEC,              // Stride decrease
    STRIDE_1,              // Set stride to one
    GOTO_0,             // Return to position 0
    MLS_WHLU,           // MLS control up
    MLS_WHLD,           // MLS control dn
    MSG_STBY,           // Show message standby
    MSG_READY,          // Show message ready
    SPD_1_U,            // Wheel up at accelerated speed 1
    SPD_1_D,            // Wheel down at accelerated speed 1
    SPD_2_U,            // Wheel up at accelerated speed 2
    SPD_2_D,            // Wheel down at accelerated speed 2
    SPD_3_U,            // Wheel up at accelerated speed 3
    SPD_3_D,            // Wheel down at accelerated speed 3
    LAY_F,              // Switch to next layer (not used at this time)
    LAY_B               // Switch to previous layer (not used at this time)
};


/* KC_NO means no keycode, ie do nothing */
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    /* One shot strides and also standard out of box mouse wheel action */
    [_BASE]        = LAYOUT(TO(_ALTERNATE1),                               // encoder press goto _ALTERNATE1
                            TURN_0,  KC_NO,   KC_NO,                       // counter zero
                            GOTO_0,   KC_NO, STRIDE_1,                     // goto postion 0, kc_no, set stride to 1
                            ENC_U, KC_WH_U, ENC_STRIDE_INC,        // 1 shot u click stride x,reg. wheel,inc stride
                            ENC_D, KC_WH_D, ENC_STRIDE_DEC         // 1 shot d click stride x,reg. wheel,inc stride
                             ),
    /* QMK accelerated mouse wheel action
    The SPD_1_x, SPD_2_x, SPD_3_x keys set three speed ranges.
    Speed is currently set at compile time. Position counter n/a. */
    [_ALTERNATE1]    = LAYOUT(TO(_ALTERNATE2),                            // encoder press goto _ALTERNATE2
                            TURN_0,  KC_NO,   KC_NO,                      // counter zero
                            GOTO_0,  KC_NO,   KC_NO,                      // goto postion 0
                            SPD_1_U, SPD_2_U, SPD_3_U,                    // up @ spd 1, up @ spd 2, up @ spd 3
                            SPD_1_D, SPD_2_D, SPD_3_D                     // dn @ spd 1, dn @ spd 2, dn @ spd 3
                             ),
    /* MLS mouse wheel speed control. The encoder sets stride
    while keys are used for up and down at stride distance. */
    [_ALTERNATE2]    = LAYOUT(TO(_LED_SETTINGS),                           // encoder press goto _LED_SETTINGS
                            TURN_0,   KC_NO, STRIDE_1,                     // counter 0, kc_no, set stride to 1
                            GOTO_0,   KC_NO, STRIDE_1,                     // goto postion 0, kc_no, set stride to 1
                            MLS_WHLU, MLS_WHLU, MLS_WHLU,                  // all keys do up
                            MLS_WHLD, MLS_WHLD, MLS_WHLD                   // all keys do dn
                             ),
    /* LED animation setting layer */
    [_LED_SETTINGS] = LAYOUT(TO(_STANDBY),                                 // encoder press goto _STANDBY
                             RM_NEXT, KC_NO, RM_TOGG,                      // Next RGB animation, kc_no, Animation on/off
                             KC_NO, KC_NO, KC_NO,
                             RM_VALU, RM_HUEU, RM_SPDU,                    // intensity up, color hue up, amim speed up
                             RM_VALD, RM_HUED, RM_SPDD                     // intensity dn, color hue dn, amim speed dn
                             ),
    [_STANDBY]      = LAYOUT(MSG_READY,                                     // encoder press goto _BASE
                             MSG_STBY, MSG_STBY, MSG_STBY,
                             MSG_STBY, MSG_STBY, MSG_STBY,
                             MSG_STBY, MSG_STBY, MSG_STBY,
                             MSG_STBY, MSG_STBY, MSG_STBY
                             )
};


static int32_t en_turns = 0;    /* being used to record number of encoder turns */
static int32_t stride = 1;

#ifdef ENCODER_MAP_ENABLE
/*  This sets what the encoder turning does at each layer.
    There has to be an enum setting for each defined layer.
    Otherwise there will be a compile error.

    Keep in mind the transparency encoding applies.
*/
const uint16_t PROGMEM encoder_map[][NUM_ENCODERS][NUM_DIRECTIONS] = {
    [_BASE]        = {ENCODER_CCW_CW(ENC_U,ENC_D)},
    [_ALTERNATE1]    = {ENCODER_CCW_CW(ENC_U,ENC_D)},
    [_ALTERNATE2]    = {ENCODER_CCW_CW(ENC_STRIDE_INC,ENC_STRIDE_DEC)},
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
    // keyboard_post_init_user();
}

// Used for debugging
// void keyboard_post_init_user(void) {
//     // This function executes once??
//     // Customize these values to desired behaviour
//     // debug_enable   = true;
//     // debug_matrix   = true;
//     // debug_keyboard = true;
//     // debug_mouse    = true;
// }


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
        case _BASE:
            oled_write_ln(PSTR("Keys: Std Mse Wheel"), false);
            oled_write_ln(PSTR("Encoder: Mse Wheel"), false);
            oled_write_ln(PSTR("Press Encoder: Next"), false);
            oled_advance_page(true);
            break;
        case _ALTERNATE1:
            oled_write_ln(PSTR("Keys: Acl Mse Wheel"), false);
            oled_write_ln(PSTR("3 Wheel Speed Modes"), false);
            oled_write_ln(PSTR("Encoder: Mse Wheel"), false);
            oled_write_ln(PSTR("Press Encoder: Next"), false);
            break;
        case _ALTERNATE2:
            oled_write_ln(PSTR("Keys: MLS Mse Wheel"), false);
            oled_write_ln(PSTR("Encoder: Stride"), false);
            oled_write_ln(PSTR("Press Encoder: Next"), false);
            oled_advance_page(true);
            break;
        case _LED_SETTINGS:
            oled_write_ln(PSTR("Settings: LED"), false);
            oled_write_ln(PSTR("Encoder: Brightness"), false);
            oled_write_ln(PSTR("Press Encoder: Next"), false);
            oled_advance_page(true);
            oled_advance_page(true);
            break;
        case _STANDBY:
            oled_write_ln(PSTR("Mode: Standby"), false);
            oled_write_ln(PSTR("Keys: Nothing"), false);
            oled_write_ln(PSTR("Press Encoder: Next"), false);
            oled_advance_page(true);
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

void oled_clean_ln( uint8_t ln_y) {
    oled_set_cursor(0, ln_y);
    oled_write_ln(PSTR(""), false);
}

void rept_encoder_turns(void){
    char buf[14];
    snprintf(buf, 14, "Pos: %ld", en_turns);
    oled_clean_write_ln(0, 6, buf);
}

void rept_Stride(void){
    char buf[18];
    snprintf(buf, 18, "Stride: %ld", stride);
    oled_clean_write_ln(0, 7, buf);
}

void rept_position_status(void){
    char buf[18];
    snprintf(buf, 18, "Pos: %ld", en_turns);
    oled_clean_write_ln(0, 6, buf);
    snprintf(buf, 18, "Stride: %ld", stride);
    oled_clean_write_ln(0, 7, buf);
}

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    /* This function executes at every matrix scan as long
        as the process_record__kb allows it. That happens when process_record_kb
        returns true. Basically on a continuous basis depending on
        what process_record_kb does. */
    switch (keycode) {
        case ENC_STRIDE_INC:
            if (record->event.pressed) {
                // when pressed
                stride++;
                oled_clean_write_ln(1, 5, "\\\\\\  Stride Chg  ///");
            } else {
                // when released
            }
            break;

        case ENC_STRIDE_DEC:
            if (record->event.pressed) {
                // when pressed
                if (stride > 1) {
                     stride--;
                    oled_clean_write_ln(1, 5, "///  Stride Chg  \\\\\\");
                }
            } else {
                // when released
            }
            break;
        case TURN_0: /* Zero the encoder counter */
            if (record->event.pressed) {
                // when pressed
                en_turns = 0;
                oled_clean_write_ln(3, 4, "///  Zero Pos \\\\\\");
            } else {
                // when released
            }
            break;
        case STRIDE_1: /* One the Stride */
            if (record->event.pressed) {
                // when pressed
                stride = 1;
                oled_clean_write_ln(1, 5, "/// Stride Reset \\\\\\");
            } else {
                // when released
            }
            break;
        case MSG_STBY: /* Standby Message */
            if (record->event.pressed) {
                // when pressed
                oled_clean_write_ln(1, 4, "// Standby Mode! \\\\");
                oled_clean_ln(5);

            } else {
                // when released
            }
            break;
        case SPD_1_U:
            if (record->event.pressed) {
                // when pressed
                tap_code(MS_ACL0);
                register_code(KC_WH_U);
                oled_clean_write_ln(1, 5, "///  Speed 1 In \\\\\\");
            } else {
                // when released
                unregister_code(KC_WH_U);
                oled_clean_ln(5);
            }
            break;
        case SPD_1_D:
            if (record->event.pressed) {
                // when pressed
                tap_code(MS_ACL0);
                register_code(KC_WH_D);
                oled_clean_write_ln(1, 5, "///  Speed 1 Out \\\\\\");
            } else {
                // when released
                unregister_code(KC_WH_D);
                oled_clean_ln(5);
            }
            break;
        case SPD_2_U:
            if (record->event.pressed) {
                // when pressed
                tap_code(MS_ACL1);
                register_code(KC_WH_U);
                oled_clean_write_ln(1, 5, "///  Speed 2 In \\\\\\");
            } else {
                // when released
                unregister_code(KC_WH_U);
                oled_clean_ln(5);
            }
            break;
        case SPD_2_D:
            if (record->event.pressed) {
                // when pressed
                tap_code(MS_ACL1);
                register_code(KC_WH_D);
                oled_clean_write_ln(1, 5, "///  Speed 2 Out \\\\\\");
            } else {
                // when released
                unregister_code(KC_WH_D);
                oled_clean_ln(5);
            }
            break;
        case SPD_3_U:
            if (record->event.pressed) {
                // when pressed
                tap_code(MS_ACL2);
                register_code(KC_WH_U);
                oled_clean_write_ln(1, 5, "///  Speed 3 In \\\\\\");
            } else {
                // when released
                unregister_code(KC_WH_U);
                oled_clean_ln(5);
            }
            break;
        case SPD_3_D:
            if (record->event.pressed) {
                // when pressed
                tap_code(MS_ACL2);
                register_code(KC_WH_D);
                oled_clean_write_ln(1, 5, "///  Speed 3 Out \\\\\\");
            } else {
                // when released
                unregister_code(KC_WH_D);
                oled_clean_ln(5);
            }
            break;
        case ENC_U:
            /* Note: an encoder event comes as a press/release pair */
            if (record->event.pressed) {
                // when pressed
                for (int i = 0; i < stride ; i++) {
                        tap_code(KC_WH_U);
                        en_turns++;
                    }
                oled_clean_ln(4);
                oled_clean_write_ln(3, 5, "/// In Scroll \\\\\\");
            } else {
                // when released
            }
            break;
        case ENC_D:
            /* Note: an encoder event comes as a press/release pair */
            if (record->event.pressed) {
                // when pressed
                for (int i = 0; i < stride ; i++) {
                        tap_code(KC_WH_D);
                        en_turns--;
                    }
                oled_clean_ln(4);
                oled_clean_write_ln(2, 5, "\\\\\\ Out Scroll ///");
            } else {
                // when released
            }
            break;
        case MLS_WHLU:
            if (record->event.pressed) {
                // when pressed
                for (int i = 0; i < stride ; i++) {
                    // register_code(KC_WH_U); // NO, does not help!
                    tap_code(KC_WH_U);
                    en_turns++;
                }
                oled_clean_ln(4);
                oled_clean_write_ln(0, 5, "// MLS In Scroll \\\\");
            } else {
                // when released
                // unregister_code(KC_WH_U);
            }
            break;
        case MLS_WHLD:
            if (record->event.pressed) {
                // when pressed
                for (int i = 0; i < stride ; i++) {
                    // register_code(KC_WH_D); // NO, does not help!
                    tap_code(KC_WH_D);
                    en_turns--;
                }
                oled_clean_ln(4);
                oled_clean_write_ln(0, 5, "// MLS Out Scroll \\\\");
            } else {
                // when released
                // unregister_code(KC_WH_D);
            }
            break;
        case GOTO_0: /* Goto position 0 */
            if (record->event.pressed) {
                // when pressed
                while (en_turns != 0){
                    if (en_turns < 0) {
                        tap_code(KC_WH_U);
                        en_turns++;
                    } else if (en_turns > 0) {\
                        tap_code(KC_WH_D);
                        en_turns--;
                    }
                }
                oled_clean_ln(4);
                oled_clean_write_ln(2, 5, "// Scrolled To 0 \\\\");
            } else {
                // when released
            }
            break;
        case MSG_READY: /* Ready Message */
            if (record->event.pressed) {
                // when pressed
                oled_clean_ln(4);
                oled_clean_write_ln(3, 5, "///  Ready  \\\\\\");
            } else {
                layer_move(0);
            }
            return false;
            break;

        case LAY_F: /* Ready Message */
            if (record->event.pressed) {
                // when pressed

            } else {
                // layer_move(0);
            }
            return false;
            break;

        case LAY_B: /* Ready Message */
            if (record->event.pressed) {
                // when pressed

            } else {
                // layer_move(0);
            }
            return false;
            break;


        default:
            break;
    }
    rept_position_status();
    return true;
};
