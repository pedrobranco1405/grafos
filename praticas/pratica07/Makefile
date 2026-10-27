CC = gcc
CFLAGS = -Wall -Wextra -Werror -std=c11

ifeq ($(OS),Windows_NT)
TARGET = main.exe
REMOVE = cmd /C del /Q
else
TARGET = main
REMOVE = rm -f
endif

all: $(TARGET)

$(TARGET): main.c bellman_ford.c bellman_ford.h
	$(CC) $(CFLAGS) main.c bellman_ford.c -o $(TARGET)

run: $(TARGET)
	./$(TARGET)

clean:
	$(REMOVE) $(TARGET)

.PHONY: all run clean
