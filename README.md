# chip8

## diaries
I've logged my progress in diaries for each day I work on this app. In each entry, I talk about the problems I ran into and the
new things I learned while working on this emulator (e.g. emulation in general, sprites, even number distributions in c++, etc.)
- https://github.com/zachtango/chip8/tree/main/diaries


## usage
I set up a compile script and have included the necessary libraries already in the repo. The SDL2.dll file is for Windows 32-bit only.
dll files for other OS can be found in https://www.libsdl.org/.

example usage: `./chip8.exe roms/tetris.rom`

![Example](/demo.gif?raw=true)

## chip8 components:
- 16 8 bit registers
- 4096 bytes of memory
- 16 bit index register
- 16 bit program counter
- 16 level stack
- 8 bit stack pointer
- 8 bit delay timer
- 8 bit sound timer
- 16 input keys
- 64 x 32 monochrome display memory

## references
- https://austinmorlan.com/posts/chip8_emulator/
- http://devernay.free.fr/hacks/chip8/C8TECH10.HTM#00E0
- https://stackoverflow.com/questions/37840270/rand-not-generating-random-numbers-even-after-srandtimenull
- https://en.cppreference.com/w/cpp/numeric/random/uniform_int_distribution
- https://stackoverflow.com/questions/68174660/function-pointer-expression-preceding-parentheses-of-apparent-call-must-have
- https://stackoverflow.com/questions/19555121/how-to-get-current-timestamp-in-milliseconds-since-1970-just-the-way-java-gets
- https://www.cprogramming.com/tutorial/function-pointers.html
- https://stackoverflow.com/questions/24370732/what-does-natural-size-really-mean-in-c
- https://www.techopedia.com/definition/5498/clock-cycle
