cmake_minimum_required(VERSION 3.18.1)
project(client-test VERSION 0.1.0)

include(CMakeDependentOption)

cmake_dependent_option(
  ONVIFSOAP_STATIC             # option variable
  "Build static library"    # description
  ON                        # default value if exposed; user can override
  "NOT BUILD_SHARED_LIBS"   # condition to expose option
  OFF                       # value if not exposed; user can't override
)
cmake_dependent_option(
        ENABLE_EXAMPLES             # option variable
        "Build demo executables"    # description
        OFF                        # default value if exposed; user can override
        "NOT ENABLE_EXAMPLES"   # condition to expose option
        ON                       # value if not exposed; user can't override
)

if (ONVIFSOAP_STATIC)
  set(CMAKE_FIND_LIBRARY_SUFFIXES ".a;.dylib;.so;")
  set(ONVIFSOAP_BUILD_TYPE STATIC)
else ()
  set(ONVIFSOAP_BUILD_TYPE SHARED)
endif ()


set(OUTPUT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/release)
set(LIBRARY_OUTPUT_PATH    ${OUTPUT_DIR})
set(EXECUTABLE_OUTPUT_PATH ${OUTPUT_DIR})

if(NOT DEFINED GENERATED_SOURCE_PATH)
    set(GENERATED_SOURCE_PATH "${CMAKE_SOURCE_DIR}/src/generated")
endif()

if (NOT DEFINED GSOAP_SRC_DIR)
  set(GSOAP_SRC_DIR  ${CMAKE_SOURCE_DIR}/subprojects/gsoap-2.8)
endif()

if (MSVC)
    add_compile_options(/bigobj)
elseif (MINGW)
    add_compile_options(-Wa,-mbig-obj)
endif ()

include_directories(${GSOAP_SRC_DIR}/gsoap/ 
                    ${GSOAP_SRC_DIR}/gsoap/import 
                    ${GSOAP_SRC_DIR}/gsoap/custom 
                    ${GSOAP_SRC_DIR}/gsoap/plugin 
                    ${GENERATED_SOURCE_PATH}
                    src)

if(NOT EXISTS ${GENERATED_SOURCE_PATH})
  file(MAKE_DIRECTORY ${GENERATED_SOURCE_PATH})
endif()

aux_source_directory(src src_dir)
aux_source_directory(src/deviceio src_deviceio)
aux_source_directory(src/devicemgmt src_devicemgmt)
aux_source_directory(src/media src_media)
aux_source_directory(src/media/media1 src_media1)
aux_source_directory(src/media/media2 src_media2)
aux_source_directory(src/discovery src_dir_disco)
aux_source_directory(${GENERATED_SOURCE_PATH} generated_dir)

#########################
#
# onvif ibrary
#
#########################
add_library(onvifsoap ${ONVIFSOAP_BUILD_TYPE} ${src_dir}
                                ${src_dir_disco}
                                ${generated_dir}
                                ${src_deviceio}
                                ${src_devicemgmt}
                                ${src_media}
                                ${src_media1}
                                ${src_media2}
                          ${GSOAP_SRC_DIR}/gsoap/dom.c 
                          ${GSOAP_SRC_DIR}/gsoap/stdsoap2.c 
                          ${GSOAP_SRC_DIR}/gsoap/plugin/logging.c
                          ${GSOAP_SRC_DIR}/gsoap/plugin/smdevp.c 
                          ${GSOAP_SRC_DIR}/gsoap/plugin/mecevp.c 
                          ${GSOAP_SRC_DIR}/gsoap/plugin/wsaapi.c 
                          ${GSOAP_SRC_DIR}/gsoap/plugin/wsseapi.c 
                          ${GSOAP_SRC_DIR}/gsoap/plugin/wsddapi.c
                          ${GSOAP_SRC_DIR}/gsoap/plugin/threads.c
                          ${GSOAP_SRC_DIR}/gsoap/plugin/httpda.c
                          ${GSOAP_SRC_DIR}/gsoap/custom/struct_timeval.c)


