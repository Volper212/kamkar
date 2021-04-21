# kamkar
A cross-platform tile-based hex game made in C++ and OpenGL ![](https://i.imgur.com/rbcgSc9.png)
## Features
- An "infinite" procedurally generated map
- Select an entity (player) by left-clicking the hex it is on (left-click on an empty hex to deselect)
- Right-click on a hex to move the selected entity to that hex, which reveals the terrain in a radius around it
- Use the arrow keys to move the camera around
- Zoom in and out using the mouse wheel
- Click Escape to close the game
## Libraries used
- The C standard library
- OpenGL 4.3 using [GLEW](http://glew.sourceforge.net/)
- [GLFW](https://www.glfw.org/) for cross-platform window and input management
- The C99 version of [FastNoiseLite](https://github.com/Auburn/FastNoiseLite) for procedural noise generation
- [stb_image](https://github.com/nothings/stb/blob/master/stb_image.h) for texture loading
## Compilation
### Windows
Open `kamkar.sln` using Visual Studio, switch to Release mode with x86 and click "Local Windows Debugger". If you want to share the executable (which you can find in the `Release` folder), make sure to copy the `textures` folder to the same folder as `kamkar.exe`.
### Linux
Install the libraries (GLEW, GLFW) using a package manager, then run `source ./build.sh`.
