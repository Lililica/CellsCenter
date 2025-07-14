# ## -----------------------------------------------------------------------
# ##
# ## If you want to learn CMake:
# ## https://julesfouchy.github.io/Learn--Clean-Code-With-Cpp/lessons/cmake/
# ##
# ## If you want to see how to add libraries to your project:
# ## https://julesfouchy.github.io/Learn--Clean-Code-With-Cpp/lessons/use-libraries/#how-to
# ##
# ## -----------------------------------------------------------------------

cmake_minimum_required(VERSION 3.20)

# You can set the name of your project here
project(DelaunayCVT)
add_executable(${PROJECT_NAME})

# Choose your C++ version
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_20)
target_compile_definitions(${PROJECT_NAME} PRIVATE CMAKE_SOURCE_DIR=\\"${CMAKE_SOURCE_DIR}\\") # For M_PI

# Enable many good warnings.
# /WX and -Werror enable "warnings as errors": This means that your code won't compile if you have any warning.
# This forces you to take warnings into account, which is a good practice because warnings are here for a reason and can save you from a lot of bugs!
# If this is too strict for you, you can remove /WX and -Werror.
if(MSVC)
# target_compile_options(${PROJECT_NAME} PRIVATE /WX /W4)
else()
    # target_compile_options(${PROJECT_NAME} PRIVATE -Werror -Wall -Wextra -Wpedantic -pedantic-errors -Wimplicit-fallthrough)
endif()

# Add Eigen3 library
include(FetchContent)


FetchContent_Declare(
    eigen
    GIT_REPOSITORY https://gitlab.com/libeigen/eigen.git
    GIT_TAG 3.4.0
    GIT_SHALLOW TRUE
)

FetchContent_MakeAvailable(Eigen)
target_link_libraries(${PROJECT_NAME} PRIVATE Eigen3::Eigen)

# Set the folder where the executable is created
set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/${CMAKE_BUILD_TYPE})

# Prevents compiler-specific extensions to C++ because they might allow code to compile on your machine but not on other people's machine
set_target_properties(${PROJECT_NAME} PROPERTIES
    CXX_EXTENSIONS OFF)

# Add all the source files (only .cpp, .c, and .cu)
file(GLOB_RECURSE MY_SOURCES CONFIGURE_DEPENDS src/*.cpp src/*.c src/*.cu)
target_sources(${PROJECT_NAME} PRIVATE ${MY_SOURCES})
target_include_directories(${PROJECT_NAME} PRIVATE src)


# Add quick-imgui library
add_subdirectory(lib/quick_imgui)
target_link_libraries(${PROJECT_NAME} PRIVATE quick_imgui::quick_imgui)

# Link OpenGL on Windows
if (WIN32)
    target_link_libraries(${PROJECT_NAME} PRIVATE opengl32)
endif()

target_compile_definitions(${PROJECT_NAME} PUBLIC SHADERS_PATH="../../src/shader/")
target_compile_definitions(${PROJECT_NAME} PUBLIC ASSETS_PATH="../../assets/")

# Enable CUDA
find_package(CUDA)

if(CUDA_FOUND)
    enable_language(CUDA)
    message(STATUS "CUDA found: ${CUDA_VERSION}")
    # Add CUDA source files if you have any, e.g. src/my_kernel.cu
    # file(GLOB_RECURSE CUDA_SOURCES CONFIGURE_DEPENDS src/*.cu)
    # target_sources(${PROJECT_NAME} PRIVATE ${CUDA_SOURCES})
    set_target_properties(${PROJECT_NAME} PROPERTIES CUDA_SEPARABLE_COMPILATION ON)
else()
    message(WARNING "CUDA not found. CUDA support will be disabled.")
endif()



# include(ExternalProject)


# set(EIGEN_INSTALL_DIR "${CMAKE_CURRENT_SOURCE_DIR}/lib/eigen/eigen-install")

# ExternalProject_Add(
#     eigen
#     DOWNLOAD_EXTRACT_TIMESTAMP ON
#     URL  https://gitlab.com/libeigen/eigen/-/archive/3.4.0/eigen-3.4.0.tar.gz
#     SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/lib/eigen/eigen-src"
#     BINARY_DIR "${CMAKE_CURRENT_SOURCE_DIR}/lib/eigen/eigen-build"
#     INSTALL_DIR "${EIGEN_INSTALL_DIR}"
#     CMAKE_ARGS
#     -DCMAKE_INSTALL_PREFIX:PATH=<INSTALL_DIR>
#     -DCMAKE_BUILD_TYPE=Release
# )

# file(MAKE_DIRECTORY ${EIGEN_INSTALL_DIR}/include)  # avoid race condition

# add_library(eigenlib INTERFACE IMPORTED GLOBAL)
# add_dependencies(eigenlib eigen)
# target_compile_features(eigenlib INTERFACE cxx_std_14)

# set_target_properties(eigenlib PROPERTIES
#   INTERFACE_INCLUDE_DIRECTORIES ${EIGEN_INSTALL_DIR}/include/eigen3
# )

# # Add eigen as a dependency to your main project
# add_dependencies(${PROJECT_NAME} eigenlib)
# # Link eigenlib to your main project
# target_link_libraries(${PROJECT_NAME} PRIVATE eigenlib)
# # Add the include directory for eigen
# target_include_directories(${PROJECT_NAME} PRIVATE ${EIGEN_INSTALL_DIR}/include/eigen3)


