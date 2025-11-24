add_library(aes STATIC
    "${CMAKE_CURRENT_LIST_DIR}/include/aes.h"
    "${CMAKE_CURRENT_LIST_DIR}/include/aes.hpp"
    "${CMAKE_CURRENT_LIST_DIR}/src/aes.c")
target_include_directories(aes PUBLIC "${CMAKE_CURRENT_LIST_DIR}/include")
