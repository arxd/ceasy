.DEFAULT_GOAL := all
CFLAGS=-g -DEBUG
$(shell mkdir -p build)

build/client.o: client.c share.c util.c
	clang -c -o $@ $< $(CFLAGS)
	
build/share.o: share.c util.c
	clang -c -o $@ $< $(CFLAGS)

build/glhelper.o: glhelper.c
	clang -c -o $@ $< $(CFLAGS)

build/util.o: util.c
	clang -c -o $@ $< $(CFLAGS)
	
build/subproc.o: subproc.c
	clang -c -o $@ $< $(CFLAGS)

build/window_glfw.o: window_glfw.c util.c
	clang -c -o $@ $< $(CFLAGS)
	
build/server.o: server.c subproc.c share.c window_glfw.c glhelper.c util.c
	clang -c -o $@ $< $(CFLAGS)

all: client server;

client: build/client.o build/share.o build/util.o
	clang -o $@ $^ 
	
server: build/server.o build/subproc.o build/window_glfw.o build/share.o build/util.o build/glhelper.o
	clang -o $@ $^ -lGLESv2 -lglfw

.PHONEY: clean

clean:
	rm -rf build
	rm -f server
	rm -f client
	