cmake_minimum_required (VERSION 3.0)

add_library(easylog STATIC easylogging++.cc)
set_target_properties(easylog PROPERTIES POSITION_INDEPENDENT_CODE ON)








