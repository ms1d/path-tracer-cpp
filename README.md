# PATH-TRACER-CPP

## About

A CUDA C++ path tracer server as a personal project.
See also `ms1d/PathTracerApi` and `ms1d/path-tracer-react`.

## Architecture

- Master process - manages 2 child processes (`http-server`, `path-tracer`)

- HTTP - accepts standard REST API requests (health check, submit render)

- Path Tracer - powered by cuda to run on the GPU; streams results back to UDP clients

## Current Build Instructions

- Configuration: `cmake --preset <preset name>`. See CMakePresets.json for presets

- Build: `cmake --build --preset <preset name>`

- Test: `cd build/<preset name> && ctest` from project root

## Stack

- CMake + Ninja for build tools

- asio for udp

- nlohmann/json for json

- cpp-httplib for tcp

- g++ & nvcc for compilers

## Contributions

Advice + guidance appreciated, but this is mainly a solo learning project for myself.
Feel free to contact me for any queries or concerns.
