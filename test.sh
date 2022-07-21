COMPILE="clang++ -std=c++20 -stdlib=libc++ -D _DEBUG=1 -I./dependencies/include -fmodules -fprebuilt-module-path=. -fmodule-map-file=module.modulemap"
for file in vec2 window tilemap
do
    $COMPILE -c -Xclang -emit-module-interface -o "$file.pcm" "$file.cpp"
done
$COMPILE -lglfw -lGL -lGLEW -o test *.test.cpp && (./test ; rm test);
