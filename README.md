# chaw
Reimplementation of 3DMMs `chomp`. This was made for the purpose of providing a cross-platform solution and to experiment with decompilation.

## Building
### Prerequisites
* git
* CMake
* a C23 capable compiler (gcc, clang, etc.)
* a native build system (Ninja, GNU make, etc.)

Just open up your favorite terminal, CD into to the cloned repository and run `cmake -B -GNinja .`; Replace `-GNinja` with your preferred generator.