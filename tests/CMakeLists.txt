# Copyright (c) 2017 Adrian Serio
# 
# Distributed under the Boost Software License, Version 1.0. (See accompanying
# file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
# 

#Create Executable
add_executable (checkpoint_test checkpoint_test.cpp)
hpx_setup_target(checkpoint_test
  COMPONENT_DEPENDENCIES iostreams
  DEPENDENCIES pxfs
 )

add_executable (file_test file_test.cpp)
hpx_setup_target(file_test
  COMPONENT_DEPENDENCIES iostreams
  DEPENDENCIES pxfs 
)

#Create Tests
add_test (CheckpointTest checkpoint_test)

add_test (FileTest file_test)

