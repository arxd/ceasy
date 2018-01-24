#!/bin/sh

clang server.c window_glfw.c glhelper.c -lGLESv2 -lglfw -o server
clang client.c -o client
