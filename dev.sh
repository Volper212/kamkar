#!/bin/sh

clang++ -o main *.cpp -lglfw -lGL -lGLEW -I./dependencies/include -std=c++20 -D _DEBUG=1 && (./main ; rm main)
