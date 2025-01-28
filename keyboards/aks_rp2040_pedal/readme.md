# aks_rp2040_pedal

![aks_rp2040_pedal](imgur.com image replace me!)

*A short description of the keyboard/project*

* Keyboard Maintainer: [Allan Seidel](https://github.com/Allan Seidel)
* Hardware Supported: *The PCBs, controllers supported*
* Hardware Availability: *Links to where you can find this hardware*

Make example for this keyboard (after setting up your build environment):

    qmk flash -kb aks_rp2040_pedal -km mlspedal1

    qmk compile -kb aks_rp2040_pedal -km mlspedal1

See the [build environment setup](https://docs.qmk.fm/#/getting_started_build_tools) and the [make instructions](https://docs.qmk.fm/#/getting_started_make_guide) for more information. Brand new to QMK? Start with our [Complete Newbs Guide](https://docs.qmk.fm/#/newbs).

## Bootloader

Enter the bootloader in 3 ways:

* **Bootmagic reset**: Hold down the key at (0,0) in the matrix (usually the top left key or Escape) and plug in the keyboard
* **Physical reset button**: Briefly press the button on the back of the PCB - some may have pads you must short instead
* **Keycode in layout**: Press the key mapped to `QK_BOOT` if it is available
