# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "")
  file(REMOVE_RECURSE
  "CMakeFiles/client1_autogen.dir/AutogenUsed.txt"
  "CMakeFiles/client1_autogen.dir/ParseCache.txt"
  "CMakeFiles/client1_tests_autogen.dir/AutogenUsed.txt"
  "CMakeFiles/client1_tests_autogen.dir/ParseCache.txt"
  "client1_autogen"
  "client1_tests_autogen"
  )
endif()
