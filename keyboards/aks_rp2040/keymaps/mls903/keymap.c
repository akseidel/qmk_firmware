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
                _LED_SETTINGS,
                _STANDBY
                };

/* Every reference name must be first defined in enum before it shows up anywhere in the code. */
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
    STD_WH_U,           // Standard QMK mouse wheel up, no counting
    STD_WH_D,           // Standard QMK mouse wheel dn, no counting
    EXP_WH_U,            // Exponential QMK mouse wheel up, with counting
    EXP_WH_D             // Exponential QMK mouse wheel dn, with counting
};


/* KC_NO means no keycode, ie do nothing */
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    /* One shot strides and also standard out of box mouse wheel action */
    [_BASE]        = LAYOUT(TO(_3SPEEDACL),                       // encoder press goto _3SPEEDACL
                            TURN_0,  KC_NO,   KC_NO,               // counter zero
                            GOTO_0,   KC_NO, STRIDE_1,             // goto postion 0, kc_no, set stride to 1
                            ENC_U, EXP_WH_U, ENC_STRIDE_INC,        // 1 shot u click stride x,reg. wheel,inc stride
                            ENC_D, EXP_WH_D, ENC_STRIDE_DEC         // 1 shot d click stride x,reg. wheel,inc stride
                             ),
    /* QMK accelerated mouse wheel action
    The SPD_1_x, SPD_2_x, SPD_3_x keys set three speed ranges.
    Speed is currently set at compile time. Position counter n/a. */
    [_3SPEEDACL]    = LAYOUT(TO(_ALTERNATE2),                     // encoder press goto _ALTERNATE2
                            TURN_0,  KC_NO,   KC_NO,               // counter zero
                            GOTO_0,  KC_NO,   KC_NO,               // goto postion 0
                            SPD_1_U, SPD_2_U, SPD_3_U,             // up @ spd 1, up @ spd 2, up @ spd 3
                            SPD_1_D, SPD_2_D, SPD_3_D              // dn @ spd 1, dn @ spd 2, dn @ spd 3
                             ),
    /* MLS mouse wheel speed control. The encoder sets stride
    while keys are used for up and down at stride distance. */
    [_ALTERNATE2]    = LAYOUT(TO(_LED_SETTINGS),                   // encoder press goto _LED_SETTINGS
                            TURN_0,   KC_NO, KC_NO,                // counter 0, kc_no, KC_NO
                            GOTO_0,   KC_NO, STRIDE_1,             // goto postion 0, kc_no, set stride to 1
                            MLS_WHLU, MLS_WHLU, MLS_WHLU,          // all keys do up
                            MLS_WHLD, MLS_WHLD, MLS_WHLD           // all keys do dn
                             ),
    /* LED animation setting layer */
    [_LED_SETTINGS] = LAYOUT(TO(_STANDBY),                         // encoder press goto _STANDBY
                             RM_NEXT, KC_NO, RM_TOGG,              // Next RGB animation, kc_no, Animation on/off
                             KC_NO, KC_NO, KC_NO,
                             RM_VALU, RM_HUEU, RM_SPDU,            // intensity up, color hue up, amim speed up
                             RM_VALD, RM_HUED, RM_SPDD             // intensity dn, color hue dn, amim speed dn
                             ),
    [_STANDBY]      = LAYOUT(MSG_READY,                            // encoder press goto _BASE
                             MSG_STBY, MSG_STBY, MSG_STBY,
                             MSG_STBY, MSG_STBY, MSG_STBY,
                             MSG_STBY, MSG_STBY, MSG_STBY,
                             MSG_STBY, MSG_STBY, MSG_STBY
                             )
};


static int16_t en_turns = 0;    /* being used to record number of encoder turns */
static int16_t stride = 1;      /* stride is a distance (number of encoder turns) concept*/
static bool position_valid = true; /* flag indicating the position counter is valid */
static int16_t active_layer = 0;  /* manually keeping track of current layer */


