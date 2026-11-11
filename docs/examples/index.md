# Examples

PVSnesLib ships with example projects that demonstrate individual library features as well as larger game-oriented systems. They are intended to be read, compiled, modified, and used as practical references.

The community-maintained [Examples gallery](https://github.com/alekmaul/pvsneslib/wiki/Examples-gallery) provides screenshots and links to the source directory for each example. The gallery is the best place to browse the complete, up-to-date collection.

## Find an example by topic

The following selection is a starting point, not an exhaustive catalogue. Example names are kept exactly as they appear in the repository and gallery.

| If you want to explore… | Start with these examples |
| --- | --- |
| A minimal working program | `hello_world` |
| Background video modes | `Mode0`, `Mode1`, `Mode3`, `Mode5`, `Mode7` |
| Background scrolling and maps | `Mode1Scroll`, `Mode1ContinuousScroll`, `Mode1MixedScroll`, `mapscroll`, `DynamicMap`, `mapbuffer` |
| Compressed graphics | `Mode1LZ77`, `Mode7LZ77` |
| Transparency and visual effects | `Transparency`, `Fading`, `Mosaic`, `Waves`, `Window`, `HDMAGradient` |
| Sprites and animation | `SimpleSprite`, `AnimatedSprite`, `DynamicSprite`, `MetaSprite`, `DynamicEngineSprite`, `DynamicEngineMetaSprite` |
| Controllers and peripherals | `controller`, `mouse`, `mouse-data-test`, `multiplay5`, `superscope` |
| Music and sound effects | `music`, `music2`, `effects`, `effectsandmusic`, `tada` |
| Text and console output | `textvwfont`, `typeconsole_ntsc`, `typeconsole_pal` |
| Game logic and reusable systems | `gastates`, `mapandobjects`, `moveobjects`, `scoring`, `timer` |
| Debugging and memory | `breakpoints`, `debug`, `memory_mapping`, `sram`, `sramoffset`, `testregion` |
| Complete game examples | `breakout`, `likemario`, `shmup`, `slopemario` |

The names above are a guide to what to inspect first. Read each example's source and Makefile to confirm the exact feature demonstrated and the assets or build options it requires.

## A good order for exploring

If you are new to PVSnesLib, follow this progression:

1. **`hello_world`** — start with a small, buildable project and understand its source and Makefile.
2. **`Mode1`** — see a basic background setup after reading [Graphics and backgrounds](../tutorials/graphics.md).
3. **`SimpleSprite`** and **`AnimatedSprite`** — move from a static background to moving objects. See [Sprites](../tutorials/sprites.md).
4. **`controller`** — connect player input to your program. See [Controller input](../tutorials/input.md).
5. **`music`** or **`effectsandmusic`** — explore the audio pipeline. See [Audio](../tutorials/audio.md).
6. **`breakout`**, **`likemario`**, or **`shmup`** — study how several techniques can be combined in a game.

## Build and modify examples

Build an example from its own directory, using the Makefile supplied with it:

```sh
make
```

For a clean rebuild:

```sh
make clean
make
```

Start by changing one thing at a time. For example, modify a displayed string, a movement value, or a visual parameter, then rebuild and test the ROM. This makes it easier to connect a source-code change to the result on screen.

Keep an untouched copy of the original example. When creating your own project, copy the complete example directory so that its Makefile, source files, and assets remain together.

> **Tip:** The top-level `snes-examples/Makefile` can build the example subdirectories that contain a Makefile. It also provides an `install` target that gathers generated `.sfc` files in a `bin` directory. Check the Makefile in your installed version before using a bulk build.

## Browse the source code

- [Complete examples gallery](https://github.com/alekmaul/pvsneslib/wiki/Examples-gallery) — screenshots and links to individual example folders.
- [PVSnesLib repository](https://github.com/alekmaul/pvsneslib) — source code, tools, library, and examples.
- [Game examples directory](https://github.com/alekmaul/pvsneslib/tree/master/snes-examples/systems/games) — examples including Breakout and LikeMario.
- [Hello World source](https://github.com/alekmaul/pvsneslib/tree/master/snes-examples/hello_world) — the small starting project described in [First project](../getting-started/first-project.md).

## Related documentation

- [First project](../getting-started/first-project.md)
- [Graphics and backgrounds](../tutorials/graphics.md)
- [Sprites](../tutorials/sprites.md)
- [Controller input](../tutorials/input.md)
- [Audio](../tutorials/audio.md)
