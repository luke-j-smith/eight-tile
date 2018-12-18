/* 
 * The 8-tile Puzzle using Manhattan priority function. 
 * Includes main(). NB: Linked data structure, Puzzle, is 
 * defined in the manhattan_display_operations.h file. 
 */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "manhattan_file_operations.h"
#include "manhattan_display_operations.h"

/* Error messages presented when unexpected behaviour. */ 
#define ARGC_ERROR "\nERROR: Unexpected input.\nPlease " \
"try something like: %s 8-Puzzle.txt\n\n"
#define MEMORY_ERROR "\nERROR: Unable to allocate space " \
"for new board\n\n."
#define CHECK_ERROR "\nERROR: Unexpected input for choice "\
"of test.\nChoice input should either be 's' to test if " \
"solution, or 'e' to test if board already exists.\n\n"
#define MOVE_ERROR "\nERROR: Unexpected move.\n\n"
#define MANHATTAN_TILE_ERROR "\nERROR: Unexpected tile " \
"input into the Manhattan number generator!\n\n"

/* Height, width and total number of tiles. */
#define YS 3
#define XS 3
#define NUM_TILES (YS * XS)
#define NUM_WO_BLANK (NUM_TILES - 1)
#define NULL_BOARD {{'0', '0', '0'}, \
{'0', '0', '0'}, {'0', '0', '0'}}
#define BLANK ' ' 
#define TRUE 1 
#define FALSE 0

/* Defines starting position (ie, original board in file).*/ 
#define ROOT_NODE -1

/* Used to distinguish between different checks of board. */
#define IS_SOLUTION 's'
#define ALREADY_EXISTS 'e'

/* Limits the upper_bound when checking if solution. */
#define SOL_ONLY 1

/* 2D array representing the solution to 8-tile puzzle. */
#define SOLUTION {{'1', '2', '3'}, {'4', '5', '6'}, \
{'7', '8', BLANK}}

/* Four Possible move/swap directions - ie, N, E, S and W.*/
#define MOVE_DIRECTIONS 4
/* 
 * Bounds check used when determining if the blank tile \ 
 * could possibly be N, E, S or W, respectively, from the \
 * current tile, t (our position in 2D array from [0][0]).
 */
#define NORTH_POSSIBLE(t) (t > XS - 1)
#define EAST_POSSIBLE(t) (((t + 1) % XS) != 0)
#define SOUTH_POSSIBLE(t) (t < ((YS * XS) - XS))
#define WEST_POSSIBLE(t) (((t + XS) % XS) != 0)

/* Values needed to move N, E, S and W using pnter arthm. */
#define MOVE_N -XS
#define MOVE_E 1 
#define MOVE_S XS
#define MOVE_W -1

/* Used to define relavant direction from centre tile. */
#define N north
#define E east
#define S south
#define W west

/* Defines tile position when generating Manhattan number.*/
#define FIRST_ROW first
#define SECOND_ROW second
#define THIRD_ROW third
#define FIRST_COL first
#define SECOND_COL second
#define THIRD_COL third

enum compass {north, east, south, west} ; 
typedef enum compass compass ;

enum order {first, second, third} ; 
typedef enum order order ;

void puzzle_from_file(char *argv, Puzzle **start, \
Puzzle **current, int counter) ;
Puzzle *allocate_space(char *top_left, int counter, \
int man_num) ; 
void find_optimal_solution(Puzzle *start, Puzzle *current, \
int *cntr) ;
void pointer_chase(Puzzle **start, Puzzle **location, \
int displacement) ;
int check_if_board(char choice, Puzzle *current, \
Puzzle *start, int place) ;
int test_against(Puzzle *test, char against[YS][XS], \
int upper_bound) ; 
void define_options_and_explore(Puzzle *start, \
Puzzle *current, int counter) ; 
void determine_search_directions(int tile, int *direction) ; 
void search_possible_directions(int tile, int *direction, \
Puzzle *start, Puzzle *current, int counter) ;
void create_next_board(Puzzle copy, int counter, int tile, \
int direction, Puzzle *start) ; 
void make_move(Puzzle *copy, int counter, int tile, \
int dirctn) ; 
int manhattan_function(char board[YS][XS], int moves) ; 
void correct_tile_position(char tile, int *y, int *x) ;
int determine_place(int man_num, Puzzle *start, \
int counter) ;  
void insert(char *top_left, Puzzle *start, \
Puzzle *location, int place, int counter, int moves, \
int total) ; 
void free_malloc_memory(Puzzle *start) ; 


