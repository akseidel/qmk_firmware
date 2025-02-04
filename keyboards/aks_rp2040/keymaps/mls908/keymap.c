/*
 * Copyright 2025 Miles Seidel <mlseidel@gmail.com>
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

/* Every reference name must be first defined in enum before it shows up anywhere in the code. */
enum layer_names { _BASE,
                _3SPEEDACL,
                _ALTERNATE2,
                _STANDBY,
                _LED_SETUP
                /* Always keep this layer to be the last declared layer_name enum so that the
                scheme for the cycle layer key skips the _LED_SETUP layer.*/
                };

/* Every reference name must be first defined in enum before it shows up anywhere in the code. */
enum custom_keycodes {
    ENC_U = SAFE_RANGE, // Encoder as mouse wheel up
    ENC_D,              // Encoder as mouse wheel dn
    TURN_0,             // Zero the encoder counts
    ENC_STRIDE_INC,     // Stride increase
    ENC_STRIDE_DEC,     // Stride decrease
    STRIDE_1,           // Set stride to one
    GOTO_0,             // Save position P, then return to position 0
    GOTO_P,             // Return to position P
    MLS_WHLU,           // MLS control up
    MLS_WHLD,           // MLS control dn
    MSG_STBY,           // Show message standby
    EXIT_STBY,          // Special requirement when exiting standby
    SPD_1_U,            // Wheel up at accelerated speed 1
    SPD_1_D,            // Wheel down at accelerated speed 1
    SPD_2_U,            // Wheel up at accelerated speed 2
    SPD_2_D,            // Wheel down at accelerated speed 2
    SPD_3_U,            // Wheel up at accelerated speed 3
    SPD_3_D,            // Wheel down at accelerated speed 3
    LAY_F,              // Switch to next layer (not used at this time)
    LAY_B,              // Switch to previous layer (not used at this time)
    STD_WH_U,           // Standard QMK mouse wheel up, no counting
    STD_WH_D,           // Standard QMK mouse wheel dn, no counting
    EXP_WH_U,           // Exponential QMK mouse wheel up, with counting
    EXP_WH_D,           // Exponential QMK mouse wheel dn, with counting
    CYCLE_RAD_LYRS,     // Cycle only through rad layers
    LED_LYR,            // Set layer to LED_SETUP
    LED_TOG,            // LED animation toggle
    LED_NXT,            // Next LED animation
    LED_I_UP,           // LED Intensity Up
    LED_I_DN,           // LED Intensity Dn
    LED_HUE_UP,         // LED HUE value (color) up
    LED_HUE_DN,         // LED HUE value (color) dn
    LED_SPD_UP,         // LED animation speed up
    LED_SPD_DN          // LED animation speed dn

};


/* KC_NO means no keycode, ie do nothing */
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    /* One shot strides and also standard out of box mouse wheel action */
    [_BASE]        = LAYOUT(                   CYCLE_RAD_LYRS,      // encoder press goto _3SPEEDACL
                            TURN_0,  GOTO_P,   KC_NO,               // counter zero
                            GOTO_0,   KC_NO, STRIDE_1,              // goto postion 0, kc_no, set stride to 1
                            ENC_U, EXP_WH_U, ENC_STRIDE_INC,        // 1 shot u click stride x,reg. wheel,inc stride
                            ENC_D, EXP_WH_D, ENC_STRIDE_DEC         // 1 shot d click stride x,reg. wheel,inc stride
                            ),
    /* QMK accelerated mouse wheel action
    The SPD_1_x, SPD_2_x, SPD_3_x keys set three speed ranges.
    Speed is currently set at compile time. Position counter n/a. */
    [_3SPEEDACL]    = LAYOUT(                  CYCLE_RAD_LYRS,      // encoder press goto _ALTERNATE2
                            TURN_0,  GOTO_P,   KC_NO,               // counter zero, set position
                            GOTO_0,  KC_NO,   KC_NO,                // goto postion 0
                            SPD_1_U, SPD_2_U, SPD_3_U,              // up @ spd 1, up @ spd 2, up @ spd 3
                            SPD_1_D, SPD_2_D, SPD_3_D               // dn @ spd 1, dn @ spd 2, dn @ spd 3
                            ),
    /* MLS mouse wheel speed control. The encoder sets stride
    while keys are used for up and down at stride distance. */
    [_ALTERNATE2]    = LAYOUT(                   CYCLE_RAD_LYRS,     // encoder press goto _LED_SETUP
                            TURN_0,   GOTO_P, KC_NO,                // counter 0, set position, KC_NO
                            GOTO_0,   KC_NO, STRIDE_1,              // goto postion 0, kc_no, set stride to 1
                            MLS_WHLU, MLS_WHLU, MLS_WHLU,           // all keys do up
                            MLS_WHLD, MLS_WHLD, MLS_WHLD            // all keys do dn
                             ),

    [_STANDBY]      = LAYOUT(                    EXIT_STBY,         // encoder press goto _BASE
                             TO(_LED_SETUP), MSG_STBY, MSG_STBY,
                             MSG_STBY, MSG_STBY, MSG_STBY,
                             MSG_STBY, MSG_STBY, MSG_STBY,
                             MSG_STBY, MSG_STBY, MSG_STBY
                             ),

    /* LED animation setting layer */
    [_LED_SETUP]    = LAYOUT(                   TO(_STANDBY),       // encoder press goto _STANDBY
                             TO(_STANDBY), KC_NO, KC_NO,            // back to _STANDBY, kc_no, kc_no
                             LED_NXT, KC_NO, LED_TOG,               // Next RGB animation, kc_no, Animation on/off
                             LED_I_UP, LED_HUE_UP, LED_SPD_UP,      // intensity up, color hue up, amim speed up
                             LED_I_DN, LED_HUE_DN, LED_SPD_DN       // intensity dn, color hue dn, amim speed dn
                             ),

};

