cmake_minimum_required(VERSION 3.20)
project(arp_greedy VERSION 0.1.0 LANGUAGES CXX)
set(CMAKE_CXX_EXTENSIONS OFF)

option(ARP_GREEDY_BUILD_PYTHON "Build the Python extension" OFF)
set(ARP_GREEDY_BOOST_INCLUDE_DIR "$ENV{ARP_GREEDY_BOOST_INCLUDE_DIR}" CACHE PATH
  "Optional directory containing boost/ headers (for standalone header archives)")

if(ARP_GREEDY_BOOST_INCLUDE_DIR)
  if(NOT EXISTS "${ARP_GREEDY_BOOST_INCLUDE_DIR}/boost/multiprecision/cpp_int.hpp")
    message(FATAL_ERROR "ARP_GREEDY_BOOST_INCLUDE_DIR must contain boost/multiprecision/cpp_int.hpp")
  endif()
  set(Boost_INCLUDE_DIRS "${ARP_GREEDY_BOOST_INCLUDE_DIR}")
else()
  find_package(Boost 1.74 CONFIG REQUIRED)
endif()
add_library(arp_greedy src/numeric.cpp src/decremental_hull.cpp src/greedy.cpp)
add_library(arp_greedy::arp_greedy ALIAS arp_greedy)
target_compile_features(arp_greedy PUBLIC cxx_std_17)
target_include_directories(arp_greedy PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>)
target_include_directories(arp_greedy SYSTEM PRIVATE ${Boost_INCLUDE_DIRS})
set_target_properties(arp_greedy PROPERTIES CXX_EXTENSIONS OFF
  POSITION_INDEPENDENT_CODE ON WINDOWS_EXPORT_ALL_SYMBOLS ON)

function(arp_compile_options target)
  if(MSVC)
    target_compile_options(${target} PRIVATE /W4 /permissive- /fp:strict)
  else()
    target_compile_options(${target} PRIVATE -Wall -Wextra -Wpedantic
      -fno-fast-math -ffp-contract=off)
  endif()
endfunction()
arp_compile_options(arp_greedy)

if(ARP_GREEDY_BUILD_PYTHON)
  set(PYBIND11_FINDPYTHON ON)
  find_package(Python REQUIRED COMPONENTS Interpreter Development.Module)
  find_package(pybind11 3.0 CONFIG REQUIRED)
  pybind11_add_module(_native MODULE bindings/module.cpp)
  target_link_libraries(_native PRIVATE arp_greedy)
  target_compile_definitions(_native PRIVATE ARP_GREEDY_VERSION="${PROJECT_VERSION}")
  arp_compile_options(_native)
  install(TARGETS _native
    LIBRARY DESTINATION arp_greedy COMPONENT Python
    RUNTIME DESTINATION arp_greedy COMPONENT Python)
endif()

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)
install(TARGETS arp_greedy EXPORT arp_greedyTargets
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
install(FILES LICENSE THIRD_PARTY_NOTICES.md DESTINATION ${CMAKE_INSTALL_DATADIR}/arp_greedy)
install(EXPORT arp_greedyTargets NAMESPACE arp_greedy::
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/arp_greedy)
configure_package_config_file(cmake/arp_greedyConfig.cmake.in
  ${CMAKE_CURRENT_BINARY_DIR}/arp_greedyConfig.cmake
  INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/arp_greedy)
write_basic_package_version_file(${CMAKE_CURRENT_BINARY_DIR}/arp_greedyConfigVersion.cmake
  VERSION ${PROJECT_VERSION} COMPATIBILITY SameMajorVersion)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/arp_greedyConfig.cmake
  ${CMAKE_CURRENT_BINARY_DIR}/arp_greedyConfigVersion.cmake
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/arp_greedy)
