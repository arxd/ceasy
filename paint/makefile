.DEFAULT_GOAL := paint
CFLAGS=-g -DEBUG -I../pixie
LFLAGS=-L../pixie -lpixie
$(shell mkdir -p build)

build/main.o: main.c
	clang -c -o $@ $< $(CFLAGS)

paint: build/main.o
	clang -o $@ $^ $(LFLAGS)

.PHONY: clean

clean:
	rm -rf build
	rm -f paint
	



