#!/bin/sh
COMPILE="clang++ -std=c++20 -D _DEBUG=1 -I./dependencies/include -fmodules-ts -fprebuilt-module-path=."
for file in vec2 window
do
    $COMPILE -c -Xclang -emit-module-interface -o "$file.pcm" "$file.cpp"
done
$COMPILE -lglfw -lGL -lGLEW -o main *.cpp && (./main ; rm main)
