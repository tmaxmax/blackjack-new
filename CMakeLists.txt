cmake_minimum_required(VERSION 3.16)

project(blackjack)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_EXE_LINKER_FLAGS " -static")

add_executable(${PROJECT_NAME} main.cxx card.cxx player.cxx game.cxx)

target_include_directories(${PROJECT_NAME} PRIVATE ${PROJECT_SOURCE_DIR})
target_link_libraries(${PROJECT_NAME} -static-libgcc -static-libstdc++)

if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /W4 /WX)
else()
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -pedantic -Werror)
endif()