int main(int argc, char *argv[])
{
  Puzzle *start = NULL, *current = NULL ;
  int counter, *cntr ;
 
  /* Used to keep track of which board in queue we are at.*/  
  counter = 0 ; 
  cntr = &counter ;
  
  /* If we exit() anywhere in code, call this function. */
  atexit(Neill_NCURS_Done) ;   

  /* Ensure that file is entered properly on command line.*/
  if(argc == 2){
      
    puzzle_from_file(argv[1], &start, &current, counter) ;
    
    /* Starting board is root and no moves made as of yet.*/
    start->parent = ROOT_NODE ; 
    start->moves = 0 ; 
              
    find_optimal_solution(start, current, cntr) ; 

    display_solution(start, counter) ;
    
    free_malloc_memory(start) ;
  }
  
  else{
    fprintf(stderr, ARGC_ERROR, argv[0]) ;
    return 1 ; 
  }
  
  return 0 ; 
}

/* All file related functions - see relevant header. */
void puzzle_from_file(char *argv, Puzzle **start, \
Puzzle **current, int counter) 
{
  FILE *et = NULL ;
  char *fb, first_board[YS][XS] = NULL_BOARD ;
  
  fb = &first_board[0][0] ;
  
  open_file(&et, argv) ; 
  
  check_file_and_fill_first_board(et, fb, argv) ; 
    
  close_file(&et, argv) ;
  
  /* Check is solvable at the start - extension work. */ 
  solvable_board_check(fb) ;
          
  *start = *current = allocate_space(fb, counter, Q_MAX) ;
    
  return ; 
}

/* Allocates space in memory for new board. */  
Puzzle *allocate_space(char *top_left, int counter, \
int man_num)
{
  Puzzle *new = NULL;
  int i ;
  char *fill ;    
  new = (Puzzle *)malloc(sizeof(Puzzle)) ; 
    
  if(new == NULL){
    fprintf(stderr, MEMORY_ERROR) ; 
    exit(EXIT_FAILURE) ; 
  }
  
  fill = &new->board[0][0] ; 
      
  for(i = 0 ; i < NUM_TILES ; i++){
    *(fill + i) = *(top_left + i) ; 
  }
  
  /* Include the Manhattan priority number for each board.*/
  new->manhattan = man_num ;
   
  new->parent = counter ; 
  
  new->next = NULL ;
      
  return new ; 
}

/* Finds the optimum solution for a solvable puzzle. */ 
void find_optimal_solution(Puzzle *start, Puzzle *current, \
int *cntr)
{
  /* 
   * While current board in the queue is not the solution, \
   * we add any 'child' boards the queue base on priority. 
   */
  while(!check_if_board(IS_SOLUTION, current, start, \
  *cntr)){

    pointer_chase(&start, &current, *cntr) ; 
      
    /* Incr counter after fctn called to move along queue.*/  
    define_options_and_explore(start, current, (*cntr)++) ;
  }
    
  return ; 
}

/* Either check if board is solution or already exists. */
int check_if_board(char choice, Puzzle *current, \
Puzzle *start, int place)
{
  Puzzle *test ;
  int check, match ;
  char sol[YS][XS] = SOLUTION ; 
    
  check = FALSE ;
  match = 0 ; 
  
  /* variable choice defines which check we carry out. */
  switch(choice){
    case IS_SOLUTION : test = current ; 
    match = test_against(test, sol, SOL_ONLY) ; 
    break ; 
    case ALREADY_EXISTS : test = start ; 
    match = test_against(test, current->board, place) ;
    break ; 
    /* Catches any error when specifying type of check. */ 
    default : fprintf(stderr, CHECK_ERROR) ; 
    exit(EXIT_FAILURE) ; 
  }
  
  (match > 0) ? (check = TRUE) : (check = FALSE) ;
  
  return check ; 
}

/* Lets us carry out the relevant test, using the bounds
 * defined in the function above. If any matches, then flag.
 */
int test_against(Puzzle *test, char against[YS][XS], \
int upper_bound)
{
  int i, match ; 
  
  match = 0 ; 
  
  for(i = 0 ; i < upper_bound ; i++){
    if(memcmp(test->board, against, sizeof(char[YS][XS])) \
    == 0){
      match++ ; 
    }
    
    test = test->next ; 
  }

  return match ; 
}

