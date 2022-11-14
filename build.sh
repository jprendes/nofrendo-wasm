#!/bin/bash

emsdk() {
    docker run --rm -v $(pwd):/src -u $(id -u):$(id -g) emscripten/emsdk "$@"
}

emsdk emcmake cmake -S ./ -B ./wasm_build -DCMAKE_BUILD_TYPE=Release

cp ./rom.nes ./wasm_build/src

emsdk emmake make -C ./wasm_build

mkdir -p ./dist

cp ./wasm_build/src/nofrendo.{js,wasm} ./index.html ./dist
