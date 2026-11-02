
cmake_minimum_required(VERSION 3.20)

project(RaylibExamples LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(FetchContent)

FetchContent_Declare(
    raylib
    GIT_REPOSITORY https://github.com/raysan5/raylib.git
    GIT_TAG 5.5
)

FetchContent_MakeAvailable(raylib)

# Find all example main.cpp files
file(GLOB_RECURSE EXAMPLE_FILES
    CONFIGURE_DEPENDS
    "${PROJECT_SOURCE_DIR}/src/*/main.cpp"
)

foreach(EXAMPLE_FILE ${EXAMPLE_FILES})

    # Get example directory name
    get_filename_component(EXAMPLE_NAME
        "${EXAMPLE_FILE}" DIRECTORY
    )

    get_filename_component(EXAMPLE_NAME
        "${EXAMPLE_NAME}" NAME
    )

    # Create separate executable
    add_executable(${EXAMPLE_NAME}
        "${EXAMPLE_FILE}"
    )

    target_link_libraries(${EXAMPLE_NAME}
        PRIVATE raylib
    )

    target_include_directories(${EXAMPLE_NAME}
        PRIVATE ${PROJECT_SOURCE_DIR}/include
    )

endforeach()
