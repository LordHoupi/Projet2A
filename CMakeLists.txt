cmake_minimum_required(VERSION 3.26)
project(Projet2A C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake_modules)


set(SDL2_PATH "C:/SDL2/SDL2-2.30.9")
set(SDL2_INCLUDE_DIR "${SDL2_PATH}/x86_64-w64-mingw32/include")
set(SDL2_LIBRARY "${SDL2_PATH}/x86_64-w64-mingw32/bin/SDL2.dll")

# Inclure SDL
find_package(SDL2 REQUIRED)
include_directories(C:/SDL2/SDL2-2.30.9/x86_64-w64-mingw32/include/SDL2)
include_directories(C:/SDL2/SDL2_image-2.8.2/x86_64-w64-mingw32/include)
include_directories(C:/SDL2/SDL2_ttf-2.22.0/x86_64-w64-mingw32/include/SDL2)
include_directories(external/cjson)

# Lier les bibliothèques SDL nécessaires
file(GLOB SOURCES "src/*.c")
add_executable(Projet2A ${SOURCES})

target_link_libraries(Projet2A PRIVATE
        C:/SDL2/SDL2-2.30.9/x86_64-w64-mingw32/lib/libSDL2main.a
        C:/SDL2/SDL2-2.30.9/x86_64-w64-mingw32/lib/libSDL2.dll.a
        C:/SDL2/SDL2_image-2.8.2/x86_64-w64-mingw32/lib/libSDL2_image.dll.a
        C:/SDL2/SDL2_ttf-2.22.0/x86_64-w64-mingw32/lib/libSDL2_ttf.dll.a
)