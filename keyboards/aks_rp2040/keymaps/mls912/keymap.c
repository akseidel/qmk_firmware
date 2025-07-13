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

/*
 * Notes for debugging! Debugging requires four things in place.
 *
 * 1) CONSOLE_ENABLE = yes must be in rules.mk
 *
 * 2) debug_enable = true;  must be made at some point.
 * This is often put into keyboard_post_init_(void)
 * Additional debug_ flags are: debug_matrix, debug_keyboard and debug_mouse.
 * Exactly what they do is? But could be allowing matrix, keyboard and mouse
 * events available for debug reference.
 *
 * 3) use print, uprintf, dprint and dprint functions to print to the "QMK CONSOLE".
 * But make sure to include \n in what is printed. Also, uprintf does nothing if
 * no variable is passed into it.
 *
 * 4) In a terminal run "qmk console" to see the debug printing. qmk console will
 * find the device, but if there is more than one device printing for QMK CONSOLE
 * then specify the device using pid:vid arguments for qmk console.
 *
 * This code contains measures to provide debug helping functions when CONSOLE_ENABLE
 * is yes. There is a function that outputs to QMK CONSOLE on startup this line:
 * "----- Console debugging is functioning! -------" so that you know the apparatus for
 * debugging with QMK CONSOLE is in place and functioning.
*/

/*
 * Version 911 is the first version where OLED operation regarding how oled buffers
 * are updated in write commands but not actually rendered until some operation
 * either intentionally or unintentionally renders the "dirty", ie unrendered data.
 * Do not use code from versions prior to 911 without fully understanding the issue.
*/

#include QMK_KEYBOARD_H
#include <time.h>
#include <errno.h>
#ifdef CONSOLE_ENABLE
    #include "print.h"
#endif
#include "quantum.h" /*Needed for oled_on() function!*/

/* Every reference name must be first defined in enum before it shows up anywhere in the code. */
enum layer_names {
                _3SPEEDACL,
                _STD,
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
    /* QMK accelerated mouse wheel action
    The SPD_1_x, SPD_2_x, SPD_3_x keys set three speed ranges.
    Speed is currently set at compile time. Position counter n/a. */
    [_3SPEEDACL]    = LAYOUT(                  CYCLE_RAD_LYRS,      // encoder press goto _ALTERNATE2
                            TURN_0,  GOTO_P,   KC_NO,               // counter zero, set position
                            GOTO_0,  KC_NO,   KC_NO,                // goto postion 0
                            SPD_1_U, SPD_2_U, SPD_3_U,              // up @ spd 1, up @ spd 2, up @ spd 3
                            SPD_1_D, SPD_2_D, SPD_3_D               // dn @ spd 1, dn @ spd 2, dn @ spd 3
                            ),
    /* One shot strides and also standard out of box mouse wheel action */
    [_STD]        = LAYOUT(                   CYCLE_RAD_LYRS,      // encoder press goto _3SPEEDACL
                            TURN_0,  GOTO_P,   KC_NO,               // counter zero
                            GOTO_0,   KC_NO, STRIDE_1,              // goto postion 0, kc_no, set stride to 1
                            ENC_U, EXP_WH_U, ENC_STRIDE_INC,        // 1 shot u click stride x,reg. wheel,inc stride
                            ENC_D, EXP_WH_D, ENC_STRIDE_DEC         // 1 shot d click stride x,reg. wheel,inc stride
                            ),
    /* MLS mouse wheel speed control. The encoder sets stride
    while keys are used for up and down at stride distance. */
    [_ALTERNATE2]    = LAYOUT(                   CYCLE_RAD_LYRS,     // encoder press goto _LED_SETUP
                            TURN_0,   GOTO_P, KC_NO,                // counter 0, set position, KC_NO
                            GOTO_0,   KC_NO, STRIDE_1,              // goto postion 0, kc_no, set stride to 1
                            MLS_WHLU, MLS_WHLU, MLS_WHLU,           // all keys do up
                            MLS_WHLD, MLS_WHLD, MLS_WHLD            // all keys do dn
                             ),

