CC = gcc
CFLAGS = -Wall -g
TARGET = quick-notes 
SRC = src/main.c src/options.c
OBJ = $(SRC:.c=.o)
LDFLAGS = -lncurses -lcmark-gfm
INCLUDE = -Iinclude


all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) $(INCLUDE) -o $@ $^ $(LDFLAGS)

%.o: %.c 
	$(CC) $(CFLAGS) $(INCLUDE) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)


run: $(TARGET)
	./$(TARGET)
