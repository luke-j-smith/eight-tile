# Extension
For the extension I decided to increase the speed of the Eight-Tile program. I achieved this by making the two following changes to the *basic* program.

#### Changing the way in which unsolvable puzzles are found
The new method assesses the starting board after it has been read from a file (assuming that the file is suitable). This method uses the total number of inversions to determine whether the puzzle is solvable - if the total number is even, the the puzzle is solvable. 
```
First we write the initial board out linearly and ignore the blank. 
For example: 5, 2, 8, 4, 1, 7, 3, 6. 
     
The number of inversions of a tile is defined as the amount of subsequent tiles that have a lower value. In our example, 4 has 2 inversions as both 1 and 3 are subsequent tiles. Therefore, the total number of inversions in our example is:
4 + 1 + 5 + 2 + 0 + 2 + 0 + 0 = 14. 

Thus, this puzzle is solvable. 
```

#### Implementation of the A* search algorithm
Information regarding the A* search algorithm can be found at: https://en.wikipedia.org/wiki/A*_search_algorithm

After some experimentation, I decided to use the A* search algorithm with the Manhattan priority function, defined as:
```
The sum of the distances (sum of the vertical and horizontal distance) from the blocks to their goal positions, plus the number of moves made so far to get to the state."
(https://www.cs.princeton.edu/courses/archive/spr10/cos226/assignments/8puzzle.html)
```

When a new board is created, its position in the queue is determined using the priority function. The lower the Manhattan number, the higher its priority in the queue (i.e., the closer to the front of the queue it should be). Note that, no board is deleted as we need to ensure no existing boards are added to the queue. Therefore, even if a new, child, board has the lowest priority out of all boards, it is placed directly after its parent board so that it will be evaluated as the solution next.

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
