# light-play

## description
- Light play is a simple 2D ray marcher.
- It simulates reflection of light in presence of (different types of mirrors) using ray marching technique.

## code
- The code is written in `C++`.
- All the src is written in `src/`.
- [Simplecpp](https://www.cse.iitb.ac.in/~ranade/simplecpp/) is used for rendering.
- `include` & `lib` contains headers and implementation of simplecpp.

## documentation
- The documentation for the code is itself.

## usage

### how to run? [linux]
- Make sure x11 and x11-dev are installed.
- Open terminal in the root directory.
    - Make a build directory using `mkdir build && cd build`.
    - Create a makefile using `cmake ..`
    - Compile code using `make`. This creates an executable named `lightplay`.
    - Use `./lightplay` to start playing.

### workflow
- There are three types of mirror (line, circle, square). Specify the number of each type when prompted.
- You can place the mirrors one by one in that order.
- Then you can place a fixed light source which emits a straight beam of light.
- Then you can place a balloon.
- You can control the light's direction by clicking inside concentric circles.
- The light reflects off of mirrors following the law of reflection.
- When the light hits the green balloon the simulation ends.

## demonstration
The following GIF illustrates the usage.

![](./github/gameplay.gif)

## roadmap
- [x] Basic ray marching.
- [x] Improve code.
    - [x] Light class.
    - [x] Mirrors class.
    - [x] Limits in number of mirrors.
    - [x] Light play class.
    - [x] Tidy code.
- [ ] Improve game.
    - [ ] Improve text box placement.
    - [ ] Predefined mirror placements as levels stored in files.
