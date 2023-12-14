## Cloking-in device with Raspberry Pi 4 using C.

A clock-in device, where user can enter a PIN code using a keypad, and codes are validated from a database. Upon valid PIN, a log entry is saved to a database. 

Features:
<ul>
  <li>RGB LED displaying colors depending on user input.</li>
  <li>Sound output depending user input.</li>
  <li>[Configuration file](config.ini) with settings for</li>
    <ul>
      <li>Keypad size and the characters on the buttons.</li>
      <li>All GPIO pin numbers.</li>
      <li>PIN lengths, timeout times and update intervals.</li>
      <li>Default audio device or manual device id.</li>
    </ul>
</ul>

TODO: Picture of the setup.

### External libraries used.
[pigpio](https://abyz.me.uk/rpi/pigpio/), for GPIO pin handling. [GitHub link.](https://github.com/joan2937/pigpio) [Public domain.](https://github.com/joan2937/pigpio/blob/master/UNLICENCE)
[SQLite](https://www.sqlite.org/index.html) database. [Public domain.](https://www.sqlite.org/copyright.html)
[Simple DirecMedia Layer](https://www.libsdl.org/), [SDL2.](https://github.com/libsdl-org/SDL) [zlib license.](https://www.libsdl.org/license.php) And
[SDL_Mixer](https://github.com/libsdl-org/SDL_mixer) for sound output. [zlib license.](https://github.com/libsdl-org/SDL_mixer/blob/main/LICENSE.txt)

### Credits
[Selkamies](https://github.com/Selkamies)
[jaakho](https://github.com/jaakho)

### TODO: License.