# header_pack

This is a tool useful for embedding resource files inside your C++ application's binary.

### Overview of the problem
For most applications it is often necessary to use some kind of data, that is not stored as code. Things like shaders, textures, fonts, csv, sounds, etc. The usual approach to this is distributing the data alongside application's binary as files. Although, when the amount of data is small, it makes sense to bake it inside the executable to reduce the potential problems, like resolving paths, handling bad formats, permissions, deleted files, etc.

There exist some ways to achieve this: [Windows resources](https://learn.microsoft.com/en-us/windows/win32/menurc/resources-overviews), [Linux objcopy](https://sourceware.org/binutils/docs/binutils/objcopy.html), [Assembly](https://stackoverflow.com/a/10692876/4259190). They are, however, all environment specific, making them hard to use in cross-platform applications.

The easiest, most robust, cross-platform, performant and safe way to embed data inside a C++ application is to just store it in a global constant. There is a useful tool called [xxd](https://linux.die.net/man/1/xxd), that does exactly this, but it is not installed by default on all systems. It also cannot be used as a tiny dependency, because it's source is within Vim's tree.

### Solution
This tool generates a simple C++ file with a single global constant definition containing the content of an input file. The content can be interpreted both as a text file and stored as a c-string or a binary data stored as an array of hex values. See the help message for detailed description of available command line arguments.

Example below:

```
$ cat input.txt
a
b
c
```

```
$ ./header_pack.exe input output.txt -t
$ cat output.txt
#pragma once

const char *var = R"DELIMETER(a
b
c
)DELIMETER";
```

```
$ ./header_pack.exe input output.txt -b
$ cat output.txt
#pragma once

const unsigned char var[] = {
    0x61, 0x0a, 0x62, 0x0a, 0x63, 0x0a,
};
```

### CMake integration
To make it easy to generate files during build time in CMake projects, header_pack provides convenience CMake functions that do the following:
- create a [custom command](https://cmake.org/cmake/help/latest/command/add_custom_command.html) that will generate the file in build directory,
- add generated file to your [target's](https://cmake.org/cmake/help/latest/manual/cmake-buildsystem.7.html#binary-targets) [sources list](https://cmake.org/cmake/help/latest/prop_tgt/SOURCES.html) to create a dependency on the custom command,
- add a directory with generated file to your target's [include directories](https://cmake.org/cmake/help/latest/prop_tgt/INCLUDE_DIRECTORIES.html) to allow C++ code include it like `#include <generated/file_name.h>`.

Example:

**CMakeLists.txt**
```
project(Test)
cmake_minimum_required(VERSION 3.10)
add_subdirectory(header_pack)

add_executable(MyExe main.cpp)
header_pack_generate_from_text(MyExe vertex_shader.glsl vertex_shader.h shader)
header_pack_generate_from_binary(MyExe texture.png texture.h texture)
```

**main.cpp**
```
#include <generated/vertex_shader.h>
#include <generated/texture.h>

int main() {
    // use variable "shader"
    // use variable "texture"
}
```
