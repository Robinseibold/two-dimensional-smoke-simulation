### Two-Dimensional Smoke Simulation
![smoke](https://raw.githubusercontent.com/Robinseibold/two-dimensional-smoke-simulation/master/images/smoke.png)

A two dimensional smoke simulation based on Jos Stam's stable fluid solver.

#### Install
The demo, **Main.cc**, currently only runs on **OS X**.
##### Dependencies
###### Main.cc
* SDL2
* OpenGL
* Cocoa

###### Fluid.cc, Fluid.h, Main.cc
* C++11

```
$ git clone https://github.com/Robinseibold/two-dimensional-smoke-simulation.git
$ make
$ make run
```
#### Usage
*  `ESC` - Exit program.
*  `SPACE` - Reset the fluid.
*  `Left mosue button hold + drag` - Generate density as well as velocity.
*  `Right mouse button hold + drag` - Generate velocity.
