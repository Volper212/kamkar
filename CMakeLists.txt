cmake_minimum_required(VERSION 3.20)
project(kamkar)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(PREBUILT_MODULE_PATH ${CMAKE_BINARY_DIR}/modules)

function(add_module name)
    file(MAKE_DIRECTORY ${PREBUILT_MODULE_PATH})
    add_custom_target(${name}.pcm
            COMMAND
                ${CMAKE_CXX_COMPILER}
                -std=c++20
                -stdlib=libc++
                -fmodules
                -c
                -fmodule-map-file=../module.modulemap
                -fprebuilt-module-path=${PREBUILT_MODULE_PATH}
                -Xclang -emit-module-interface
                -o ${PREBUILT_MODULE_PATH}/${name}.pcm
                ${CMAKE_CURRENT_SOURCE_DIR}/${ARGN}
            )
endfunction()

add_compile_options(-stdlib=libc++)
add_compile_options(-fmodules)
add_compile_options(-fmodule-map-file=../module.modulemap)
add_compile_options(-fprebuilt-module-path=${PREBUILT_MODULE_PATH})
add_link_options(-lGLEW)
add_link_options(-lGL)
add_link_options(-lglfw)
include_directories(dependencies/include)

if(CMAKE_BUILD_TYPE)
    add_compile_options(-O3)
    add_link_options(-flto)
    add_link_options(-Wl,-flto=O3)
else()
    add_compile_options(-D _DEBUG=1)
endif()

add_module(vec2 vec2.cpp)
add_module(window window.cpp)

add_executable(main
    vec2.cpp
    window.cpp
    main.cpp
    graphics.cpp
    noise.cpp
    stb_image.cpp
    tilemap.cpp
)

add_dependencies(main vec2.pcm)
add_dependencies(main window.pcm)