#ifdef ENCODER_MAP_ENABLE
/*  This sets what the encoder turning does at each layer.
    There has to be an enum setting for each defined layer and action.
    Otherwise there will be a compile error.

    Keep in mind the transparency encoding applies.
*/
const uint16_t PROGMEM encoder_map[][NUM_ENCODERS][NUM_DIRECTIONS] = {
    [_BASE]        = {ENCODER_CCW_CW(ENC_U,ENC_D)}, // custom function for encoder turn being up & dn
    [_3SPEEDACL]    = {ENCODER_CCW_CW(ENC_U,ENC_D)}, // custom function for encoder turn being up & dn
    [_ALTERNATE2]    = {ENCODER_CCW_CW(ENC_STRIDE_INC,ENC_STRIDE_DEC)}, // custom function for encoder turn being change strive value
    [_LED_SETTINGS] = {ENCODER_CCW_CW(RM_VALD, RM_VALU)}, // LED brightness
    [_STANDBY]      = {ENCODER_CCW_CW(KC_NO, KC_NO)}      // do nothing
};
#endif

#ifdef OLED_ENABLE

// trying to render the logo only during startup
#    ifndef SHOW_LOGO
#        define SHOW_LOGO 1500
#    endif

// variables used for showing logo only at startup
static bool     logo_is_visible      = true;
static uint32_t oled_logo_timer = 0;

/* constant that is the data for the logo graphic */
static const char PROGMEM mls_logo[] = {
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
    oled_write_raw_P(mls_logo, sizeof(mls_logo));
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
    char buf[17];
    if (position_valid){
        snprintf(buf, 17, "Pos: %d", en_turns);
        oled_clean_write_ln(0, 6, buf);
    } else {
        oled_clean_write_ln(0, 6, "Pos: Now Invalid");
    }
    switch (active_layer) {
        case _3SPEEDACL:
            oled_clean_ln(7);
            break;
        default:
            snprintf(buf, 17, "Stride: %d", stride);
            oled_clean_write_ln(0, 7, buf);
            break;
    }
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
            active_layer = _BASE;
            break;
        case _3SPEEDACL:
            oled_write_ln(PSTR("Keys: Acl Mse Wheel"), false);
            oled_write_ln(PSTR("3 Wheel Speed Modes"), false);
            oled_write_ln(PSTR("Encoder: Mse Wheel"), false);
            oled_write_ln(PSTR("Press Encoder: Next"), false);
            active_layer = _3SPEEDACL;
            break;
        case _ALTERNATE2:
            oled_write_ln(PSTR("Keys: MLS Mse Wheel"), false);
            oled_write_ln(PSTR("Encoder: Stride"), false);
            oled_write_ln(PSTR("Press Encoder: Next"), false);
            oled_advance_page(true);
            active_layer = _ALTERNATE2;
            break;
        case _LED_SETTINGS:
            oled_write_ln(PSTR("Mode: LED Settings"), false);
            oled_write_ln(PSTR("Encoder: Brightness"), false);
            oled_write_ln(PSTR("Press Encoder: Next"), false);
            oled_advance_page(true);
            oled_advance_page(true);
            active_layer = _LED_SETTINGS;
            break;
        case _STANDBY:
            oled_write_ln(PSTR("Mode: Standby"), false);
            oled_write_ln(PSTR("Keys: Nothing"), false);
            oled_write_ln(PSTR("Press Encoder: Next"), false);
            oled_advance_page(true);
            active_layer = _STANDBY;
            break;
        default:
            oled_write_ln_P(PSTR("Undefined"), false);
            oled_advance_page(true);
            oled_advance_page(true);
            active_layer = _BASE;
    }
    return true;
}

#endif

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

