cmake_minimum_required(VERSION 2.8)

set(Smoke_Simulation_Source
    # main
    main.cc

    # smoke simlator
    smoke_simulator.cc
    smoke.cc
    emittor.cc
    particle.cc
    navier_stoke_solver.cc

    # Camera
    camera.cc
)

#-------------------------------------------------------------------------------
# Set definitions
#-------------------------------------------------------------------------------
add_definitions(${NANOGUI_EXTRA_DEFS})

#-------------------------------------------------------------------------------
# Set include directories
#-------------------------------------------------------------------------------
include_directories(
  ${FREETYPE_INCLUDE_DIRS}
  ${NANOGUI_EXTRA_INCS}
)


#-------------------------------------------------------------------------------
# Set link directories
#-------------------------------------------------------------------------------
link_directories(
  ${FREETYPE_LIBRARY_DIRS}
)

#-------------------------------------------------------------------------------
# Add executable
#-------------------------------------------------------------------------------
add_executable(smoke_simulation ${Smoke_Simulation_Source})

target_link_libraries(smoke_simulation
    nanogui ${NANOGUI_EXTRA_LIBS}
    ${FREETYPE_LIBRARIES}
    ${CMAKE_THREADS_INIT}
)

find_package(OpenMP REQUIRED)
target_link_libraries(smoke_simulation PRIVATE OpenMP::OpenMP_CXX)

#-------------------------------------------------------------------------------
# Platform-specific configurations for target
#-------------------------------------------------------------------------------
if(APPLE)
  set_property( TARGET smoke_simulation APPEND_STRING PROPERTY COMPILE_FLAGS
                "-Wno-deprecated-declarations -Wno-c++11-extensions")
endif(APPLE)

# Put executable in build directory root
set(EXECUTABLE_OUTPUT_PATH ../bin)
