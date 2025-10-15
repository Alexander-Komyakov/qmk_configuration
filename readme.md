## How to configure and flash keyboard

```
convert .c to keymap.json

qmk c2json -kb handwired/tractyl_manuform/5x6_right/arduinomicro -km default keyboards/handwired/tractyl_manuform/5x6_right/arduinomicro/keymaps/keymap.c

compile and flash to aruino pro micro

qmk flash -kb handwired/tractyl_manuform/5x6_right/arduinomicro -km default

Double click red key on keyboard to flash

```
