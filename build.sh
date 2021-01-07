#!/bin/sh

clang++ -o main *.cpp -lglfw -lGL -I./dependencies/include -std=c++20 -O3 -flto -Wl,-flto=O3