/* Moves us along the linked list to required position. */ 
void pointer_chase(Puzzle **start, Puzzle **location, \
int displacement)
{
  Puzzle *temp ; 
  int i ; 
  
  temp = *start ;
  
  for(i = 0 ; i < displacement ; i++){
    temp = temp->next ; 
  }
  
  *location = temp ; 
    
  return ;
}

/* For each tile, bounds the directions we look for blank.*/
void define_options_and_explore(Puzzle *start, \
Puzzle *current, int counter) 
{
  char *top_left ;
  int tile ;
   
  /* Stores values used in pntr arthm to reach N, E, S, W.*/
  int search_direction[MOVE_DIRECTIONS] ;
    
  top_left = &current->board[0][0] ; 
    
  for(tile = 0 ; tile < NUM_TILES ; tile++){
    /* If currently blank tile, no point doing anything! */ 
    if(*(top_left + tile) != BLANK){
      determine_search_directions(tile, search_direction) ;
         
      search_possible_directions(tile, search_direction, \
      start, current, counter) ;   
    }
  }
  
  return ;
}

/* Bounds checks - is possible to search N, E, S, W. */ 
void determine_search_directions(int tile, int *direction)
{
  /* 
   * If search direction possible, the amount needed to be \
   * added to reach the cell from current position/pointer \
   * is entered into corresponding cell of options array. 
   */
  NORTH_POSSIBLE(tile) ? (*(direction + N) = MOVE_N) : \
  (*(direction + N) = FALSE) ; 
  
  EAST_POSSIBLE(tile) ? (*(direction + E) = MOVE_E) : \
  (*(direction + E) = FALSE) ;
  
  SOUTH_POSSIBLE(tile) ? (*(direction + S) = MOVE_S) : \
  (*(direction + S) = FALSE) ;
  
  WEST_POSSIBLE(tile) ? (*(direction + W) = MOVE_W) : \
  (*(direction + W) = FALSE) ;

  return ; 
}

/* Checks adjacent tiles using pointer arithmetic */ 
void search_possible_directions(int tile, int *direction, \
Puzzle *start, Puzzle *current, int counter)
{
  Puzzle copy ; 
  int i ; 
  char *tile_location ;
          
  for(i = 0 ; i < MOVE_DIRECTIONS ; i++){
    copy = *current ;
    
    tile_location = &copy.board[0][0] + tile ;
    /* 
     * If adjacent tile is the blank tile, then create next\
     * board. Okay if direction[i] is zero, as the tile we \
     * are evaluating can never be the blank tile, as above.
     */
    if(*(tile_location + direction[i]) == BLANK){
      create_next_board(copy, counter, tile, direction[i], \
      start) ; 
    }
  }
  
  return ; 
}

/* Creates next board and checks whether already exists. */
void create_next_board(Puzzle copy, int counter, int tile, \
int direction, Puzzle *start)
{
  Puzzle *potential = NULL; 
  char *top_left ;
  int place, moves, man_num ; 
    
  /* Keeps track of the total number of unique boards. */ 
  static int total = 0 ;
  
  place = 0 ; 
  top_left = &copy.board[0][0] ; 

  /* Create new board by making appropriate move. */
  make_move(&copy, counter, tile, direction) ; 
  
  potential = &copy ; 
  
  /* If new board does not exist, add to queue. */  
  if(!check_if_board(ALREADY_EXISTS, potential, start, \
  total)){
    /* Set moves to be amount taken to reach prior board.*/
    moves = potential->moves ;
    
    /* Calculate the Manhattan number, increase moves. */
    man_num = manhattan_function(potential->board, \
    ++moves) ; 

    /* Determines place of new board using priority. */ 
    place = determine_place(man_num, start, counter) ;

    total++ ; 

    /* If new board has lowest priority, then add to back.*/
    if(total + 1 == place){
      pointer_chase(&start, &potential, total - 1) ; 
      
      potential->next = allocate_space(top_left, counter, \
      man_num) ;
    }

    /* Otherwise, insert into relevant place of queue. */ 
    else{
      insert(top_left, start, potential, place, counter, \
      moves, man_num) ;
    }
  }
  
  return ; 
}

