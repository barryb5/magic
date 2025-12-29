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

include(FetchContent)

find_package(Python COMPONENTS Interpreter Development REQUIRED)
find_package(pybind11 CONFIG REQUIRED)

find_package(nlohmann_json 3.12.0 CONFIG QUIET)

if(NOT nlohmann_json_FOUND)
  message(STATUS "nlohmann_json >= 3.12.0 not found. Fetching with FetchContent...")

  # Optional: avoid building tests from the dependency
  set(JSON_BuildTests OFF CACHE BOOL "" FORCE)

  FetchContent_Declare(
    nlohmann_json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG v3.12.0
  )
  FetchContent_MakeAvailable(nlohmann_json)
endif()

file(GLOB SOURCE_FILES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")

add_executable(engine
    main.cpp
    ${SOURCE_FILES}
)

target_include_directories(engine PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(engine PRIVATE 
  nlohmann_json::nlohmann_json
  pybind11::embed
  Python::Python
)

get_filename_component(MAGIC_API_DIR
  "${CMAKE_CURRENT_SOURCE_DIR}/../api"
  ABSOLUTE
)

target_compile_definitions(engine PRIVATE
  MAGIC_API_DIR="${MAGIC_API_DIR}"
)