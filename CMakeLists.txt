cmake_minimum_required(VERSION 3.28)
project(Candle LANGUAGES CXX)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
option(BUILD_SHARED_LIBS "Build shared libraries" OFF)

include(FetchContent)
FetchContent_Declare(SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 3.0.x
    GIT_SHALLOW ON
    EXCLUDE_FROM_ALL
    SYSTEM)
FetchContent_MakeAvailable(SFML)


set(CANDLE_HEADERS
	include/Candle/LightingArea.hpp
	include/Candle/LightSource.hpp
	include/Candle/RadialLight.hpp
	include/Candle/DirectedLight.hpp
	include/Candle/geometry/Line.hpp
	include/Candle/geometry/Polygon.hpp
        include/Candle/geometry/Vector2.hpp
	include/Candle/graphics/Color.hpp
	include/Candle/graphics/VertexArray.hpp
	include/Candle/Constants.hpp
)

set(CANDLE_SRC
	src/LightingArea.cpp
	src/LightSource.cpp
	src/RadialLight.cpp
	src/DirectedLight.cpp
	src/Line.cpp
	src/Polygon.cpp
	src/Color.cpp
	src/VertexArray.cpp
	src/Constants.cpp
)

add_library(Candle-s STATIC ${CANDLE_SRC} ${CANDLE_HEADERS})
target_include_directories(Candle-s PUBLIC include)
target_link_libraries(Candle-s PRIVATE SFML::Graphics)
target_compile_features(Candle-s PRIVATE cxx_std_17)

option(RADIAL_LIGHT_FIX "Use RadialLight fix for errors with textures" OFF)

if(RADIAL_LIGHT_FIX)
	target_compile_definitions(Candle-s PUBLIC -DRADIAL_LIGHT_FIX)
endif()

# Demo target
option(BUILD_DEMO "Build demo application" OFF)

if (BUILD_DEMO)
	set(DEMO_SRC demo.cpp)
	add_executable(demo ${DEMO_SRC})
	target_include_directories(demo PRIVATE include)
	target_link_libraries(demo PRIVATE SFML::Graphics SFML::System Candle-s)
	target_compile_features(demo PRIVATE cxx_std_17)
endif()