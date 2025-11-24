add_library(uulog STATIC
    "${CMAKE_CURRENT_LIST_DIR}/include/uulog.hh"
    "${CMAKE_CURRENT_LIST_DIR}/src/uulog.cc")
target_compile_features(uulog PUBLIC cxx_std_20)
target_include_directories(uulog PUBLIC "${CMAKE_CURRENT_LIST_DIR}/include")
