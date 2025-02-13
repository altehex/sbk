SRC = high_level.c udp.c

EXAMPLE_SRC = walk.c # position.c

CC = gcc
AR = ar

CFLAGS = -O3 -Wall -Iinclude -g
LDFLAGS = -Wl,--gc-sections

LIBS = -lsbk -lc -lm


all: build build/libsbk.a examples

build:
	mkdir -p build/examples

build/libsbk.a: $(foreach OBJ,$(SRC:%.c=%.o),build/$(OBJ))
	$(AR) rs $@ $^

examples: $(foreach EX,$(EXAMPLE_SRC:%.c=%),build/examples/$(EX))

build/examples/%: examples/%.c
	$(CC) $(CFLAGS) -c $^ -o $(^:%.c=%.o)
	$(CC) $(LDFLAGS) -Lbuild $(^:%.c=%.o) -o $@ $(LIBS)

build/%.o: src/%.c
	$(CC) $(CFLAGS) -c $^ -o $@


clean:
	rm -rf build
	rm -f *~
	rm -f examples/*~
	rm -f src/*~
	rm -f include/sbk/*~
