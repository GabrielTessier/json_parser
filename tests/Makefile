CC = gcc
CFLAGS = -Wall -Wextra -g -O2 -I../include
# truc cracra pour récupérer le chemin absolu vers le répertoire lib/
LIBDIR = $(shell pwd)/../lib/
LDFLAGS = -Wl,-rpath=$(LIBDIR) -L$(LIBDIR) -ljson

TESTS = json-01-test
SRC = $(TESTS:=.c)
OBJ = $(TESTS:=.o)

all: $(TESTS)

%-test: %.o
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $^

.PHONY: clean

clean:
	rm -rf *.o *~ $(TESTS)