    [_STANDBY]      = LAYOUT(                    EXIT_STBY,         // encoder press goto _STD
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

#ifdef CONSOLE_ENABLE
    /* Flag as a fresh debug state to show verification message once at the QMK CONSOLE. */
    static bool new_born_debug = true;
#endif

#define VERSION_NO 912

/* These values are used in the scheme to cycle through layers but skip some layers */
#define LYR_CYCLE_START 0
#define LYR_CYCLE_END 3
/* Values used to name oled page (line) number */
#define MSG_LINE_0 0
#define MSG_LINE_1 1
#define MSG_LINE_2 2
#define MSG_LINE_3 3
#define MSG_LINE_4 4
#define MSG_LINE_5 5
#define MSG_LINE_6 6
#define MSG_LINE_7 7

/*For DIY oled timeout purposes.*/
#define DIY_OLED_TIMEOUT 1    // in minutes
static uint16_t idle_timer = 0;
static uint8_t halfmin_counter = 0;
static uint8_t dim_oled_brightness = 40;
static bool dimmed_mode = false;
static uint8_t prior_rgb_matrix_val;

#ifdef ENCODER_MAP_ENABLE
/*  This sets what the encoder turning does at each layer.
    There has to be an enum setting for each defined layer and action.
    Otherwise there will be a compile error.
*/
const uint16_t PROGMEM encoder_map[][NUM_ENCODERS][NUM_DIRECTIONS] = {
    [_STD]        = {ENCODER_CCW_CW(ENC_U,ENC_D),   // custom function for encoder turn being up & dn
                    ENCODER_CCW_CW(ENC_U,ENC_D)     // 2nd encoder if installed
                    },
    [_3SPEEDACL]    = {ENCODER_CCW_CW(ENC_U,ENC_D), // custom function for encoder turn being up & dn
                    ENCODER_CCW_CW(ENC_U,ENC_D)     // 2nd encoder if installed
                    },
    [_ALTERNATE2]    = {ENCODER_CCW_CW(ENC_STRIDE_INC,ENC_STRIDE_DEC),  // custom function for encoder turn being change strive value
                        ENCODER_CCW_CW(ENC_STRIDE_INC,ENC_STRIDE_DEC)   // 2nd encoder if installed
                        },
    [_LED_SETUP] = {ENCODER_CCW_CW(LED_I_DN, LED_I_UP),     // LED Intensity
                    ENCODER_CCW_CW(LED_HUE_DN, LED_HUE_UP)  // 2nd encoder if installed
                    },
    [_STANDBY]      = {ENCODER_CCW_CW(KC_NO, KC_NO),    // do nothing
                        ENCODER_CCW_CW(KC_NO, KC_NO)    // 2nd encoder if installed
                    }
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
    /* clear logo and set logo_is_visible flag */
    /* oled_clear() clears the screen buffer, but does not update the screen
    ie it is "dirty". oled_render_dirty() sends the updated portions to
    the screen. */
    oled_clear();
    oled_render_dirty(true);
    logo_is_visible = false;
}

void init_logo_timer(void) {
    oled_logo_timer = timer_read32();
};

void keyboard_init_kb(void) {
    /* Making sure the adafruit macropad gpio pins used for the I2C
    connector are setup for input for the 2nd encoder instead of
    for I2C bus use */
    gpio_set_pin_input_high(20);
    gpio_set_pin_input_high(21);
}

void keyboard_post_init_kb(void) {
    /* This function executes once after most of the keyboard is initialized.
    The logo gets rendered and its timer to be visible is set. The logo is
    visible flag is already set as default true.*/
    render_logo();
    init_logo_timer();
    layer_state_set_user(layer_state);
}

#ifdef CONSOLE_ENABLE
    /* Used for debugging to the QMK CONSOLE.  */

    void keyboard_post_init_user(void) {
        // Customize these values to desired behaviour
        debug_enable=true;
        // debug_keyboard=true;
        // debug_mouse=true;
    }

    /* This function is called within user oled_task, ie every matrix scan.
    It sends the debug is functioning message once to QMK CONSOLE so that it
    is known for sure that the debug messages are working. */
    void send_debug_verification(void) {
            if (new_born_debug){
                print("------ Console debugging is functioning! -------\n");
            }
            new_born_debug = false;
    }
#endif

/* - A writing to oled helper function
oled_clean_write_ln()
First cleans the desired ln_y oled line (frame) before then writing the desired text
at the desired ln_y position. This gets around the problem of having residual text
from the previous writes on that line.
Oled remains "dirty", ie new data is not actually rendered yet.*/
void oled_clean_write_ln(uint8_t ln_x, uint8_t ln_y, char wrds[], bool inverted, bool cntrd ) {
    if (cntrd){
        ln_x = (21 - (strlen(wrds)))/2;
    }
    oled_set_cursor(0, ln_y);
    oled_write_ln(PSTR(""), false);
    oled_set_cursor(ln_x, ln_y);
    oled_write_ln(PSTR(wrds), inverted);
    /* Note, at this point the oled has new data, "dirty", that have yet to be rendered.
    Clean_write_ln is called many times within the oled_task_user function, so do not render now.*/
}

/* - A writing to oled helper function
clears an oled line (page) by line number */
void oled_clean_ln( uint8_t ln_y) {
    oled_set_cursor(0, ln_y);
    oled_write_ln(PSTR(""), false);
}

void rpt_pos(void){
    /* oled display position value
    Data goes to oled buffer and will not be rendered right away.*/
    char buf[20];
    if (position_valid){
        snprintf(buf, 20, "Pos: %d", en_turns);
        oled_clean_write_ln(0, MSG_LINE_1, buf, false, false);
    } else {
        oled_clean_write_ln(0, MSG_LINE_1, "Pos: Now Invalid", false, false);
    }
}

void rpt_stride(void){
    /* oled display stride value.
    Data goes to oled buffer and will not be rendered right away.*/
    char buf[20];
    switch (active_layer) {
        case _3SPEEDACL:
            oled_clean_ln(MSG_LINE_2);
            break;
        default:
            snprintf(buf, 20, "Stride: %d", stride);
            oled_clean_write_ln(0, MSG_LINE_2, buf, false, false);
            break;
    }
}

void rpt_position_etc(void){
    /* Report radiology use function status like pos and stride.
    Data goes to oled buffer and will not be rendered right away.*/
    rpt_pos();
    rpt_stride();
}

void do_layer_mode_msg(char *l_desc, bool invert){
    oled_clean_write_ln(0, MSG_LINE_0, l_desc, invert, false);
    rpt_position_etc();
}

void rpt_led_status(void){
        /* reports all the current LED setup information to the oled */
        char buf[20];
        uint8_t anim_mode = rgb_matrix_get_mode();
         // msg template    (0, MSG_LINE_0,"xxxxxxxxxxxxxxxxxxxx", false);
        if (rgb_matrix_is_enabled()){
            snprintf(buf, 20,                   "Mode: %d          On", anim_mode);
            oled_clean_write_ln(0, MSG_LINE_1, buf, false, false);
        } else {
            snprintf(buf, 20,                   "Mode: %d         Off", anim_mode);
            oled_clean_write_ln(0, MSG_LINE_1, buf, true, false);
        }

        uint8_t intensity = rgb_matrix_get_val();
        snprintf(buf, 20, "Intensity: %d", intensity);
        oled_clean_write_ln(0, MSG_LINE_2, buf, false, false);

        uint8_t hue = rgb_matrix_get_hue();
        snprintf(buf, 20, "Color: %d", hue);
        oled_clean_write_ln(0, MSG_LINE_2+1, buf, false, false);

        uint8_t spd = rgb_matrix_get_speed();
        snprintf(buf, 20, "Speed: %d", spd);
        oled_clean_write_ln(0, MSG_LINE_2+2, buf, false, false);

}

bool oled_task_kb(void) {
    /* This function executes at every matrix scan. It provides the opportunity
    to not call the oled_task_user that executes the oled updates. The logo_is_visible
    flag is being used to not continue with oled updates. The logo has already been
    rendered during keyboard_post_init_kb. So it is showing after boot and does not
    need to be rendered again. Instead the logo timer is checked to decide when to
    erase the logo and change the logo_is_visible flag. */

    if (logo_is_visible) {
        if ((timer_elapsed32(oled_logo_timer) < SHOW_LOGO)) {
            /* Allow the logo to remain showing during SHOW_LOGO */
            return false;
        } else {
            /* Remove the logo and set logo_is_visible flag */
            clear_screenlogo();
        }
    } else {
        oled_task_user();
    }
    return false;
}

bool oled_task_user(void) {
    /* This function executes at every matrix scan as long as the oled_task_kb allows
    it. That happens when oled_task_kb returns true. Basically on a continuous basis
    depending on what oled_task_kb does. */

    /* It is critical to understand that writing to the oled updates the oled's line
    buffers but does not send the new "dirty" data to the oled to be rendered. To
    keep oled flickering at a minimum, try to render as least as possible. Thus
    the render command is issued at the oled_task_user end and is not needed anywhere
    else because oled_task_user runs for every matrix scan.*/

    #ifdef CONSOLE_ENABLE
        send_debug_verification();
    #endif

    /* At the end of this oled_task_user() is an oled_render-dirty() command. Nothing is
    properly displayed without it! But oled_render-dirty() resets the oled's display
    timeout period making OLED_TIMEOUT not functional. The DIY display dimming code
    provides a lucky way to bypass this line by exiting now when in dimmed_mode. */
    if (dimmed_mode){
        return true;
    }

    /* Writing to the oled buffers on a per current layer basis.*/
    // msg template    "xxxxxxxxxxxxxxxxxxxx"
    switch (get_highest_layer(layer_state)) {
        case _STD:

            do_layer_mode_msg("--- Std Mse Wheel ---", false);
            oled_clean_ln(MSG_LINE_4);
            oled_clean_ln(MSG_LINE_5);
            oled_clean_write_ln(0, MSG_LINE_6, "Encoder: Mse Wheel", false, false);
            oled_clean_write_ln(0, MSG_LINE_7, "Press Encoder: Next", false, false);
            active_layer = _STD;
            // new_born = false;
            break;
        case _3SPEEDACL:

            do_layer_mode_msg("--- Acl Mse Wheel ---", false);
            oled_clean_write_ln(0, MSG_LINE_5, "3 Wheel Speed Modes", false, false);
            oled_clean_write_ln(0, MSG_LINE_6, "Encoder: Mse Wheel", false, false);
            oled_clean_write_ln(0, MSG_LINE_7, "Press Encoder: Next", false, false);
            active_layer = _3SPEEDACL;
            break;
        case _ALTERNATE2:

            do_layer_mode_msg("--- MLS Mse Wheel ---", false);
            oled_clean_ln(MSG_LINE_5);
            oled_clean_write_ln(0, MSG_LINE_6, "Encoder: Stride", false, false);
            oled_clean_write_ln(0, MSG_LINE_7, "Press Encoder: Next", false, false);

            active_layer = _ALTERNATE2;
            break;
        case _STANDBY:
            oled_clean_write_ln(0, MSG_LINE_0, "--- Standby Mode ---", false, false);
            oled_clean_ln(MSG_LINE_1);
            oled_clean_write_ln(0, MSG_LINE_2, "#1 Key: LED Setup", false, false);
            oled_clean_ln(MSG_LINE_3);
            oled_clean_ln(MSG_LINE_4);
            oled_clean_ln(MSG_LINE_5);
            char buf[20];
            snprintf(buf, 20, "Version %d", VERSION_NO);
            oled_clean_write_ln(0, MSG_LINE_6, buf, false, false);
            oled_clean_write_ln(0, MSG_LINE_7, "Press Encoder: Next", false, false);

            active_layer = _STANDBY;
            break;
        case _LED_SETUP:
            oled_clean_write_ln(0, MSG_LINE_0, "---  LED Setup  ---", false, false);
            oled_clean_ln(MSG_LINE_1);
            oled_clean_ln(MSG_LINE_2);
            oled_clean_ln(MSG_LINE_3);
            oled_clean_ln(MSG_LINE_4);
            oled_clean_ln(MSG_LINE_5);
            oled_clean_write_ln(0, MSG_LINE_6, "Encoder: Intensity", false, false);
            oled_clean_write_ln(0, MSG_LINE_7, "Press Encoder: Next", false, false);
            rpt_led_status();
            active_layer = _LED_SETUP;
            break;
        default:
            break;
    }
    /* Nothing is properly displayed without this. But oled_render-dirty() resets the
    oled's display timeout period making OLED_TIMEOUT not functional. The DIY display
    dimming code provides a lucky way to bypass this line. */
    oled_render_dirty(true);

    return true;
}

#endif  /* for OLED_ENABLE */


void be_not_dimmed(void){
    if (OLED_BRIGHTNESS > oled_get_brightness()) {
        oled_set_brightness(OLED_BRIGHTNESS);
    }
    oled_scroll_off();

    /**/
    rgb_matrix_enable_noeeprom();
    /* There is not a direct way to set the led matrix brightness
    to a specific value other than 0 or 255.. This cranks the brightness
    as high as it can go to the prior_rgb_matrix_val.*/
    uint8_t decs = prior_rgb_matrix_val / RGB_MATRIX_VAL_STEP;
    uint8_t i;
    for (i = 1; i < decs; i++) {
        rgb_matrix_increase_val_noeeprom();
    }
}

void be_dimmed(void){

    oled_set_brightness(dim_oled_brightness);
    oled_scroll_right();

    prior_rgb_matrix_val = rgb_matrix_get_val();

    /* Use this instead when wanting keys leds to be off. */
    // rgb_matrix_disable_noeeprom();

    /* There is not a direct way to set the led matrix brightness
    to a specific value other than 0 or 255. This cranks the brightness
    as low as it can go above 0 via the decrease_val function.
    RGB_MATRIX_VAL_STEP being set to 1 allows the value to decrease
    to 1, but there is not any noticeable difference until val is 10.*/
    uint8_t decs = prior_rgb_matrix_val / RGB_MATRIX_VAL_STEP;
    uint8_t i;
    for (i = 1; i < decs; i++) {
        rgb_matrix_decrease_val_noeeprom();
    }

}

/*For DIY oled timeout purposes.*/
void matrix_scan_user(void) {
    /* Using matrix_scan_user to increment the idle_timer.
    The idle_time is reset elsewhere to 0 when any macropad
    key or control has triggered activity.*/
    // idle_timer needs to be set one time
    if (idle_timer == 0) idle_timer = timer_read();

    if (timer_elapsed(idle_timer) > 30000) {
        halfmin_counter++;
        idle_timer = timer_read();
    }

    if (halfmin_counter >= DIY_OLED_TIMEOUT * 2) {
        be_dimmed();
        halfmin_counter = 0;
        dimmed_mode = true;
    }
}




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


int nanosleep(const struct timespec *req, struct timespec *rem);
int msleep(long msec){
    /* msleep(): Sleep for the requested number of milliseconds. */
    struct timespec ts;
    int res;

    if (msec < 0)
    {
        errno = EINVAL;
        return -1;
    }

    ts.tv_sec = msec / 1000;
    ts.tv_nsec = (msec % 1000) * 1000000;

    do {
        res = nanosleep(&ts, &ts);
    } while (res && errno == EINTR);

    return res;
}

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
    oled_clean_write_ln(0, MSG_LINE_3, "    + Hyper In +", false, false);
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
    oled_clean_write_ln(0, MSG_LINE_3, " - Hyper Out -", false, true);
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
        oled_clean_write_ln(0, MSG_LINE_3, "| +  Stride Chg  + |", false, true);
        }
}

