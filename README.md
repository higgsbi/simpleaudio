# Simple Audio

Simple audio is minimal audio manipulation program  
Functions include incrementing, decrementing, muting input, and muting output.  

Simple audio relies on PulseAudio as a backend and LibNotify for on an screen display.  

No configuration is provided by design, but values can easily be changed in `src/main.c`  
  
To compile, use `./build.sh --release`  
The executable file will be `out/simpleaudio` 

## Dependencies

- [CTK (automatically included)](https://github.com/higgsbi/ctk)
- [CMake (> 3.0)](https://cmake.org/)
- [PulseAudio](https://www.freedesktop.org/wiki/Software/PulseAudio/)
- [LibNotify](https://github.com/GNOME/libnotify)
- Any notification daemon

## Arguments

<pre>
--mute       toggles output mute status  
--mute-mic   toggles input mute status  
--higher     increments current output volume by 5  
--lower      decrements current output volume by 5  
</pre>