static int16_t en_turns = 0;        /* being used to record number of encoder turns */
static int16_t prev_pos = 0;        /* being used to record number of encoder turns back to position p */
static int16_t stride = 1;          /* stride is a distance (number of encoder turns) concept*/
static bool position_valid = true;  /* flag indicating the position counter is valid */
static int16_t active_layer = 0;    /* manually keeping track of current layer */
static bool new_born = true;        /* in a fresh state */

/* These values are used in the scheme to cycle through layers but skip some layers */
#define LYR_CYCLE_START 0
#define LYR_CYCLE_END 3
/* Values used to name oled page (line) number */
#define MSG_LINE_ACTION 0
#define MSG_LINE_POS 1
#define MSG_LINE_STRIDE 2
#define MSG_LINE_KEYS 4
#define MSG_LINE_ENCODER 5
#define MSG_LINE_ALT1 6


#ifdef ENCODER_MAP_ENABLE
/*  This sets what the encoder turning does at each layer.
    There has to be an enum setting for each defined layer and action.
    Otherwise there will be a compile error.
*/
const uint16_t PROGMEM encoder_map[][NUM_ENCODERS][NUM_DIRECTIONS] = {
    [_BASE]        = {ENCODER_CCW_CW(ENC_U,ENC_D)}, // custom function for encoder turn being up & dn
    [_3SPEEDACL]    = {ENCODER_CCW_CW(ENC_U,ENC_D)}, // custom function for encoder turn being up & dn
    [_ALTERNATE2]    = {ENCODER_CCW_CW(ENC_STRIDE_INC,ENC_STRIDE_DEC)}, // custom function for encoder turn being change strive value
    [_LED_SETUP] = {ENCODER_CCW_CW(LED_I_DN, LED_I_UP)}, // LED Intensity
    [_STANDBY]      = {ENCODER_CCW_CW(KC_NO, KC_NO)}      // do nothing
};
#endif

#ifdef OLED_ENABLE

// logo render time during startup
#    ifndef SHOW_LOGO
#        define SHOW_LOGO 2000
#    endif

// variables used for showing logo only at startup
static bool     logo_is_visible = true;
static uint32_t oled_logo_timer = 0;

