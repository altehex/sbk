SRC = crc.c high_level.c joint.c low_level.c safety.c syncio.c udp.c

EXAMPLE_SRC = walk.c position.c pushup.c


ifeq (1, $(ARM))
	CC = arm-linux-gnueabi-gcc
	AR = arm-linux-gnueabi-ar
else
	CC = x86_64-linux-gnu-gcc
	AR = x86_64-linux-gnu-gcc-ar
endif

SCP = scp


CFLAGS = -O3 -Wall -Iinclude \
		 -fdata-sections \
		 -ffunction-sections
LDFLAGS = -Wl,--gc-sections \
          -Lbuild

ifeq (1, $(DEBUG))
	CFLAGS += -g -DDEBUG
endif

LIBS = -lsbk -lc -lm -lpthread


all: build build/libsbk.a examples


build:
	mkdir -p build/examples

build/libsbk.a: $(foreach OBJ,$(SRC:%.c=%.o),build/$(OBJ))
	$(AR) rs $@ $^

examples: $(foreach EX,$(EXAMPLE_SRC:%.c=%),build/examples/$(EX))

build/examples/%: examples/%.c
	$(CC) $(CFLAGS) -c $^ -o $(^:%.c=%.o)
	$(CC) $(LDFLAGS) $(^:%.c=%.o) -o $@ $(LIBS)

build/%.o: src/%.c
	$(CC) $(CFLAGS) -c $^ -o $@


send:
	$(SCP) -r ../sbk/ pi@192.168.123.161:/home/pi


clean:
	rm -rf build
	rm -f examples/*.o
	rm -f *~
	rm -f examples/*~
	rm -f src/*~
	rm -f include/sbk/*~

	rm -rf html/ latex/
