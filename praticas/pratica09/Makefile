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

$(TARGET): main.c cobertura.c cobertura.h
	$(CC) $(CFLAGS) main.c cobertura.c -o $(TARGET)

run: $(TARGET)
	./$(TARGET)

clean:
	$(REMOVE) $(TARGET)

.PHONY: all run clean
