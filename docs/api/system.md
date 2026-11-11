# System and memory API

This part of the API covers lower-level services used by applications, including memory transfers, interrupts, and system helpers.

**Open:** [PVSnesLib generated API reference](https://alekmaul.github.io/pvsneslib/)

**Source headers:** [PVSnesLib SNES headers](https://github.com/alekmaul/pvsneslib/tree/master/pvsneslib/include/snes)

## Look up

- DMA and memory-transfer helpers.
- Interrupt-related declarations.
- System types and low-level definitions.
- Memory and utility routines.

These functions interact closely with SNES hardware. Check parameter requirements and timing constraints in the generated documentation and the matching source before using them.

## Related guide

[SNES hardware](../hardware/index.md) introduces the console resources and timing concepts behind these APIs. The [Development tools](../tools/index.md) page describes the build utilities used by PVSnesLib projects.
