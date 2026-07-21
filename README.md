# CgePix - Pixel Manipulation Library for C

A compact, dependency-free C library for pixel manipulation, color handling,
and image data management.

## Features

- Support for indexed, grayscale, RGB/BGR, RGBA/BGRA/ARGB/ABGR, and packed
  formats (e.g. RGB565, RGBA1010102)
- Color types: RGBA, HSVA, HSLA with 16-bit per channel
- Palette support for indexed images
- Color space conversion: RGBA - HSVA - HSLA
- Alpha handling: premultiply and unpremultiply
- Multiple blending modes: normal, multiply, screen, overlay, darken, lighten,
  difference, exclusion, and more
- Subimage view creation and row-wise pixel access
- Safe pixel get/set with format conversion

## Build Systems

- CMake - supports Linux, macOS, Windows (MSVC, MinGW)
- Makefile.posix - for GCC/Clang on POSIX systems
- Makefile.mingw - for MinGW on Windows
- Makefile.win32 - for MSVC with NMake

Builds a static library. No shared library or external dependencies.

## Usage Example

```c
#include "CgePix.h"
#include <stdio.h>
#include <stdlib.h>

int main() {
    uint32_t width = 256;
    uint32_t height = 256;

    CgePix img = {0};
    int format = CGE_PIX_RGBA8;
    int flags = 0;

    size_t dataSize = CgePixInitLayout(width, height, format, flags, &img);
    img.data = malloc(dataSize);

    CgeColor red;
    CgeColorSetRGBA8(&red, 255, 0, 0, 255);

    for (uint32_t y = 0; y < height; y++) {
        for (uint32_t x = 0; x < width; x++) {
            CgePixSetColor(&img, x, y, &red);
        }
    }

    FILE *file = fopen("output.raw", "wb");
    fwrite(img.data, 1, dataSize, file);
    fclose(file);

    free(img.data);
    return 0;
}
```

## Portability

- Written in C89 + stdint.h and stddef.h
- No dynamic memory allocation in library functions
- No external dependencies

## License

0BSD - a permissive license with no attribution required.
