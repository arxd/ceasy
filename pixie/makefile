.DEFAULT_GOAL := all
CFLAGS=-g -DEBUG
$(shell mkdir -p build)

build/client.o: client.c pixie.h util.c
	clang -c -o $@ $< $(CFLAGS)
	
build/share.o: share.c util.c
	clang -c -o $@ $< $(CFLAGS)

build/glhelper.o: glhelper.c util.c
	clang -c -o $@ $< $(CFLAGS)

build/util.o: util.c
	clang -c -o $@ $< $(CFLAGS)
	
build/subproc.o: subproc.c util.c
	clang -c -o $@ $< $(CFLAGS)

build/window_glfw.o: window_glfw.c util.c
	clang -c -o $@ $< $(CFLAGS)
		
build/server.o: server.c subproc.c share.c window_glfw.c glhelper.c util.c iomem.h shaders.h
	clang -c -o $@ $< $(CFLAGS)
	
shaders.h: ccglsl shaders.glsl
	./ccglsl shaders.glsl shaders.h

build/shader_compiler.o: shader_compiler.c util.c
	clang -c -o $@ $< $(CFLAGS)

all: libpixie.a pixie ccglsl

libpixie.a: build/client.o build/util.o
	ar rcs $@ $^

pixie: build/server.o build/subproc.o build/window_glfw.o build/share.o build/util.o build/glhelper.o 
	clang -o $@ $^ -lGLESv2 -lglfw

ccglsl: build/shader_compiler.o build/glhelper.o build/util.o
	clang -o $@ $^ -lGLESv2 -lglfw


.PHONEY: clean all

clean:
	rm -rf build
	rm -f pixie
	rm -f ccglsl
	rm -f libpixie.a
	rm -f shaders.h
	