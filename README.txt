### SMT ###

## Overview ##
SMT or Space Maelstrom Transcendence, is a simple sh'mup game developed in C++ and SDL, as a 1st year programming project.
The objective is to simply stay alive and earn as many points as possible, upgrading your ship along the way.
Will eventually be open-sourced fully and licensed under the MIT License.

The name "SMT" is derived from how the code started out, as a practice "Simple Motion Test" to learn the SDL library and its extensions.
It may not yet be an enjoyable or finished game, but needless to say, SMT has gone beyond its original name.

Code, images, sounds and bgm are all (C) James Wright <hokazenoflames@gmail.com>
For ownership and license details of other assets such as libraries, see "LICENSE.txt"

## Running SMT ##
1. Download the source. There will eventually be a source-less, just executables + resources download made available.
2. If your platform has a binary for it already compiled, copy it from that folder (e.g. "win32" for windows machines) to the root directory.
3. Run that executable.
(4. If there is no working executable for your system, see "Compiling SMT" below)

## Compiling SMT ##
1. Download the source. Bitbucket provides a .zip if you don't have git, other wise use "git clone"
2. Install the dependencies for your system:
    - SDL 1.2
    - SDL_image 1.2
    - SDL_mixer 1.2
    - SDL_ttf 2.0
    - some sort of C++ compiler (e.g. g++, clang, mingw32-g++)
    - [optional] either an IDE or a decent command line shell
3. Ensure that the libs and dev environment are setup properly. Covering every possible environment is beyond the scope of this file,
   but pointers are to ensure you've included the SDL libs in your IDE or that on unix-like systems, they're installed within your
   library path.
4. Compile main.cpp. If invoking your compiler manually, include the relevant flags to add the SDL libs to the linker (g++ needs
   "-lSDL -lSDL_image -lSDL_mixer -lSDL_ttf")
5. This generates a binary for your OS and CPU architecture in the root directory. Execute it at your leisure.
(6. If you've got a working build for a platform not yet provided, contact me and I'll upload your binary for others to enjoy)
