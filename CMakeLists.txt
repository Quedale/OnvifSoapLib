cmake_minimum_required(VERSION 3.0.0)
project(client-test VERSION 0.1.0)

include(CTest)
enable_testing()

set(OUTPUT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/release)
set(LIBRARY_OUTPUT_PATH    ${OUTPUT_DIR})
set(EXECUTABLE_OUTPUT_PATH ${OUTPUT_DIR})

set(GSOAP_SRC_DIR  ${PROJECT_SOURCE_DIR}/gsoap-2.8)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -c -Wall -DWITH_DOM")
add_definitions(-DWITH_DOM -DWITH_OPENSSL -DWITH_GZIP -DBUILD_SHARD)
add_compile_options(/bigobj)

find_package(OpenSSL REQUIRED)
find_package(ZLIB REQUIRED)

include_directories(src/onvif_device.h src/onvif_list.h src/client.h)
include_directories(${GSOAP_SRC_DIR}/gsoap/ 
                    ${GSOAP_SRC_DIR}/gsoap/import 
                    ${GSOAP_SRC_DIR}/gsoap/custom 
                    ${GSOAP_SRC_DIR}/gsoap/plugin 
                    src/generated
                    src)

if(NOT EXISTS ${PROJECT_SOURCE_DIR}/src/generated)
file(MAKE_DIRECTORY ${PROJECT_SOURCE_DIR}/src/generated)
endif()

aux_source_directory(src/generated generated_src)

add_library(onvifsoap SHARED ${GSOAP_SRC_DIR}/gsoap/dom.c 
                                ${GSOAP_SRC_DIR}/gsoap/stdsoap2.c 
                                ${GSOAP_SRC_DIR}/gsoap/plugin/smdevp.c 
                                ${GSOAP_SRC_DIR}/gsoap/plugin/mecevp.c 
                                ${GSOAP_SRC_DIR}/gsoap/plugin/wsaapi.c 
                                ${GSOAP_SRC_DIR}/gsoap/plugin/wsseapi.c 
                                ${GSOAP_SRC_DIR}/gsoap/plugin/wsddapi.c
                                ${GSOAP_SRC_DIR}/gsoap/plugin/threads.c
                                ${GSOAP_SRC_DIR}/gsoap/plugin/httpda.c
                                ${GSOAP_SRC_DIR}/gsoap/custom/struct_timeval.c 
                                src/onvif_list.c 
                                src/onvif_device.c 
                                src/client.c
                                ${generated_src})

target_link_libraries(onvifsoap PRIVATE OpenSSL::SSL OpenSSL::Crypto ZLIB::ZLIB)

add_subdirectory(example)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
