# Basic
The basic (i.e., slow) version of the Eight-Tile program is able to determine whether a board is solvable and if it is, the solution to the problem is displayed using ncurses in the terminal.

## Usage
This program can be compiled using the *makefile*:
```
make
```

The program can then be run using the *makefile* with the input file specified:
```
make run BOARD="board_name.8tile"
```

Four example Eight-Tile boards have been provided - Three are valid and one is invalid - they are called: ```1.8tile``` (5 move solution), ```2.8tile``` (10 move solution),  ```3.8tile``` (20 move solution) and ```4.8tile``` (invalid).

### Requirements
The compiler used in the *makefile* is GCC (https://gcc.gnu.org/) and you will need to have this installed in order to use it. Also, due to the use of ncurses, this will only work in unix-style terminals.

## neillncurses
I have not written the ```neillncurses.h``` file. This was written by Neill Campbell (http://www.bristol.ac.uk/engineering/people/neill-w-campbell/index.html) to facilitate the use of ncurses in the assignment (as the direct use of this library was not being assessed).
