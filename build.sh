#!/bin/sh
COMPILE="clang++ -std=c++20 -O3 -I./dependencies/include -fmodules-ts -fprebuilt-module-path=."
for file in vec2 window
do
    $COMPILE -c -Xclang -emit-module-interface -o "$file.pcm" "$file.cpp"
done
$COMPILE -flto -Wl,-flto=O3 -lglfw -lGL -lGLEW -o main *.cpp
