
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


# Find each example's main.cpp under src/
file(GLOB EXAMPLE_MAINS CONFIGURE_DEPENDS
    "${PROJECT_SOURCE_DIR}/src/*/main.cpp"
)


foreach(EXAMPLE_MAIN ${EXAMPLE_MAINS})

    # Get example directory
    get_filename_component(EXAMPLE_DIR
        "${EXAMPLE_MAIN}" DIRECTORY
    )

    # Get executable name from folder
    get_filename_component(EXAMPLE_NAME
        "${EXAMPLE_DIR}" NAME
    )

    # Gather all C/C++ sources in that folder
    file(GLOB EXAMPLE_SOURCES CONFIGURE_DEPENDS
        "${EXAMPLE_DIR}/*.cpp"
        "${EXAMPLE_DIR}/*.c"
    )


    # Create executable
    add_executable(${EXAMPLE_NAME}
        ${EXAMPLE_SOURCES}
    )


    # Link raylib
    target_link_libraries(${EXAMPLE_NAME}
        PRIVATE raylib
    )


    # Include project headers
    target_include_directories(${EXAMPLE_NAME}
        PRIVATE ${PROJECT_SOURCE_DIR}/include
    )


    # Define absolute asset directory
    target_compile_definitions(${EXAMPLE_NAME}
        PRIVATE
        ASSET_DIR="${EXAMPLE_DIR}/assets"
    )

endforeach()