void do_enc_stride_dec(bool pressed){
    /* Encoder turn decreases the stride value by 1 */
    if (pressed) {
        if (stride > 1) {
            stride--;
            oled_clean_write_ln(0, MSG_LINE_3, "| -  Stride Chg  - |", false, true);
        }else{
            oled_clean_write_ln(0, MSG_LINE_3, "|  Positive Only!  |", true, true);
        }
    }
}

void do_turn_0(bool pressed){
    /* Resets the encoder turns counter to 0 */
    if (pressed) {
        en_turns = 0;
        position_valid = true;
        oled_clean_write_ln(0, MSG_LINE_3, "000 Zero Pos 000", false, true);
    }
}

void do_stride_1(bool pressed){
    /* Resets the stride value to 1*/
    if (pressed) {
        stride = 1;
        oled_clean_write_ln(0, MSG_LINE_3, "| 1 Stride Reset 1 |", true, true);
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
        oled_clean_write_ln(0, MSG_LINE_3, "+ Speed 1 In +", false, true);
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
        oled_clean_write_ln(0, MSG_LINE_3, " - Speed 1 Out -", false, true);
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
        oled_clean_write_ln(0, MSG_LINE_3, "++ Speed 2 In ++", false, true);
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
        /* Centering the text on oled requires odd number of characters */
        oled_clean_write_ln(0, MSG_LINE_3, "-- Speed 2 Out --", false, true);
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
        oled_clean_write_ln(0, MSG_LINE_3, " +++ Speed 3 In +++", false, true);
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
        /* Centering the text on oled requires odd number of characters */
        oled_clean_write_ln(0, MSG_LINE_3, "--- Speed 3 Out ---", false, true);
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

        /* Centering the text on oled requires odd number of characters */
        oled_clean_write_ln(0, MSG_LINE_3, "+++ In Scroll +++", false, true);
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
        oled_clean_write_ln(0, MSG_LINE_3, " --- Out Scroll ---", false, true);
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
        char buf[20];
        snprintf(buf, 20, "++ In Scroll %d ++", stride);
        oled_clean_write_ln(0, MSG_LINE_3, buf, false, true);
        #ifdef CONSOLE_ENABLE
            uprintf("buf is %s\n", buf);
            int a = strlen(buf);
            uprintf("strlen(buf) is %i\n", a);
        #endif
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
        char buf[20];
        snprintf(buf, 20, "++ Out Scroll %d ++", stride);
        oled_clean_write_ln(0, MSG_LINE_3, buf, false, true);
        #ifdef CONSOLE_ENABLE
            uprintf("buf is %s\n", buf);
            int a = strlen(buf);
            uprintf("strlen(buf) is %i\n", a);
        #endif
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
            // msg template     (0, MSG_LINE_0,"xxxxxxxxxxxxxxxxxxxxx", false);
            oled_clean_write_ln(0, MSG_LINE_3, "-- Scrolled To 0 --", false, true);
            } else {
                oled_clean_write_ln(0, MSG_LINE_3, " !! Not Applicable !!", true, true);
            }
    } else {
        if (!position_valid){
            oled_clean_write_ln(0, MSG_LINE_3, " !! Not Applicable !!", false, true);
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
            oled_clean_ln(MSG_LINE_0);
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
                    // msg template    (0, MSG_LINE_0,"xxxxxxxxxxxxxxxxxxxxx", false);
                    oled_clean_write_ln(0, MSG_LINE_3,"~  A Boundary Set  ~", false, true);
                    break;
                case 1:
                    oled_clean_write_ln(0, MSG_LINE_3,"~   Boundary In    ~", false, true);
                    break;
                case -1:
                    oled_clean_write_ln(0, MSG_LINE_3,"~   Boundary Out   ~", false, true);
                    break;
                default:
                    break;
            }
            } else {
                oled_clean_write_ln(0, MSG_LINE_3, " !! Not Applicable !!", true, true);
            }
    } else {
        if (!position_valid){
            oled_clean_write_ln(0, MSG_LINE_3, " !! Not Applicable !!", false, true);
        }
    }
}

