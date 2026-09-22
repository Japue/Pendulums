# Pendulums

## Modes
- "single": makes a single pendulum according to general and single parameters
- "grid": makes a grid of pendulum with starting angles according to the grid with general and grid parameters
- "graph": takes a single pendulum and graphs it's angles on the x and y axes

## Parameters
### General
- runtime: amount of milliseconds the simulation will run
- timestep: this determines the amount of steps in the solver which is equal to runtime/timestep
- l: the length scaling for SFML, 100 is good
- dot_size: size of the nodes, 5 is good

### Single
angles: takes the two starting angles of the pendulum in degrees like this [90, 45]

### Grid
grid_radius: determines how many pendulums are drawn, radius 1 makes 9 pendulums, with a radius of 1 around the center
grid_angle_interval: determines the angle spacing of the pendulums, the grid is made of an x and y axis to which the angles are mapped, 1 degree would mean that the next pendulum to the right will start with an angle of 1 degree more than the one before, the center is at (0, 0)

### Options
#### Trace
trace: true or false, enables a trace
trace_length: sets how many steps should be in the trace