void countable_wh_u(bool pressed){
    static deferred_token rep_token = INVALID_DEFERRED_TOKEN;
    if (!pressed) { // key released: stop repeating.
    cancel_deferred_exec(rep_token);
    rep_token = INVALID_DEFERRED_TOKEN;
  } else if (!rep_token) { // key pressed: start repeating.
    uint32_t thiskey_rep_callback(uint32_t trigger_time, void* cb_arg) {
        tap_code(KC_WH_U);
        en_turns++;
        if (repeat_cnt < sizeof(REP_DELAY_MS) - 1) {
            ++repeat_cnt;
        }
        return pgm_read_byte(REP_DELAY_MS + repeat_cnt);
    }
    tap_code(KC_WH_U);  // Initial tap of the key.
    oled_clean_ln(4);
    oled_clean_write_ln(4, 5, "+ Hyper In +");
    en_turns++;
    // Schedule key to repeat.
    rep_token = defer_exec(REP_DELAY_MS[0], thiskey_rep_callback, NULL);
    repeat_cnt = 0;
  }
}

void countable_wh_d(bool pressed){
    static deferred_token rep_token = INVALID_DEFERRED_TOKEN;
    if (!pressed) { // key released: stop repeating.
    cancel_deferred_exec(rep_token);
    rep_token = INVALID_DEFERRED_TOKEN;
  } else if (!rep_token) { // key pressed: start repeating.
    uint32_t thiskey_rep_callback(uint32_t trigger_time, void* cb_arg) {
        tap_code(KC_WH_D);
        en_turns--;
        if (repeat_cnt < sizeof(REP_DELAY_MS) - 1) {
            ++repeat_cnt;
        }
        return pgm_read_byte(REP_DELAY_MS + repeat_cnt);
    }
    tap_code(KC_WH_D);  // Initial tap of the key.
    oled_clean_ln(4);
    oled_clean_write_ln(4, 5, "- Hyper Out -");
    en_turns--;
    // Schedule key to repeat.
    rep_token = defer_exec(REP_DELAY_MS[0], thiskey_rep_callback, NULL);
    repeat_cnt = 0;
  }
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
        case STD_WH_U:
        /* Standard QMK mouse wheel up. As of this time, we cannot keep
        count of the number sent, so the en_turns is not updated.*/
            if (record->event.pressed) {
                // when pressed
                register_code(KC_WH_U);
                position_valid = false;
                oled_clean_ln(4);
                oled_clean_write_ln(4, 5, "+ Scroll In +");
            } else {
                // when released
                unregister_code(KC_WH_U);
                oled_clean_ln(5);
            }
            break;

        case STD_WH_D:
        /* Standard QMK mouse wheel dn. As of this time, we cannot keep
        count of the number sent, so the en_turns is not updated.*/
            if (record->event.pressed) {
                // when pressed
                register_code(KC_WH_D);
                oled_clean_ln(4);
                oled_clean_write_ln(3, 5, "- Scroll Out -");
                position_valid = false;
            } else {
                // when released
                unregister_code(KC_WH_D);
                oled_clean_ln(5);
            }
            break;

        case EXP_WH_U:
            /* Hyper standard QMK mouse wheel up, keeping
            count of the number sent, so the en_turns is not updated.*/
            countable_wh_u(record->event.pressed);
            break;

        case EXP_WH_D:
            /* Hyper standard QMK mouse wheel up, keeping
            count of the number sent, so the en_turns is not updated.*/
            countable_wh_d(record->event.pressed);
            break;

        case ENC_STRIDE_INC:
        /* Encoder turn increases the stride value by 1 */
            if (record->event.pressed) {
                stride++;
                oled_clean_write_ln(1, 5, "|+|  Stride Chg  |+|");
            } else {
            }
            break;

        case ENC_STRIDE_DEC:
        /* Encoder turn decreases the stride value by 1 */
            if (record->event.pressed) {
                // when pressed
                if (stride > 1) {
                    stride--;
                    oled_clean_write_ln(1, 5, "|-|  Stride Chg  |-|");
                }
            } else {
                // when released
            }
            break;
        case TURN_0:
        /* Resets the encoder turns counter to 0 */
            if (record->event.pressed) {
                // when pressed
                en_turns = 0;
                position_valid = true;
                oled_clean_write_ln(3, 4, "000  Zero Pos 000");
                oled_clean_ln(5);
            } else {
                // when released
            }
            break;
        case STRIDE_1:
        /* Resets the stride value to 1*/
            if (record->event.pressed) {
                // when pressed
                stride = 1;
                oled_clean_write_ln(1, 5, "|1| Stride Reset |1|");
            } else {
                // when released
            }
            break;
        case MSG_STBY:
        /* oled display the standby message, used when a key is pressed
        while in standby mode to remind user they are in standby mode.
        Otherwise they might wonder why nothing happens. */
            if (record->event.pressed) {
                // when pressed
                oled_clean_write_ln(1, 4, "!@# Standby Mode !@#");
                oled_clean_ln(5);
            } else {
                // when released
            }
            break;
        case SPD_1_U:
        /* After first setting accelerated (actually constant) mouse wheel event speed to
        speed 1, send continuous mouse wheel up events. As of this time, we cannot keep
        count of the number sent, so the en_turns is not updated.*/
            if (record->event.pressed) {
                // when pressed
                tap_code(MS_ACL0);
                register_code(KC_WH_U);
                oled_clean_write_ln(2, 5, "+ Speed 1 In +");
                position_valid = false;
            } else {
                // when released
                unregister_code(KC_WH_U);
                oled_clean_ln(5);
            }
            break;
        case SPD_1_D:
        /* After first setting accelerated (actually constant) mouse wheel event speed to
        speed 1, send continuous mouse wheel dn events. As of this time, we cannot keep
        count of the number sent, so the en_turns is not updated.*/
            if (record->event.pressed) {
                // when pressed
                tap_code(MS_ACL0);
                register_code(KC_WH_D);
                oled_clean_write_ln(3, 5, "- Speed 1 Out -");
                position_valid = false;
            } else {
                // when released
                unregister_code(KC_WH_D);
                oled_clean_ln(5);
            }
            break;
        case SPD_2_U:
        /* After first setting accelerated (actually constant) mouse wheel event speed to
        speed 2, send continuous mouse wheel up events. As of this time, we cannot keep
        count of the number sent, so the en_turns is not updated.*/
            if (record->event.pressed) {
                // when pressed
                tap_code(MS_ACL1);
                register_code(KC_WH_U);
                oled_clean_write_ln(3, 5, "++ Speed 2 In ++");
                position_valid = false;
            } else {
                // when released
                unregister_code(KC_WH_U);
                oled_clean_ln(5);
            }
            break;
        case SPD_2_D:
        /* After first setting accelerated (actually constant) mouse wheel event speed to
        speed 2, send continuous mouse wheel dn events. As of this time, we cannot keep
        count of the number sent, so the en_turns is not updated.*/
            if (record->event.pressed) {
                // when pressed
                tap_code(MS_ACL1);
                register_code(KC_WH_D);
                oled_clean_write_ln(2, 5, "-- Speed 2 Out --");
                position_valid = false;
            } else {
                // when released
                unregister_code(KC_WH_D);
                oled_clean_ln(5);
            }
            break;
        case SPD_3_U:
        /* After first setting accelerated (actually constant) mouse wheel event speed to
        speed 3, send continuous mouse wheel up events. As of this time, we cannot keep
        count of the number sent, so the en_turns is not updated.*/
            if (record->event.pressed) {
                // when pressed
                tap_code(MS_ACL2);
                register_code(KC_WH_U);
                oled_clean_write_ln(1, 5, "+++ Speed 3 In +++");
                position_valid = false;
            } else {
                // when released
                unregister_code(KC_WH_U);
                oled_clean_ln(5);
            }
            break;
        case SPD_3_D:
        /* After first setting accelerated (actually constant) mouse wheel event speed to
        speed 3, send continuous mouse wheel dn events. As of this time, we cannot keep
        count of the number sent, so the en_turns is not updated.*/
            if (record->event.pressed) {
                // when pressed
                tap_code(MS_ACL2);
                register_code(KC_WH_D);
                oled_clean_write_ln(1, 5, "--- Speed 3 Out ---");
                position_valid = false;
            } else {
                // when released
                unregister_code(KC_WH_D);
                oled_clean_ln(5);
            }
            break;
        case ENC_U:
        /* Send single mouse wheel up, stride times.
        Used when encoder is turned (ccw).
        The en_turns value is updated for each wheel up sent.
        Note: an encoder event comes as a press/release pair */
            if (record->event.pressed) {
                // when pressed
                for (int i = 0; i < stride ; i++) {
                        tap_code(KC_WH_U);
                        en_turns++;
                    }
                oled_clean_ln(4);
                oled_clean_write_ln(2, 5, "+++ In Scroll +++");
            } else {
                // when released
            }
            break;
        case ENC_D:
        /* Send single mouse wheel dn, stride times.
        Used when encoder is turned (cw).
        The en_turns value is updated for each wheel dn sent.
        Note: an encoder event comes as a press/release pair */
            if (record->event.pressed) {
                // when pressed
                for (int i = 0; i < stride ; i++) {
                        tap_code(KC_WH_D);
                        en_turns--;
                    }
                oled_clean_ln(4);
                oled_clean_write_ln(2, 5, "--- Out Scroll ---");
            } else {
                // when released
            }
            break;
        case MLS_WHLU:
        /* currently identical to ENC_U
        Send single mouse wheel up, stride times.
        Used for keypresses in MSL mode.
        The en_turns value is updated for each wheel up sent.*/
            if (record->event.pressed) {
                // when pressed
                for (int i = 0; i < stride ; i++) {
                    // register_code(KC_WH_U); // NO, does not help!
                    tap_code(KC_WH_U);
                    en_turns++;
                }
                oled_clean_ln(4);
                oled_clean_write_ln(0, 5, "+++ MLS In Scroll +++");
            } else {
                // when released
                // unregister_code(KC_WH_U);
            }
            break;
        case MLS_WHLD:
        /* currently identical to ENC_D
        Send single mouse wheel dn, stride times.
        Used for keypresses in MSL mode.
        The en_turns value is updated for each wheel dn sent.
        Note: an encoder event comes as a press/release pair */
            if (record->event.pressed) {
                // when pressed
                for (int i = 0; i < stride ; i++) {
                    // register_code(KC_WH_D); // NO, does not help!
                    tap_code(KC_WH_D);
                    en_turns--;
                }
                oled_clean_ln(4);
                oled_clean_write_ln(0, 5, "--- MLS Out Scroll ---");
            } else {
                // when released
                // unregister_code(KC_WH_D);
            }
            break;
        case GOTO_0:
        /* Sends mouse wheel up or mouse wheel dn commands the number of times
        required to return to 0 position.*/
            if (record->event.pressed) {
                // when pressed
                if (position_valid){
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
                oled_clean_write_ln(1, 5, "=> Scrolled To 0 <=");
            } else {
                oled_clean_ln(4);
                oled_clean_write_ln(1, 5, "!! Not Applicable !!");
            }
            } else {
                // when released
            }
            break;
        case MSG_READY:
        /* oled display a ready message*/
            if (record->event.pressed) {
                // when pressed
                oled_clean_ln(4);
                oled_clean_write_ln(3, 5, "---  Ready  ---");
            } else {
                layer_move(0);
            }
            return false;
            break;

        case LAY_F:
        /* not implemented, intended to be key press jumps to next layer*/
            if (record->event.pressed) {
                // when pressed

            } else {
                // layer_move(0);
            }
            return false;
            break;

        case LAY_B:
        /* not implemented, intended to be key press jumps to previous layer*/
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
    report_position_etc();  // update oled display for position etc.
    return true;
};
