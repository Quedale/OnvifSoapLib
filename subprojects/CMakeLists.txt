cmake_minimum_required(VERSION 3.0.0)
project(subprojects)

#############
# Check if dependency are available
#############
# message (ERROR " checking pkg CUTILS inside '${CMAKE_CURRENT_SOURCE_DIR}/CUtils/build/lib/pkgconfig'")
# set(ENV{PKG_CONFIG_PATH} "$ENV{PKG_CONFIG_PATH}:${CMAKE_CURRENT_SOURCE_DIR}/CUtils/build/lib/pkgconfig")
# find_package(PkgConfig)
# pkg_check_modules(CUTILS QUIET cutils)
find_library(CUTILS_LIB
    NAMES cutils
    PATHS ${CMAKE_CURRENT_SOURCE_DIR}/CUtils/build/lib
    NO_CACHE
)

if(NOT CUTILS_LIB)
  message (STATUS " CUtils libary not found")
  include(ExternalProject)
  ExternalProject_Add(cutils-project
    GIT_REPOSITORY    https://github.com/Quedale/CUtils.git
      GIT_TAG           main
      SOURCE_DIR        "${CMAKE_SOURCE_DIR}/cutils/"
      BUILD_IN_SOURCE 1
      CMAKE_ARGS  		-DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_SOURCE_DIR}/cutils/build
  )
else()
  message (STATUS " CUtils library already available.")
endif()