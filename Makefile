SRC = high_level.c udp.c

EXAMPLE_SRC = walk.c

CC = gcc
AR = ar

CFLAGS = -O3 -Wall -Iinclude -g -fopt-info-all
LDFLAGS = -Wl,--gc-sections


all: build build/libsbk.a examples

build:
	mkdir -p build/examples

build/libsbk.a: $(foreach OBJ,$(SRC:%.c=%.o),build/$(OBJ))
	$(AR) rs $@ $^

examples: $(foreach EX,$(EXAMPLE_SRC:%.c=%),build/examples/$(EX))

build/examples/%: examples/%.c
	$(CC) $(CFLAGS) -c $^ -o $(^:%.c=%.o) &> gcc.log
	$(CC) $(LDFLAGS) -Lbuild  $(^:%.c=%.o) -o $@ -lsbk -lc -lm

build/%.o: src/%.c
	$(CC) $(CFLAGS) -c $^ -o $@


clean:
	rm -rf build
	rm -f *~
	rm -f examples/*~
	rm -f src/*~
	rm -f include/*~
