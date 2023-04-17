# CHIP-8

A CHIP-8 emulator. Structured such that the frontend (as in, the input and display portions of the program) are separate from the emulation backend, so the CHIP-8 emulator could be used as a library not tied to any specific framework.

## Controls (keys and the corresponding CHIP-8 button):

| Keyboard   | CHIP-8  |
|:----------:|:-------:|
| 1 2 3 4    | 1 2 3 C |
| Q W E R    | 4 5 6 D |
| A S D F    | 7 8 9 E |
| Z X C V    | A 0 B F |

## Tested on the following test ROMs:

* [Sergey Naydenov's](https://github.com/metteo/chip8-test-rom)
* [Timendus's](https://github.com/timendus/chip8-test-suite)
* [Corax89's](https://github.com/corax89/chip8-test-rom)
* [Delay timer and random number tests](https://github.com/mattmikolay/chip-8)

## TODO:

emulator back-end:
* more fully implement Super CHIP-8 and XO-CHIP instructions
* documentation
* comments?

frontend:
* put debug console in-window (`~` to bring it up or something) (not really as needed with the new debugger in app)
* add an in-app memory viewer (probably would make a new window)
* actually add audio
* quick save / quick load
* configurable keys
* rapid-fire keys (by holding shift or something)
* pause/fast-forward/slow-motion/step forward keys
* rewind? and/or step backward? (also a back-end issue)
* screenshot? and/or record video?
* change emulator within the app (instead of having to recompile)
