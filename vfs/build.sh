#!/bin/sh

clang server.c util.c window_glfw.c glhelper.c -lGLESv2 -lglfw -o server
clang client.c -o client