if(ANDROID)
  ################################################
  # These dependencies aren't inplemented in bootstrap yet.
  # The will be implemented in the parent android project soon
  ################################################
  find_library(CUTILS cutils    REQUIRED NO_CMAKE_SYSTEM_PATH NO_SYSTEM_ENVIRONMENT_PATH NO_CMAKE_FIND_ROOT_PATH PATHS "${SUBPROJECTS_ROOT}/CUtils/dist/lib")
  find_library(NTLM   ntlm      REQUIRED NO_CMAKE_SYSTEM_PATH NO_SYSTEM_ENVIRONMENT_PATH NO_CMAKE_FIND_ROOT_PATH PATHS "${SUBPROJECTS_ROOT}/libntlm-1.8/dist/lib")
  find_library(GLIB   glib-2.0  REQUIRED NO_CMAKE_SYSTEM_PATH NO_SYSTEM_ENVIRONMENT_PATH NO_CMAKE_FIND_ROOT_PATH PATHS "${GSTREAMER_ROOT}/lib")
  find_library(SSL    ssl       REQUIRED NO_CMAKE_SYSTEM_PATH NO_SYSTEM_ENVIRONMENT_PATH NO_CMAKE_FIND_ROOT_PATH PATHS "${GSTREAMER_ROOT}/lib")
  find_library(CRYPTO crypto    REQUIRED NO_CMAKE_SYSTEM_PATH NO_SYSTEM_ENVIRONMENT_PATH NO_CMAKE_FIND_ROOT_PATH PATHS "${GSTREAMER_ROOT}/lib")
  find_library(GOBJ gobject-2.0 REQUIRED NO_CMAKE_SYSTEM_PATH NO_SYSTEM_ENVIRONMENT_PATH NO_CMAKE_FIND_ROOT_PATH PATHS "${GSTREAMER_ROOT}/lib")
  target_include_directories(onvifsoap PRIVATE ${GSTREAMER_ROOT}/include)
  target_include_directories(onvifsoap PRIVATE ${GSTREAMER_ROOT}/include/openssl)
  target_include_directories(onvifsoap PRIVATE ${GSTREAMER_ROOT}/include/glib-2.0)
  target_include_directories(onvifsoap PRIVATE ${GSTREAMER_ROOT}/lib/glib-2.0/include)
  target_include_directories(onvifsoap PRIVATE ${SUBPROJECTS_ROOT}/CUtils/dist/include)
  target_include_directories(onvifsoap PRIVATE ${SUBPROJECTS_ROOT}/libntlm-1.8/dist/include)
  target_link_libraries(onvifsoap PRIVATE ${CUTILS} ${NTLM} ${GLIB} ${GOBJ} ${SSL} ${CRYPTO})

  #glib dependencies
  add_library(iconv INTERFACE IMPORTED)
  add_library(ffi INTERFACE IMPORTED)
  set_target_properties(iconv PROPERTIES IMPORTED_LOCATION ${GSTREAMER_ROOT}/lib/libiconv.a)
  set_target_properties(ffi PROPERTIES IMPORTED_LOCATION ${GSTREAMER_ROOT}/lib/libffi.a)
  target_link_libraries(onvifsoap PRIVATE iconv ffi)
else()
  set(ENV{PKG_CONFIG_PATH} "$ENV{PKG_CONFIG_PATH}:${CMAKE_CURRENT_SOURCE_DIR}/subprojects/CUtils/build/dist/lib/pkgconfig")
  set(ENV{PKG_CONFIG_PATH} "$ENV{PKG_CONFIG_PATH}:${CMAKE_CURRENT_SOURCE_DIR}/subprojects/libntlm/build/dist/lib/pkgconfig")
  find_package(PkgConfig REQUIRED)
  pkg_check_modules(CUTILS REQUIRED cutils)
  pkg_check_modules(NTLM REQUIRED libntlm)
  pkg_check_modules(GLIB REQUIRED glib-2.0)
  pkg_check_modules(GOBJECT REQUIRED gobject-2.0)
  pkg_check_modules(ZSTD QUIET libzstd)
  target_include_directories(onvifsoap PRIVATE ${CUTILS_INCLUDE_DIRS} ${NTLM_INCLUDE_DIRS} ${GLIB_INCLUDE_DIRS} ${GOBJECT_INCLUDE_DIRS})
  target_link_libraries(onvifsoap PRIVATE ${ZSTD_LDFLAGS} ${CUTILS_LDFLAGS} ${NTLM_LDFLAGS} ${GLIB_LDFLAGS} ${GOBJECT_LDFLAGS})
  find_package(OpenSSL REQUIRED)
  find_package(ZLIB REQUIRED)
  find_package(Threads REQUIRED)
  target_link_libraries(onvifsoap PRIVATE OpenSSL::SSL OpenSSL::Crypto ${CMAKE_DL_LIBS})
  target_link_libraries(onvifsoap PRIVATE ZLIB::ZLIB)
  target_link_libraries(onvifsoap PRIVATE Threads::Threads)
  target_compile_options(onvifsoap PRIVATE "SHELL: -Wall")
endif()

