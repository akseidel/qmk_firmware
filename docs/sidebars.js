/**
 * Creating a sidebar enables you to:
 - create an ordered group of docs
 - render a sidebar for each doc of that group
 - provide next/previous navigation

 The sidebars can be generated from the filesystem, or explicitly defined here.

 Create as many sidebars as you want.
 */

// @ts-check

/** @type {import('@docusaurus/plugin-content-docs').SidebarsConfig} */
const sidebars = {
    // By default, Docusaurus generates a sidebar from the docs folder structure
    summary: [
        {
            type: "doc",
            id: "README",
            label: "About QMK",
        },
        {
            type: "category",
            label: "Tutorial",
            link: {
                type: "doc",
                id: "newbs",
            },
            items: [
                // "newbs",
                "newbs_getting_started",
                "newbs_building_firmware",
                "newbs_flashing",
                "support",
                "newbs_building_firmware_workflow",
                "newbs_learn_more_resources",
                "syllabus",
            ],
        },
        {
            type: "category",
            label: "FAQs",
            link: {
                type: "doc",
                id: "faq_general",
            },
            items: [
                // "faq_general",
                "faq_build",
                "faq_misc",
                "faq_debug",
                "faq_keymap",
                "squeezing_avr",
                "reference_glossary",
            ],
        },
        {
            type: "category",
            label: "Configurator",
            link: {
                type: "doc",
                id: "newbs_building_firmware_configurator",
            },
            items: [
                // "newbs_building_firmware_configurator",
                "configurator_step_by_step",
                "configurator_troubleshooting",
                "configurator_architecture",
                {
                    type: "category",
                    label: "QMK API",
                    link: {
                        type: "doc",
                        id: "api_overview",
                    },
                    items: [
                        // "api_overview",
                        "api_docs",
                        "reference_configurator_support",
                        "configurator_default_keymaps",
                    ],
                },
            ],
        },
        {
            type: "category",
            label: "CLI",
            link: {
                type: "doc",
                id: "cli",
            },
            items: [
                // "cli",
                "cli_configuration",
                "cli_commands",
                "cli_tab_complete",
            ],
        },
        {
            type: "category",
            label: "Using QMK",
            items: [
                {
                    Guides: [
                        "custom_quantum_functions",
                        "driver_installation_zadig",
                        "keymap",
                        {
                            "Development Environments": [
                                "getting_started_docker",
                                "getting_started_vagrant",
                            ],
                        },
                        {
                            Flashing: ["flashing", "flashing_bootloadhid"],
                            IDEs: ["other_eclipse", "other_vscode"],
                            "Git Best Practices": [
                                "newbs_git_best_practices",
                                "newbs_git_using_your_master_branch",
                                "newbs_git_resolving_merge_conflicts",
                                "newbs_git_resynchronize_a_branch",
                            ],
                        },
                    ],
                },
                {
                    "Simple Keycodes": [
                        "keycodes",
                        "keycodes_basic",
                        "reference_keymap_extras",
                        "feature_advanced_keycodes",
                        "quantum_keycodes",
                        "keycodes_magic",
                    ],
                },
                {
                    "Advanced Keycodes": [
                        "feature_command",
                        "feature_dynamic_macros",
                        "feature_grave_esc",
                        "feature_leader_key",
                        "mod_tap",
                        "feature_macros",
                        "feature_mouse_keys",
                        "feature_programmable_button",
                        "feature_space_cadet",
                        "keycodes_us_ansi_shifted",
                    ],
                },
                {
                    "Software Features": [
                        "feature_auto_shift",
                        "feature_autocorrect",
                        "feature_caps_word",
                        "feature_combo",
                        "feature_debounce_type",
                        "feature_eeprom",
                        "feature_key_lock",
                        "feature_key_overrides",
                        "feature_layers",
                        "one_shot_keys",
                        "feature_os_detection",
                        "feature_rawhid",
                        "feature_secure",
                        "feature_send_string",
                        "feature_sequencer",
                        "feature_swap_hands",
                        "feature_tap_dance",
                        "tap_hold",
                        "feature_tri_layer",
                        "feature_unicode",
                        "feature_userspace",
                        "feature_wpm",
                    ],
                },
                {
                    "Hardware Features": [
                        {
                            Displays: [
                                "quantum_painter",
                                "quantum_painter_lvgl",
                                "feature_hd44780",
                                "feature_st7565",
                                "feature_oled_driver",
                            ],
                            Lighting: [
                                "feature_backlight",
                                "feature_led_matrix",
                                "feature_rgblight",
                                "feature_rgb_matrix",
                            ],
                        },
                        "feature_audio",
                        "feature_bluetooth",
                        "feature_bootmagic",
                        "feature_converters",
                        "custom_matrix",
                        "feature_digitizer",
                        "feature_dip_switch",
                        "feature_encoders",
                        "feature_haptic_feedback",
                        "feature_joystick",
                        "feature_led_indicators",
                        "feature_midi",
                        "feature_pointing_device",
                        "feature_ps2_mouse",
                        "feature_split_keyboard",
                        "feature_stenography",
                        "feature_velocikey",
                    ],
                },
                {
                    "Keyboard Building": [
                        "easy_maker",
                        "porting_your_keyboard_to_qmk",
                        "hand_wire",
                        "isp_flashing_guide",
                    ],
                },
            ],
        },
    ],
};

module.exports = sidebars;