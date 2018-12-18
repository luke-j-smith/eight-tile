/* 
 * Header file including all display/print related functions
 * for extension of 8-tile. The linked data structure, 
 * Puzzle, is defined on line 74 (updated for Manhattan). 
 */
#include <stdlib.h>
#include "manhattan_large_numbers.h"
#include "neillncurses.h"

/* Errors related to the creation of the display. */ 
#define DISPLAY_ERROR "\nERROR: Unexpected tile position " \
"input when creating large display.\n\n"
#define ENLARGE_ERROR "\nERROR: Unexpected tile input " \
"when enlarging the array for moving display.\n\n"

/* The maximum numbers of moves is 9! = 362880. */ 
#define Q_MAX 362880

/* Solution is located at place (counter - 1) in queue. */  
#define SOLUTION_PLACE(counter) (counter - 1)

#define ROOT_NODE -1

/* As starting board is printed, num. moves is one less. */ 
#define MOVES(prints) (prints - 1)

/* Array starts at [0], therefore +1 needed to reach XS. */ 
#define END_OF_ROW(i) (i + 1)

/* Defines the colour scheme to be used with ncurses. */ 
#define NC_NUMBER_TILES "12345678"
#define NC_BLANK_TILE " "
#define NC_BOARDER "/"
#define NC_SENTENCE_SPACE "!"
#define NC_TILE_BACK "Y"

/* Text used in into. Plus the length and heigh of each. */ 
#define INTRO1 "\n\n\n!!!The!puzzle!is!solvable!:)"
#define INTRO2 "\n\n\n!!!The!starting!board!is:"
#define INTRO3 "\n\n\n!!!The!solution!is!as!follows:"
#define LNGTH1 31
#define LNGTH2 28
#define LNGTH3 33
#define L_HEIGHT 1

/* Specifies how long each display is kept on screen. */
#define ONE_SECOND 1000
#define TWO_SECONDS 2000
/*
 * Used to define size of the large display: 
 * Boarder around edge of board - width of 3; 
 * Boarder between tiles - width of 2; 
 * Tile size - 9 by 9; 
 * Total displace width/height - 3 tiles, 2 outer boarders 
 * and 2 inner boarders; and 
 * Boarder character - '/' to match ncurses colour scheme.
 */
#define SIDE_GAP 3
#define TILE_GAP 2
#define TILE_SIZE 9
#define DISP_SIZE ((XS*TILE_SIZE) + (2*SIDE_GAP) \
+ ((XS - 1)*TILE_GAP))
#define BOARDER '/'

/* Defines correct tile pos. when creating large display. */ 
#define THIRDS(n) (SIDE_GAP + n*(TILE_SIZE + TILE_GAP))
#define TOP_THIRD(n) ((n == 0) || (n == 1) || (n == 2))
#define BOTTOM_THIRD(n) ((n == 6) || (n == 7) || (n == 8))
#define TP 0
#define MDL 1
#define BTM 2

/* Puzzle: Dynamic data structure used throughout program.*/
struct puzzle{
  char board[YS][XS] ;
  int parent ;
  int moves ; 
  int manhattan ; 
  struct puzzle *next ; 
} ;
typedef struct puzzle Puzzle ;

void display_solution(Puzzle *start, int counter) ; 
void sort_print_array(Puzzle *start, \
int print_steps[Q_MAX], int counter, int *num) ;
void record_of_solution(Puzzle *start, int *print_steps, \
int steps) ; 
void define_colour_scheme(NCURS_Simplewin *move) ; 
void puzzle_introduction(NCURS_Simplewin *move, \
Puzzle *start) ; 
void make_array_big(char normal[YS][XS], \
char big[DISP_SIZE][DISP_SIZE]) ; 
void solution_graphic(NCURS_Simplewin *move, \
Puzzle *start, int print_steps[Q_MAX], int moves) ;
void moving_display(NCURS_Simplewin *move, Puzzle *start, \
int print_steps) ; 
void determine_tile_position(int k, int *y, int *x) ;
void determine_tile(char change[TILE_SIZE][TILE_SIZE], \
char tile) ; 
void pointer_chase(Puzzle **start, Puzzle **location, \
int displacement) ;


/* Solution displayed with ncurses, with record of steps. */
void display_solution(Puzzle *start, int counter)
{
  NCURS_Simplewin disp ;
  int prints, *steps ;
  
  /* Ensure array big enough for largest number of moves. */
  int print_steps[Q_MAX] = {0} ; 
   
  prints = 0 ; 
  steps = &prints ;  

  sort_print_array(start, print_steps, counter, steps) ;
    
  record_of_solution(start, print_steps, MOVES(prints)) ;

  Neill_NCURS_Init(&disp) ;
    
  define_colour_scheme(&disp) ;
      
  puzzle_introduction(&disp, start) ;
  
  solution_graphic(&disp, start, print_steps, \
  MOVES(prints)) ; 
   
  return ; 
}

