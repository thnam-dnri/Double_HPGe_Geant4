# CMakeLists.txt for Dual HPGe Detector Simulation

cmake_minimum_required(VERSION 3.8 FATAL_ERROR)
project(HPGeDual)

# Find required packages
find_package(Geant4 REQUIRED ui_all vis_all)
include(${Geant4_USE_FILE})

# Find ROOT
find_package(ROOT REQUIRED)
include(${ROOT_USE_FILE})

# Include directories
include_directories(${PROJECT_SOURCE_DIR}/include)
include_directories(${Geant4_INCLUDE_DIR})

# Set C++ standard (ROOT 6.20 on remote requires at least C++14)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Locate sources and headers
file(GLOB sources ${PROJECT_SOURCE_DIR}/src/*.cc)
file(GLOB headers ${PROJECT_SOURCE_DIR}/include/*.hh)

# Add the executable
add_executable(HPGeDual HPGeDual.cc ${sources} ${headers})

# Link against Geant4 and ROOT libraries
target_link_libraries(HPGeDual ${Geant4_LIBRARIES} ${ROOT_LIBRARIES})

# Ensure ROOT detects availability of std::string_view in newer libstdc++
target_compile_definitions(HPGeDual PRIVATE R__HAS_STD_STRING_VIEW)

# Copy macro files to build directory
set(HPGeDual_SCRIPTS
    init_vis.mac
    gui.mac
    vis.mac
    run.mac
)

foreach(_script ${HPGeDual_SCRIPTS})
    configure_file(
        ${PROJECT_SOURCE_DIR}/${_script}
        ${PROJECT_BINARY_DIR}/${_script}
        COPYONLY
    )
endforeach()

# Install the executable
install(TARGETS HPGeDual DESTINATION bin)
