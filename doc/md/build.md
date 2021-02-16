@page build Build the library

[TOC]

If you have experience with a personal workflow, you can build Candle as a regular SFML library and the demo as an executable. You just have to add the `include` folder to your include path and use `demo.cpp` and `src/*` as the source files.

If that's not the case, there are two alternatives.   

# CMake

You can build the static library and the demo program using CMake.

```shell
mkdir build && cd build
cmake .. -DBUILD_DEMO=ON
cmake --build .
```

This will generate `libCandle-s.a` or (`Candle-s.lib` on Windows) in `build/lib` folder and the `demo` program (or `demo.exe`) in `build/bin`.

If it happens that CMake can't find the SFML files, you might need to set manually `SFML_ROOT` inside the `CMakeLists.txt` (uncomment and complete line 15).

# Make

For Linux users, the old Candle build system is still available. You just have to

```shell
make release
```

to build `libCandle-s.a` and the `demo` program inside the folder `release`. Alternatively, you can use

```shell
make debug
```

to build the debug version inside the folder `debug`.