/* Fills array with each correct board location in queue. */ 
void sort_print_array(Puzzle *start, \
int print_steps[Q_MAX], int counter, int *steps)
{
  Puzzle *position ; 
  int i, j ;
  
  i = 0 ; 
  j = SOLUTION_PLACE(counter) ; 
  
  /* Fills print_steps array by tracing back from sol. */ 
  do{
    /* Fill array with parent's position in queue. */ 
    print_steps[i] = j ;
    
    /* Pointer chase to parent board location in queue. */ 
    pointer_chase(&start, &position, j) ; 
    
    j = position->parent ;
     
    /* Counter the number of steps  to reach solution. */
    (*steps)++ ;
    
    /* Walk through array, as many times a needed. */ 
    i++ ;
  } while(j != ROOT_NODE) ; 
    
  return ; 
}

/* Print record of steps to solution for the user. */  
void record_of_solution(Puzzle *start, int *print_steps, \
int steps) 
{
  Puzzle *print ;
  char *top_left ;  
  int i, j, track ; 
  
  track = 0 ; 
  
  for(j = steps ; j >= 0 ; j--){
    printf("\nBoard number %d:\n", track) ; 
    
    pointer_chase(&start, &print, *(print_steps + j)) ; 

    top_left = &print->board[0][0] ;
    
    /* Print board. */ 
    for(i = 0 ; i < NUM_TILES ; i++){
    
      printf("%c ", *(top_left + i)) ;
      
      if((END_OF_ROW(i) % XS) == 0){
        printf("\n") ; 
      }
    }
    
    track++ ; 
  }
  
  printf("\nTotal number of moves in solution: %d\n\n", \
  steps) ; 
  
  return ; 
}

void define_colour_scheme(NCURS_Simplewin *move)
{
  /*
   * Defines the text and background colour in display:
   * Numbered tiles - blue number, yellow background; 
   * Blank tile - white;
   * Boarder around tiles - blue; and
   * Spaces between words - black. 
   */   
  Neill_NCURS_CharStyle(move, NC_NUMBER_TILES, COLOR_BLUE, \
  COLOR_BLUE, A_BOLD) ;
  
  Neill_NCURS_CharStyle(move, NC_TILE_BACK, \
  COLOR_YELLOW, COLOR_YELLOW, A_STANDOUT) ;
  
  Neill_NCURS_CharStyle(move, NC_BLANK_TILE, COLOR_WHITE, \
  COLOR_WHITE, A_STANDOUT) ;
  
  Neill_NCURS_CharStyle(move, NC_BOARDER, COLOR_BLUE, \
  COLOR_BLUE, A_STANDOUT) ;
  
  Neill_NCURS_CharStyle(move, NC_SENTENCE_SPACE, \
  COLOR_BLACK, COLOR_BLACK, A_STANDOUT) ;
    
  return ; 
}

/* Introduces the puzzle to the user before it is solved. */ 
void puzzle_introduction(NCURS_Simplewin *move, \
Puzzle *start) 
{
  char intro_1[LNGTH1] = INTRO1 ;
  char intro_2[LNGTH2] = INTRO2 ; 
  char intro_3[LNGTH3] = INTRO3 ;
  char big[DISP_SIZE][DISP_SIZE] ; 
  
  /* Enlarges arrays to make display easier to read. */ 
  make_array_big(start->board, big) ; 
  
  /* Display array to screen, state length and height. */
  Neill_NCURS_PrintArray(intro_1, LNGTH1, L_HEIGHT, move) ;

  /* Wait 1 second. */ 
  Neill_NCURS_Delay(ONE_SECOND) ;

  Neill_NCURS_PrintArray(intro_2, LNGTH2, L_HEIGHT, move) ;

  Neill_NCURS_Delay(ONE_SECOND) ;
  
  /* Displays 2D array, by pointing to the first element. */
  Neill_NCURS_PrintArray(&big[0][0], DISP_SIZE, DISP_SIZE, \
  move) ;
  
  /* Wait 2 secondS. */ 
  Neill_NCURS_Delay(TWO_SECONDS) ;
  
  Neill_NCURS_PrintArray(intro_3, LNGTH3, L_HEIGHT, move) ;

  Neill_NCURS_Delay(ONE_SECOND) ;
  
  return ;  
}

/* Creates a large array from each board for display. */
void make_array_big(char normal[YS][XS], \
char big[DISP_SIZE][DISP_SIZE])
{
  unsigned short i, j, k ;
  int x, y, *x_start, *y_start ;  
  char *tl ;
  char change[TILE_SIZE][TILE_SIZE] ; 

  x_start = &x ;
  y_start = &y ;
  tl = &normal[0][0] ; 
  
  /* Initially fill all big array with background colour.*/ 
  for(j = 0 ; j < DISP_SIZE ; j++){
    for(i = 0 ; i < DISP_SIZE ; i++){
      big[j][i] = BOARDER ; 
    }
  }
  
  /* Tiles put into corresponding position on big board. */
  for(k = 0 ; k < NUM_TILES ; k++){
    determine_tile_position(k, y_start, x_start) ;
    
    /* Determines which tile we wish to enlarge. */ 
    determine_tile(change, *(tl + k)) ;

    /* Put each enlarged tile graphic in correct position.*/  
    for(j = y ; j < y + TILE_SIZE ; j++){      
      for(i = x ; i < x + TILE_SIZE ; i++){
        big[j][i] = change[j - y][i - x] ; 
      }
    }
  } 
  
  return ; 
}

