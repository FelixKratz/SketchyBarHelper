# SketchyBar Helper
This demo implementation shows how to directly communicate with
[SketchyBar](https://github.com/FelixKratz/SketchyBar) from C or C++ to create
advanced and fast helper programs.

The relevant function from the `sketchybar.h` header is:
```c
char* sketchybar(char* message);
```
it takes a message, e.g. "--query bar", and returns the response from
sketchybar.


## Compile
To compile the example run:
```bash
clang -std=c99 -framework Carbon provider.c -o provider
```
the example contains a simple `CFRunLoopTimer` that fires every two seconds,
where the current cpu utilization and the current time is computed and passed
directly to the appropriate sketchybar items from my config, see `cpu.h` and
`clock.h`.
