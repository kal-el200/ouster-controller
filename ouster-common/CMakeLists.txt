#cmake_minimum_required(VERSION 3.8)
#project(OusterLiDARSimulator)

set(CMAKE_CXX_STANDARD 20)
#add_definitions(-D_WIN32_WINNT=0x0601)

# Include directories
#include_directories(${CMAKE_SOURCE_DIR}/ouster_client/include)
#include_directories(${CMAKE_SOURCE_DIR}/vcpkg/packages/boost-asio_x64-windows/include)
#include_directories(${CMAKE_SOURCE_DIR}/vcpkg/installed/x64-windows/include)

if(MSVC)
    # Debug symbols for MSVC
    add_compile_options(/Zi)  # Generate debug info
    add_link_options(/DEBUG)  # Generate debug info in PDB
else()
    # Debug symbols for GCC/Clang
    add_compile_options(-g)
endif()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_CURRENT_BINARY_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_CURRENT_BINARY_DIR})

include_directories(${CMAKE_SOURCE_DIR}/vcpkg/installed/x64-windows/include)

add_executable(client_example client_example.cpp)
find_package(OpenCV REQUIRED)

if(MSVC)
    if (CMAKE_BUILD_TYPE STREQUAL "Debug")
        # Debug build - keep runtime checks
        target_compile_options(client_example PRIVATE /W4)
    else()
        # Release build - enable optimizations
        target_compile_options(client_example PRIVATE /O2 /W4)
    endif()
endif()

target_link_libraries(client_example
    PRIVATE
    ouster_client
    ${OpenCV_LIBS}  # Add OpenCV libraries
)

add_executable(client_packet_example client_packet_example.cpp)
target_link_libraries(client_packet_example PRIVATE OusterSDK::ouster_client)

add_executable(config_example config_example.cpp)
target_link_libraries(config_example PRIVATE OusterSDK::ouster_client)

add_executable(lidar_simulator lidar_simulator.cpp)
target_link_libraries(lidar_simulator PRIVATE OusterSDK::ouster_client)

if(TARGET OusterSDK::ouster_pcap)
  add_executable(lidar_scan_example lidar_scan_example.cpp helpers.cpp)
  target_link_libraries(lidar_scan_example PRIVATE OusterSDK::ouster_client OusterSDK::ouster_pcap)

  add_executable(representations_example representations_example.cpp helpers.cpp)
  target_link_libraries(representations_example PRIVATE OusterSDK::ouster_client OusterSDK::ouster_pcap)
else()
  message(STATUS "No ouster_pcap library available; skipping examples")
endif()

if(TARGET OusterSDK::ouster_osf)
  add_executable(osf_reader_example osf_reader_example.cpp)
  target_link_libraries(osf_reader_example PRIVATE OusterSDK::ouster_osf)
  
  add_executable(osf_writer_example osf_writer_example.cpp)
  target_link_libraries(osf_writer_example PRIVATE OusterSDK::ouster_osf)
else()
  message(STATUS "No ouster_osf library available; skipping examples")
endif()

if(TARGET OusterSDK::ouster_viz)
  add_executable(viz_example viz_example.cpp)
  target_link_libraries(viz_example PRIVATE OusterSDK::ouster_client OusterSDK::ouster_viz)

  add_executable(viz_events_example viz_events_example.cpp)
  target_link_libraries(viz_events_example PRIVATE OusterSDK::ouster_client OusterSDK::ouster_viz)
else()
  message(STATUS "No ouster_viz library available; skipping examples")
endif()