if (MSVC)
  #I care little for MSVC warning. Support is from good faith
  # target_compile_options(gsoap PRIVATE "SHELL: -wd4244 -wd4267")
  set_source_files_properties(${GSOAP_SRC_DIR}/gsoap/dom.c 
                              ${GSOAP_SRC_DIR}/gsoap/stdsoap2.c 
                              ${GSOAP_SRC_DIR}/gsoap/plugin/logging.c
                              ${GSOAP_SRC_DIR}/gsoap/plugin/smdevp.c 
                              ${GSOAP_SRC_DIR}/gsoap/plugin/mecevp.c 
                              ${GSOAP_SRC_DIR}/gsoap/plugin/wsaapi.c 
                              ${GSOAP_SRC_DIR}/gsoap/plugin/wsseapi.c 
                              ${GSOAP_SRC_DIR}/gsoap/plugin/wsddapi.c
                              ${GSOAP_SRC_DIR}/gsoap/plugin/threads.c
                              ${GSOAP_SRC_DIR}/gsoap/plugin/httpda.c
                              ${GSOAP_SRC_DIR}/gsoap/custom/struct_timeval.c PROPERTIES COMPILE_FLAGS -wd4244 -wd4267)
  target_compile_options(onvifsoap PRIVATE "SHELL: -wd4100 -wd4244 -wd4255 -wd4706 -wd4820 -wd4996 -wd5045")
else()
  target_compile_options(onvifsoap PRIVATE "SHELL: -fvisibility=hidden")
  # target_compile_options(gsoap PRIVATE "SHELL: -Wformat-truncation=0 -Wno-deprecated-declarations -Wno-format -fPIC")
  set_source_files_properties(${GSOAP_SRC_DIR}/gsoap/dom.c 
                              ${GSOAP_SRC_DIR}/gsoap/stdsoap2.c 
                              ${GSOAP_SRC_DIR}/gsoap/plugin/logging.c
                              ${GSOAP_SRC_DIR}/gsoap/plugin/smdevp.c 
                              ${GSOAP_SRC_DIR}/gsoap/plugin/mecevp.c 
                              ${GSOAP_SRC_DIR}/gsoap/plugin/wsaapi.c 
                              ${GSOAP_SRC_DIR}/gsoap/plugin/wsseapi.c 
                              ${GSOAP_SRC_DIR}/gsoap/plugin/wsddapi.c
                              ${GSOAP_SRC_DIR}/gsoap/plugin/threads.c
                              ${GSOAP_SRC_DIR}/gsoap/plugin/httpda.c
                              ${GSOAP_SRC_DIR}/gsoap/custom/struct_timeval.c PROPERTIES COMPILE_FLAGS -w)
endif()

target_compile_options(onvifsoap PRIVATE "SHELL: -DWITH_GZIP -DWITH_OPENSSL -c -Wextra -Wpedantic -Wno-unused-parameter -DWITH_NONAMESPACES -DWITH_NOEMPTYSTRUCT -DWITH_NOEMPTYNAMESPACES -DWITH_DOM -DWITH_NTLM -DBUILD_SHARD -DSOAP_H_FILE=onvifsoapH.h")
target_compile_options(onvifsoap PRIVATE "-Wno-gnu-zero-variadic-macro-arguments")

if(ENABLE_EXAMPLES)
  add_subdirectory(example)
endif()

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
set(CMAKE_INSTALL_BINDIR ${CMAKE_INSTALL_PREFIX}/bin)
set(CMAKE_INSTALL_LIBDIR ${CMAKE_INSTALL_PREFIX}/lib)
set(CMAKE_INSTALL_INCLUDEDIR ${CMAKE_INSTALL_PREFIX}/include)

#Defining headers to export
set(headers ${PROJECT_SOURCE_DIR}/src/discovery/discoverer.h 
		        ${PROJECT_SOURCE_DIR}/src/discovery/onvif_discovery.h 
		        ${PROJECT_SOURCE_DIR}/src/discovery/probmatch.h
            ${PROJECT_SOURCE_DIR}/src/discovery/discovered_obj.h
            ${PROJECT_SOURCE_DIR}/src/onvif_base_service.h
		        ${PROJECT_SOURCE_DIR}/src/onvif_credentials.h
            ${PROJECT_SOURCE_DIR}/src/onvif_device.h
            ${PROJECT_SOURCE_DIR}/src/shard_export.h
            ${PROJECT_SOURCE_DIR}/src/SoapObject.h)
set(deviceio_headers 
            ${PROJECT_SOURCE_DIR}/src/deviceio/onvif_deviceio_audio_output_configuration_options.h
            ${PROJECT_SOURCE_DIR}/src/deviceio/onvif_tokens.h
            ${PROJECT_SOURCE_DIR}/src/deviceio/onvif_deviceio_service.h)
