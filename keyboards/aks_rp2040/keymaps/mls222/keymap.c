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

enum layer_names {
    _PEDAL,
    _ALTERNATE,
    _LED_SETTINGS,
    _STANDBY
};

enum custom_keycodes {
    PEDAL_L = SAFE_RANGE,
    PEDAL_M,
    PEDAL_R
};

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [_PEDAL] = LAYOUT(
                                TO(_ALTERNATE),  // encoder press to _ALTERNATE
        KC_BRID,    KC_BRIU,    KC_NO,  // computer screen brightness
        KC_NO,      KC_NO,      KC_NO,
        KC_NO,      KC_NO,      KC_NO,
        // MS_BTN1,    MS_BTN2,    MS_BTN3  // pedal buttons
        PEDAL_L,    PEDAL_M,    PEDAL_R  // pedal buttons in macros
    ),
    [_ALTERNATE] = LAYOUT(
                                TO(_LED_SETTINGS), // encoder press to _LED_SETTINGS
        KC_BRID,    KC_BRIU,    KC_NO,  // computer screen brightness
        KC_NO,      KC_NO,      KC_NO,
        KC_NO,      KC_NO,      KC_NO,
        // MS_BTN1,    MS_BTN2,    MS_BTN3  // pedal buttons
        PEDAL_L,    PEDAL_M,    PEDAL_R  // pedal buttons in macros
    ),
    [_LED_SETTINGS] = LAYOUT(
                                TO(_STANDBY), // encoder press to _STANDBY
        RM_NEXT, KC_NO,   RM_TOGG, // Next RGB animation, nada, Toggle animations
        KC_NO,   KC_NO,   KC_NO,
        RM_VALU, RM_HUEU, RM_SPDU, // intensity up, color hue up, amim speed up
        RM_VALD, RM_HUED, RM_SPDD  // intensity dn, color hue dn, amim speed dn
    ),
    [_STANDBY] = LAYOUT(
                                TO(_PEDAL), // encoder press to _PEDAL
        KC_NO,      KC_NO,      KC_NO,
        KC_NO,      KC_NO,      KC_NO,
        KC_NO,      KC_NO,      KC_NO,
        KC_NO,      KC_NO,      KC_NO
    ),
};

#ifdef ENCODER_MAP_ENABLE
    /*  This sets what the encoder turns do at each layer.
        There has to be a setting for each defined layer.
        Otherwise there will be a compile error.

        Keep in mind the transparency encoding applies.
    */
    const uint16_t PROGMEM encoder_map[][NUM_ENCODERS][NUM_DIRECTIONS] = {
    [_PEDAL]     = { ENCODER_CCW_CW(KC_WH_D, KC_WH_U) },  // mouse wheel down and up
    [_ALTERNATE] = { ENCODER_CCW_CW(KC_WH_D, KC_WH_U) }, // mouse wheel down and up
    [_LED_SETTINGS] = { ENCODER_CCW_CW(RM_VALD, RM_VALU) }, // LED brightness
    [_STANDBY]  = { ENCODER_CCW_CW(KC_NO,   KC_NO) } // do nothing
    };
#endif


#ifdef OLED_ENABLE

    // trying to render the logo only during startup
    #ifndef SHOW_LOGO
    #    define SHOW_LOGO 1000
    #endif

    // settings for showing logo only at startup
    static uint32_t post_startup = 0;
    static uint32_t oled_logo_timer = 0;

    static const char PROGMEM aks_mls_logo[] = {
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
        // set the oled display rotation
        bool oled_init(oled_rotation_t(OLED_ROTATION_0));
        // send logo data to oled
        oled_write_raw_P(aks_mls_logo, sizeof(aks_mls_logo));
    }

    //Setup some mask which can be or'd with bytes to turn off pixels
    const uint8_t single_bit_masks[8] = {127, 191, 223, 239, 247, 251, 253, 254};

    static void fade_display(void) {
        //Define the reader structure
        oled_buffer_reader_t reader;
        uint8_t buff_char;
        if (random() % 30 == 0) {
            srand(timer_read());
            // Fetch a pointer for the buffer byte at index 0. The return structure
            // will have the pointer and the number of bytes remaining from this
            // index position if we want to perform a sequential read by
            // incrementing the buffer pointer
            reader = oled_read_raw(0);
            //Loop over the remaining buffer and erase pixels as we go
            for (uint16_t i = 0; i < reader.remaining_element_count; i++) {
                //Get the actual byte in the buffer by dereferencing the pointer
                buff_char = *reader.current_element;
                if (buff_char != 0) {
                    oled_write_raw_byte(buff_char & single_bit_masks[rand() % 8], i);
                }
                //increment the pointer to fetch a new byte during the next loop
                reader.current_element++;
            }
        }
    }

    bool oled_task_kb(void) {
        if ((timer_elapsed32(oled_logo_timer) < SHOW_LOGO)){
            render_logo();
        }else{
            fade_display();
            post_startup = 1;
            oled_task_user();
            return true;
        }
        return false;
    }


    bool oled_task_user(void) {

        switch(post_startup){
            case 0:
                oled_set_cursor(0, 3);
                break;
            case 1:
                oled_set_cursor(0, 1);
                break;
            default:
                oled_set_cursor(0, 5);
        }


        switch (get_highest_layer(layer_state)) {
            case _PEDAL:
                oled_write_ln(PSTR("Pedal: Action On"), false);
                oled_write_ln(PSTR("Encoder: Mouse Wheel"), false);
                oled_advance_page(true);

                break;
            case _ALTERNATE:
                oled_write_ln(PSTR("Alt Pedal: Action On"), false);
                oled_write_ln(PSTR("Encoder: Mouse Wheel"), false);
                oled_advance_page(true);

                break;
            case _LED_SETTINGS:
                oled_write_ln(PSTR("LED Settings"), false);
                oled_write_ln(PSTR("Encoder: Brightness"), false);
                oled_write_ln(PSTR("Pedal: Action Off"), false);
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

        return false;
    }

#endif


bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
    case  PEDAL_L:
        // The foot pedal action seems to on the release
        if (record->event.pressed) {
            // when keycode pressed
            register_code(MS_BTN1);
            oled_set_cursor(0, 6);
            oled_write_ln(PSTR("<= Pedal Left"), false);
        } else {
            // when keycode released
            unregister_code(MS_BTN1);
            oled_set_cursor(0, 6);
            oled_write_ln(PSTR("<= Pedal Left"), false);
        }
        break;

    case  PEDAL_M:
            // The foot pedal action seems to on the release
        if (record->event.pressed) {
            // when keycode pressed
            register_code(MS_BTN2);
            oled_set_cursor(2, 6);
            oled_write_ln(PSTR(">> Pedal Middle <<"), false);
        } else {
            // when keycode released
            oled_set_cursor(2, 6);
            unregister_code(MS_BTN2);
            oled_write_ln(PSTR(">> Pedal Middle <<"), false);
        }
        break;

    case  PEDAL_R:
        // The foot pedal action seems to on the release
        if (record->event.pressed) {
            // when keycode pressed
            register_code(MS_BTN3);
            oled_set_cursor(7, 6);
            oled_write_ln(PSTR("Pedal Right =>"), false);
        } else {
            // when keycode released
            oled_set_cursor(7, 6);
            unregister_code(MS_BTN3);
            oled_write_ln(PSTR("Pedal Right =>"), false);
        }
        break;
    }
    return true;
};
