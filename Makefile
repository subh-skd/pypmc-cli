CC ?= gcc
CFLAGS ?= -Wall -Wextra -O2 -std=c99

SRCS = src/main.c src/config.c src/venv.c src/core.c
TARGET = pypmc

ifeq ($(OS),Windows_NT)
    TARGET := $(TARGET).exe
    CFLAGS += -D_CRT_SECURE_NO_WARNINGS
endif

$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -f $(TARGET) pypmc.exe

.PHONY: clean
