# Include the local .env file
include(./.env.cmake OPTIONAL RESULT_VARIABLE LOCAL_ENV)
message(STATUS "Local .env file included successfully.")

# Include the local .env file
include(./.build.cmake OPTIONAL RESULT_VARIABLE LOCAL_BUILD)
message(STATUS "Local .build file included successfully.")

# Set the minumum required version
cmake_minimum_required(VERSION ${CMAKE_MIN_VERSION})
message(STATUS "CMake minimum required version set to ${CMAKE_MIN_VERSION}.")

# Create the project
project(${PROJECT_NAME} VERSION ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}.${PROJECT_VERSION_TWEAK})
message(STATUS "Project name set to \"${PROJECT_NAME}\".")
message(STATUS "Project version set to ${PROJECT_VERSION}.")

# Configure the info header file
configure_file(${PROJECT_SOURCE_DIR}/info/ProjectInfo.hpp.in ${PROJECT_SOURCE_DIR}/info/ProjectInfo.hpp)
message(STATUS "Configured info header file successfully.")

# Enable testing
include(CTest)
enable_testing()
message(STATUS "Testing enabled successfilly")

# Find the source files
file(GLOB_RECURSE SOURCES ${PROJECT_SOURCE_DIR}/src/*.cpp)
list(LENGTH SOURCES SOURCE_COUNT)

if(SOURCE_COUNT EQUAL 0)
    message(FATAL_ERROR "ERROR: No sources found!")
elseif(ENGINE_SOURCE_COUNT EQUAL 1)
    message(STATUS "1 source found.")
else()
    message(STATUS "${SOURCE_COUNT} sources found.")
endif()

# Create the executable
add_executable(${PROJECT_NAME} ${SOURCES})
target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_20)
message(STATUS "Executable created successfully.")

# Add the include directories
target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/include ${PROJECT_SOURCE_DIR}/info ${PROJECT_SOURCE_DIR}/src ${PROJECT_SOURCE_DIR}/external/ImGui/include ${PROJECT_SOURCE_DIR}/external/Core/include ${PROJECT_SOURCE_DIR}/external/stb/include ${PROJECT_SOURCE_DIR}/external/Vulkan/include)
message(STATUS "Include directories added.")

# Add the link directories and link the libraries
target_link_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/external/Core/lib ${PROJECT_SOURCE_DIR}/external/ImGui/lib ${PROJECT_SOURCE_DIR}/external/Vulkan/lib)
message(STATUS "Link directories added.")

if(${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
    target_link_libraries(${PROJECT_NAME} ImGui-${TARGET_ARCH}Wireframe-Core-${TARGET_ARCH}vulkan-${TARGET_ARCH})
elseif(${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
    target_link_libraries(${PROJECT_NAME} ImGui-${TARGET_ARCH} Wireframe-Core-${TARGET_ARCH} vulkan X11 xkbcommon)
endif()
message(STATUS "Link libraries added.")

# Add the precompiled header
target_precompile_headers(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/include/WireframeEngineEditor.hpp)
message(STATUS "Precompiled header added.")

# Create the wrapper library
add_library(Wireframe-Editor ${PROJECT_SOURCE_DIR}/wrapper/Wrapper.cpp)
target_compile_features(Wireframe-Editor PUBLIC cxx_std_20)
target_compile_options(Wireframe-Editor PUBLIC -fPIC)
message(STATUS "Wrapper library created successfully")

# Add the include directories
target_include_directories(Wireframe-Editor PUBLIC ${PROJECT_SOURCE_DIR}/include ${PROJECT_SOURCE_DIR}/info ${PROJECT_SOURCE_DIR}/src ${PROJECT_SOURCE_DIR}/external/ImGui/include ${PROJECT_SOURCE_DIR}/external/Core/include ${PROJECT_SOURCE_DIR}/external/stb/include ${PROJECT_SOURCE_DIR}/external/Vulkan/include)
message(STATUS "Include directories added")

set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
