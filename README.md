# FixFile Project

### Outline

Since I have always been involved in cross-platform programming, the most frequent simple 'fix', was to the line endings, due to the fact that various editors produced different results ... 

It started as fixfile.asm, 16-bit assembler, then progressed to 32-bits C, as FixF32, now as FixFile... but usually I store it in an active PATH, as ff ...

This repository replaces the [FixF32][1] homepage.

  [1]: http://geoffair.com/ms/fixf32.htm
  
### Building

Uses [CMake][10] to generate the build files of your choice.

```
  cd build  # do not build in the root
  cmake -S .. [options] [-A Win32|x64]
  cmake --build . --config Release # for windows, OR
  make # for Unix Makefile
```

The `options` include -

```
   * `-G "name of generator"` to select what you will use to compile and link the applications.
   * `-DCMAKE_INSTALL_PREFIX:PATH=/path/for/install`, defaults /usr/local in unix.
   * The are many other cmake options...
```

There is a build-me.bat file, which can maybe be modified to suit your environment... and thanks to the easy sharing of this repo, there is now a build-me.sh which should work in your unix/linux/mak environment.

  [10]: https://cmake.org/download/

### License

Where applicable, this project is under -

```
   * GNU GPL version 2
   * Copyright (c) 1089 ... 2020 - Geoff R. McLane
```

See [LICENSE.txt][20] for details.

  [20]: LICENSE.txt
  
### Usage

TODO:
   
Have FUN... Geoff... 20200611

; eof