/* constant that is the data for the logo graphic */
static const char PROGMEM mls_logo[] = {
    // 'mls_logo_layers_totop', 128x24px
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x30,
    0x70, 0xE0, 0xC0, 0x80, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x80, 0xC0, 0xE0, 0x70, 0x30,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x80,
    0xC0, 0x60, 0x30, 0x10, 0x08, 0x08,
    0x08, 0x08, 0x08, 0x08, 0x18, 0x10,
    0x30, 0x60, 0xC0, 0x80, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x80, 0x80, 0x00, 0x00,
    0x18, 0x38, 0x70, 0xE0, 0xC3, 0x87,
    0x0E, 0x1C, 0x38, 0x70, 0xE1, 0xC3,
    0xE7, 0xFE, 0x7C, 0x3E, 0x1F, 0x3E,
    0x7C, 0xFE, 0xE7, 0xC3, 0xC1, 0xE0,
    0x70, 0x38, 0x1C, 0x0E, 0x87, 0xC3,
    0xE0, 0x70, 0x38, 0x10, 0x00, 0x80,
    0x80, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0xFE, 0x02, 0x02, 0x02,
    0x02, 0x02, 0x02, 0x02, 0x02, 0x02,
    0x02, 0x7F, 0xC0, 0x80, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x80, 0xC0, 0xFF,
    0x02, 0x02, 0x02, 0x02, 0x02, 0x02,
    0x02, 0x02, 0x02, 0x02, 0x02, 0x02,
    0x02, 0x02, 0x02, 0x02, 0x02, 0xFE,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0xC0, 0xC0, 0x80,
    0x00, 0x18, 0x38, 0x70, 0xE1, 0xC3,
    0x87, 0x0E, 0x1C, 0x38, 0xF0, 0xE0,
    0xE1, 0xF3, 0x7F, 0x3E, 0x1E, 0x0F,
    0x87, 0xC3, 0x61, 0x30, 0x18, 0x0C,
    0x06, 0x0C, 0x18, 0x30, 0x61, 0xC3,
    0x87, 0x0F, 0x1E, 0x3C, 0x7E, 0xF7,
    0xE3, 0xE1, 0xF0, 0xB8, 0x1C, 0x0E,
    0x07, 0x83, 0xC1, 0xE0, 0x70, 0x38,
    0x18, 0x00, 0x80, 0xC0, 0xC0, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0xFF, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x01,
    0x03, 0x02, 0x06, 0x04, 0x04, 0x04,
    0x04, 0x04, 0x06, 0x06, 0x07, 0x0F,
    0x19, 0x31, 0x63, 0xC6, 0x8C, 0x18,
    0x30, 0x60, 0xC0, 0x80, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0xFF, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x01, 0x03, 0x07, 0x0E, 0x9C, 0xF8,
    0xF0, 0xF1, 0x7B, 0x3F, 0x1E, 0x0F,
    0x07, 0x03, 0xF9, 0xF8, 0x78, 0xE0,
    0x00, 0xE0, 0x78, 0xF8, 0xF8, 0x00,
    0xF8, 0xF8, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x70, 0xF8, 0xC8, 0xC8,
    0x88, 0x98, 0x11, 0x03, 0x07, 0x0F,
    0x1F, 0x3E, 0x7F, 0xFB, 0xF1, 0xF0,
    0xF8, 0x9C, 0x0E, 0x07, 0x03, 0x01,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0xFF, 0x80, 0x80, 0x80, 0x80, 0x80,
    0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
    0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
    0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
    0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
    0x81, 0x83, 0x86, 0x8C, 0x8C, 0x87,
    0x83, 0x80, 0x80, 0x80, 0x80, 0x80,
    0x80, 0x80, 0x80, 0xFF, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x80, 0xC0, 0xE0, 0x71,
    0x3B, 0x1F, 0x0F, 0x8F, 0xDE, 0xFC,
    0xF8, 0xF0, 0xE0, 0xC0, 0x8F, 0x0F,
    0x00, 0x07, 0x0E, 0x07, 0x00, 0x0F,
    0x0F, 0x00, 0x0F, 0x0F, 0x08, 0x08,
    0x08, 0x08, 0x00, 0x00, 0x04, 0x0C,
    0x08, 0x09, 0x09, 0x0F, 0x87, 0xC0,
    0xE0, 0xF0, 0xF8, 0xFC, 0xFE, 0x9F,
    0x0F, 0x0F, 0x1F, 0x3B, 0x71, 0xE0,
    0xC0, 0x80, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x1F, 0x10, 0x10, 0x10,
    0x10, 0x10, 0x10, 0x10, 0x10, 0x10,
    0x10, 0x10, 0x10, 0x10, 0x10, 0x10,
    0x10, 0x10, 0xF0, 0xF6, 0x16, 0x10,
    0x16, 0x16, 0x10, 0x16, 0xF6, 0xF0,
    0x10, 0x10, 0x10, 0x10, 0x10, 0x10,
    0x10, 0x10, 0x10, 0x10, 0x10, 0x10,
    0x10, 0x10, 0x10, 0x10, 0x10, 0x1F,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x03, 0x03, 0x01,
    0x30, 0x38, 0x1C, 0x0E, 0x07, 0x83,
    0xC1, 0xE0, 0x70, 0x39, 0x1F, 0x0F,
    0x87, 0xCF, 0xFE, 0x7C, 0x78, 0xF0,
    0xE1, 0xC3, 0x86, 0x0C, 0x18, 0x30,
    0x60, 0x30, 0x18, 0x0C, 0x86, 0xC3,
    0xE1, 0xF0, 0xF8, 0x7C, 0x7E, 0xFF,
    0xCF, 0x87, 0x0F, 0x1D, 0x38, 0x70,
    0xE1, 0xC3, 0x87, 0x0E, 0x1C, 0x38,
    0x30, 0x00, 0x01, 0x03, 0x03, 0x00,
    0x00, 0x00, 0x00, 0x80, 0x80, 0x80,
    0x80, 0x80, 0x80, 0x80, 0x80, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x02, 0x03, 0x03, 0x03, 0x03,
    0x83, 0x83, 0x03, 0x03, 0x03, 0x03,
    0x03, 0x03, 0x03, 0x03, 0x02, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x80, 0x80,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x03, 0x03, 0x01, 0x10, 0x38, 0x1C,
    0x0E, 0x87, 0xC3, 0xE1, 0x70, 0x38,
    0x1C, 0x0F, 0x87, 0xC7, 0xEF, 0x7F,
    0x3E, 0x7C, 0xF8, 0x7C, 0x3E, 0x7F,
    0xFF, 0xC7, 0x83, 0x07, 0x0E, 0x1C,
    0x38, 0x70, 0xE1, 0xC3, 0x07, 0x0E,
    0x1C, 0x08, 0x00, 0x01, 0x01, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0xFF,
    0xFF, 0x31, 0x31, 0x31, 0x71, 0xF1,
    0xDB, 0x9F, 0x0E, 0x00, 0x00, 0x98,
    0xDC, 0xCC, 0x6C, 0x6C, 0xFC, 0xF8,
    0x00, 0x00, 0xF0, 0xF8, 0x1C, 0x0C,
    0x0C, 0x18, 0xFF, 0xFF, 0x00, 0x00,
    0xFC, 0xFC, 0x18, 0x0C, 0x0C, 0x1C,
    0xF8, 0xF0, 0x00, 0x98, 0xDC, 0xCC,
    0x6C, 0x6C, 0xFC, 0xF8, 0x00, 0x00,
    0xF0, 0xF8, 0x1C, 0x0C, 0x0C, 0x18,
    0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x01, 0x01, 0x00,
    0x00, 0x0C, 0x0E, 0x07, 0x03, 0x01,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x01, 0x03, 0x07,
    0x0E, 0x0C, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x07, 0x07, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x03, 0x07, 0x06, 0x04,
    0x00, 0x03, 0x07, 0x06, 0x06, 0x06,
    0x03, 0x07, 0x04, 0x00, 0x01, 0x03,
    0x07, 0x06, 0x06, 0x03, 0x07, 0x07,
    0x00, 0x00, 0x3F, 0x3F, 0x03, 0x06,
    0x06, 0x07, 0x03, 0x01, 0x00, 0x03,
    0x07, 0x06, 0x06, 0x06, 0x03, 0x07,
    0x04, 0x00, 0x01, 0x03, 0x07, 0x06,
    0x06, 0x03, 0x07, 0x07, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00,
};

void render_logo(void) {
    // set the oled display rotation
    // note: this oled only does 0 and 180 in its hardware
    // rotating 90 must be done in software
    bool oled_init(oled_rotation_t(OLED_ROTATION_0));
    // bool oled_init(oled_rotation_t(OLED_ROTATION_1));  // does not work 1/22/2025
    // send logo data to oled
    oled_write_raw_P(mls_logo, sizeof(mls_logo));
}

