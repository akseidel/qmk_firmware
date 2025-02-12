/* This is the original QMK USB descriptor for PROGRAMMABLE_BUTTON */

#ifdef PROGRAMMABLE_BUTTON_ENABLE
    HID_RI_USAGE_PAGE(8, 0x0C),            // Consumer
    HID_RI_USAGE(8, 0x01),                 // Consumer Control
    HID_RI_COLLECTION(8, 0x01),            // Application
        HID_RI_REPORT_ID(8, REPORT_ID_PROGRAMMABLE_BUTTON),
        HID_RI_USAGE(8, 0x03),             // Programmable Buttons
        HID_RI_COLLECTION(8, 0x04),        // Named Array
            HID_RI_USAGE_PAGE(8, 0x09),    // Button
            HID_RI_USAGE_MINIMUM(8, 0x01), // Button 1
            HID_RI_USAGE_MAXIMUM(8, 0x20), // Button 32
            HID_RI_LOGICAL_MINIMUM(8, 0x00),
            HID_RI_LOGICAL_MAXIMUM(8, 0x01),
            HID_RI_REPORT_COUNT(8, 32),
            HID_RI_REPORT_SIZE(8, 1),
            HID_RI_INPUT(8, HID_IOF_DATA | HID_IOF_VARIABLE | HID_IOF_ABSOLUTE),
        HID_RI_END_COLLECTION(0),
    HID_RI_END_COLLECTION(0),
#endif



#ifdef CUSTOM_USB_DESCRIPTION_ENABLE
    HID_RI_USAGE_PAGE(8, 0x0C),            // Consumer
    HID_RI_USAGE(8, 0x03),                 // Programmable Buttons
    HID_RI_COLLECTION(8, 0x01),            // Application
        HID_RI_REPORT_ID(8, REPORT_ID_PROGRAMMABLE_BUTTON),
        HID_RI_USAGE(8, 0x03),             // Programmable Buttons
        HID_RI_COLLECTION(8, 0x04),        // Named Array
            HID_RI_USAGE_PAGE(8, 0x09),    // Button
            HID_RI_USAGE_MINIMUM(8, 0x01), // Button 1
            HID_RI_USAGE_MAXIMUM(8, 0x20), // Button 32
            HID_RI_LOGICAL_MINIMUM(8, 0x00),
            HID_RI_LOGICAL_MAXIMUM(8, 0x01),
            HID_RI_REPORT_COUNT(8, 32),
            HID_RI_REPORT_SIZE(8, 1),
            HID_RI_INPUT(8, HID_IOF_CONSTANT | HID_IOF_VARIABLE | HID_IOF_ABSOLUTE),
        HID_RI_END_COLLECTION(0),
        HID_RI_COLLECTION(8, 0x02),        // Logical
            HID_RI_USAGE_PAGE(8, 0x08),    // LED
            HID_RI_USAGE(8, 0x4b),         // On/Off Control
            HID_RI_REPORT_SIZE(8, 8),
            HID_RI_REPORT_COUNT(8, 35),
            HID_RI_OUTPUT(8, HID_IOF_VARIABLE),
        HID_RI_END_COLLECTION(0),
    HID_RI_END_COLLECTION(0),
#endif



#ifdef GENERIC_INDICATOR_LED_ENABLE
    HID_RI_USAGE_PAGE(8, 0x0C),            // Consumer
    HID_RI_USAGE(8, 0x01),                 // Consumer Control
    HID_RI_COLLECTION(8, 0x01),            // Application
        HID_RI_USAGE(8, 0x03),             // Programmable Buttons
        HID_RI_COLLECTION(8, 0x02),        // Logical
            HID_RI_USAGE_PAGE(8, 0x08),    // LED
            HID_RI_USAGE(8, 0x4b),         // On/Off Control
            HID_RI_REPORT_COUNT(8, 35),
            HID_RI_REPORT_SIZE(8, 1),
            HID_RI_OUTPUT(8, HID_IOF_VARIABLE),
        HID_RI_END_COLLECTION(0),
    HID_RI_END_COLLECTION(0),
#endif
