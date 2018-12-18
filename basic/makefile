TARGET1 = eighttile
TARGET2 = file_operations
TARGET3 = display_operations
TARGET4 = large_numbers
TARGET5 = neillncurses

COMP = gcc
CFLAGS = -pedantic -Wall -Wextra -Wfloat-equal -ansi -O2
CLIBS = -lncurses -lm

$(TARGET1) : $(TARGET1).c $(TARGET2).h $(TARGET3).h $(TARGET4).h $(TARGET5).h

				$(COMP) $(TARGET1).c -o $(TARGET1) $(CFLAGS) $(CLIBS)

clean :
				rm -f $(TARGET1)

run :
				./$(TARGET1) $(BOARD)
