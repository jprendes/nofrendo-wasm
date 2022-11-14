# arduino-wasm

This is Nofrendo compiled to WebAssembly.

# build it

You need to place a ROM in the project root, with the name `rom.nes`.
Then run `./build.sh`.

# Play it

Once it's compiled, serve the files in the `./dist` folder, e.g., with `npx serve ./dist`.
