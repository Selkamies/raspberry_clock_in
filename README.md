## Cloking in device with Raspberry Pi 4 using C.

A clocking in device, where user can enter a PIN code using a keypad, and codes are validated from a database. Upon valid PIN, a log entry is saved to a database. 

### Features

- RGB LED displaying colors depending on user input.
- Sound output depending user input.
- [Configuration file](config/config.ini) with settings for:
  - Keypad size and the characters on the keypad keys.
  - All GPIO pin numbers.
  - PIN lengths, timeout times and update intervals.
  - Default audio device or manual device id.

TODO: Picture of the setup.

### External libraries used.
- [pigpio](https://abyz.me.uk/rpi/pigpio/), for GPIO pin handling. [GitHub link](https://github.com/joan2937/pigpio). License: [Public domain](https://github.com/joan2937/pigpio/blob/master/UNLICENCE).
- [SQLite](https://www.sqlite.org/index.html) database. License: [Public domain](https://www.sqlite.org/copyright.html).
- [Simple DirecMedia Layer, SDL 2](https://www.libsdl.org/). [GitHub link](https://github.com/libsdl-org/SDL). License: [zlib license](https://www.libsdl.org/license.php).
- [SDL_Mixer](https://github.com/libsdl-org/SDL_mixer) for sound output. License: [zlib license](https://github.com/libsdl-org/SDL_mixer/blob/main/LICENSE.txt).

### Credits
- [Selkamies](https://github.com/Selkamies)
- [jaakho](https://github.com/jaakho)

### TODO: License.