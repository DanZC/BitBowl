# Bit Bowl

A bowling game written in C++ for the [WASM-4](https://wasm4.org) fantasy console.

You can play the game [here](https://danzc.itch.io/bit-bowl).

The game was made in C++, using zero standard library code, and only using the external WASM-4 API as well as handwritten code. All standard functions (sprintf, memcpy, strlen, abs) were implemented using C++ code and calls to __builtin_* primitives for primitive WebAssembly operations (like sqrt and popcount). 

The game features a regular bowling mode, playable by up to four players, locally or via netplay.  The player can control the direction of the ball and must time their throw for optimal power. If the player scores a new record in bowling, they will be able to enter their name into a high score table which is automatically saved.

The game also features a survival mode, a single-player mode where the player must hit as many spares in a row as they can. If they fail to hit a spare, their game is over.

Like in the regular bowling game, if the player scores a new record in survival mode, they will be able to enter their name into a high score table which is automatically saved. 

## Building

Build the cart by running:

```shell
make
```

Then run it with:

```shell
w4 run build/cart.wasm
```
