cmake_minimum_required(VERSION 3.15)
project(SDL2_OpenGL_Triangle LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# --- Adjust paths for your installation ---
set(SDL2_DIR "C:/SDL2/x86_64-w64-mingw32")
set(GLEW_DIR "C:/GLEW/glew-2.1.0")

# --- Include and library directories ---
include_directories(
    ${SDL2_DIR}/include
    ${GLEW_DIR}/include
)
link_directories(
    ${SDL2_DIR}/lib
    ${GLEW_DIR}/lib/Release/x64
)

# --- Find OpenGL (system) ---
find_package(OpenGL REQUIRED)



# --- Executable ---
add_executable(${PROJECT_NAME}
    src/main.cpp
)

# --- Link everything ---
target_link_libraries(${PROJECT_NAME}
    ${OPENGL_LIBRARIES}
    mingw32
    SDL2main
    SDL2
    glew32
)

# --- Copy shaders and DLLs after build ---
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    # Copy the shaders folder
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_SOURCE_DIR}/shaders
            $<TARGET_FILE_DIR:${PROJECT_NAME}>/shaders
    # Copy SDL2.dll and glew32.dll
    COMMAND ${CMAKE_COMMAND} -E copy
            ${CMAKE_SOURCE_DIR}/SDL2.dll
            ${CMAKE_SOURCE_DIR}/glew32.dll
            $<TARGET_FILE_DIR:${PROJECT_NAME}>
)
