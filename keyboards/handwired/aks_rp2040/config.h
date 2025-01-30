// Copyright 2023 Allan Seidel (@Allan Seidel)
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

/*
 * Feature disable options
 *  These options are also useful to firmware size reduction.
 */

/* disable debug print */
//#define NO_DEBUG

/* disable print */
//#define NO_PRINT

/* disable action features */
//#define NO_ACTION_LAYER
//#define NO_ACTION_TAPPING
//#define NO_ACTION_ONESHOT

/* OLED SPI Defines */
#define OLED_DISPLAY_128X64
#define OLED_IC OLED_IC_SH1106

/* OLED SPI Pins */
#define OLED_DC_PIN GP11
#define OLED_CS_PIN GP12
#define OLED_RST_PIN GP13

#define OLED_BRIGHTNESS 128

/* Shift OLED columns by 2 pixels */
#define OLED_COLUMN_OFFSET 2

/* Divisor for OLED */
#define OLED_SPI_DIVISOR 4

/* ChibiOS SPI definitions */
#define SPI_DRIVER SPID1
#define SPI_SCK_PIN GP26
#define SPI_MOSI_PIN GP27

/* Double tap the pico reset button to enter bootloader. */
/* Pico reset button is the RUN pin (side hole #30). */
#define RP2040_BOOTLOADER_DOUBLE_TAP_RESET
#define RP2040_BOOTLOADER_DOUBLE_TAP_RESET_LED GP25
#define RP2040_BOOTLOADER_DOUBLE_TAP_RESET_TIMEOUT 500U

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
#define MK_W_INTERVAL_2 20 // MS_ACL2 , default 20

/*
Accelerated (default mouse):
These are the default values if not modified like above.
MOUSEKEY_WHEEL_DELAY	10	Delay between pressing a wheel key and wheel movement
MOUSEKEY_WHEEL_INTERVAL	80	Time between wheel movements
MOUSEKEY_WHEEL_DELTA	1	Wheel movement step size (leave this be!)
MOUSEKEY_WHEEL_MAX_SPEED	8	Maximum number of scroll steps per scroll action
MOUSEKEY_WHEEL_TIME_TO_MAX	40	Time until maximum scroll speed is reached

MK_W_INTERVAL_UNMOD	40	Time between scroll steps (unmodified)
MK_W_OFFSET_0	1	Scroll steps per scroll action (MS_ACL0)
MK_W_INTERVAL_0	360	Time between scroll steps (MS_ACL0)
MK_W_OFFSET_1	1	Scroll steps per scroll action (MS_ACL1)
MK_W_INTERVAL_1	120	Time between scroll steps (MS_ACL1)
MK_W_OFFSET_2	1	Scroll steps per scroll action (MS_ACL2)
MK_W_INTERVAL_2	20	Time between scroll steps (MS_ACL2)

The following does not apply for our use being scroll only.
(cursor) Holding movement keys accelerates the cursor until it reaches its maximum speed.
(cursor) MOUSEKEY_DELAY	10	Delay between pressing a movement key and cursor movement
(cursor) MOUSEKEY_INTERVAL	20	Time between cursor movements in milliseconds
(cursor) MOUSEKEY_MOVE_DELTA	8	Step size
(cursor) MOUSEKEY_MAX_SPEED	10	Maximum cursor speed at which acceleration stops
(cursor) MOUSEKEY_TIME_TO_MAX	30	Time until maximum cursor speed is reached
*/