set(devicemgmt_headers 
            ${PROJECT_SOURCE_DIR}/src/devicemgmt/onvif_device_capabilities.h
            ${PROJECT_SOURCE_DIR}/src/devicemgmt/onvif_device_datetime.h
            ${PROJECT_SOURCE_DIR}/src/devicemgmt/onvif_device_hostnameinfo.h
            ${PROJECT_SOURCE_DIR}/src/devicemgmt/onvif_device_info.h
            ${PROJECT_SOURCE_DIR}/src/devicemgmt/onvif_device_interface.h
            ${PROJECT_SOURCE_DIR}/src/devicemgmt/onvif_device_ntp.h
            ${PROJECT_SOURCE_DIR}/src/devicemgmt/onvif_device_scopes.h
            ${PROJECT_SOURCE_DIR}/src/devicemgmt/onvif_device_service.h
            ${PROJECT_SOURCE_DIR}/src/devicemgmt/onvif_device_services.h
            ${PROJECT_SOURCE_DIR}/src/devicemgmt/onvif_prefix_ip_address.h)
set(media_headers
            ${PROJECT_SOURCE_DIR}/src/media/onvif_media_audio_output_config_options.h
            ${PROJECT_SOURCE_DIR}/src/media/onvif_media_audio_output_config.h
            ${PROJECT_SOURCE_DIR}/src/media/onvif_media_audio_output_configs.h
            ${PROJECT_SOURCE_DIR}/src/media/onvif_media_audio_source_config.h
            ${PROJECT_SOURCE_DIR}/src/media/onvif_media_audio_source_configs.h
            ${PROJECT_SOURCE_DIR}/src/media/onvif_media_profile.h
            ${PROJECT_SOURCE_DIR}/src/media/onvif_media_profiles.h
            ${PROJECT_SOURCE_DIR}/src/media/onvif_media_service.h
            ${PROJECT_SOURCE_DIR}/src/media/onvif_media_service_capabilities.h
            ${PROJECT_SOURCE_DIR}/src/media/onvif_media_snapshot.h
            ${PROJECT_SOURCE_DIR}/src/media/onvif_uri.h)
set(media1_headers
            ${PROJECT_SOURCE_DIR}/src/media/media1/onvif_media1_audio_output_config_options.h
            ${PROJECT_SOURCE_DIR}/src/media/media1/onvif_media1_audio_output_configs.h
            ${PROJECT_SOURCE_DIR}/src/media/media1/onvif_media1_audio_source_configs.h
            ${PROJECT_SOURCE_DIR}/src/media/media1/onvif_media1_profile.h
            ${PROJECT_SOURCE_DIR}/src/media/media1/onvif_media1_profiles.h
            ${PROJECT_SOURCE_DIR}/src/media/media1/onvif_media1_service.h
            ${PROJECT_SOURCE_DIR}/src/media/media1/onvif_media1_service_capabilities.h
            ${PROJECT_SOURCE_DIR}/src/media/media1/onvif_media1_uri.h)
set(media2_headers
            ${PROJECT_SOURCE_DIR}/src/media/media2/onvif_media2_audio_output_config_options.h
            ${PROJECT_SOURCE_DIR}/src/media/media2/onvif_media2_audio_output_configs.h
            ${PROJECT_SOURCE_DIR}/src/media/media2/onvif_media2_audio_source_configs.h
            ${PROJECT_SOURCE_DIR}/src/media/media2/onvif_media2_service.h
            ${PROJECT_SOURCE_DIR}/src/media/media2/onvif_media2_service_capabilities.h
            ${PROJECT_SOURCE_DIR}/src/media/media2/onvif_media2_profile.h
            ${PROJECT_SOURCE_DIR}/src/media/media2/onvif_media2_profiles.h
            ${PROJECT_SOURCE_DIR}/src/media/media2/onvif_media2_uri.h)

#Setting pkgconfig file file variables
set(prefix ${CMAKE_INSTALL_PREFIX})
set(exec_prefix ${CMAKE_INSTALL_BINDIR})
set(libdir ${CMAKE_INSTALL_LIBDIR})
set(includedir ${CMAKE_INSTALL_INCLUDEDIR})

if(NOT WIN32)
  #Create pkgconfig file
  configure_file(onvifsoap.pc.in onvifsoap.pc @ONLY) 
  install(FILES ${PROJECT_BINARY_DIR}/onvifsoap.pc DESTINATION "${CMAKE_INSTALL_LIBDIR}/pkgconfig")
endif()

install(TARGETS onvifsoap EXPORT onvifsoap DESTINATION "${CMAKE_INSTALL_LIBDIR}")
install(FILES ${headers} DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}")
install(FILES ${deviceio_headers} DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/deviceio")
install(FILES ${devicemgmt_headers} DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/devicemgmt")
install(FILES ${media_headers} DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/media")
install(FILES ${media1_headers} DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/media/media1")
install(FILES ${media2_headers} DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/media/media2")