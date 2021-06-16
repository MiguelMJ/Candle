cmake_minimum_required(VERSION 3.12)

project(Candle)

# set C++11 standard
set(CMAKE_CXX_STANDARD 11)

# set output files directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY lib)


# might need to set manually SFML_ROOT to the install location of SFML for Windows users
# set(SFML_ROOT "")

# if the user wants to use static SFML libs
# set(SFML_STATIC_LIBRARIES TRUE)

find_package(SFML 2.5 REQUIRED COMPONENTS  graphics)


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

# Static library target
add_library(Candle-s STATIC ${CANDLE_SRC})
target_include_directories(Candle-s PRIVATE include)
target_link_libraries(Candle-s sfml-graphics)

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
	target_link_libraries(demo PRIVATE sfml-graphics Candle-s)
endif()
