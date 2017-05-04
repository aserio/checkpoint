# Adrian Serio
# Will need to add when invoking cmake:
#   "-DHPX_DIR=~/lib/hpx_1.0.0/lib/cmake/HPX \
#    -DCMAKE_CXX_FLAGS=stdlib=libc++ \
#    -DCMAKE_CXX_COMPILER=clang++"

cmake_minimum_required (VERSION 2.6)
project (checkpoint)
find_package (HPX)
add_executable (checkpoint checkpoint.cpp)
hpx_setup_target(checkpoint
  COMPONENT_DEPENDENCIES iostreams
 )
set(CMAKE_CXX_FLAGS "-pthread")