void do_exit_standby(bool pressed){
/* oled display a ready message*/
    if (pressed) {
        layer_move(LYR_CYCLE_START);
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
    /* Note, rpt_led_status() is not needed. It is called during oled_task. */
}

void do_led_nxt(bool pressed){
    if (pressed) {
        rgb_matrix_step();
    }
    /* Note, rpt_led_status() is not needed. It is called during oled_task. */
}

void do_led_i_up(bool pressed){
    if (pressed) {
        rgb_matrix_increase_val_noeeprom();
    }
   /* Note, rpt_led_status() is not needed. It is called during oled_task. */
}

void do_led_i_dn(bool pressed){
    if (pressed) {
        rgb_matrix_decrease_val_noeeprom();
    }
    /* Note, rpt_led_status() is not needed. It is called during oled_task. */
}

void do_led_hue_up(bool pressed){
    if (pressed) {
        rgb_matrix_increase_hue_noeeprom();
    }
    /* Note, rpt_led_status() is not needed. It is called during oled_task. */
}

void do_led_hue_dn(bool pressed){
    if (pressed) {
        rgb_matrix_decrease_hue_noeeprom();
    }
    /* Note, rpt_led_status() is not needed. It is called during oled_task. */
}

void do_led_spd_up(bool pressed){
    if (pressed) {
        rgb_matrix_increase_speed();
    }
    /* Note, rpt_led_status() is not needed. It is called during oled_task. */
}

void do_led_spd_dn(bool pressed){
    if (pressed) {
        rgb_matrix_decrease_speed();
    }
    /* Note, rpt_led_status() is not needed. It is called during oled_task. */
}

void wake_up(void){
    idle_timer = timer_read();
    halfmin_counter = 0;
    /* wake_up resets the idle_timer when keys are pressed, but things go
    on in be_not_dimmed that should happen only when coming out of dimmed
    mode.*/
    if (dimmed_mode){
        be_not_dimmed();
        dimmed_mode = false;
    }
}

layer_state_t layer_state_set_user(layer_state_t state) {
    if (dimmed_mode){
        return state;
    }
    /* rgb_matrix_mode_noeeprom(1) sets to animation #1*/
    rgb_matrix_mode_noeeprom(1);
    uint8_t cur_val = rgb_matrix_get_val();
    switch (get_highest_layer(state)) {
        case _LED_SETUP:
            break;
        case _STANDBY:
            /* hue, sat, value */
            rgb_matrix_sethsv_noeeprom(0, 255, cur_val);
            break;
        case _STD:
            /* hue, sat, value */
            /* Some hues require a minimum led power because each led r,g & b has a
            different brightness for the same voltage. Some might not even be lit.
            This line can be used to make sure the brightness is enough for the hue.
            rgb_matrix_sethsv_noeeprom(24, 255, ((16) > (cur_val) ? (16) : (cur_val))); */
            rgb_matrix_sethsv_noeeprom(24, 255, cur_val);
            break;
        case _3SPEEDACL:
            /* hue, sat, value */
            rgb_matrix_sethsv_noeeprom(85, 255, cur_val);
            break;
        case _ALTERNATE2:
            /* hue, sat, value */
            rgb_matrix_sethsv_noeeprom(168, 255, cur_val);
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

    /* Only wake up for any activity when in dimmed mode.*/
    if (dimmed_mode){
        wake_up();
        return true;
    }

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
            /* Exit Standby layer */
            do_exit_standby(record->event.pressed);
            break;

        case CYCLE_RAD_LYRS:
            /* Cycle to next layer in the rad use layers.*/
            do_cycle_rad_lyr(record->event.pressed);
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

    /*For DIY oled timeout purposes.*/
    if (record->event.pressed) {
        wake_up();
    }

    return true;
};
