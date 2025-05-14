# CMakeLists.txt for Geant4 Beam Test Project

cmake_minimum_required(VERSION 3.8 FATAL_ERROR)
project(BeamTest)

# Find Geant4 package, activating all available UI and Vis drivers by default
option(WITH_GEANT4_UIVIS "Build with Geant4 UI and Vis drivers" ON)
if(WITH_GEANT4_UIVIS)
  find_package(Geant4 REQUIRED ui_all vis_all)
else()
  find_package(Geant4 REQUIRED)
endif()

# Setup Geant4 include directories and compile definitions
include(${Geant4_USE_FILE})

# Specify directories
set(SRC_DIR ${PROJECT_SOURCE_DIR}/src)
set(INCLUDE_DIR ${PROJECT_SOURCE_DIR}/include)

# IMPORTANT: Add the include directory to the include path
include_directories(${INCLUDE_DIR})

# Explicitly list source files
set(SOURCES
    ${SRC_DIR}/ActionInitialization.cc
    ${SRC_DIR}/DetectorConstruction.cc
    ${SRC_DIR}/EventAction.cc
    ${SRC_DIR}/MagneticField.cc
    ${SRC_DIR}/PrimaryGeneratorAction.cc
    ${SRC_DIR}/RunAction.cc
    ${SRC_DIR}/SteppingAction.cc
    ${SRC_DIR}/RFCavityField.cc
    ${PROJECT_SOURCE_DIR}/main.cc  # Main is in the project root folder
)

# Add the executable, and link it to the Geant4 libraries
add_executable(beamTest ${SOURCES})
target_link_libraries(beamTest ${Geant4_LIBRARIES})

# Add the standard installation target
install(TARGETS beamTest DESTINATION bin)

# Copy all macro files to build directory
set(BEAM_TEST_SCRIPTS
  init_vis.mac
  run.mac
)

foreach(_script ${BEAM_TEST_SCRIPTS})
  configure_file(
    ${PROJECT_SOURCE_DIR}/${_script}
    ${PROJECT_BINARY_DIR}/${_script}
    COPYONLY
  )
endforeach()