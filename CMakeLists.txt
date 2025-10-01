cmake_minimum_required(VERSION 3.23)

project(colla VERSION 0.1.0 LANGUAGES CXX)

add_library(colla INTERFACE)

target_include_directories(colla INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

include(GNUInstallDirs)

install(TARGETS colla
    EXPORT collaTargets
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

install(EXPORT collaTargets
    FILE collaTargets.cmake
    NAMESPACE colla::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/colla
)

option(BUILD_EXAMPLES "Build examples" OFF)
if(BUILD_EXAMPLES)
  include_directories(include)
  add_subdirectory(examples)
endif()
