CC = gcc
CFLAGS = -Wall -Wextra -Werror -std=c11
TARGET = main
SRC = main.c grafo_matriz.c grafo_lista.c
HEADERS = grafo_matriz.h grafo_lista.h

ifeq ($(OS),Windows_NT)
TARGET = main.exe
endif

all: $(TARGET)

$(TARGET): $(SRC) $(HEADERS)
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET) $(LDFLAGS) $(LDLIBS)

run: $(TARGET)
	./$(TARGET)

clean:
ifeq ($(OS),Windows_NT)
	powershell -NoProfile -Command "if (Test-Path -LiteralPath '$(TARGET)') { Remove-Item -LiteralPath '$(TARGET)' -Force }"
else
	$(RM) $(TARGET)
endif

.PHONY: all run clean
