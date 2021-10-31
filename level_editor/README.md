# Level Editor

## Building

It is necessary to build raylib before building the Level Editor. Go to `game-tools/level_editor/libs/raylib/src` and run

```
make PLATFORM=PLATFORM_DESKTOP
```

which should produce a `libraylib.a` file in `game-tools/level_editor/libs/raylib`.

If something went wrong during building, refer to the [raylib wiki](https://github.com/raysan5/raylib/wiki).

Next, to build the Level Editor go to `game-tools/level_editor` and run

```
make
```

and launch it with

```
./editor
```
