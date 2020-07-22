CC=g++
CFLAGS=-g -lSDL2
FILES=src/*.cpp
EXEC=build

bleak: $(FILES)
	$(CC) $(FILES) $(CFLAGS) -o $(EXEC)

run: $(EXEC)
	./$(EXEC)
