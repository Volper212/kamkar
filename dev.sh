#!/bin/sh

clang++ -o main *.cpp -lglfw -lGL -I./dependencies/include -std=c++20 && (./main ; rm main)
