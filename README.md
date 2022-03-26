# Tootl Wren

A Godot GDNative library port that implements [Wren Scripting](https://wren.io/) from [T:OoTL - Godot Edition](https://github.com/Elatronion/televoid-godot).

## Dependencies

- scons
- g++
- mingw-w64
- emscripten

## Installation

Build the godot-cpp libraries using scons within the godot-cpp directory.

```
scons platform=<platform> generate_bindings=yes -j4
```

Then, run the makefile within the root directory.
This will build the plugin in a new `./bin/platform/` directory.

## Usage

The built plugin can be added to Godot using GDNative.

The makefile is simple enough to use.

```sh
make CONFIG=release TARGET_OS=linux
```

You may change the `CONFIG` to `debug` or `TARGET_OS` to `windows`.

## License

MIT
