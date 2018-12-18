/* 
 * The 8-tile Puzzle. Includes main() function. 
 * NB: Linked data structure, Puzzle, is defined in the 
 * display_operations.h file. 
 */
#include <stdio.h>
#include <stdlib.h>
#include "file_operations.h"
#include "display_operations.h"

/* Error messages presented when unexpected behaviour. */ 
#define ARGC_ERROR "\nERROR: Unexpected input.\nPlease " \
"try something like: %s 8-Puzzle.txt\n\n"
#define UNSOLVABLE_BOARD_ERROR "\nERROR: Unfortunately, " \
"this board is unsolvable :(\nTry another!\n\n"
#define MEMORY_ERROR "\nERROR: Unable to allocate space " \
"for new board\n\n."
#define CHECK_ERROR "\nERROR: Unexpected input for choice "\
"of test.\nChoice input should either be 's' to test if " \
"solution, or 'e' to test if board already exists.\n\n"
#define MOVE_ERROR "\nERROR: Unexpected move.\n\n"

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

#define END_OF_QUEUE(queue_place) (queue_place + 1)

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

enum compass {north, east, south, west} ; 
typedef enum compass compass ;

void puzzle_from_file(char *argv, Puzzle **start, \
Puzzle **current, int queue_place) ;
Puzzle *allocate_space(char *top_left, int counter) ; 
void find_optimal_solution(Puzzle *start, Puzzle *current, \
int *qp) ;
void pointer_chase(Puzzle **start, Puzzle **location, \
int displacement) ;
void define_options_and_explore(Puzzle *start, \
Puzzle *current, int counter, int *qp, int *soln) ; 
void determine_search_directions(int tile, int *direction) ; 
void search_possible_directions(int tile, int *direction, \
Puzzle *start, Puzzle *current, int counter, int *qp, \
int *soln) ;
void create_next_board(Puzzle copy, int counter, int tile, \
int direction, Puzzle *start, int *qp, int *soln) ; 
void make_move(Puzzle *copy, int counter, int tile, \
int dirctn) ; 
int check_if_board(char choice, Puzzle *current, \
Puzzle *start, int place) ;
int test_against(Puzzle *test, char against[YS][XS], \
int upper_bound) ;
void free_malloc_memory(Puzzle *start) ;


int main(int argc, char *argv[])
{
  Puzzle *start = NULL, *current = NULL ;
  int queue_place, *qp ;

  /* Used to keep track of total number of unique boards. */ 
  queue_place = 0 ; 
  qp = &queue_place ; 
  
  /* If we exit() anywhere in code, call this function. */
  atexit(Neill_NCURS_Done) ;   

  /* Ensure that file is entered properly on command line.*/
  if(argc == 2){
      
    puzzle_from_file(argv[1], &start, &current, \
    queue_place) ;
    
    /* Define the starting board as not having a parent. */
    start->parent = ROOT_NODE ; 
              
    find_optimal_solution(start, current, qp) ; 

    display_solution(start, queue_place) ;
    
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
Puzzle **current, int queue_place) 
{
  FILE *et = NULL ;
  char *fb, first_board[YS][XS] = NULL_BOARD ;
  
  fb = &first_board[0][0] ;
  
  open_file(&et, argv) ; 
  
  check_file_and_fill_first_board(et, fb, argv) ; 
    
  close_file(&et, argv) ;
        
  *start = *current = allocate_space(fb, queue_place) ;
    
  return ; 
}

/* Allocates space in memory for new board. */  
Puzzle *allocate_space(char *top_left, int counter)
{
  Puzzle *new = NULL;
  int i ;
  char *fill ;
  
  new = (Puzzle *)malloc(sizeof(Puzzle)) ;
  
  fill = &new->board[0][0] ; 
    
  if(new == NULL){
    fprintf(stderr, MEMORY_ERROR) ; 
    exit(EXIT_FAILURE) ; 
  }
        
  for(i = 0 ; i < NUM_TILES ; i++){
    *(fill + i) = *(top_left + i) ; 
  }
  
  new->parent = counter ; 
  
  new->next = NULL ;
      
  return new ; 
}

/* Finds the optimum solution for a solvable puzzle. */ 
void find_optimal_solution(Puzzle *start, Puzzle *current, \
int *qp) 
{ 
  /* Used to keep track of which board in queue we are at.*/  
  static int counter =  0 ;
  
  /* Lets us test for solution when we create a new board.*/
  int correct_board, *soln ;
  
  correct_board = FALSE ; 
  soln = &correct_board ; 
    
  /* 
   * Checks if any 'child' board created in previous 
   * iterations is solutions. Continues if not.  
   */
  while(correct_board != TRUE){
    pointer_chase(&start, &current, counter) ; 

    /* Incr counter after fctn called to move along queue.*/  
    define_options_and_explore(start, current, counter++, \
    qp, soln) ;
     
    /* Unsolvable if we cannot find solution in queue. */      
    if(counter == END_OF_QUEUE(*qp)){
      fprintf(stderr, UNSOLVABLE_BOARD_ERROR) ; 
      exit(EXIT_FAILURE) ; 
    }
  }
    
  return ; 
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
Puzzle *current, int counter, int *qp, int *soln) 
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
      start, current, counter, qp, soln) ;   
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
Puzzle *start, Puzzle *current, int counter, int *qp, \
int *soln)
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
      start, qp, soln) ; 
    }
  }
  
  return ; 
}

/* Creates next board and checks whether already exists. */
void create_next_board(Puzzle copy, int counter, int tile, \
int direction, Puzzle *start, int *qp, int *soln)
{
  Puzzle *potential = NULL;
  char *top_left ;

  top_left = &copy.board[0][0] ; 

  /* Create new board by making appropriate move. */
  make_move(&copy, counter, tile, direction) ; 
  
  potential = &copy ; 
  
  /* If new board does not exist, add to back of queue. */  
  if(!check_if_board(ALREADY_EXISTS, potential, start, \
  *qp)){
    pointer_chase(&start, &potential, (*qp)++) ; 
    
    potential->next = allocate_space(top_left, counter) ;
    
    potential = potential->next ;
    
    /* Flag if new board is solution, then we can stop. */
    *soln = check_if_board(IS_SOLUTION, potential, start, \
    *qp) ; 
    
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
