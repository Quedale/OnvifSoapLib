cmake_minimum_required(VERSION 3.0.0)
project(client-test VERSION 0.1.0)

include(CTest)
enable_testing()

add_executable(client-test client-test.c)
target_link_libraries(client-test onvifsoap)

#I care little for MSVC warning. Support is from good faith
if (MSVC)
  target_compile_options(client-test PRIVATE "SHELL: -wd4244 -wd4996")
endif()

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
