BlindAM
=======

Blind Auto Mechanic

A game about race cars evolution. A variation of BoxCar2D that uses selection algorythm described by Richard Dawkins in The Blind Watchman.
The entire population (about 200 individuals) lives simultaneously. When a new individual is born it starts its evaluation run on a race track. Whenever it is unable to continue any further it is ready to breed. Every given period of time 3 cars are randomally selected from those ready to breed. The two that advanced the most on track kill the third and produce an offspring.

To build under Linux: delete SDL directory and use "make all" command.
Required dependencies: libsdl-dev, libsdl-image1.2-dev
