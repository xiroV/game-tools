# Level Editor

## Installation

The level editor depends on [raylib](https://github.com/raysan5/raylib/) and [raygui](https://github.com/raysan5/raygui/). These will have to be downloaded using a script or manually.

The latest tested versions are:

 - raylib 4.5.0
 - raygui 3.6

### Automatic installation

**NOTE:** Only tested on Linux (64-bit), and uses `curl` and `tar`.

To automatically fetch the dependencies, run

```bash
./fetch_dependencies
```

Then build the level editor with

```bash
make
```

### Manual installation

 - Download a release of raylib from the [raylib releases page](https://github.com/raysan5/raylib/releases).
 - Extract the content of the folder of the archive into `game-tools/level_editor/lib/raylib`.
 - Download a release of raygui from the [raygui releases page](https://github.com/raysan5/raygui/releases).
 - Extract the content of the folder of the archive into `game-tools/level_editor/lib/raygui`.

If something went wrong during building, refer to the [raylib wiki](https://github.com/raysan5/raylib/wiki).

Then build the level editor with

```bash
./make
```

## Running
Launch the level editor with

```
./editor
```

A level file (`.json` or `.lvl`) can be supplied as input with

```bash
./editor MyLevel.json
```
