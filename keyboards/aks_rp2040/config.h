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

#pragma once

/* OLED SPI Defines */
#define OLED_DISPLAY_128X64
#define OLED_IC OLED_IC_SH1106

/* OLED SPI Pins */
#define OLED_DC_PIN GP24
#define OLED_CS_PIN GP22
#define OLED_RST_PIN GP23

#define OLED_BRIGHTNESS 128

/* Shift OLED columns by 2 pixels */
#define OLED_COLUMN_OFFSET 2

/* Divisor for OLED */
#define OLED_SPI_DIVISOR 4

/* ChibiOS SPI definitions */
#define SPI_DRIVER SPID1
#define SPI_SCK_PIN GP26
#define SPI_MOSI_PIN GP27
#define SPI_MISO_PIN GP28

/* Double tap the side button to enter bootloader */
#define RP2040_BOOTLOADER_DOUBLE_TAP_RESET
#define RP2040_BOOTLOADER_DOUBLE_TAP_RESET_LED GP13
#define RP2040_BOOTLOADER_DOUBLE_TAP_RESET_TIMEOUT 500U

#define RGB_MATRIX_MAXIMUM_BRIGHTNESS 255

/* Mousekey speed control */
// #define MK_MOMENTARY_ACCEL
#define MK_3_SPEED
#define MK_COMBINED
#define MOUSEKEY_WHEEL_DELAY 10  // Time between wheel movements in default mode.
#define MOUSEKEY_WHEEL_INTERVAL 60  // Time between wheel movements in default mode.
#define MOUSEKEY_WHEEL_TIME_TO_MAX	20 // Time until maximum scroll speed is reached
#define MK_W_INTERVAL_UNMOD 10 // default is 40
#define MK_W_INTERVAL_0 360 // MS_ACL0 , default 360
#define MK_W_INTERVAL_1 120 // MS_ACL1 , default 120
#define MK_W_INTERVAL_2 20 // MS_ACL2 , default 360

/*
Accelerated (default mouse):
These are the default values if not modified like above.
MOUSEKEY_WHEEL_DELAY	10	Delay between pressing a wheel key and wheel movement
MOUSEKEY_WHEEL_INTERVAL	80	Time between wheel movements
MOUSEKEY_WHEEL_DELTA	1	Wheel movement step size (leave this be!)
MOUSEKEY_WHEEL_MAX_SPEED	8	Maximum number of scroll steps per scroll action
MOUSEKEY_WHEEL_TIME_TO_MAX	40	Time until maximum scroll speed is reached
The following does not apply for our use being scroll only.
(cursor) Holding movement keys accelerates the cursor until it reaches its maximum speed.
(cursor) MOUSEKEY_DELAY	10	Delay between pressing a movement key and cursor movement
(cursor) MOUSEKEY_INTERVAL	20	Time between cursor movements in milliseconds
(cursor) MOUSEKEY_MOVE_DELTA	8	Step size
(cursor) MOUSEKEY_MAX_SPEED	10	Maximum cursor speed at which acceleration stops
(cursor) MOUSEKEY_TIME_TO_MAX	30	Time until maximum cursor speed is reached
*/
