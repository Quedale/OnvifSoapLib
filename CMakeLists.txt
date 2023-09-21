cmake_minimum_required(VERSION 3.0.0)
project(client-test VERSION 0.1.0)

include(CTest)
enable_testing()

set(OUTPUT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/release)
set(LIBRARY_OUTPUT_PATH    ${OUTPUT_DIR})
set(EXECUTABLE_OUTPUT_PATH ${OUTPUT_DIR})

set(GSOAP_SRC_DIR  ${CMAKE_SOURCE_DIR}/subprojects/gsoap-2.8)

if (MSVC)
    add_compile_options(/bigobj)
elseif (MINGW)
    add_compile_options(-Wa,-mbig-obj)
endif ()

find_package(OpenSSL REQUIRED)
find_package(ZLIB REQUIRED)
find_package(Threads REQUIRED)

include_directories(${GSOAP_SRC_DIR}/gsoap/ 
                    ${GSOAP_SRC_DIR}/gsoap/import 
                    ${GSOAP_SRC_DIR}/gsoap/custom 
                    ${GSOAP_SRC_DIR}/gsoap/plugin 
                    src/generated
                    src)

if(NOT EXISTS ${CMAKE_SOURCE_DIR}/src/generated)
file(MAKE_DIRECTORY ${CMAKE_SOURCE_DIR}/src/generated)
endif()

aux_source_directory(src src_dir)
aux_source_directory(src/generated generated_dir)

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
                                ${src_dir}
                                ${generated_dir})

if(ZLIB_FOUND)
  message("Linking ZLIB")
  target_link_libraries(onvifsoap PRIVATE ZLIB::ZLIB)
  target_compile_options(onvifsoap PRIVATE "SHELL: -DWITH_GZIP")
else()
  message(WARNING "ZLIB not found...")
endif()

if(OPENSSL_FOUND)
  message("Linking OpenSSL")
  target_link_libraries(onvifsoap PRIVATE OpenSSL::SSL OpenSSL::Crypto)
  target_compile_options(onvifsoap PRIVATE "SHELL: -DWITH_OPENSSL")
else()
  message(FATAL_ERROR "OpenSSL not found...")
endif()

if(Threads_FOUND)
  message("Linking thread")
  target_link_libraries(onvifsoap PRIVATE Threads::Threads)
else()
  message(FATAL_ERROR  "Threads not found...")
endif()


if(WIN32)
  target_link_libraries(onvifsoap PRIVATE wsock32 ws2_32)
endif()
target_compile_options(onvifsoap PRIVATE "SHELL: -c -Wall -DWITH_DOM -DWITH_NOEMPTYSTRUCT -DBUILD_SHARD")
add_subdirectory(example)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
