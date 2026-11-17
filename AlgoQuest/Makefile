# Optional: run "mingw32-make" from the MSYS2 UCRT64 shell
CC      = gcc
CFLAGS  = -g -std=c11 -Wall -Wextra -O1
SRC     = $(wildcard *.c graphics/*.c structures/*.c)
LDFLAGS = -lraylib -lopengl32 -lgdi32 -lwinmm

AlgoQuest.exe: $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $@ $(LDFLAGS)

clean:
	del AlgoQuest.exe
