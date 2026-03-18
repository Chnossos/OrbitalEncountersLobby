# Orbital Encounters Lobby

EPITECH @ 2015~2016

This is the Lobby Server of the video-game project Orbital Encounters that my team and I developped during our two last years of study.

## Technologies

This project is made using Boost::Asio only (updated to v1.90.0).

## How to build

- Make sure you have a C++14 toolchain installed system-wide
- Install CMake >= 3.23
- Install and configure [Conan][1]
- Install latest Ninja
- Then:
  ```shell
  conan install -b missing -c tools.cmake.cmaketoolchain:generator="Ninja Multi-Config"
  cmake --preset conan-default
  cmake --build build --preset conan-release
  ```
  
## How to run

Binaries are located in `<root>/bin` folder.

**Note:** The `Client` project is a test helper used during development.

[1]: https://docs.conan.io/2/installation.html