/* Swaps tile with blank tile and updates parent value. */
void make_move(Puzzle *copy, int counter, int tile, \
int dirctn)
{ 
  char *t ; 
  
  t = &copy->board[0][0] + tile ; 

  /* Swap blank (in N, E, S, or W cell) with number tile. */ 
  switch(dirctn){
    case MOVE_N : *(t + dirctn) = *t ; *t = BLANK ; break ;
    case MOVE_E : *(t + dirctn) = *t ; *t = BLANK ; break ; 
    case MOVE_S : *(t + dirctn) = *t ; *t = BLANK ; break ;
    case MOVE_W : *(t + dirctn) = *t ; *t = BLANK ; break ;
    /*Use default case to catch any unexpected moves. */
    default : fprintf(stderr, MOVE_ERROR) ; 
    exit(EXIT_FAILURE) ; 
  } 
  
  copy->parent = counter ;
  
  return ; 
}

/* Manhattan priority function determines place in queue. */ 
int manhattan_function(char board[YS][XS], int moves)
{
  int i, j, manhattan_number, correct_x, correct_y, *x, *y ;

  x = &correct_x ; 
  y = &correct_y ; 
  manhattan_number = 0 ; 
  /* 
   * For all non-blank tiles, sum the absolute values of 
   * the coordinate distances (j, i) of each tile in their
   * current position and their correct position (y, x). 
   */
  for(j = 0 ; j < YS ; j++){
    for(i = 0 ; i < XS ; i++){
      if(board[j][i] != BLANK){
        correct_tile_position(board[j][i], y, x) ;
        
        manhattan_number += (abs(correct_y - j) + \
        abs(correct_x - i)) ; 
      }
    }
  }

  /* Add the number of moves already made get Manhatn. no.*/ 
  manhattan_number += moves ; 
  
  return manhattan_number ; 
}

/* Return the coordinates of each tile in solution state. */
void correct_tile_position(char tile, int *y, int *x)
{
  switch(tile){
  case CHAR_1 : *y = FIRST_ROW ; *x = FIRST_COL ; break ;
  case CHAR_2 : *y = FIRST_ROW ; *x = SECOND_COL ; break ;
  case CHAR_3 : *y = FIRST_ROW ; *x = THIRD_COL ; break ;
  case CHAR_4 : *y = SECOND_ROW ; *x = FIRST_COL ; break ;
  case CHAR_5 : *y = SECOND_ROW ; *x = SECOND_COL ; break ;
  case CHAR_6 : *y = SECOND_ROW ; *x = THIRD_COL ; break ;
  case CHAR_7 : *y = THIRD_ROW ; *x = FIRST_COL ; break ;
  case CHAR_8 : *y = THIRD_ROW ; *x = SECOND_COL ; break ;
  /* Use default to catch any unexpected input. */
  default :  fprintf(stderr, MANHATTAN_TILE_ERROR) ;
  exit(EXIT_FAILURE) ; 
  }

  return ; 
}

/* Priority used to determine new boards place in queue. */
int determine_place(int man_num, Puzzle *start, \
int counter)
{
  unsigned int place ; 
  Puzzle *test ; 
  
  /* Cannot have a place in the queue before parent board.*/
  place = counter ;
  
  pointer_chase(&start, &test, counter) ;

  /* The lower the man_num, the further forward in queue. */
  while(test != NULL){
    if(man_num > test->manhattan){
      place++ ;
    }
    test = test->next ;
  }

  /* New boards place in the queue is one after the count.*/
  return place + 1 ;
}

/* Function used to insert a board into the linked list. */
void insert(char *top_left, Puzzle *start, \
Puzzle *location, int place, int counter, int moves, \
int total)
{
  Puzzle *temp ; 
  
  /* Get us to the position 1 before where we want to go.*/
  pointer_chase(&start, &location, place - 1) ; 
  /* 
   * Store the part of the list that starts in the position
   * we want to move into. Don't want to lose this!
   */
  temp = location->next ; 

  /* Overwrite with new state/info. so in correct place. */ 
  location->next = allocate_space(top_left, counter, total);
    
  /* Move along one. */ 
  location = location->next ; 

  location->moves = moves ; 
  /*
   * Add the start of the list we are storing to end of the
   * list with our new board in (now have whole list again). 
   */ 
  location->next = temp ;
  
  return ; 
}

/* Frees all memory that has been allocated using malloc. */
void free_malloc_memory(Puzzle *start)
{
  Puzzle *clear ; 
  
  while((clear = start) != NULL){
    start = start->next ; 
    
    free(clear) ; 
  }
  
  return ; 
}
