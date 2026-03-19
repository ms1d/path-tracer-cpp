# PATH-TRACER-CPP

## About

A CUDA C++ path tracer server as a personal project.
See also `ms1d/PathTracerApi` and `ms1d/path-tracer-react`.

## Program flow

1. Input (JSON) containing tris, verts, materials and other scene data via tcp

2. Start async cuda render job (path tracing)

3. As pixels finish, send them over a udp connection indexed by frame number & position

4. Client can handle the pixels received

## Current Build Instructions

- Configuration: `cmake --preset <preset name>`. See CMakePresets.json for presets

- Build: `ninja -C build`

- Test: `cd build && ctest` from project root

## Stack

- CMake + Ninja for build tools

- asio for udp

- nlohmann/json for json

- cpp-httplib for tcp

- g++ & nvcc for compilers

## Contributions

Advice + guidance appreciated, but this is mainly a solo learning project for myself.
Feel free to contact me for any queries or concerns.
