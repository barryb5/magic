cmake_minimum_required(VERSION 3.11)
project (engine)

# Set Configuration specific DEFINES
IF ( CMAKE_BUILD_TYPE STREQUAL "Debug" )
  message(STATUS "Debug build")
  add_definitions ( "-DUSE_PERFORMANCE_DATA")
  add_definitions ( "-DDEBUG " )
  add_definitions ( "-D_DEBUG " )
  add_definitions ( "-rdynamic ")

  add_definitions ( "-Wno-deprecated ")
  add_definitions ( "-Wno-unknown-pragmas ")

  add_compile_options(
      -DDEBUG
      -D_DEBUG
      -Og
      -g3
  )
ELSE()
  add_definitions ( "-Wall ")
  add_definitions ( "-DNDEBUG " )
  add_definitions ( "-DUSE_PERFORMANCE_DATA")
  add_definitions ( "-O1 ")
  add_definitions ( "-march=native ")
  add_definitions ( "-mtune=native ")
  add_definitions ( "-Wno-deprecated ")
  add_definitions ( "-Wno-unknown-pragmas ")
  add_definitions ( "-rdynamic ")
ENDIF()

find_package(nlohmann_json 3.12.0 REQUIRED)

add_executable(engine
    main.cpp
    src/card.cpp
    src/player.cpp
    src/helpers.cpp)

target_link_libraries(engine PRIVATE nlohmann_json::nlohmann_json)