/* Determines the location of each tile on large board. */ 
void determine_tile_position(int k, int *y, int *x)
{
  int i, third ; 
  
  /* Defines horizontal (i) position of tile in display. */ 
  for(i = 0 ; i < YS ; i++){
    if((k - i) % YS == 0){
      *x = THIRDS(i) ; 
    } 
  }
  
  /* Finds which vertical third of the board tile is in. */ 
  TOP_THIRD(k) ? (third = TP) : (third = MDL) ;
  
  if(third == MDL){
    BOTTOM_THIRD(k) ? (third = BTM) : (third = MDL) ;
  }  

  /* Defines vertical (j) position of tile in display. */ 
  switch(third){
    case TP : *y = THIRDS(TP) ; break ; 
    case MDL : *y = THIRDS(MDL) ; break ; 
    case BTM : *y = THIRDS(BTM) ; break ; 
    /* Default used to exit if unexpected input. */  
    default : fprintf(stderr, DISPLAY_ERROR) ;
    exit(EXIT_FAILURE) ; 
  }
  
  return ; 
}

/* Fills 2D array with large graphic the inputted tile. */
void determine_tile(char change[TILE_SIZE][TILE_SIZE], \
char tile) 
{
  /* Predefine large number graphics for each tile. */ 
  int i, j ; 
  char tbnk[TILE_SIZE][TILE_SIZE] = LBNK ; 
  char t1[TILE_SIZE][TILE_SIZE] = L1 ; 
  char t2[TILE_SIZE][TILE_SIZE] = L2 ; 
  char t3[TILE_SIZE][TILE_SIZE] = L3 ; 
  char t4[TILE_SIZE][TILE_SIZE] = L4 ; 
  char t5[TILE_SIZE][TILE_SIZE] = L5 ; 
  char t6[TILE_SIZE][TILE_SIZE] = L6 ; 
  char t7[TILE_SIZE][TILE_SIZE] = L7 ; 
  char t8[TILE_SIZE][TILE_SIZE] = L8 ;
  char fill[TILE_SIZE][TILE_SIZE] ; 
  char *f = NULL, *swap = NULL ; 
  
  f = &fill[0][0] ; 
  
  switch(tile){
    case BLANK : swap = &tbnk[0][0] ; break ; 
    case CHAR_1 : swap = &t1[0][0] ; break ; 
    case CHAR_2 : swap = &t2[0][0] ; break ; 
    case CHAR_3 : swap = &t3[0][0] ; break ; 
    case CHAR_4 : swap = &t4[0][0] ; break ; 
    case CHAR_5 : swap = &t5[0][0] ; break ; 
    case CHAR_6 : swap = &t6[0][0] ; break ; 
    case CHAR_7 : swap = &t7[0][0] ; break ; 
    case CHAR_8 : swap = &t8[0][0] ; break ; 
    /* Catches any error when inputting tiles for display.*/
    default : fprintf(stderr, ENLARGE_ERROR) ;
    exit(EXIT_FAILURE) ;
  }
  
  for(i = 0 ; i < TILE_SIZE*TILE_SIZE ; i++){
    *(f + i) = *(swap + i) ; 
  }

  /* Fill the TILE_SIZE by TILE_SIZE array with graphic. */
  for(j = 0 ; j < TILE_SIZE ; j++){      
    for(i = 0 ; i < TILE_SIZE ; i++){
      change[j][i] = fill[j][i] ; 
    }
  }
  
  return ;  
}

/* Animated step-by-step guide to the solution. */ 
void solution_graphic(NCURS_Simplewin *move, \
Puzzle *start, int print_steps[Q_MAX], int moves)
{
  int i ;
  
  i = moves ;
  
  while((!move->finished) && (i >= 0)){
  
    moving_display(move, start, print_steps[i]) ;
     
    Neill_NCURS_Delay(ONE_SECOND) ; 
    
    /* Wait for mouse click, or ESC, to finish loop. */
    Neill_NCURS_Events(move) ; 
    
    i-- ; 
  } 

  return ; 
}

/* Large visualisation of current the board for each step.*/
void moving_display(NCURS_Simplewin *move, Puzzle *start, \
int print_steps)
{ 
  Puzzle *position ; 
  char big[DISP_SIZE][DISP_SIZE] ; 
    
  pointer_chase(&start, &position, print_steps) ; 
  
  make_array_big(position->board, big) ; 
  
  Neill_NCURS_PrintArray(&big[0][0], DISP_SIZE, DISP_SIZE, \
  move) ;

  return ;  
}
