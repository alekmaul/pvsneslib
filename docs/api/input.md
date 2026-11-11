# Input API

The input API documents how a PVSnesLib program reads the state of supported controllers and peripherals.

**Open:** [PVSnesLib generated API reference](https://alekmaul.github.io/pvsneslib/)

**Source headers:** [PVSnesLib SNES headers](https://github.com/alekmaul/pvsneslib/tree/master/pvsneslib/include/snes)

## Look up

- Joypad identifiers and button constants.
- Current, newly pressed, and held button states.
- Additional controller support.
- Mouse initialization and state.
- Super Scope detection and input flags.

For exact semantics, check the Doxygen entry and the header for the version you are using. Input state may be updated as part of the library's frame and interrupt handling.

## Related guide

[Controller input](../tutorials/input.md) gives practical examples and explains the supported peripherals.
