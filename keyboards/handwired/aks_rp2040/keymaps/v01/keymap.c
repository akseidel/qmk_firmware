// Copyright 2023 QMK
// SPDX-License-Identifier: GPL-2.0-or-later

#include QMK_KEYBOARD_H


enum layer_names {_BASE,
                _ALTERNATE1,
                _ALTERNATE2,
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
    LAY_B,              // Switch to previous layer (not used at this time)
    NEXT_APP            // Switch to next application on computer
};

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    /*
     * The matrix coded LAYOUT is this:
     * ┌─────┬─────┬─────┬───────────┐
     * │ 0,0 │ 0,1 │ 0,2 │ 0,3 Enc 1 │
     * ├─────┼─────┼─────┼───────────┤
     * │ 1,0 │ 1,1 │ 1,2 │ 1,3 Enc 2 │
     * ├─────┼─────┼─────┼───────────┤
     * │ 2,0 │ 2,1 │ 2,2 │ 2,3 Enc 3 │
     * ├─────┼─────┼─────┼───────────┤
     * │ 3,0 │ 3,1 │ 3,2 │ 3,3 Enc 4 │
     * └─────┴─────┴─────┴───────────┘
     *
     * But is physically mounted like this:
     *
     *                  ┌───────────┐  ┌───────────┐
     *                  │ 0,3 Enc 1 │  │ 1,3 Enc 2 │
     *                  └───────────┘  └───────────┘
     * ┌─────┬─────┬─────┬─────┐
     * │ 3,0 │ 2,0 │ 1,0 │ 0,0 │       ┌───────────┐
     * ├─────┼─────┼─────┼─────┤       │ 2,3 Enc 3 │
     * │ 3,1 │ 2,1 │ 1,1 │ 0,1 │       └───────────┘
     * ├─────┼─────┼─────┼─────┤       ┌───────────┐
     * │ 3,2 │ 2,2 │ 1,2 │ 0,2 │       │ 3,3 Enc 4 │
     * └─────┴─────┴─────┴─────┘       └───────────┘
     *
     *
     */




    [_BASE] = LAYOUT(
        KC_NO,      KC_NO,      NEXT_APP,       KC_NO,
        KC_NO,      KC_NO,      KC_NO,       KC_NO,
        KC_NO,      KC_NO,      KC_NO,        KC_NO,
        KC_NO,      KC_NO,      KC_NO,        TO(_ALTERNATE1)
    ),
    [_ALTERNATE1] = LAYOUT(
        KC_NO,      KC_NO,      KC_NO,      KC_NO,
        KC_NO,      KC_NO,      KC_NO,      KC_NO,
        KC_NO,      KC_NO,      KC_NO,      KC_NO,
        KC_NO,      KC_NO,    KC_NO,      TO(_ALTERNATE2)
    ),
    [_ALTERNATE2] = LAYOUT(
        KC_NO,    KC_NO,    KC_NO,    KC_NO,
        KC_NO,    KC_NO,    KC_NO,    KC_NO,
        KC_NO,    KC_NO, KC_NO,    KC_NO,
        KC_NO,    KC_NO,    KC_NO,    TO(_STANDBY)
    ),
    [_STANDBY] = LAYOUT(
        KC_NO,      KC_NO,      KC_NO,      KC_NO,
        KC_NO,      KC_NO,      KC_NO,      KC_NO,
        KC_NO,      KC_NO,      KC_NO,      KC_NO,
        KC_NO,      KC_NO,      KC_NO,      TO(_BASE)
    )
};

static int16_t en_turns = 0;    /* being used to record number of encoder turns */
static int16_t stride = 1;      /* stride is a distance (number of encoder turns) concept*/

#if defined(ENCODER_MAP_ENABLE)
const uint16_t PROGMEM encoder_map[][NUM_ENCODERS][NUM_DIRECTIONS] = {
    [_BASE] =       { ENCODER_CCW_CW(KC_NO, KC_NO), ENCODER_CCW_CW(KC_NO, KC_NO),
                    ENCODER_CCW_CW(KC_NO, KC_NO),
                    ENCODER_CCW_CW(KC_NO, KC_NO)
                    },
    [_ALTERNATE1] =   { ENCODER_CCW_CW(KC_NO,KC_NO),  ENCODER_CCW_CW(KC_NO, KC_NO),
                    ENCODER_CCW_CW(KC_NO, KC_NO),
                    ENCODER_CCW_CW(KC_NO,KC_NO)
                    },
    [_ALTERNATE2] =   { ENCODER_CCW_CW(KC_NO,KC_NO),  ENCODER_CCW_CW(KC_NO, KC_NO),
                    ENCODER_CCW_CW(KC_NO, KC_NO),
                    ENCODER_CCW_CW(KC_NO,KC_NO)
                    },
    [_STANDBY] =    { ENCODER_CCW_CW(KC_NO, KC_NO),     ENCODER_CCW_CW(KC_NO, KC_NO),
                    ENCODER_CCW_CW(KC_NO, KC_NO),
                    ENCODER_CCW_CW(KC_NO, KC_NO)
                    },
};
#endif





bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {

        case NEXT_APP:
            if (record->event.pressed){
                SEND_STRING(SS_LGUI("\t"));
            }
            return false;
            break;
    }
    return true;
}




#ifdef OLED_ENABLE

// trying to render the logo only during startup
#    ifndef SHOW_LOGO
#        define SHOW_LOGO 1000
#    endif

// variables used for showing logo only at startup
static bool     logo_is_visible = false;
static uint32_t oled_logo_timer = 0;

        static const char PROGMEM mls_logo[] = {
            // 'mls_logo_layers_totop', 128x24px
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc0, 0x20, 0x10, 0x18,
            0x08, 0xc4, 0xc4, 0x84, 0x02, 0x02, 0x02, 0x83, 0xc1, 0x01, 0x01, 0xc1, 0xc1, 0x01, 0x01, 0x01,
            0x01, 0x01, 0x03, 0x82, 0xc2, 0x42, 0x44, 0x44, 0x84, 0x08, 0x18, 0x10, 0x20, 0xc0, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc0, 0xc0, 0x80, 0x00, 0x00, 0x00, 0x80, 0xc0, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xe0, 0xe0,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0x30, 0x40, 0x80, 0x80,
            0x00, 0x3f, 0x01, 0x03, 0x07, 0x02, 0x01, 0x3f, 0x3f, 0x00, 0x00, 0x3f, 0x3f, 0x20, 0x20, 0x20,
            0x20, 0x00, 0x00, 0x13, 0x27, 0x26, 0x26, 0x3e, 0x1c, 0x00, 0x80, 0x80, 0x40, 0x30, 0x0f, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3f, 0x01, 0x03, 0x07, 0x02, 0x01, 0x3f, 0x3f, 0x00, 0x00,
            0x18, 0x3d, 0x25, 0x25, 0x3f, 0x3e, 0x00, 0x00, 0x1e, 0x3f, 0x21, 0x21, 0x21, 0x00, 0x3f, 0x3f,
            0x02, 0x03, 0x03, 0x00, 0x1e, 0x3f, 0x21, 0x21, 0x3f, 0x1e, 0x00, 0xff, 0xff, 0x21, 0x21, 0x3f,
            0x1e, 0x00, 0x00, 0x18, 0x3d, 0x25, 0x25, 0x3f, 0x3e, 0x00, 0x1e, 0x3f, 0x21, 0x21, 0x3f, 0x3f,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,
            0x01, 0x02, 0x02, 0x06, 0x04, 0x04, 0x04, 0x0c, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08,
            0x08, 0x08, 0x0c, 0x04, 0x04, 0x04, 0x06, 0x02, 0x02, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
        };

        void render_logo(void) {
            /* set the oled display rotation
            note: this oled only does 0 and 180 in its hardware
            rotating 90 must be done in software */
            bool oled_init(oled_rotation_t(OLED_ROTATION_0));
            // send logo data to oled
            oled_write_raw_P(mls_logo, sizeof(mls_logo));
            logo_is_visible = true;
        }

        void clear_screenlogo(void) {
            // if logo not cleared yet
            if (logo_is_visible) {
                for (uint8_t i = 0; i < OLED_DISPLAY_HEIGHT; ++i) {
                    for (uint8_t j = 0; j < OLED_DISPLAY_WIDTH; ++j) {
                        oled_write_raw_byte(0x0, i * OLED_DISPLAY_WIDTH + j);
                    }
                }
                logo_is_visible = false; // flag logo as cleared
            }
        }

        void init_logo_timer(void) {
            oled_logo_timer = timer_read32();
        };

        void keyboard_post_init_kb(void) {
            /* This function executes once after most of the keyboard is initialized.
            The logo gets rendered and its timer to be visible is set. */
            render_logo();
            init_logo_timer();
        }


        bool oled_task_kb(void) {
            /* This function executes at every matrix scan. It provides the opportunity
            to not call the oled_task_user that executes the oled updates. The logo_is_visible
            flag is being used to not continue with oled updates. The logo has already been
            rendered during keyboard_post_init_kb. So it is showing after boot and does not
            need to be rendered again. Instead the logo timer is checked to decide when to
            erase the logo and change the logo_is_visible flag. */
            if (!logo_is_visible) {
                oled_task_user();
                return false;
            }
            if ((timer_elapsed32(oled_logo_timer) < SHOW_LOGO)) {
                /* Allow the logo to remain showing during SHOW_LOGO */
                return false;
            } else {
                /* Remove the logo and set logo_is_visible flag */
                clear_screenlogo();
                return false;
            }
        }

        bool oled_task_user(void) {
            /* This function executes at every matrix scan as long
             as the oled_task_kb allows it. That happens when oled_task_kb
            returns true. Basically on a continuous basis depending on
            what oled_task_kb does. */

            /* writing to the oled per the current layer */
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

void report_position_etc(void){
    /* oled display position value and other information */
    char buf[18];
    snprintf(buf, 18, "Pos: %d", en_turns);
    oled_clean_write_ln(0, 6, buf);
    snprintf(buf, 18, "Stride: %d", stride);
    oled_clean_write_ln(0, 7, buf);
}
