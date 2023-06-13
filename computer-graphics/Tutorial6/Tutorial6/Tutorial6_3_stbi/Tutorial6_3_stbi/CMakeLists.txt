cmake_minimum_required(VERSION 3.0.0)
project(MAIN_GL)

set(CMAKE_CXX_STANDARD 14)
# Redirect to the `include` folder of your own GLAD
set(GLAD_H /Users/x/glad/include)
set(GLFW_H /usr/local/include)
set(FT_H /usr/local/opt/freetype/include/freetype2)
set(STBI_H ./)
include_directories(${GLAD_H} ${GLFW_H} ${FT_H} ${STBI_H})

# Link the library of GLFW
set(GLFW_LINK /usr/local/lib/libglfw.3.dylib)
set(FT_LINK /usr/local/opt/freetype/lib/libfreetype.dylib)
link_libraries(${GLFW_LINK} ${FT_LINK})

# Link the library of GLFW
set(GLM_H /Users/x/glm-0.9.9.1/glm)
include_directories(${GLM_H})

set(SOURCE_MAIN main.cpp)
# Redirect to the `glad.c` file of your own GLAD
set(SOURCE_GLAD /Users/x/glad/src/glad.c)
# Redirect `main` to the source direction rather than in `build`
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR})
add_executable(main ${SOURCE_MAIN} ${SOURCE_GLAD})

if (APPLE)
    target_link_libraries(main "-framework OpenGL")
endif()

include(CTest)
enable_testing()

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)