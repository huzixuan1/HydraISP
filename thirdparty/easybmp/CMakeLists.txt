cmake_minimum_required (VERSION 3.0)

add_library(easybmp STATIC EasyBMP.cpp)
set_target_properties(easybmp PROPERTIES POSITION_INDEPENDENT_CODE ON)


