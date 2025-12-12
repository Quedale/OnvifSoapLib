cmake_minimum_required(VERSION 3.5.0)
project(client-test VERSION 0.1.0)

include(CTest)
enable_testing()

add_executable(client-test client-test.c)

find_package(PkgConfig REQUIRED)
pkg_check_modules(CUTILS REQUIRED cutils)
pkg_check_modules(GLIB REQUIRED glib-2.0)
pkg_check_modules(GOBJECT REQUIRED gobject-2.0)

target_include_directories(client-test PRIVATE ${CUTILS_INCLUDE_DIRS} ${GLIB_INCLUDE_DIRS} ${GOBJECT_INCLUDE_DIRS})
target_link_libraries(client-test PRIVATE onvifsoap ${CUTILS_LDFLAGS} ${GLIB_LDFLAGS} ${GOBJECT_LDFLAGS})
  
#I care little for MSVC warning. Support is from good faith
if (MSVC)
  target_compile_options(client-test PRIVATE "SHELL: -wd4244 -wd4996 -wd5045")
endif()


add_executable(disco-test disco-test.c)
target_include_directories(disco-test PRIVATE ${CUTILS_INCLUDE_DIRS} ${GLIB_INCLUDE_DIRS} ${GOBJECT_INCLUDE_DIRS})
target_link_libraries(disco-test PRIVATE onvifsoap ${CUTILS_LDFLAGS} ${GLIB_LDFLAGS} ${GOBJECT_LDFLAGS})

add_executable(udp-test udp-test.c)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