void clear_screenlogo(void) {
    /* clear logo is logo is still visible */
    if (logo_is_visible) {
        for (uint8_t i = 0; i < OLED_DISPLAY_HEIGHT; ++i) {
            for (uint8_t j = 0; j < OLED_DISPLAY_WIDTH; ++j) {
                oled_write_raw_byte(0x0, i * OLED_DISPLAY_WIDTH + j);
            }
        }
    }
    logo_is_visible = false;
}

void init_logo_timer(void) {
    oled_logo_timer = timer_read32();
};

void keyboard_post_init_kb(void) {
    /* This function executes once after most of the keyboard is initialized.
    The logo gets rendered and its timer to be visible is set. The logo is
    visible flag is already set as default true.*/
    render_logo();
    init_logo_timer();
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

/* oled_clean_write_ln() first cleans the desired ln_y oled line (frame)
before then writing the desired text at the desired ln_y position.
This gets around the problem of having residual text from previous
writes on that line. */
void oled_clean_write_ln(uint8_t ln_x, uint8_t ln_y, char wrds[], bool inverted ) {
    oled_set_cursor(0, ln_y);
    oled_write_ln(PSTR(""), false);
    oled_set_cursor(ln_x, ln_y);
    oled_write_ln(PSTR(wrds), inverted);
}

/* clears an oled line (page) by line number */
void oled_clean_ln( uint8_t ln_y) {
    oled_set_cursor(0, ln_y);
    oled_write_ln(PSTR(""), false);
}

void rpt_pos(void){
    /* oled display position value */
    char buf[20];
    if (position_valid){
        snprintf(buf, 20, "Pos: %d", en_turns);
        oled_clean_write_ln(0, MSG_LINE_POS, buf, false);
    } else {
        oled_clean_write_ln(0, MSG_LINE_POS, "Pos: Now Invalid", false);
    }
}

void rpt_stride(void){
    /* oled display stride value*/
    char buf[20];
    switch (active_layer) {
        case _3SPEEDACL:
            oled_clean_ln(MSG_LINE_STRIDE);
            break;
        default:
            snprintf(buf, 20, "Stride: %d", stride);
            oled_clean_write_ln(0, MSG_LINE_STRIDE, buf, false);
            break;
    }
}

void rpt_position_etc(void){
    /* report radiology use function status like pos and stride */
    rpt_pos();
    rpt_stride();
}

void rpt_led_status(void){
        /* reports all the current LED setup information to the oled */
        char buf[20];
        uint8_t anim_mode = rgb_matrix_get_mode();
         // msg template    (0, MSG_LINE_ACTION,"xxxxxxxxxxxxxxxxxxxx", false);
        if (rgb_matrix_is_enabled()){
            snprintf(buf, 20,                   "Mode: %d          On", anim_mode);
            oled_clean_write_ln(0, MSG_LINE_POS, buf, false);
        } else {
            snprintf(buf, 20,                   "Mode: %d         Off", anim_mode);
            oled_clean_write_ln(0, MSG_LINE_POS, buf, true);
        }

        uint8_t intensity = rgb_matrix_get_val();
        snprintf(buf, 20, "Intensity: %d", intensity);
        oled_clean_write_ln(0, MSG_LINE_STRIDE, buf, false);

        uint8_t hue = rgb_matrix_get_hue();
        snprintf(buf, 20, "Color: %d", hue);
        oled_clean_write_ln(0, MSG_LINE_STRIDE+1, buf, false);

        uint8_t spd = rgb_matrix_get_speed();
        snprintf(buf, 20, "Speed: %d", spd);
        oled_clean_write_ln(0, MSG_LINE_STRIDE+2, buf, false);

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
    return false;
}


bool oled_task_user(void) {
    /* This function executes at every matrix scan as long as the oled_task_kb allows
    it. That happens when oled_task_kb returns true. Basically on a continuous basis
    depending on what oled_task_kb does. */

    /* writing to the oled per the current layer */
    // msg template    "xxxxxxxxxxxxxxxxxxxx"
    switch (get_highest_layer(layer_state)) {
        case _BASE:
            /* Sheesh! */
            if (!new_born){
                oled_advance_page(false);
                oled_advance_page(false);
                oled_advance_page(false);
                oled_advance_page(false);

            }else{
                oled_clean_write_ln(3, MSG_LINE_ACTION, "---  Ready  ---", false);
                rpt_position_etc();
                oled_write_ln(PSTR(""), false);
            }
            oled_write_ln(PSTR(" -- Std Mse Wheel --"), false);
            oled_write_ln(PSTR("Encoder: Mse Wheel"), false);
            oled_write_ln(PSTR("Press Encoder: Next"), false);
            oled_write_ln(PSTR(""), false);
            active_layer = _BASE;
            new_born = false;
            break;
        case _3SPEEDACL:
            /* Advance_page moves to next oled line while keeping what
            is already on the current line. The top four lines are not
            erased from what was already there.*/
            oled_advance_page(false);
            oled_advance_page(false);
            oled_advance_page(false);
            oled_advance_page(false);
            oled_write_ln(PSTR(" -- Acl Mse Wheel --"), false);
            oled_write_ln(PSTR("3 Wheel Speed Modes"), false);
            oled_write_ln(PSTR("Encoder: Mse Wheel"), false);
            oled_write_ln(PSTR("Press Encoder: Next"), false);
            active_layer = _3SPEEDACL;
            break;
        case _ALTERNATE2:
            /* Advance_page moves to next oled line while keeping what
            is already on the current line. The top four lines are not
            erased from what was already there.*/
            oled_advance_page(false);
            oled_advance_page(false);
            oled_advance_page(false);
            oled_advance_page(false);
            oled_write_ln(PSTR(" -- MLS Mse Wheel --"), false);
            oled_write_ln(PSTR("Encoder: Stride"), false);
            oled_write_ln(PSTR(""), false);
            oled_write_ln(PSTR("Press Encoder: Next"), false);
            active_layer = _ALTERNATE2;
            break;
        case _STANDBY:
            oled_write_ln(PSTR("  -- Standby Mode --"), false);
            oled_write_ln(PSTR(""), false);
            oled_write_ln(PSTR(""), false);
            oled_write_ln(PSTR(""), false);
            oled_write_ln(PSTR(""), false);
            oled_write_ln(PSTR(""), false);
            oled_write_ln(PSTR("#1 Key: LED Setup"), false);
            oled_write_ln(PSTR("Press Encoder: Next"), false);
            active_layer = _STANDBY;
            break;
        case _LED_SETUP:
            oled_write_ln(PSTR("  --  LED Setup  --"), false);
            oled_advance_page(false);
            oled_advance_page(false);
            oled_advance_page(false);
            oled_advance_page(false);
            oled_write_ln(PSTR(""), false);
            oled_write_ln(PSTR("Encoder: Intensity"), false);
            oled_write_ln(PSTR("Press Encoder: Next"), false);
            active_layer = _LED_SETUP;
            break;
        default:
            break;
    }
    return true;
}

#endif  /* for OLED_ENABLE */

/* These values are used for the repeating scroll key method that provides
a point where the scroll can be counted. The technique is Borrowed from
https://getreuer.info**/
static uint8_t repeat_cnt = 0;
/* This array customizes the rate at which the key repeats.
Units are milliseconds. Values must be between 1 and 255. */
static const uint8_t REP_DELAY_MS[] PROGMEM = {
    // Delay before the first repeat in ms.
    80,
    // The next repeats at periods in ms.
    40, 40, 30, 30, 20, 20, 20, 20, 15, 15, 15, 15, 15,
    // Subsequent repeats in ms.
    10};

void do_countable_wh_u(bool pressed){
    static deferred_token rep_token = INVALID_DEFERRED_TOKEN;
    if (!pressed) { // key released: stop repeating.
    cancel_deferred_exec(rep_token);
    rep_token = INVALID_DEFERRED_TOKEN;
  } else if (!rep_token) { // key pressed: start repeating.
    uint32_t thiskey_rep_callback(uint32_t trigger_time, void* cb_arg) {
        tap_code(KC_WH_U);
        en_turns++;
        rpt_pos();
        if (repeat_cnt < sizeof(REP_DELAY_MS) - 1) {
            ++repeat_cnt;
        }
        return pgm_read_byte(REP_DELAY_MS + repeat_cnt);
    }
    tap_code(KC_WH_U);  // Initial tap of the key.
    oled_clean_write_ln(4, MSG_LINE_ACTION, "+ Hyper In +", false);
    en_turns++;
    rpt_pos();
    // Schedule key to repeat.
    rep_token = defer_exec(REP_DELAY_MS[0], thiskey_rep_callback, NULL);
    repeat_cnt = 0;
  }
}

void do_countable_wh_d(bool pressed){
    static deferred_token rep_token = INVALID_DEFERRED_TOKEN;
    if (!pressed) { // key released: stop repeating.
    cancel_deferred_exec(rep_token);
    rep_token = INVALID_DEFERRED_TOKEN;
  } else if (!rep_token) { // key pressed: start repeating.
    uint32_t thiskey_rep_callback(uint32_t trigger_time, void* cb_arg) {
        tap_code(KC_WH_D);
        en_turns--;
        rpt_pos();
        if (repeat_cnt < sizeof(REP_DELAY_MS) - 1) {
            ++repeat_cnt;
        }
        return pgm_read_byte(REP_DELAY_MS + repeat_cnt);
    }
    tap_code(KC_WH_D);  // Initial tap of the key.
    oled_clean_write_ln(4, MSG_LINE_ACTION, "- Hyper Out -", false);
    en_turns--;
    rpt_pos();
    // Schedule key to repeat.
    rep_token = defer_exec(REP_DELAY_MS[0], thiskey_rep_callback, NULL);
    repeat_cnt = 0;
  }
}
void do_enc_stride_inc(bool pressed){
    /* Encoder turn increases the stride value by 1 */
    if (pressed) {
        stride++;
        oled_clean_write_ln(1, MSG_LINE_ACTION, "| +  Stride Chg  + |", false);
        }
}

void do_enc_stride_dec(bool pressed){
    /* Encoder turn decreases the stride value by 1 */
    if (pressed) {
        if (stride > 1) {
            stride--;
            oled_clean_write_ln(1, MSG_LINE_ACTION, "| -  Stride Chg  - |", false);
        }else{
            oled_clean_write_ln(1, MSG_LINE_ACTION, "|  Positive Only!  |", true);
        }
    }
}

void do_turn_0(bool pressed){
    /* Resets the encoder turns counter to 0 */
    if (pressed) {
        en_turns = 0;
        position_valid = true;
        oled_clean_write_ln(3, 0, "000  Zero Pos 000", false);
    }
}

void do_stride_1(bool pressed){
    /* Resets the stride value to 1*/
    if (pressed) {
        stride = 1;
        oled_clean_write_ln(1, 0, "| 1 Stride Reset 1 |", false);
    }
}

void do_msg_stby(bool pressed){
    /* flash the oled display, used when a key is pressed
    while in standby mode to remind user they are in standby mode.*/
    if (pressed) {
        oled_invert(true);
    } else {
        oled_invert(false);
    }
}

void do_spd_1_u(bool pressed) {
    /* After first setting accelerated (actually constant) mouse wheel event speed to
    speed 1, send continuous mouse wheel up events. As of this time, we cannot keep
    count of the number sent, so the en_turns is not updated.*/
    if (pressed) {
        // when pressed
        tap_code(MS_ACL0);
        register_code(KC_WH_U);
        oled_clean_write_ln(3, 0, "+ Speed 1 In +", false);
        position_valid = false;
    } else {
        // when released
        unregister_code(KC_WH_U);
        oled_clean_ln(0);
    }
}

void do_spd_1_d(bool pressed) {
/* After first setting accelerated (actually constant) mouse wheel event speed to
speed 1, send continuous mouse wheel dn events. As of this time, we cannot keep
count of the number sent, so the en_turns is not updated.*/
    if (pressed) {
        // when pressed
        tap_code(MS_ACL0);
        register_code(KC_WH_D);
        oled_clean_write_ln(3, 0, "- Speed 1 Out -", false);
        position_valid = false;
    } else {
        // when released
        unregister_code(KC_WH_D);
        oled_clean_ln(0);
    }
}

void do_spd_2_u(bool pressed) {
/* After first setting accelerated (actually constant) mouse wheel event speed to
speed 2, send continuous mouse wheel up events. As of this time, we cannot keep
count of the number sent, so the en_turns is not updated.*/
    if (pressed) {
        // when pressed
        tap_code(MS_ACL1);
        register_code(KC_WH_U);
        oled_clean_write_ln(3, 0, "++ Speed 2 In ++", false);
        position_valid = false;
    } else {
        // when released
        unregister_code(KC_WH_U);
        oled_clean_ln(0);
    }
}

void do_spd_2_d(bool pressed) {
/* After first setting accelerated (actually constant) mouse wheel event speed to
speed 2, send continuous mouse wheel dn events. As of this time, we cannot keep
count of the number sent, so the en_turns is not updated.*/
    if (pressed) {
        tap_code(MS_ACL1);
        register_code(KC_WH_D);
        oled_clean_write_ln(2, 0, "-- Speed 2 Out --", false);
        position_valid = false;
    } else {
        unregister_code(KC_WH_D);
        oled_clean_ln(0);
    }
}

void do_spd_3_u(bool pressed) {
/* After first setting accelerated (actually constant) mouse wheel event speed to
speed 3, send continuous mouse wheel up events. As of this time, we cannot keep
count of the number sent, so the en_turns is not updated.*/
    if (pressed) {
        tap_code(MS_ACL2);
        register_code(KC_WH_U);
        oled_clean_write_ln(1, 0, "+++ Speed 3 In +++", false);
        position_valid = false;
    } else {
        unregister_code(KC_WH_U);
        oled_clean_ln(0);
    }
}

void do_spd_3_d(bool pressed) {
 /* After first setting accelerated (actually constant) mouse wheel event speed to
speed 3, send continuous mouse wheel dn events. As of this time, we cannot keep
count of the number sent, so the en_turns is not updated.*/
    if (pressed) {
        tap_code(MS_ACL2);
        register_code(KC_WH_D);
        oled_clean_write_ln(1, 0, "--- Speed 3 Out ---", false);
        position_valid = false;
    } else {
        unregister_code(KC_WH_D);
        oled_clean_ln(0);
    }
}

void do_enc_u(bool pressed) {
/* Send single mouse wheel up, stride times.
Used when encoder is turned (ccw).
The en_turns value is updated for each wheel up sent.
Note: an encoder event comes as a press/release pair */
    if (pressed) {
        for (int i = 0; i < stride ; i++) {
                tap_code(KC_WH_U);
                en_turns++;
            }
        oled_clean_write_ln(2, MSG_LINE_ACTION, "+++ In Scroll +++", false);
    }
}

void do_enc_d(bool pressed) {
/* Send single mouse wheel dn, stride times.
Used when encoder is turned (cw).
The en_turns value is updated for each wheel dn sent.
Note: an encoder event comes as a press/release pair */
    if (pressed) {
        for (int i = 0; i < stride ; i++) {
                tap_code(KC_WH_D);
                en_turns--;
            }
        oled_clean_write_ln(1, MSG_LINE_ACTION, "--- Out Scroll ---", false);
    }
}


void do_mls_whlu (bool pressed) {
/* currently identical to ENC_U
Send single mouse wheel up, stride times.
Used for keypresses in MSL mode.
The en_turns value is updated for each wheel up sent.*/
    if (pressed) {
        for (int i = 0; i < stride ; i++) {
            // register_code(KC_WH_U); // NO, does not help!
            tap_code(KC_WH_U);
            en_turns++;
        }
        oled_clean_write_ln(1, MSG_LINE_ACTION, "++ MLS In Scroll ++", false);
    }
}

void do_mls_whld (bool pressed) {
/* currently identical to ENC_D
Send single mouse wheel dn, stride times.
Used for keypresses in MSL mode.
The en_turns value is updated for each wheel dn sent.
Note: an encoder event comes as a press/release pair */
    if (pressed) {
        for (int i = 0; i < stride ; i++) {
            // register_code(KC_WH_D); // NO, does not help!
            tap_code(KC_WH_D);
            en_turns--;
        }
        oled_clean_write_ln(1, MSG_LINE_ACTION, "-- MLS Out Scroll --", false);
    }
}

void do_goto_0(bool pressed) {
/* Sends mouse wheel up or mouse wheel dn commands the number of times
required to return to 0 position.*/
    if (pressed) {
        if (position_valid){
            prev_pos = en_turns;
            while (en_turns != 0){
                if (en_turns < 0) {
                    tap_code(KC_WH_U);
                    en_turns++;
                } else if (en_turns > 0) {\
                    tap_code(KC_WH_D);
                    en_turns--;
                }
            }
            oled_clean_write_ln(1, MSG_LINE_ACTION, "-- Scrolled To 0 --", false);
            } else {
                oled_clean_write_ln(0, MSG_LINE_ACTION, "!! Not Applicable !!", true);
            }
    } else {
        if (!position_valid){
            oled_clean_write_ln(0, MSG_LINE_ACTION, "!! Not Applicable !!", false);
        }
    }
}

void do_goto_p(bool pressed) {
/* Sends mouse wheel up or mouse wheel dn commands the number of times
required to return to previous position p.*/
    if (pressed) {
        if (position_valid){
            int16_t delta_turns = prev_pos - en_turns;
            int8_t dir = 0;
            prev_pos = en_turns;
            oled_clean_ln(MSG_LINE_ACTION);
            while (delta_turns != 0){
                if (delta_turns < 0) {
                    tap_code(KC_WH_D);
                    delta_turns++;
                    en_turns--;
                    dir = -1;
                } else if (delta_turns > 0) {
                    tap_code(KC_WH_U);
                    delta_turns--;
                    en_turns++;
                    dir = 1;
                }
            }
            switch(dir){
                case 0:
                    // msg template    (0, MSG_LINE_ACTION,"xxxxxxxxxxxxxxxxxxxx", false);
                    oled_clean_write_ln(0, MSG_LINE_ACTION,"~  A Boundary Set  ~", false);
                    break;
                case 1:
                    oled_clean_write_ln(0, MSG_LINE_ACTION,"~   Boundary In    ~", false);
                    break;
                case -1:
                    oled_clean_write_ln(0, MSG_LINE_ACTION,"~   Boundary Out   ~", false);
                    break;
                default:
                    break;
            }
            } else {
                oled_clean_write_ln(1, MSG_LINE_ACTION, "!! Not Applicable !!", true);
            }
    } else {
        if (!position_valid){
            oled_clean_write_ln(1, MSG_LINE_ACTION, "!! Not Applicable !!", false);
        }
    }
}

void do_exit_standby(bool pressed){
/* oled display a ready message*/
    if (pressed) {
        layer_move(LYR_CYCLE_START);
        oled_clean_write_ln(3, MSG_LINE_ACTION, "---  Ready  ---", false);
        rpt_position_etc();
    }

}

void do_cycle_rad_lyr(bool pressed){
    if (!pressed){return;}
    uint8_t current_layer = get_highest_layer(layer_state);
    if (current_layer > LYR_CYCLE_END || current_layer < LYR_CYCLE_START){return;}
    uint8_t next_layer = current_layer +1;
    if (next_layer > LYR_CYCLE_END){
        next_layer = LYR_CYCLE_START;
    }
    layer_clear();
    layer_move(next_layer);
    return;
}

void do_led_tog(bool pressed){
    if (pressed) {
        if (rgb_matrix_is_enabled()) {
            rgb_matrix_disable();
        } else {
            rgb_matrix_enable();
        }
    }
    rpt_led_status();
}

void do_led_nxt(bool pressed){
    if (pressed) {
        rgb_matrix_step();
    }
    rpt_led_status();
}

void do_led_i_up(bool pressed){
    if (pressed) {
        rgb_matrix_increase_val();
    }
    rpt_led_status();
}

void do_led_i_dn(bool pressed){
    if (pressed) {
        rgb_matrix_decrease_val();
    }
    rpt_led_status();
}

void do_led_hue_up(bool pressed){
    if (pressed) {
        rgb_matrix_increase_hue();
    }
    rpt_led_status();
}

void do_led_hue_dn(bool pressed){
    if (pressed) {
        rgb_matrix_decrease_hue();
    }
    rpt_led_status();
}

void do_led_spd_up(bool pressed){
    if (pressed) {
        rgb_matrix_increase_speed();
    }
    rpt_led_status();
}

void do_led_spd_dn(bool pressed){
    if (pressed) {
        rgb_matrix_decrease_speed();
    }
    rpt_led_status();
}

layer_state_t layer_state_set_user(layer_state_t state) {
    switch (get_highest_layer(state)) {
        case _LED_SETUP:
            rpt_led_status();
            break;
        default:
            break;
    }
  return state;
}

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    /*
    This function executes at every matrix scan as long as the
    process_record__kb allows it. That happens when process_record_kb
    returns true. Basically on a continuous basis depending on
    what process_record_kb does. */
    /*
    tap_code(keycode) is press followed by release
    register_code(keycode) is press
    unregister_code(keycode) is release
    */

    switch (keycode) {
        case EXP_WH_U:
            /* Hyper standard QMK mouse wheel up, keeping
            count of the number sent, so the en_turns is not updated.*/
            do_countable_wh_u(record->event.pressed);
            rpt_position_etc();  // update oled display for position etc.
            break;

        case EXP_WH_D:
            /* Hyper standard QMK mouse wheel up, keeping
            count of the number sent, so the en_turns is not updated.*/
            do_countable_wh_d(record->event.pressed);
            rpt_position_etc();  // update oled display for position etc.
            break;

        case ENC_STRIDE_INC:
            /* Encoder turn increases the stride value by 1 */
            do_enc_stride_inc(record->event.pressed);
            rpt_position_etc();  // update oled display for position etc.
            break;

        case ENC_STRIDE_DEC:
            /* Encoder turn decreases the stride value by 1 */
            do_enc_stride_dec(record->event.pressed);
            rpt_position_etc();  // update oled display for position etc.
            break;

        case TURN_0:
          /* Resets the encoder turns counter to 0 */
            do_turn_0(record->event.pressed);
            rpt_position_etc();  // update oled display for position etc.
            break;

        case STRIDE_1:
           /* Resets the stride value to 1*/
            do_stride_1(record->event.pressed);
            rpt_position_etc();  // update oled display for position etc.
            break;

        case MSG_STBY:
            /* oled display the standby message, used when a key is pressed
            while in standby mode to remind user they are in standby mode.
            Otherwise they might wonder why nothing happens. */
            do_msg_stby(record->event.pressed);
            break;

        case SPD_1_U:
            /* After first setting accelerated (actually constant) mouse wheel event speed to
            speed 1, send continuous mouse wheel up events. As of this time, we cannot keep
            count of the number sent, so the en_turns is not updated.*/
            do_spd_1_u(record->event.pressed);
            rpt_position_etc();  // update oled display for position etc.
            break;

        case SPD_1_D:
            /* After first setting accelerated (actually constant) mouse wheel event speed to
            speed 1, send continuous mouse wheel dn events. As of this time, we cannot keep
            count of the number sent, so the en_turns is not updated.*/
            do_spd_1_d(record->event.pressed);
            rpt_position_etc();  // update oled display for position etc.
            break;

        case SPD_2_U:
            /* After first setting accelerated (actually constant) mouse wheel event speed to
            speed 2, send continuous mouse wheel up events. As of this time, we cannot keep
            count of the number sent, so the en_turns is not updated.*/
            do_spd_2_u(record->event.pressed);
            rpt_position_etc();  // update oled display for position etc.
            break;

        case SPD_2_D:
            /* After first setting accelerated (actually constant) mouse wheel event speed to
            speed 2, send continuous mouse wheel dn events. As of this time, we cannot keep
            count of the number sent, so the en_turns is not updated.*/
            do_spd_2_d(record->event.pressed);
            rpt_position_etc();  // update oled display for position etc.
            break;

        case SPD_3_U:
            /* After first setting accelerated (actually constant) mouse wheel event speed to
            speed 3, send continuous mouse wheel up events. As of this time, we cannot keep
            count of the number sent, so the en_turns is not updated.*/
            do_spd_3_u(record->event.pressed);
            rpt_position_etc();  // update oled display for position etc.
            break;

        case SPD_3_D:
            /* After first setting accelerated (actually constant) mouse wheel event speed to
            speed 3, send continuous mouse wheel dn events. As of this time, we cannot keep
            count of the number sent, so the en_turns is not updated.*/
            do_spd_3_d(record->event.pressed);
            rpt_position_etc();  // update oled display for position etc.
            break;

        case ENC_U:
            /* Send single mouse wheel up, stride times.
            Used when encoder is turned (ccw).
            The en_turns value is updated for each wheel up sent.
            Note: an encoder event comes as a press/release pair */
            do_enc_u(record->event.pressed);
            rpt_position_etc();  // update oled display for position etc.
            break;

        case ENC_D:
            /* Send single mouse wheel dn, stride times.
            Used when encoder is turned (cw).
            The en_turns value is updated for each wheel dn sent.
            Note: an encoder event comes as a press/release pair */
            do_enc_d(record->event.pressed);
            rpt_position_etc();  // update oled display for position etc.
            break;

        case MLS_WHLU:
            /* currently identical to ENC_U
            Send single mouse wheel up, stride times.
            Used for keypresses in MSL mode.
            The en_turns value is updated for each wheel up sent.*/
            do_mls_whlu(record->event.pressed);
            rpt_position_etc();  // update oled display for position etc.
            break;

        case MLS_WHLD:
            /* currently identical to ENC_D
            Send single mouse wheel dn, stride times.
            Used for keypresses in MSL mode.
            The en_turns value is updated for each wheel dn sent.
            Note: an encoder event comes as a press/release pair */
            do_mls_whld(record->event.pressed);
            rpt_position_etc();  // update oled display for position etc.
            break;

        case GOTO_0:
            /* Sends mouse wheel up or mouse wheel dn commands the number of times
            required to return to 0 position.*/
            do_goto_0(record->event.pressed);
            rpt_position_etc();  // update oled display for position etc.
            break;

        case EXIT_STBY:
            /* oled display a ready message*/
            do_exit_standby(record->event.pressed);
            return false;
            break;

        case CYCLE_RAD_LYRS:
            /* Cycle to next layer in the rad use layers */
            do_cycle_rad_lyr(record->event.pressed);
            return false;
            break;

        case GOTO_P:
            /* Sends mouse wheel up or mouse wheel dn commands the number of times
            required to return to position p.*/
            do_goto_p(record->event.pressed);
            rpt_position_etc();  // update oled display for position etc.
            break;

        case LED_TOG:
            /* Toggles LED animation */
            do_led_tog(record->event.pressed);
            break;

        case LED_NXT:
            /* Set animation to next animation */
            do_led_nxt(record->event.pressed);
            break;

        case LED_I_UP:
            /* LED intensity up */
            do_led_i_up(record->event.pressed);
            break;

        case LED_I_DN:
            /* LED intensity dn */
            do_led_i_dn(record->event.pressed);
            break;

        case LED_HUE_UP:
            /* LED hue number up */
            do_led_hue_up(record->event.pressed);
            break;

        case LED_HUE_DN:
            /* LED hue number up */
            do_led_hue_dn(record->event.pressed);
            break;

        case LED_SPD_UP:
            /* Animation speed up */
            do_led_spd_up(record->event.pressed);
            break;

        case LED_SPD_DN:
            /* Animation speed dn */
            do_led_spd_dn(record->event.pressed);
            break;

        default:
            break;
    }
    return true;
};

