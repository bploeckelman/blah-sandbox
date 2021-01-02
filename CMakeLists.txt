cmake_minimum_required(VERSION 3.17)
project(blah_sandbox)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#include blah
add_subdirectory(libs/blah)

# add game sources
add_executable(${PROJECT_NAME}
        src/main.cpp
        src/game.cpp
)

# reference blah
target_link_libraries(${PROJECT_NAME} blah)

# copy SDL2 to the build dir
set(SDL2_DLL "" CACHE FILEPATH "SDL2 DLL Path")
if (SDL2_ENABLED)
    add_custom_command(
            TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy ${SDL2_DLL} $<TARGET_FILE_DIR:${PROJECT_NAME}>
    )
endif()