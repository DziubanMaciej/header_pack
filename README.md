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
To make it easy to generate files during build time in CMake projects, *header_pack* provides a convenience CMake function. Syntax of the function:
```
header_pack_generate(<TEXT | BINARY>
                     <input_file_name>
                     <output_file_name>
                     <target>
                     [VARIABLE <variable>]
                     [VALUES_PER_LINE <values_per_line>]
                     [INPUT_SOURCE_GROUP <input_source_group>
                     [OUTPUT_SOURCE_GROUP <output_source_group>)
```

The function creates a [custom command](https://cmake.org/cmake/help/latest/command/add_custom_command.html) that will generate a file named `<output_file_name>` out of `<input_file_name>` inside the build directory. This file is then added to [sources list](https://cmake.org/cmake/help/latest/prop_tgt/SOURCES.html) of `<target>`. Due to the way CMake handles custom commands, this function has to be called in the same directory that the target was created.

If `<variable>` is specified, it selects C++ variable name in the generated file. Default is "variable".

If `<values_per_line>` is specified, it selects the number of bytes per line in `BINARY` mode. Ignored in other modes. Default is 16.

Options `<input_source_group>` and/or `<output_source_group>` are forwarded to [source_group](https://cmake.org/cmake/help/latest/command/source_group.html) CMake call for input and output files respectively. This is only a convenience option for IDEs like Visual Studio and doesn't affect the resulting program.

Example:

**CMakeLists.txt**
```
project(Test)
cmake_minimum_required(VERSION 3.10)
add_subdirectory(header_pack)

add_executable(MyExe main.cpp)
header_pack_generate(TEXT MyExe vertex_shader.glsl vertex_shader.h VARIABLE shader)
header_pack_generate(BINARY MyExe texture.png texture.h VARIABLE texture)
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
