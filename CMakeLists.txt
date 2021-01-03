cmake_minimum_required(VERSION 3.17)
project(blah_sandbox)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# configure and include SDL to build from git submodule sources in /lib on our dev platform
set(SDL2_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/libs/sdl/include)
add_subdirectory(libs/sdl)

# include blah
add_subdirectory(libs/blah)

# add game sources
add_executable(${PROJECT_NAME}
        src/main.cpp
        src/game.cpp
        src/world.cpp
        src/content.cpp
        src/components/animator.cpp
        src/components/collider.cpp
        src/components/player.cpp
        src/components/mover.cpp
)

# reference blah and SDL
# NOTE: without linking SDL here we get unresolved externals during the link step for blah
target_link_libraries(${PROJECT_NAME} blah SDL2)

# copy SDL2 to the build dir
# TODO: don't think this is working correctly, need to determine dll name based on target m_type?
# NOTE: copying it from <target-build-dir>/lib/sdl to <target-build-dir> manually works for now
#set(SDL2_DLL "" CACHE FILEPATH "SDL2 DLL Path")
#if (SDL2_ENABLED)
#    add_custom_command(
#            TARGET ${PROJECT_NAME} POST_BUILD
#            COMMAND ${CMAKE_COMMAND} -E copy ${SDL2_DLL} $<TARGET_FILE_DIR:${PROJECT_NAME}>
#    )
#endif()