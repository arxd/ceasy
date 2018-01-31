.DEFAULT_GOAL := mei
CFLAGS=-g -DEBUG -I../pixie
LFLAGS=-L../pixie -lpixie
$(shell mkdir -p build)

build/mei.o: mei.c
	clang -c -o $@ $< $(CFLAGS)

mei: build/mei.o
	clang -o $@ $^ $(LFLAGS)

.PHONY: clean

clean:
	rm -rf build
	rm -f mei
	



