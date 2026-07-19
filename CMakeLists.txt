cmake_minimum_required (VERSION 3.0)

project(HydraISP)


include_directories(thirdparty/easylog)
include_directories(thirdparty/easybmp)
include_directories(thirdparty/json/nlohmann)
include_directories(src/include)

add_subdirectory(thirdparty/easylog)
add_subdirectory(thirdparty/easybmp)
add_subdirectory(src)


set (
    SRCS
    main.cpp
)

message(STATUS "sources lists " ${SRCS})

add_executable(isp_testbed ${SRCS})

target_link_libraries(isp_testbed PRIVATE isp_algo easylog easybmp)








