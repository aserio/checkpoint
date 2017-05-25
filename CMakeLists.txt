# Copyright (c) 2017 Adrian Serio
# 
# Distributed under the Boost Software License, Version 1.0. (See accompanying
# file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
# 
# Will need to add when invoking cmake:
#   "-DHPX_DIR=~/lib/hpx_1.0.0/lib/cmake/HPX \

cmake_minimum_required (VERSION 2.6)
project (checkpoint)
find_package (HPX)

#Adding HPXIO library
include_directories(/home/aserio/git/pxfs/include/) #headers
add_library (pxfs SHARED IMPORTED GLOBAL)
set_target_properties (pxfs PROPERTIES 
 IMPORTED_LOCATION 
   /home/aserio/lib/pxfs-v0.1.0-gcc6.3.0/lib/libhpx_base_file.so 
 IMPORTED_LOCATION_DEBUG
  /home/aserio/lib/pxfs-v0.1.0-gcc6.3.0-debug/lib/libhpx_base_file.so )

#Including Headers
include_directories (${PROJECT_SOURCE_DIR}/include/)

#Create Executable
add_executable (checkpoint checkpoint.cpp)
hpx_setup_target(checkpoint
  COMPONENT_DEPENDENCIES iostreams
  DEPENDENCIES pxfs
 )
