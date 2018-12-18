/* Header including all file related functions for 8-tile.*/
#include <stdlib.h>

/* Errors messages related to file operations abd content.*/
#define ERROR_OPENING_FILE "\nERROR: Cannot open file: " \
"%s.\nPlease try again.\n\n"
#define FILE_CONTENT_ERROR "\nERROR: Unexpected character "\
"representing a board tile found in file.\n"
#define DUPLICATE_MISSING_CHAR_ERROR "\nERROR: There " \
"should be exactly one tile of each kind in the board.\n" \
"Ie, only one of the following: ' ', '1', '2', '3', '4', " \
"'5', '6','7', '8'.\n\n"
#define ERROR_CLOSING_FILE "\nERROR: Cannot close file: "\
"%s.\nPlease try again.\n\n"

/* Height, width and total number of tiles. */
#define YS 3
#define XS 3
#define NUM_TILES (YS * XS)

/* Total number of tiles not including the blank tile. */
#define NUM_WO_BLANK (NUM_TILES - 1)

/* Blank tile defined as space character. */ 
#define BLANK ' ' 

/* Ignore EOF, newline and return characters in file! */ 
#define IGNORED_CHAR(c) ((c == EOF) || (c == '\n') \
|| (c == '\r'))

/* Char and number representations used to check file. */ 
#define TILE_BLANK 0
#define TILE_1 1
#define TILE_2 2
#define TILE_3 3
#define TILE_4 4
#define TILE_5 5
#define TILE_6 6
#define TILE_7 7
#define TILE_8 8
#define CHAR_1 '1'
#define CHAR_2 '2'
#define CHAR_3 '3'
#define CHAR_4 '4'
#define CHAR_5 '5'
#define CHAR_6 '6'
#define CHAR_7 '7'
#define CHAR_8 '8'

void open_file(FILE **et, char *argv) ; 
void check_file_and_fill_first_board(FILE *et, \
char *board, char *argv) ; 
void characters_from_file_count(char c, \
int correct_chars_check[NUM_TILES], FILE *et, char *argv) ; 
void check_starting_board( \
int correct_chars_check[NUM_TILES], FILE *et, char *argv) ; 
void close_file(FILE **et, char *argv) ; 


void open_file(FILE **et, char *argv)
{ 
  if((*et = fopen(argv, "r")) == NULL){
    fprintf(stderr, ERROR_OPENING_FILE, argv) ; 
    exit(EXIT_FAILURE) ; 
  }
  
  else{
    printf("\nFile: %s has been successfully opened.\n", \
    argv) ;      
  }
   
  return ; 
}

/* Contains functions that check the validity of the file.*/
void check_file_and_fill_first_board(FILE *et, \
char *board, char *argv)
{
  int i ; 
  int correct_chars_check[NUM_TILES] = {0} ; 
  char c = '\0' ;
  
  i = 0 ; 
  
  while((c = getc(et)) != EOF){
    /* When setting up 1st board, ignore EOF, \n and \r. */
    if(!IGNORED_CHAR(c)){
      /* Used to ensure file contains the correct chars. */ 
      characters_from_file_count(c, correct_chars_check, \
      et, argv) ;
      
      *(board + i) = c ; 
      
      i++ ;  
    }
  }
  
  check_starting_board(correct_chars_check, et, argv) ; 
  
  return ;
}

/* Totals the number of each 'correct' character in file. */
void characters_from_file_count(char c, \
int correct_chars_check[NUM_TILES], FILE *et, char *argv) 
{ 
  switch(c){
  case BLANK : correct_chars_check[TILE_BLANK]++ ; break ; 
  case CHAR_1 : correct_chars_check[TILE_1]++ ; break ;
  case CHAR_2 : correct_chars_check[TILE_2]++ ; break ;
  case CHAR_3 : correct_chars_check[TILE_3]++ ; break ;
  case CHAR_4 : correct_chars_check[TILE_4]++ ; break ;
  case CHAR_5 : correct_chars_check[TILE_5]++ ; break ;
  case CHAR_6 : correct_chars_check[TILE_6]++ ; break ;
  case CHAR_7 : correct_chars_check[TILE_7]++ ; break ;
  case CHAR_8 : correct_chars_check[TILE_8]++ ; break ;
  
  /* Default case is used to catch any incorrect chars. */
  default :  fprintf(stderr, FILE_CONTENT_ERROR \
  "Character: '%c' should not be in the file. Please " \
  "remove and try again.\n\n", c) ;
  close_file(&et, argv) ; 
  exit(EXIT_FAILURE) ; 
  }

  return ; 
}

/* Ensures exactly one of each correct char in file. */
void check_starting_board( \
int correct_chars_check[NUM_TILES], FILE *et, char *argv)
{
  int i ; 

  for(i = 0 ; i < NUM_TILES ; i++){
    if(correct_chars_check[i] != 1){
      fprintf(stderr, DUPLICATE_MISSING_CHAR_ERROR) ;
      close_file(&et, argv) ; 
      exit(EXIT_FAILURE) ; 
    }
  }

  return ; 
}

void close_file(FILE **et, char *argv)
{
  if(fclose(*et) != 0){
    fprintf(stderr, ERROR_CLOSING_FILE, argv) ;
    exit(EXIT_FAILURE) ; 
  } 
    
  else{
    printf("\nFile: %s has been successfully closed.\n", \
    argv) ;      
  }

  return ; 
}
