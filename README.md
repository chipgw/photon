Photon
=========
Photon is a simple game where you move along a laser, making your path using mirrors to complete various goals.

Dependencies:
-------------
* [CMake]
* [Freetype2]
* [GLEW] Windows only
* [GLM]
* [LibXML2]
* [Lua] 5.2
* [PhysFS]
* [SDL2]
* GPU supporting OpenGL 2 or later.

Building
--------
* Clone or download the source code.
* In the source folder, create a `build` folder.
* In the build folder, run `cmake ..`.
* If you want to use a different generator than your platform default, add `-G <generator>` to the cmake command, with your desired generator. A list of generators can be found by running `cmake -h`.
* The project files should now be generated in `build`.

License
-------
[MIT License]


[CMake]:http://www.cmake.org
[Freetype2]:http://freetype.org
[GLEW]:http://glew.sourceforge.net
[GLM]:http://glm.g-truc.net/
[LibXML2]:http://xmlsoft.org
[Lua]:http://lua.org
[PhysFS]:http://icculus.org/physfs/
[SDL2]:http://libsdl.org
[MIT License]:LICENSE
