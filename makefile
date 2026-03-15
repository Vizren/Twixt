CC = gcc
CFLAGS = -std=c99 -Wall -Wextra -O2

OBJ = main.o twixt.o board.o link.o
TARGET = twixt

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJ)

main.o: main.c twixt.h
	$(CC) $(CFLAGS) -c main.c

twixt.o: twixt.c twixt.h board.h link.h
	$(CC) $(CFLAGS) -c twixt.c

board.o: board.c board.h
	$(CC) $(CFLAGS) -c board.c

link.o: link.c link.h
	$(CC) $(CFLAGS) -c link.c

clean:
	rm -f $(OBJ) $(TARGET)

.PHONY: all clean