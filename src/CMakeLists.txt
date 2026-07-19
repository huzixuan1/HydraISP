cmake_minimum_required (VERSION 3.0)

project(isp_algo)

include_directories(../thirdparty/easylog)
include_directories(../thirdparty/json/nlohmann)
include_directories(../include)

aux_source_directory(sources/modules/ ISP_MOD_SRCS_LISTS)
aux_source_directory(sources/common/  ISP_COM_SRCS_LISTS)

add_library(${PROJECT_NAME} SHARED
    ${ISP_MOD_SRCS_LISTS}
    ${ISP_COM_SRCS_LISTS}
    sources/common/logging.cpp
    ../parse.cpp
)

target_link_libraries(${PROJECT_NAME} PRIVATE easylog easybmp)
set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME "isp_algo")