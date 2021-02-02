#!/bin/sh

clang++ -o main *.cpp -lglfw -lGL -lGLEW -I./dependencies/include -std=c++20 -O3 -flto -Wl,-flto=O3
