# Test WreNwge App

This is an app used to test WreNwge during development.

It also showcases how to split a game into multiple bundles. The
[`source`](source) directory gets packed into `test.bndl` and contains the
game's source code as well as the [`game.json`](source/game.json) file. The
[`assets`](assets) directory gets packed into `test-assets.bndl` and contains
all the textures, sounds, etc. used by the test app. Note that the
`test-assets` bundle is loaded by the game by constructing a `Bundle` object,
see [this line in `main.wren`](source/main.wren#L10). WrenNwge will only load
the source code bundle for you, and you have to load all other bundles yourself.
