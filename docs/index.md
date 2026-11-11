# Create your own Super Nintendo games in C.

PVSnesLib is an open-source development library and toolchain for creating homebrew games and applications for the Super Nintendo Entertainment System (SNES) **using the C programming language**.

Write your game's logic in C, use PVSnesLib's APIs for graphics, sprites, controller input and audio, then build a ROM for the SNES. The library also includes low-level assembly components and tools that support the 65816-based development toolchain.

<div class="home-hero-actions">
  <a class="md-button md-button--primary" href="getting-started/installation/">Get started</a>
  <a class="md-button" href="https://github.com/alekmaul/pvsneslib">View on GitHub</a>
</div>

<div class="home-feature-grid">
  <a class="home-feature" href="getting-started/">
    <strong>Build SNES games in C</strong>
    <span>Install the toolchain, compile a C project and run your first Super Nintendo ROM.</span>
  </a>
  <a class="home-feature" href="tutorials/">
    <strong>Use the SNES hardware</strong>
    <span>Explore practical guides for backgrounds, sprites, controller input and audio.</span>
  </a>
  <a class="home-feature" href="api/">
    <strong>Explore the API</strong>
    <span>Browse the generated Doxygen reference for PVSnesLib functions and declarations.</span>
  </a>
</div>

## Games made with PVSnesLib

A few projects created with the library. These games are showcases; their source code is not included here.

<div class="game-showcase-grid">
  <article class="game-showcase-card">
    <img src="assets/images/games/sydneycod.gif" alt="Sydney Cod gameplay animation" loading="lazy">
    <div class="game-showcase-copy">
      <strong>Sydney Cod</strong>
      <span>A game made with PVSnesLib.</span>
    </div>
  </article>
  <article class="game-showcase-card">
    <img src="assets/images/games/eyra.gif" alt="Eyra gameplay animation" loading="lazy">
    <div class="game-showcase-copy">
      <strong>Eyra</strong>
      <span>A game made with PVSnesLib.</span>
    </div>
  </article>
  <article class="game-showcase-card">
    <img src="assets/images/games/chipchallenge.gif" alt="Chip Challenge gameplay animation" loading="lazy">
    <div class="game-showcase-copy">
      <strong>Chip Challenge</strong>
      <span>A game made with PVSnesLib.</span>
    </div>
  </article>
</div>

## Learn from the source

These example games are included in the PVSnesLib repository. Explore their C source code and use them to understand how a complete game is structured.

<div class="game-showcase-grid game-showcase-grid--examples">
  <article class="game-showcase-card">
    <img src="assets/images/games/breakout.gif" alt="Breakout gameplay animation" loading="lazy">
    <div class="game-showcase-copy">
      <strong>Breakout</strong>
      <span>Study a complete Breakout-style game.</span>
      <a href="https://github.com/alekmaul/pvsneslib/tree/master/snes-examples/systems/games">Browse game example sources</a>
    </div>
  </article>
  <article class="game-showcase-card">
    <img src="assets/images/games/likemario.gif" alt="LikeMario gameplay animation" loading="lazy">
    <div class="game-showcase-copy">
      <strong>LikeMario</strong>
      <span>Explore a platform-game example.</span>
      <a href="https://github.com/alekmaul/pvsneslib/tree/master/snes-examples/systems/games">Browse game example sources</a>
    </div>
  </article>
</div>

## Explore all examples

The community-maintained Wiki gallery brings together screenshots and information about many projects and examples made with PVSnesLib.

<div class="home-hero-actions">
  <a class="md-button" href="https://github.com/alekmaul/pvsneslib/wiki/Examples-gallery">Explore the examples gallery</a>
</div>

## Start building

You can start with the supplied C `hello_world` project, then learn how to add backgrounds, sprites, controller input and sound. Follow the [Getting started](getting-started/index.md) to build your first ROM.
